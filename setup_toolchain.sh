#!/bin/bash
# ============================================
# SmartHome - Qt Creator 工具链配置助手
# 适用于 STM32MP1 / FSMP1A 开发板 Ubuntu 20.04 环境
# ============================================

set -e

SDK_PATH="${SDK_PATH:-}"
CROSS_COMPILE="${CROSS_COMPILE:-arm-ostl-linux-gnueabi-}"

echo "========================================"
echo "  SmartHome 工具链配置检查"
echo "========================================"

# 检查本地 Qt 环境
if command -v qmake &>/dev/null; then
    echo "[OK] 系统 qmake: $(qmake -query QT_VERSION) ($(command -v qmake))"
else
    echo "[WARN] 未找到系统 qmake，建议安装: sudo apt install qt5-default qtcreator"
fi

# 检查交叉编译工具链
if [ -n "${CROSS_COMPILE}" ] && command -v ${CROSS_COMPILE}g++ &>/dev/null; then
    echo "[OK] 交叉编译器: $(command -v ${CROSS_COMPILE}g++)"
else
    echo "[WARN] 未找到交叉编译器 ${CROSS_COMPILE}g++"
fi

# 提示 SDK 路径
if [ -n "${SDK_PATH}" ] && [ -d "${SDK_PATH}" ]; then
    echo "[OK] SDK 路径: ${SDK_PATH}"
else
    echo "[INFO] 未设置 SDK_PATH，交叉编译前请 source SDK 环境脚本，例如："
    echo "  source /opt/st/stm32mp1-openstlinux/5.15-yocto-kirkstone/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi"
fi

echo ""
echo "Qt Creator 配置步骤:"
echo "1. 工具 -> 选项 -> 设备 -> 添加 -> 通用 Linux 设备 ->"
echo "   名称: FSMP1A, 主机: 192.168.7.1, 用户名: root (或你的用户名)"
echo "2. 工具 -> 选项 -> 编译器 -> 添加 -> GCC ->"
echo "   C++ 编译器: $(command -v ${CROSS_COMPILE}g++ 2>/dev/null || echo '你的 SDK g++ 路径')"
echo "3. 工具 -> 选项 -> Qt 版本 -> 添加 -> 选择 SDK 中的 qmake"
echo "4. 工具 -> 选项 -> 套件(Kit) -> 添加 -> 选择上述编译器、Qt 版本、设备"
echo "5. 项目 -> 管理套件 -> 选择 ARM 套件 -> 构建 -> 选择 Release"
echo "6. 构建 -> 运行 -> 部署到 192.168.7.1"
