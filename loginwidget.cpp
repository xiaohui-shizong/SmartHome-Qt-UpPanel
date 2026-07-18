#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "mainform.h"
#include <QDebug>
#include <QPixmap>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
    , mainForm(nullptr)
    , timer(nullptr)
{
    ui->setupUi(this);
    i = 0;

    // 密码框设为密码模式
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    // 下拉模式列表
    QStringList list;
    list.append("666");
    list.append("999");
    list.append("睡眠模式");
    list.append("工作模式");
    ui->comboBox->addItems(list);

    // 数值调节
    ui->doubleSpinBox->setRange(0, 100);
    ui->doubleSpinBox->setValue(45);
    ui->doubleSpinBox->setSingleStep(10);

    // 背景图片
    ui->label_3->setPixmap(QPixmap(":/bg.png"));
    ui->label_3->setScaledContents(true);

    // 软键盘: 为两个输入框安装事件过滤器
    keyboard = new KeyBoard();
    ui->lineEdit->installEventFilter(keyboard);
    ui->lineEdit_2->installEventFilter(keyboard);

    // 定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LoginWidget::timeready);
    timer->start(1000);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_pushButton_clicked()
{
    // 登录验证
    if (ui->lineEdit->text() == "wang" && ui->lineEdit_2->text() == "123456") {
        qDebug() << "登录成功";

        if (!mainForm) {
            mainForm = new MainForm();
            qDebug() << "MainForm窗口已创建";
        }

        mainForm->show();
        this->hide();
        qDebug() << "主窗口已隐藏，跳转到MainForm";
    } else {
        qDebug() << "账号或密码错误";
    }
}

void LoginWidget::on_radioButton_clicked(bool checked)
{
    if (checked) {
        ui->lineEdit_2->setEchoMode(QLineEdit::Normal);
    } else {
        ui->lineEdit_2->setEchoMode(QLineEdit::Password);
    }
}

void LoginWidget::on_pushButton_2_clicked()
{
    this->close();
}

void LoginWidget::on_horizontalSlider_sliderMoved(int position)
{
    ui->progressBar->setValue(position);
}

void LoginWidget::timeready()
{
    i++;
    qDebug() << i;
}
