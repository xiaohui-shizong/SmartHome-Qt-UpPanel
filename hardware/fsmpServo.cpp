#include "fsmpServo.h"
#include <QFile>
#include <QDebug>

fsmpServo::fsmpServo(QObject *parent, int channel)
    : QObject(parent), pwmChannel(channel), currentAngle(90)
{
    // FSMP1A 开发板 PWM 路径（根据实际板子引脚调整）
    pwmPath = QString("/sys/class/pwm/pwmchip0/pwm%1").arg(pwmChannel);
}

bool fsmpServo::initPwm()
{
    // 检查 PWM 通道是否已导出
    if (!QFile::exists(pwmPath)) {
        QFile exportFile("/sys/class/pwm/pwmchip0/export");
        if (exportFile.open(QIODevice::WriteOnly)) {
            exportFile.write(QString::number(pwmChannel).toUtf8());
            exportFile.close();
            QThread::msleep(100); // 等待系统创建节点
        } else {
            qDebug() << "[舵机] 无法导出 PWM" << pwmChannel;
            return false;
        }
    }

    // 设置周期 20000000ns (20ms)
    {
        QFile period(pwmPath + "/period");
        if (period.open(QIODevice::WriteOnly)) {
            period.write("20000000");
            period.close();
        }
    }

    return true;
}

void fsmpServo::writePwm(int value)
{
    initPwm();

    QFile dutyCycle(pwmPath + "/duty_cycle");
    if (dutyCycle.open(QIODevice::WriteOnly)) {
        dutyCycle.write(QString::number(value).toUtf8());
        dutyCycle.close();
    }

    // 使能 PWM
    QFile enable(pwmPath + "/enable");
    if (enable.open(QIODevice::WriteOnly)) {
        enable.write("1");
        enable.close();
    }
}

void fsmpServo::setAngle(int angle)
{
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    currentAngle = angle;

    /**
     * SG90 舵机脉宽计算：
     * 周期 20ms = 20000us
     * 0度   = 500us 脉宽
     * 180度 = 2500us 脉宽
     * 线性插值: pulse_us = 500 + (angle/180) * 2000
     */
    int pulseUs = 500 + (int)((double)angle / 180.0 * 2000.0);
    int pulseNs = pulseUs * 1000; // 转换为纳秒

    writePwm(pulseNs);

    qDebug() << "[舵机] 角度:" << angle << "度, 脉宽:" << pulseUs << "us";
}

void fsmpServo::openDoor()
{
    setAngle(90);
    qDebug() << "[舵机] 门已打开";
}

void fsmpServo::closeDoor()
{
    setAngle(0);
    qDebug() << "[舵机] 门已关闭";
}

void fsmpServo::dryClothes()
{
    setAngle(180);
    qDebug() << "[舵机] 已晾衣服";
}

void fsmpServo::collectClothes()
{
    setAngle(0);
    qDebug() << "[舵机] 已收衣服";
}
