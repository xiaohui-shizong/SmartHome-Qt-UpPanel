# SmartHome-Qt-UpPanel
## 项目介绍
基于Qt开发的智能家居上位机可视化控制平台，通过串口对接STM32 A7嵌入式开发板，实现硬件设备监控、串口通信、MQTT云平台数据上报、AI对话交互、二维码生成等功能。
## 技术栈
- 开发语言：C++ / Qt
- 硬件底层：STM32 A7单片机
- 通信方式：SerialPort串口、MQTT网络协议
## 功能说明
1. hardware页面：下发指令控制STM32板载温湿度传感器、LED、风扇、舵机、蜂鸣器等外设，实时读取硬件状态
2. aipage页面：对接大模型API，实现AI问答交互
3. mqttpage页面：连接MQTT服务器，将STM32采集的传感器数据上传云端
4. 配套自定义虚拟键盘、二维码生成工具、界面图片资源库
## 运行步骤
1. 克隆仓库：git clone https://github.com/xiaohui-shizong/SmartHome-Qt-UpPanel.git
2. linux虚拟机下使用Qt Creator打开 SmartHome.pro 工程文件
3. 串口连接STM32 A7开发板，配置MQTT服务参数后编译运行
