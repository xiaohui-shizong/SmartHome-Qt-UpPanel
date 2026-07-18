#include "hardwarepage.h"
#include "ui_hardwarepage.h"
#include <QFile>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <cstdint>

// 音符频率定义
#define L1 262
#define L2 294
#define L3 330
#define L4 349
#define L5 392
#define L6 440
#define L7 494
#define M1 523
#define M2 587
#define M3 659
#define M4 698
#define M5 784
#define M6 880
#define M7 988
#define H1 1046
#define H2 1175
#define H3 1318
#define H4 1397
#define H5 1568
#define H6 1760
#define H7 1967

// ==================== 构造/析构 ====================

HardwarePage::HardwarePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HardwarePage)
    , timer(nullptr)
{
    ui->setupUi(this);
    ui->btn_led->setText("全部开灯");

    // 初始化硬件模块
    myLed = new fsmpLeds(this);
    myport = new SerialPort;

    // 风扇滑动条：范围 0-100
    ui->slider_fan->setRange(0, 100);
    connect(ui->slider_fan, &QSlider::sliderMoved, this, &HardwarePage::on_slider_fan_sliderMoved);
    connect(ui->slider_fan, &QSlider::valueChanged, this, &HardwarePage::on_slider_fan_sliderMoved);

    // 显式连接报警和自动模式按钮（确保可靠触发）
    connect(ui->btn_alarmOn, &QPushButton::toggled, this, &HardwarePage::on_btn_alarmOn_toggled);
    connect(ui->btn_autoMode, &QPushButton::toggled, this, &HardwarePage::on_btn_autoMode_toggled);

    // 定时刷新温湿度+照度（1秒一次）
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &HardwarePage::timeready);
    connect(timer, &QTimer::timeout, this, &HardwarePage::updateTime);
    timer->start(1000);

    // 摄像头初始化
    mycamera = new fsmpCamera("/dev/video1", 640, 480);
    mycamera->setPixDelay(30);
    connect(mycamera, &fsmpCamera::pixReady, this, &HardwarePage::picready);

    // 人体红外检测
    connect(&myevent, &fsmpEvents::peopleDetected, this, &HardwarePage::peopleready);

    log("系统初始化完成，所有硬件就绪");
}

HardwarePage::~HardwarePage()
{
    delete ui;
}

// ==================== 设备控制接口（供外部MQTT/AI调用）====================

void HardwarePage::setLed(bool on)
{
    if (on) {
        myLed->on(fsmpLeds::LED1);
        myLed->on(fsmpLeds::LED2);
        myLed->on(fsmpLeds::LED3);
        ui->btn_led->setText("全部关灯");
        ledState = 1;
    } else {
        myLed->off(fsmpLeds::LED1);
        myLed->off(fsmpLeds::LED2);
        myLed->off(fsmpLeds::LED3);
        ui->btn_led->setText("全部开灯");
        ledState = 0;
    }
    emit deviceStateChanged("led", on ? "on" : "off", on ? "1" : "0");
}

void HardwarePage::setFanSpeed(int speed)
{
    if (speed > 0) {
        myFan.setSpeed(speed);   // setSpeed 内部已写入 PWM
        ui->slider_fan->setValue(speed);
    } else {
        myFan.stop();
    }
    emit deviceStateChanged("fan", "speed", QString::number(speed));
}

void HardwarePage::triggerBeeper(bool on)
{
    if (on) {
        mybeeper.start();
    } else {
        mybeeper.stop();
    }
    emit deviceStateChanged("beeper", on ? "on" : "off", on ? "1" : "0");
}

void HardwarePage::openDoor() {myport->openPort();myport->writeByte('2');}
void HardwarePage::closeDoor() { myport->openPort();myport->writeByte('4');}
void HardwarePage::dryClothes() {myport->openPort();myport->writeByte('3'); }
void HardwarePage::collectClothes() { myport->openPort();myport->writeByte('6');}
void HardwarePage::startCamera() { mycamera->start(); ui->label_camStatus->setText("监控状态: 运行中"); ui->label_camStatus->setStyleSheet("color:#27AE60; font-size:11px;"); }

double HardwarePage::getTemperature() { return mytemhum.temperature(); }
double HardwarePage::getHumidity() { return mytemhum.humidity(); }

// ==================== LED 控制 ====================

void HardwarePage::on_btn_led_clicked()
{
    setLed(ledState == 0);
    log(QString("LED → %1").arg(ledState ? "全亮" : "全灭"));
}

// ==================== 蜂鸣器 ====================

