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
## 运行截图
1. 登录界面
<img width="798" height="517" alt="屏幕截图 2026-07-18 184406" src="https://github.com/user-attachments/assets/b85bbc5a-1fb9-4ba9-bad8-2c9f0a16aebc" />

2. 智能硬件控制页
<img width="817" height="557" alt="屏幕截图 2026-07-18 184509" src="https://github.com/user-attachments/assets/62faad78-411e-4c4b-8de5-75c27d55ec81" />

3. MQTT物联网通信页面
<img width="818" height="556" alt="屏幕截图 2026-07-18 184558" src="https://github.com/user-attachments/assets/6ce287bf-9ba8-4ce1-8945-38f80146b6d2" />

4. AI智能对话页面
<img width="820" height="552" alt="屏幕截图 2026-07-18 184855" src="https://github.com/user-attachments/assets/0b3eac60-00b8-4be2-8d70-3371dbd39f94" />

5. 自定义虚拟键盘
<img width="850" height="198" alt="屏幕截图 2026-07-18 184937" src="https://github.com/user-attachments/assets/d27c9db1-0216-4fd5-8417-54b5007a79b6" />

# 附加小项目：Modern Calculator 标准科学计算器
## 项目简介
基于Qt Widget开发的多功能科学计算器，支持四则运算、开平方、平方、倒数、百分比运算，界面采用网格布局自适应排版。

## 技术布局
整体界面使用 **QGridLayout网格布局** 整齐排布全部功能按键，输入显示框垂直置顶，所有控件自适应窗口缩放，无绝对坐标布局。

## 功能点
1. 基础四则运算：+ - * / 小数点计算
2. 科学运算：开平方√、平方x²、倒数1/x、百分比%
3. 编辑功能：CE清空、C全清、退格删除输入字符

## 界面截图
<img width="396" height="632" alt="屏幕截图 2026-07-18 190659" src="https://github.com/user-attachments/assets/7df473ec-8ab0-417a-b512-b1ee99fa0b69" />





