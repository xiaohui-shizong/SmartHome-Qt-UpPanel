#ifndef HARDWAREPAGE_H
#define HARDWAREPAGE_H

#include <QWidget>
#include <QTimer>
#include "hardware/fsmpLed.h"
#include "hardware/fsmpFan.h"
#include "hardware/fsmpBeeper.h"
#include "hardware/fsmpTempHum.h"
#include "hardware/fsmpEvents.h"
#include "hardware/fsmpCamera.h"
#include "hardware/fsmpLight.h"
#include "hardware/fsmpVibrator.h"
#include <QThread>
#include "SerialPort.h"

namespace Ui {
class HardwarePage;
}

class HardwarePage : public QWidget
{
    Q_OBJECT

public:
    explicit HardwarePage(QWidget *parent = nullptr);
    ~HardwarePage();

    // 供外部（MQTT/AI页面）调用的设备控制接口
    void setLed(bool on);           // 控制LED
    void setFanSpeed(int speed);    // 设置风扇转速
    void triggerBeeper(bool on);    // 蜂鸣器报警
    void openDoor();                // 开门
    void closeDoor();               // 关门
    void dryClothes();              // 晾衣服
    void collectClothes();          // 收衣服
    void startCamera();             // 打开监控
    double getTemperature();         // 获取温度
    double getHumidity();           // 获取湿度

signals:
    // 设备状态变化信号，供 MQTT 页面上报
    void deviceStateChanged(const QString &device, const QString &action, const QString &value);

private slots:
    // 设备控制按钮
    void on_btn_led_clicked();
    void on_btn_camera_clicked();
    void on_btn_playSong_clicked();
    void on_btn_vibrator_clicked();
    void on_radioBeeper_clicked(bool checked);
    void on_slider_fan_sliderMoved(int position);

    // 舵机控制
    void on_btn_doorOpen_clicked();
    void on_btn_doorClose_clicked();
    void on_btn_dryClothes_clicked();
    void on_btn_collectClothes_clicked();

    // 自动报警
    void on_btn_alarmOn_toggled(bool checked);

    // 自动模式
    void on_btn_autoMode_toggled(bool checked);

    // 定时与环境监测
    void timeready();
    void picready(const QImage &pix);
    void peopleready(bool detected);
    void updateTime();



private:
    Ui::HardwarePage *ui;

    // 硬件对象
    fsmpLeds *myLed;
    fsmpFan myFan;
    fsmpBeeper mybeeper;
    fsmpTempHum mytemhum;
    fsmpCamera *mycamera;
    fsmpEvents myevent;
    fsmpVibrator myvibrator;
    fsmpLight myLight;
    SerialPort *myport;

    // 状态变量
    QTimer *timer;
    bool isPlayingSong = false;
    bool autoAlarmEnabled = false;     // 自动报警开关
    bool autoModeEnabled = false;      // 总自动模式开关
    bool cameraAutoOn = false;         // 是否已自动开过监控（避免重复）
    int ledState = 0;                  // LED状态 0=关 1=开

    // 自动化状态（避免 static 变量状态残留）
    bool alarmActive = false;          // 报警是否已触发
    bool fanAutoOn = false;            // 风扇是否已自动开启
    bool clothesDryAuto = false;       // 晾衣架是否已自动触发

    // 音符频率定义
    void playBirthdaySong();

    // 自动化逻辑
    void checkAutoAlarm(double temp, double hum);   // 检查温湿度报警
    void checkAutomation(double temp, double hum);   // 检查自动化联动

    // 日志辅助
    void log(const QString &msg);
};

#endif // HARDWAREPAGE_H