void HardwarePage::playBirthdaySong()
{
    uint32_t notes[] = {
        M5,M5,M6,M5,H1,M7,
        M5,M5,M6,M5,H2,H1,
        M5,M5,H5,H3,H1,M7,M6,
        H4,H4,H3,H1,H2,H1
    };
    uint16_t beats[] = {
        200,200,400,400,400,800,
        200,200,400,400,400,800,
        200,200,400,400,400,400,800,
        200,200,400,400,400,800
    };
    int len = sizeof(notes)/sizeof(notes[0]);
    for (int i = 0; i < len; i++) {
        mybeeper.setRate(notes[i]);
        mybeeper.start();
        QThread::msleep(beats[i]);
        mybeeper.stop();
        QThread::msleep(50);
    }
}

void HardwarePage::on_radioBeeper_clicked(bool checked)
{
    if (checked) {
        isPlayingSong = true;
        playBirthdaySong();
        log("蜂鸣器 → 播放歌曲中...");
    } else {
        isPlayingSong = false;
        mybeeper.stop();
        log("蜂鸣器 → 已停止");
    }
}

// ==================== 风扇 ====================

void HardwarePage::on_slider_fan_sliderMoved(int position)
{
    setFanSpeed(position);
    log(QString("风扇 → 转速 %1%").arg(position));
}

// ==================== 唱歌按钮 ====================

void HardwarePage::on_btn_playSong_clicked()
{
    playBirthdaySong();
    log("蜂鸣器 → 播放生日歌");
}

// ==================== 振动马达 ====================

void HardwarePage::on_btn_vibrator_clicked()
{
    myvibrator.setParameter(0xf000, 30000);
    myvibrator.start();
    log("振动马达 → 已启动");
}

// ==================== 摄像头 ====================

void HardwarePage::on_btn_camera_clicked()
{
    startCamera();
    log("摄像头 → 手动开启监控");
}

