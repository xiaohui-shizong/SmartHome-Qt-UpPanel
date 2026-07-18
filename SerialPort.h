#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <string>
#include <cstddef>
#include <sys/types.h>
#include <termios.h>

class SerialPort
{
public:
    explicit SerialPort(const std::string &device = "/dev/ttyUSB0");
    ~SerialPort();

    SerialPort(const SerialPort &) = delete;
    SerialPort &operator=(const SerialPort &) = delete;

    bool openPort(int baudRate = 115200);
    void closePort();

    bool isOpen() const;
    ssize_t writeData(const void *data, std::size_t length);
    bool writeByte(char byte);

private:
    bool configurePort(int baudRate);
    speed_t baudRateToSpeed(int baudRate) const;

private:
    std::string m_device;
    int m_fd;
};

#endif // SERIAL_PORT_H
