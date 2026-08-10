#include "uart_comm.h"

UartComm uart;

void setup() {
    uart.init(115200);
    uart.sendLine("ESP32 ready!");
    uart.sendLine("USB Serial Communication Started");
}

void loop() {
    //Serial.println("Hi");
    if (uart.available()) {
        String msg = uart.readLine();
        if (msg.length() > 0) {
            msg.trim();
            uart.sendLine("Echo: " + msg);
        }
    }
    delay(10);
}