void HardwarePage::picready(const QImage &pix)
{
    ui->label_camPreview->setPixmap(QPixmap::fromImage(pix).scaled(
        ui->label_camPreview->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
}

// ==================== 人体红外检测 ====================

void HardwarePage::peopleready(bool detected)
{
    if (detected) {
        ui->label_pir->setText("有人");
        ui->label_pir->setStyleSheet("color: #E74C3C; font-size:14px; font-weight:bold;");
        log("⚠️ 人体传感器 → 检测到有人！");

        // 自动模式：有人则自动打开监控
        if (autoModeEnabled && !cameraAutoOn) {
            startCamera();
            cameraAutoOn = true;
            log("📷 自动联动 → 检测到人，自动启动监控");
        }
    } else {
        ui->label_pir->setText("没人");
        ui->label_pir->setStyleSheet("color: #4A90D9; font-size:14px; font-weight:bold;");
        cameraAutoOn = false;  // 人走了，下次可以再次自动触发
    }
}

// ==================== 舵机控制 ====================

void HardwarePage::on_btn_doorOpen_clicked()
{ openDoor(); log("舵机 → 开门(90度)"); }

void HardwarePage::on_btn_doorClose_clicked()
{ closeDoor(); log("舵机 → 关门(0度)"); }

void HardwarePage::on_btn_dryClothes_clicked()
{ dryClothes(); log("舵机 → 晾衣服(180度)"); }

void HardwarePage::on_btn_collectClothes_clicked()
{ collectClothes(); log("舵机 → 收衣服(0度)"); }

// ==================== 自动报警 ====================

void HardwarePage::on_btn_alarmOn_toggled(bool checked)
{
    autoAlarmEnabled = checked;
    if (checked) {
        ui->btn_alarmOn->setText("关闭自动报警");
        ui->label_alarmStatus->setText("报警：已开启");
        ui->label_alarmStatus->setStyleSheet("color: #27AE60; font-size:11px; font-weight:bold;");
        log(QString("🔔 自动报警已开启 (温度>%1℃, 湿度>%2%)")
            .arg(ui->spin_tempThreshold->value())
            .arg(ui->spin_humThreshold->value()));
    } else {
        ui->btn_alarmOn->setText("开启自动报警");
        ui->label_alarmStatus->setText("报警: 关闭");
        ui->label_alarmStatus->setStyleSheet("color: #E74C3C; font-size:11px; font-weight:bold;");
        mybeeper.stop();
        alarmActive = false;     // 关闭时重置状态
        log("自动报警已关闭");
    }
}

// ==================== 自动模式总开关 ====================

void HardwarePage::on_btn_autoMode_toggled(bool checked)
{
    autoModeEnabled = checked;
    if (checked) {
        ui->btn_autoMode->setText("关闭自动模式");
        ui->label_autoStatus->setText("自动模式: 运行中");
        ui->label_autoStatus->setStyleSheet("color: #E67E22; font-size:11px; font-weight:bold;");
        log("🤖 自动模式已开启");
    } else {
        ui->btn_autoMode->setText("开启自动模式");
        ui->label_autoStatus->setText("自动模式: 关闭");
        ui->label_autoStatus->setStyleSheet("color: #27AE60; font-size:11px; font-weight:bold;");
        fanAutoOn = false;       // 关闭时重置风扇状态
        clothesDryAuto = false;  // 关闭时重置晾衣架状态
        log("自动模式已关闭");
    }
}

// ==================== 定时刷新 + 自动化判断核心 ====================

void HardwarePage::timeready()
{
    double temp = mytemhum.temperature();
    double hum = mytemhum.humidity();
    double light = myLight.getValue();

    // 更新UI显示
    ui->spin_temp->setValue(temp);
    ui->spin_hum->setValue(hum);
    ui->spin_light->setValue(light);

    // 执行自动化逻辑
    checkAutoAlarm(temp, hum);
    checkAutomation(temp, hum);
}

/**
 * @brief 自动温度/湿度报警
 * 当温度或湿度超过阈值时，触发蜂鸣器报警
 */
void HardwarePage::checkAutoAlarm(double temp, double hum)
{
    if (!autoAlarmEnabled) {
        alarmActive = false;
        return;
    }

    double tempThresh = ui->spin_tempThreshold->value();
    double humThresh = ui->spin_humThreshold->value();

    bool shouldAlarm = false;
    QString reason;

    if (temp > tempThresh) {
        shouldAlarm = true;
        reason = QString("温度超标! %1℃ > %2℃")
                     .arg(QString::number(temp, 'f', 1))
                     .arg(tempThresh);
    }
    if (hum > humThresh) {
        shouldAlarm = true;
        if (!reason.isEmpty()) reason += " | ";
        reason += QString("湿度超标! %1% > %2%")
                      .arg(QString::number(hum, 'f', 1))
                      .arg(humThresh);
    }

    qDebug() << "[Alarm] autoAlarmEnabled=" << autoAlarmEnabled
             << "temp=" << temp << "thresh=" << tempThresh
             << "hum=" << hum << "thresh=" << humThresh
             << "shouldAlarm=" << shouldAlarm << "alarmActive=" << alarmActive;

    if (shouldAlarm && !alarmActive) {
        alarmActive = true;
        triggerBeeper(true);
        ui->label_alarmStatus->setText("🚨 " + reason);
        log("🚨 报警触发! " + reason);
    } else if (!shouldAlarm && alarmActive) {
        alarmActive = false;
        triggerBeeper(false);
        ui->label_alarmStatus->setText("正常");
        log("✅ 环境恢复正常，停止报警");
    }
}

/**
 * @brief 自动化联动总控制
 *
 * 规则：
 * 1. 温度超标 → 自动通风（开风扇）
 * 2. 湿度超标 → 自动晾衣服
 * 3. 检测到人 → 自动开监控（已在 peopleready 中处理）
 */
void HardwarePage::checkAutomation(double temp, double hum)
{
    if (!autoModeEnabled) return;

    double tempThresh = ui->spin_tempThreshold->value();
    double humThresh = ui->spin_humThreshold->value();

    // 规则1：温度过高→自动通风
    if (temp > tempThresh && !fanAutoOn) {
        setFanSpeed(70);  // 中速通风
        fanAutoOn = true;
        log("🌀 自动联动 → 温度过高，自动开启风扇通风");
    } else if (temp <= (tempThresh - 2.0) && fanAutoOn) {
        // 温度回落到阈值以下2度后关闭
        setFanSpeed(0);
        fanAutoOn = false;
        log("🌀 自动联动 → 温度已恢复，关闭风扇");
    }

    // 规则2：湿度过高→自动晾衣服
    if (hum > humThresh && !clothesDryAuto) {
        dryClothes();
        clothesDryAuto = true;
        log("👕 自动联动 → 湿度过高，自动晾衣服");
    } else if (hum <= (humThresh - 10.0) && clothesDryAuto) {
        // 湿度大幅下降后收衣服
        collectClothes();
        clothesDryAuto = false;
        log("👕 自动联动 → 湿度已降，自动收衣服");
    }
}

// ==================== 辅助函数 ====================

void HardwarePage::log(const QString &msg)
{
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString currentLog = ui->label_log->text();
    if (currentLog.contains("系统就绪"))
        ui->label_log->setText(QString("[%1] %2").arg(timeStr, msg));
    else
        ui->label_log->setText(QString("[%1] %2\n").arg(timeStr, msg) + currentLog.left(300));

    qDebug() << "[SmartHome]" << msg;
}

void HardwarePage::updateTime()
{
    ui->label_time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}
