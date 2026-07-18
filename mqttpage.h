#ifndef MQTTPAGE_H
#define MQTTPAGE_H

#include <QWidget>
#include <QtMqtt/qmqttclient.h>
#include <QTimer>
#include "text2qrcode/text2qrcode.h"
#include "fsmpKeyboard/keyboard.h"

// 仅前向声明，避免循环依赖
class HardwarePage;

QT_BEGIN_NAMESPACE
namespace Ui { class MqttPage; }
QT_END_NAMESPACE

class MqttPage : public QWidget
{
    Q_OBJECT

public:
    explicit MqttPage(QWidget *parent = nullptr);
    ~MqttPage();

    // 设置硬件页面引用（供主界面调用）
    void setHardwarePage(HardwarePage *hw);

private slots:
    void on_pushButton_clicked();
    void mqttconnect();
    void mqttisconnect();
    void mqttmessage();

    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();

    void timeready();

    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();

private:
    Ui::MqttPage *ui;
    QMqttClient *m_client;
    QTimer *timer;
    text2Qrcode *qrcodehelper;
    KeyBoard *keyboard;

    HardwarePage *hwPage = nullptr;
};

#endif // MQTTPAGE_H
