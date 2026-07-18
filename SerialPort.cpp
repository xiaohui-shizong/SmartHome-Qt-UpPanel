#include "SerialPort.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

SerialPort::SerialPort(const std::string &device)
    : m_device(device),
      m_fd(-1)
{
}

SerialPort::~SerialPort()
{
    closePort();
}

bool SerialPort::openPort(int baudRate)
{
    if (isOpen()) {
        return true;
    }

    m_fd = ::open(m_device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_fd < 0) {
        std::perror("open serial port");
        return false;
    }

    if (!configurePort(baudRate)) {
        closePort();
        return false;
    }

    // 改为阻塞模式，和原 C 程序 fcntl(fd, F_SETFL, 0) 的效果一致。
    if (::fcntl(m_fd, F_SETFL, 0) < 0) {
        std::perror("fcntl serial port");
        closePort();
        return false;
    }

    std::printf("open success: %s\n", m_device.c_str());
    return true;
}

void SerialPort::closePort()
{
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
}

bool SerialPort::isOpen() const
{
    return m_fd >= 0;
}

ssize_t SerialPort::writeData(const void *data, std::size_t length)
{
    if (!isOpen() || data == nullptr || length == 0) {
        errno = EINVAL;
        return -1;
    }

    return ::write(m_fd, data, length);
}

bool SerialPort::writeByte(char byte)
{
    const ssize_t ret = writeData(&byte, 1);
    if (ret < 0) {
        std::perror("write serial port");
        return false;
    }
    return ret == 1;
}

bool SerialPort::configurePort(int baudRate)
{
    struct termios options;
    std::memset(&options, 0, sizeof(options));

    if (::tcgetattr(m_fd, &options) != 0) {
        std::perror("tcgetattr");
        return false;
    }

    const speed_t speed = baudRateToSpeed(baudRate);
    if (speed == 0) {
        std::fprintf(stderr, "unsupported baud rate: %d\n", baudRate);
        return false;
    }

    ::cfsetispeed(&options, speed);
    ::cfsetospeed(&options, speed);

    // 输入模式：关闭软件流控、回车换行转换、奇偶校验标记等。
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
                         INLCR | IGNCR | ICRNL | IXON);

    // 输出模式：原始输出。
    options.c_oflag &= ~OPOST;

    // 本地模式：关闭回显、规范模式、信号字符等。
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    // 控制模式：8 数据位、无校验、使能接收、本地连接。
    options.c_cflag &= ~(CSIZE | PARENB);
    options.c_cflag |= (CS8 | CREAD | CLOCAL);

#ifdef CRTSCTS
    // 关闭硬件流控。
    options.c_cflag &= ~CRTSCTS;
#endif

    // 1 个停止位。
    options.c_cflag &= ~CSTOPB;

    // 读取控制：至少读到 1 字节，最多等待 0.1 秒。
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 1;

    if (::tcsetattr(m_fd, TCSANOW, &options) != 0) {
        std::perror("tcsetattr");
        return false;
    }

    return true;
}

speed_t SerialPort::baudRateToSpeed(int baudRate) const
{
    switch (baudRate) {
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
#ifdef B230400
    case 230400: return B230400;
#endif
#ifdef B460800
    case 460800: return B460800;
#endif
#ifdef B921600
    case 921600: return B921600;
#endif
    default: return 0;
    }
}
