#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>

class QTabWidget;
class HardwarePage;
class MqttPage;
class AiPage;

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = nullptr);
    ~MainForm();

private:
    QTabWidget *tabWidget;
    HardwarePage *hardwarePage;
    MqttPage *mqttPage;
    AiPage *aiPage;
};

#endif // MAINFORM_H
