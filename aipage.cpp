#include "aipage.h"
#include "ui_aipage.h"
#include "hardwarepage.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDateTime>
#include <QTextCursor>
#include <QRegularExpression>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QDebug>

#define API_KEY "sk-9028aac27e564733a2b8ee000af8335d"

AiPage::AiPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AiPage)
    , manager(new QNetworkAccessManager(this))
    , keyboard(new KeyBoard(this))
    , hwPage(nullptr)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);

    // 强化强制提示词，要求AI必须附带控制指令
    QJsonObject sysMsg;
    sysMsg["role"] = "system";
    QString contentText = QString(
        "你是SmartHome智能家居AI管家，可控制设备：\n"
        "1. LED灯：true开 / false关\n"
        "2. 风扇：0~100数字代表转速，0为关闭\n"
        "3. 蜂鸣器：true响 / false停\n"
        "4. 门舵机：open开门 / close关门\n"
        "5. 晾衣架：dry晾衣 / collect收衣\n"
        "6. 摄像头：true开启监控\n\n"
        "【硬性强制规则，绝对不能忽略】\n"
        "只要用户说开灯、关灯、调风扇、开门等任何设备操作，你的回复末尾**必须固定带上 [DEVICE_CMD]JSON[/DEVICE_CMD]**，少这条指令视为回答错误。\n"
        "示例对话：\n"
        "用户：开灯\n"
        "你输出：好的，已为你打开全屋灯光\n[DEVICE_CMD]{\"led\":true}[/DEVICE_CMD]\n"
        "用户：风扇开到60\n"
        "你输出：风扇转速已调整至60%\n[DEVICE_CMD]{\"fan\":60}[/DEVICE_CMD]\n"
        "用户：关门\n"
        "你输出：房门已关闭\n[DEVICE_CMD]{\"door\":\"close\"}[/DEVICE_CMD]\n"
        "用户：随便聊天不用控制设备，则不需要加指令标签。"
    );
    sysMsg.insert("content", contentText);
    conversationHistory.append(sysMsg);

    // 软键盘

    keyboard = new KeyBoard();
    ui->lineEdit->installEventFilter(keyboard);
    connect(manager, &QNetworkAccessManager::finished,
            this, &AiPage::onReplyFinished);
    connect(ui->lineEdit, &QLineEdit::returnPressed,
            this, &AiPage::on_pushButton_clicked);
}

AiPage::~AiPage()
{
    delete keyboard;
    delete ui;
}

void AiPage::setHardwarePage(HardwarePage *hw)
{
    hwPage = hw;
}

// ==================== 聊天界面 ====================
void AiPage::appendToChat(const QString &role, const QString &content)
{
    QString label;
    QString color;
    if (role == "user") {
        label = "用户";
        color = "#185FA5";
    } else {
        label = "管家";
        color = "#3B6D11";
    }

    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->textEdit->append(
        QString("<b style='color:%1'>%2</b> "
                "<span style='color:#888; font-size:12px;'>%3</span><br>%4")
            .arg(color, label, timeStr, content.toHtmlEscaped()));
    ui->textEdit->append("");

    // 记录对话历史
    QJsonObject msg;
    msg["role"] = role == "user" ? "user" : "assistant";
    msg["content"] = content;
    conversationHistory.append(msg);
}

void AiPage::on_pushButton_clicked()
{
    QString userText = ui->lineEdit->text().trimmed();
    if (userText.isEmpty()) return;

    ui->lineEdit->clear();
    appendToChat("user", userText);

    ui->pushButton->setEnabled(false);
    ui->textEdit->append("<i style='color:#888;'>正在思考...</i>\n");
    sendToDeepSeek();
}

// ==================== DeepSeek API ====================
void AiPage::sendToDeepSeek()
{
    QUrl url("https://api.deepseek.com/chat/completions");
    QNetworkRequest request(url);

    // 忽略SSL证书校验，适配嵌入式板子
    QSslConfiguration sslCfg = QSslConfiguration::defaultConfiguration();
    sslCfg.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslCfg);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QByteArray("Bearer ") + API_KEY);

    QJsonObject body;
    body["model"] = "deepseek-chat";
    body["stream"] = false;
    body["messages"] = conversationHistory;

    QJsonDocument doc(body);
    manager->post(request, doc.toJson());
}

