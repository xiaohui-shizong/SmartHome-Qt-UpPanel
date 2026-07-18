# SmartHome-Qt
## 项目介绍：STM32 A7 智能家居上位机
基于Qt开发的智能家居上位机可视化控制平台，通过串口对接STM32、A7嵌入式开发板，实现硬件设备监控、串口通信、MQTT云平台数据上报、AI对话交互、二维码生成等功能。
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
4. 编译依赖：Qt5 Widget、Qt SerialPort 串口模块、MQTT 客户端库、二维码生成库
## 运行截图
1. 登录界面
<img width="798" height="517" alt="屏幕截图 2026-07-18 184406" src="https://github.com/user-attachments/assets/b85bbc5a-1fb9-4ba9-bad8-2c9f0a16aebc" />

2. 智能硬件控制页
<img width="817" height="557" alt="屏幕截图 2026-07-18 184509" src="https://github.com/user-attachments/assets/62faad78-411e-4c4b-8de5-75c27d55ec81" />
界面离线状态说明：未连接 STM32 A7 实训开发板时，硬件无数据回传，环境监测温湿度、照度数值默认占位为 0；通过串口连接单片机后，软件可实时接收板载传感器采集数据，同时下发灯光、风扇、舵机等硬件控制指令。

3. MQTT物联网通信页面
<img width="818" height="556" alt="屏幕截图 2026-07-18 184558" src="https://github.com/user-attachments/assets/6ce287bf-9ba8-4ce1-8945-38f80146b6d2" />

4. AI智能对话页面
<img width="820" height="552" alt="屏幕截图 2026-07-18 184855" src="https://github.com/user-attachments/assets/0b3eac60-00b8-4be2-8d70-3371dbd39f94" />

5. 自定义虚拟键盘
<img width="850" height="198" alt="屏幕截图 2026-07-18 184937" src="https://github.com/user-attachments/assets/d27c9db1-0216-4fd5-8417-54b5007a79b6" />

## 项目后续优化与拓展方向
1. **数据可视化优化**
新增 QChart 图表模块，将 STM32 上传的温湿度、照度数据绘制实时曲线，直观展示环境变化趋势，支持历史数据本地保存与查看。

2. **AI 指令功能升级**
完善自然语言解析逻辑，支持多设备联动复合指令（如 “关灯并打开风扇”），增加指令记忆、定时任务功能，提升智能交互体验。

3. **本地数据持久化**
引入 Qt 文件读写模块，将运行日志、传感器历史数据保存至本地 txt/json 文件，程序重启后可加载历史记录。

4. **界面功能完善**
优化离线模拟逻辑，无硬件连接时可手动输入模拟温湿度数值；新增登录注册界面，区分游客 / 管理员权限。

5. **物联网功能拓展**
增加 MQTT 数据下发回执校验，补充云端历史数据查询功能；拓展蓝牙通信方式，兼容无线连接 STM32 开发板。

6. **工具模块扩充**
完善二维码工具，支持自定义文本、设备编号生成二维码；优化内置虚拟键盘，适配数字、中英文混合输入场景。

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





