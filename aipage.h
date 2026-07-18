#ifndef AIPAGE_H
#define AIPAGE_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include "fsmpKeyboard/keyboard.h"
#include "hardwarepage.h"
#include <QSslConfiguration>
#include <QSslSocket>

namespace Ui {
class AiPage;
}

class AiPage : public QWidget
{
    Q_OBJECT

public:
    explicit AiPage(QWidget *parent = nullptr);
    ~AiPage();

    // 设置硬件页面引用，用于AI控制设备
    void setHardwarePage(HardwarePage *hw);

private slots:
    void on_pushButton_clicked();
    void onReplyFinished(QNetworkReply *reply);

private:
    Ui::AiPage *ui;
    QNetworkAccessManager *manager;
    QJsonArray conversationHistory;
    KeyBoard *keyboard;

    HardwarePage *hwPage = nullptr;  // 硬件控制页面的引用

    void sendToDeepSeek();
    void appendToChat(const QString &role, const QString &content);

    // 解析AI返回的JSON并执行设备控制
    bool parseAndExecuteDeviceCommand(const QString &aiResponse);
};

#endif // AIPAGE_H
