#include "mqttpage.h"
#include "ui_mqttpage.h"
#include "hardwarepage.h"
#include <QRandomGenerator>
#include <QDebug>

MqttPage::MqttPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MqttPage)
    , m_client(nullptr)
{
    ui->setupUi(this);

    m_client = new QMqttClient(this);
    connect(m_client, &QMqttClient::connected, this, &MqttPage::mqttconnect);
    connect(m_client, &QMqttClient::disconnected, this, &MqttPage::mqttisconnect);
    connect(m_client, &QMqttClient::messageReceived, this, &MqttPage::mqttmessage);

    // 温湿度定时上报
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MqttPage::timeready);

    qrcodehelper = new text2Qrcode(this);

    keyboard = new KeyBoard();
    ui->lineEdit->installEventFilter(keyboard);
    ui->lineEdit_2->installEventFilter(keyboard);
    ui->lineEdit_3->installEventFilter(keyboard);
    ui->lineEdit_4->installEventFilter(keyboard);
}

MqttPage::~MqttPage()
{
    delete ui;
}

void MqttPage::setHardwarePage(HardwarePage *hw)
{
    hwPage = hw;
}

// ========== 连接/断开 ==========

void MqttPage::on_pushButton_clicked()
{
    QString hostname = ui->lineEdit->text();
    qint16 port = ui->lineEdit_2->text().toUShort();
    m_client->setHostname(hostname);
    m_client->setPort(port);
    m_client->connectToHost();
}

void MqttPage::mqttconnect()
{
    ui->label_5->setText("连接成功！");
    // 连接成功后自动开始温湿度上报
    timer->start(5000);
}

void MqttPage::mqttisconnect()
{
    ui->label_5->setText("断开成功！");
    timer->stop();
    qDebug() << "MQTT disconnected";
}

void MqttPage::mqttmessage()
{
    ui->label_5->setText("收到消息");
}

void MqttPage::on_pushButton_2_clicked()
{
    m_client->disconnectFromHost();
}

// ========== LED 控制 ==========

void MqttPage::on_pushButton_3_clicked()
{
    QString message = "{\"lamp\":true}";
    QString topic = ui->lineEdit_3->text();
    qDebug() << message;
    m_client->publish(topic, message.toUtf8());
}

void MqttPage::on_pushButton_4_clicked()
{
    QString message = "{\"lamp\":false}";
    QString topic = ui->lineEdit_3->text();
    m_client->publish(topic, message.toUtf8());
}

// ========== 警报控制 ==========

void MqttPage::on_pushButton_5_clicked()
{
    QString message = "{\"alarm\":true}";
    QString topic = ui->lineEdit_3->text();
    m_client->publish(topic, message.toUtf8());
}

void MqttPage::on_pushButton_6_clicked()
{
    QString message = "{\"alarm\":false}";
    QString topic = ui->lineEdit_3->text();
    m_client->publish(topic, message.toUtf8());
}

// ========== 二维码 ==========

void MqttPage::on_pushButton_7_clicked()
{
    QString host = ui->lineEdit->text();
    QString port = ui->lineEdit_2->text();
    QString subTopic = ui->lineEdit_4->text();
    QString pubTopic = ui->lineEdit_3->text();

    QString configJson = QString("{\"id\":\"n/a\",\"ipAddress\":\"%1\",\"port\":%2,\"clientId\":\"%3\",\"subscribeTopic\":\"%4\",\"publishTopic\":\"%5\"}")
        .arg(host)
        .arg(port.toInt())
        .arg(QRandomGenerator::global()->generate())
        .arg(subTopic)
        .arg(pubTopic);

    QPixmap pixmap = qrcodehelper->toPixmap(configJson);
    ui->label_6->setPixmap(pixmap);
    ui->label_6->setScaledContents(true);
}

// ========== 温湿度上报 ==========

void MqttPage::timeready()
{

        // 1. 检查 MQTT 连接状态
        int st = m_client->state();
        if (st != QMqttClient::Connected) {
            qDebug() << "[MQTT·上报] 跳过 — 未连接, state=" << st;
            return;
        }

        // 2. 检查硬件页面引用
        if (!hwPage) {
            qDebug() << "[MQTT·上报] hwPage 为空，无法读取温湿度";
            return;
        }

        // 3. 从硬件页面的同一个传感器实例读取（避免重复打开 IIO 设备）
        double temp = hwPage->getTemperature();
        double hum  = hwPage->getHumidity();

        // 4. 防御：硬件读取失败时可能返回 NaN/Inf
        if (qIsNaN(temp) || qIsInf(temp)) {
            qDebug() << "[MQTT·上报] 温度无效(NaN/Inf)，跳过上报";
            return;
        }
        if (qIsNaN(hum) || qIsInf(hum)) {
            qDebug() << "[MQTT·上报] 湿度无效(NaN/Inf)，跳过上报";
            return;
        }

        // 5. 读取发布主题
        QString topic = ui->lineEdit_3->text().trimmed();
        if (topic.isEmpty()) {
            qDebug() << "[MQTT·上报] 发布主题为空，跳过";
            return;
        }

        // 6. 构造 JSON 并发布
        QString message = QString("{\"temp\":%1,\"hum\":%2}")
                              .arg(QString::number(temp, 'f', 1))
                              .arg(QString::number(hum,  'f', 1));
        qDebug() << "[MQTT·上报] topic=" << topic << "message=" << message;

        // QoS=1 确保至少送达一次，比默认 QoS=0（fire-and-forget）更可靠
        m_client->publish(topic, message.toUtf8(), 1);



}

void MqttPage::on_pushButton_8_clicked()
{
    // 手动开始温湿度上报
    if (m_client->state() == QMqttClient::Connected) {
        timer->start(5000);
        ui->label_5->setText("温湿度上报已开启");
    }
}

void MqttPage::on_pushButton_9_clicked()
{
    // 停止温湿度上报
    timer->stop();
    ui->label_5->setText("温湿度上报已停止");
}
