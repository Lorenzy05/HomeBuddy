#ifndef UART_COMM_H
#define UART_COMM_H

#include <Arduino.h>

class UartComm {
public:
    UartComm();
    void init(unsigned long baud = 115200);
    void send(const String& msg);
    void sendLine(const String& msg);
    bool available();
    String readLine();
    String readAll();
    void flush();
};

#endif