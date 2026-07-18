#include "mainform.h"
#include "hardwarepage.h"
#include "mqttpage.h"
#include "aipage.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTabBar>

MainForm::MainForm(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SmartHome 智能管家系统");
    resize(820, 520);
    setStyleSheet("QWidget{background-color:#F5F5F5;}");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    tabWidget = new QTabWidget(this);

    // Tab样式 - 蓝色主题
    tabWidget->setStyleSheet(
        "QTabWidget::pane{border:1px solid #ccc; background:white;}"
        "QTabBar::tab{min-width:120px; min-height:40px; font-size:14px; padding:8px 16px;"
        "  border:1px solid #ccc; border-bottom:none; border-top-left-radius:6px; border-top-right-radius:6px;}"
        "QTabBar::tab:selected{background:#4A90D9; color:white; font-weight:bold;}"
        "QTabBar::tab:!selected{background:#E8E8E8; color:#555;}"
        "QTabBar::tab:hover:!selected{background:#D0D0D0;}"
    );

    // 创建三页
    hardwarePage = new HardwarePage();
    mqttPage = new MqttPage();
    aiPage = new AiPage();

    // 关键：将硬件页面引用传递给MQTT和AI页面，实现跨页设备控制
    mqttPage->setHardwarePage(hardwarePage);
    aiPage->setHardwarePage(hardwarePage);

    tabWidget->addTab(hardwarePage, "智能硬件控制");
    tabWidget->addTab(mqttPage, "物联网MQTT");
    tabWidget->addTab(aiPage, "AI管家");

    layout->addWidget(tabWidget);
}

MainForm::~MainForm()
{
}