void AiPage::onReplyFinished(QNetworkReply *reply)
{
    ui->pushButton->setEnabled(true);

    // 移除"正在思考..."
    QString text = ui->textEdit->toPlainText();
    int lastNewline = text.lastIndexOf("\n正在思考...");
    if (lastNewline != -1) {
        text = text.left(lastNewline);
        ui->textEdit->setPlainText(text);
    }

    if (reply->error() != QNetworkReply::NoError) {
        appendToChat("assistant", "请求失败：" + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();

    QString content = jsonObj["choices"]
                          .toArray()[0]
                          .toObject()["message"]
                          .toObject()["content"]
                          .toString();

    // 先显示AI的完整回复
    appendToChat("assistant", content);

    // 解析并执行设备控制命令
    bool cmdExecuted = parseAndExecuteDeviceCommand(content);

    // 如果执行了设备操作，追加显示
    if (cmdExecuted) {
        ui->textEdit->append("<span style='color:#27AE60; font-size:11px;'>⚙️ 设备控制指令已执行</span>\n");
    }

    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textEdit->setTextCursor(cursor);

    reply->deleteLater();
}

bool AiPage::parseAndExecuteDeviceCommand(const QString &aiResponse)
{
    if (!hwPage) {
        qWarning() << "[AI] 硬件页面未设置，无法执行设备控制";
        return false;
    }

    // ========== 新增兜底：直接识别用户输入关键词，AI没输出标签也能控制 ==========
    QString userMsg = conversationHistory.last().toObject()["content"].toString();
    // 开灯
    if(userMsg.contains("开灯")){
        hwPage->setLed(true);
        qDebug() << "关键词兜底：开灯";
        return true;
    }
    // 关灯
    if(userMsg.contains("关灯")){
        hwPage->setLed(false);
        qDebug() << "关键词兜底：关灯";
        return true;
    }
    // 开门
    if(userMsg.contains("开门")){
        hwPage->openDoor();
        qDebug() << "关键词兜底：开门";
        return true;
    }
    // 关门
    if(userMsg.contains("关门")){
        hwPage->closeDoor();
        qDebug() << "关键词兜底：关门";
        return true;
    }
    // 晾衣服
    if(userMsg.contains("晾衣服")){
        hwPage->dryClothes();
        qDebug() << "关键词兜底：晾衣";
        return true;
    }
    // 收衣服
    if(userMsg.contains("收衣服")){
        hwPage->collectClothes();
        qDebug() << "关键词兜底：收衣";
        return true;
    }
    // 开风扇
    if(userMsg.contains("开风扇")){
        hwPage->setFanSpeed(70);
        qDebug() << "关键词兜底：开风扇";
        return true;
    }
    // 关风扇
    if(userMsg.contains("关风扇")){
        hwPage->setFanSpeed(0);
        qDebug() << "关键词兜底：关风扇";
        return true;
    }
    // 开监控
    if(userMsg.contains("打开监控") || userMsg.contains("开启摄像头")){
        hwPage->startCamera();
        qDebug() << "关键词兜底：开启监控";
        return true;
    }

    // ========== 原有正则匹配标签逻辑 ==========
    QRegularExpression regex(R"(\[DEVICE_CMD\](.*?)\[\/DEVICE_CMD\])",
                            QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator it = regex.globalMatch(aiResponse);

    QStringList executedActions;
    bool anyExecuted = false;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString jsonStr = match.captured(1).trimmed();

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &err);
        if (err.error != QJsonParseError::NoError) continue;

        QJsonObject cmd = doc.object();

        if (cmd.contains("led")) {
            hwPage->setLed(cmd["led"].toBool());
            executedActions.append(cmd["led"].toBool() ? "LED已开启" : "LED已关闭");
            anyExecuted = true;
        }
        if (cmd.contains("fan")) {
            hwPage->setFanSpeed(cmd["fan"].toInt(0));
            int speed = cmd["fan"].toInt(0);
            executedActions.append(speed > 0 ? QString("风扇转速%1%").arg(speed) : "风扇已关闭");
            anyExecuted = true;
        }
        if (cmd.contains("beeper")) {
            hwPage->triggerBeeper(cmd["beeper"].toBool());
            executedActions.append(cmd["beeper"].toBool() ? "蜂鸣器已响" : "蜂鸣器已停止");
            anyExecuted = true;
        }
        if (cmd.contains("door")) {
            QString d = cmd["door"].toString().trimmed();
            if (d == "open" || d == "打开" || d == "开") { hwPage->openDoor(); executedActions.append("门已打开"); }
            else if (d == "close" || d == "关闭" || d == "关") { hwPage->closeDoor(); executedActions.append("门已关闭"); }
            anyExecuted = true;
        }
        if (cmd.contains("clothes")) {
            QString c = cmd["clothes"].toString().trimmed();
            if (c == "dry" || c == "晾" || c == "晾衣") { hwPage->dryClothes(); executedActions.append("晾衣架已展开"); }
            else if (c == "collect" || c == "收" || c == "收衣") { hwPage->collectClothes(); executedActions.append("晾衣架已收回"); }
            anyExecuted = true;
        }
        if (cmd.contains("camera") && cmd["camera"].toBool()) {
            hwPage->startCamera();
            executedActions.append("监控已启动");
            anyExecuted = true;
        }
        if (cmd.contains("alarm") && cmd["alarm"].toBool()) {
            hwPage->triggerBeeper(true);
            executedActions.append("警报已触发");
            anyExecuted = true;
        }
    }

    if (!anyExecuted) {
        QJsonParseError err2;
        QJsonDocument doc2 = QJsonDocument::fromJson(aiResponse.trimmed().toUtf8(), &err2);
        if (err2.error == QJsonParseError::NoError && doc2.isObject()) {
            QJsonObject directCmd = doc2.object();

            if (directCmd.contains("led")) { hwPage->setLed(directCmd["led"].toBool()); executedActions.append(directCmd["led"].toBool()?"LED开":"LED关"); anyExecuted = true; }
            if (directCmd.contains("fan"))
            {
                int fanSpeed = directCmd["fan"].toInt();
                hwPage->setFanSpeed(fanSpeed);
                QString fanTip = fanSpeed > 0 ? QString("风扇转速%1").arg(fanSpeed) : "风扇关闭";
                executedActions.append(fanTip);
                anyExecuted = true;
            }
            if (directCmd.contains("beeper")) { hwPage->triggerBeeper(directCmd["beeper"].toBool()); executedActions.append(directCmd["beeper"].toBool()?"蜂鸣器响":"蜂鸣器停"); anyExecuted=true; }
            if (directCmd.contains("camera") && directCmd["camera"].toBool()) { hwPage->startCamera(); executedActions.append("监控启动"); anyExecuted=true; }
        }
    }

    if (anyExecuted) {
        qDebug() << "[AI] 执行设备控制:" << executedActions.join(", ");
    }

    return anyExecuted;
}
