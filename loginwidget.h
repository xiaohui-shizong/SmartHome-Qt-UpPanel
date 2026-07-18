#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QTimer>
#include "fsmpKeyboard/keyboard.h"

namespace Ui {
class LoginWidget;
}

class MainForm;

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private slots:
    void on_pushButton_clicked();
    void on_radioButton_clicked(bool checked);
    void on_pushButton_2_clicked();
    void on_horizontalSlider_sliderMoved(int position);
    void timeready();

private:
    Ui::LoginWidget *ui;
    MainForm *mainForm;
    QTimer *timer;
    KeyBoard *keyboard;
    int i;
};

#endif // LOGINWIDGET_H
