#include "uart_comm.h"

UartComm::UartComm() {}

void UartComm::init(unsigned long baud) {
    Serial.begin(baud);
    // 使用延时代替 while (!Serial)
    delay(2000);  // 等待 USB 稳定
    Serial.setTimeout(10);
}

void UartComm::send(const String& msg) {
    Serial.print(msg);
}

void UartComm::sendLine(const String& msg) {
    Serial.println(msg);
}

bool UartComm::available() {
    return Serial.available() > 0;
}

String UartComm::readLine() {
    if (!available()) return "";
    return Serial.readStringUntil('\n');
}

String UartComm::readAll() {
    if (!available()) return "";
    return Serial.readString();
}

void UartComm::flush() {
    while (Serial.available()) {
        Serial.read();
    }
}