#ifndef FSMTPERVO_H
#define FSMTPERVO_H

#include <QObject>
#include <QFile>
#include <QTimer>
#include<QThread>
/**
 * @brief SG90 舵机控制类（FSMP1A 开发板）
 *
 * 通过 PWM 控制舵机角度，用于模拟开门/关门、晾衣/收衣等。
 * 控制脉冲周期 20ms，脉宽 0.5ms~2.5ms 对应 -90度~+90度（180度舵机）。
 * GPIO 使用 PWM 输出，默认使用 PB1 引脚（可根据实际板子修改）。
 */
class fsmpServo : public QObject
{
    Q_OBJECT

public:
    explicit fsmpServo(QObject *parent = nullptr, int pwmChannel = 0);

    /**
     * @brief 设置舵机目标角度
     * @param angle 角度值，范围 0~180 度
     */
    void setAngle(int angle);

    /**
     * @brief 获取当前角度
     * @return 当前角度值 (0~180)
     */
    int angle() const { return currentAngle; }

    /**
     * @brief 打开门（模拟）
     * 设置为 90 度（或自定义开门角度）
     */
    void openDoor();

    /**
     * @brief 关闭门（模拟）
     * 设置为 0 度
     */
    void closeDoor();

    /**
     * @brief 晾衣服（展开）
     * 设置为 180 度
     */
    void dryClothes();

    /**
     * @brief 收衣服（收起）
     * 设置为 0 度
     */
    void collectClothes();

private:
    int pwmChannel;
    int currentAngle = 90;
    QString pwmPath;
    QString exportPath;

    bool initPwm();
    void writePwm(int value);
};

#endif // FSMTPERVO_H
