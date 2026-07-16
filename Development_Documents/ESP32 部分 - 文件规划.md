## ⚙️ ESP32 部分 - 文件规划

### 📊 总览：需要 8 个文件

| # | 文件名 | 核心功能 | 行数估算 |
|:-:|--------|---------|:-------:|
| 1 | `main.cpp` | 程序入口，初始化硬件，主循环调度 | ~80 |
| 2 | `protocol.h` | 串口指令解析器（字符串 → 结构体） | ~80 |
| 3 | `motor_control.cpp` | L298N 驱动，PWM 调速 | ~60 |
| 4 | `servo_control.cpp` | SG90 舵机控制（PAN/TILT/TRAY） | ~50 |
| 5 | `ir_receiver.cpp` | HX1838 红外遥控解码 | ~70 |
| 6 | `imu_sensor.cpp` | MPU6050 姿态读取（I2C） | ~60 |
| 7 | `led_display.cpp` | OLED 表情 + NeoPixel 灯效 | ~70 |
| 8 | `priority_manager.cpp` | 控制优先级管理（遥控强制接管） | ~50 |
| **合计** | | | **~520 行** |

---

## 📁 每个文件的详细说明

### 1. `main.cpp` — 程序入口 & 主循环

**功能**：
- 初始化所有硬件（串口、GPIO、I2C、PWM）
- 主循环 `loop()` 调度所有模块
- 每 100ms 发送 IMU 数据给树莓派
- 每 500ms 发送电池电量
- 实时响应串口指令和遥控器按键
- 看门狗保护（防止卡死）

**伪代码**：
```cpp
#include "protocol.h"
#include "motor_control.h"
#include "servo_control.h"
#include "ir_receiver.h"
#include "imu_sensor.h"
#include "led_display.h"
#include "priority_manager.h"

void setup() {
    Serial.begin(115200);
    initMotor();
    initServo();
    initIR();
    initIMU();
    initLED();
    initPriorityManager();
    Serial.println("HomeBuddy ESP32 Ready");
}

void loop() {
    // 1. 处理串口指令（来自树莓派）
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        parseCommand(cmd);  // 在 protocol.h 中实现
    }
    
    // 2. 处理遥控器按键（最高优先级）
    int irKey = readIR();
    if (irKey != 0) {
        handleRemoteKey(irKey);  // 在 ir_receiver.cpp 中实现
    }
    
    // 3. 每 100ms 发送 IMU 数据
    static unsigned long lastIMU = 0;
    if (millis() - lastIMU > 100) {
        float roll = getRoll();
        float pitch = getPitch();
        Serial.printf("!IMU#ROLL%.2f#PITCH%.2f#END\n", roll, pitch);
        lastIMU = millis();
    }
    
    // 4. 每 500ms 发送电池电量
    static unsigned long lastBAT = 0;
    if (millis() - lastBAT > 500) {
        float voltage = readBattery();
        Serial.printf("!BATTERY#%.2f#END\n", voltage);
        lastBAT = millis();
    }
    
    // 5. 更新 LED 表情 + 灯效（非阻塞）
    updateLEDs();
}
```

---

### 2. `protocol.h` — 串口协议解析器

**功能**：
- 定义所有指令格式（`!MOTOR`、`!PAN`、`!EMOJI` 等）
- 解析字符串指令 → 提取参数
- 调用对应的控制函数
- 非法指令处理（返回 `!ERR`）

**示例**：
```cpp
// protocol.h
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "motor_control.h"
#include "servo_control.h"
#include "led_display.h"

void parseCommand(String cmd) {
    if (cmd.startsWith("!MOTOR#")) {
        int left = getValue(cmd, 'L');
        int right = getValue(cmd, 'R');
        setMotorSpeed(left, right);
    }
    else if (cmd.startsWith("!PAN#")) {
        int angle = getValue(cmd, 'P');
        setPan(angle);
    }
    else if (cmd.startsWith("!TILT#")) {
        int angle = getValue(cmd, 'T');
        setTilt(angle);
    }
    else if (cmd.startsWith("!TRAY#")) {
        int angle = getValue(cmd, 'Y');
        setTray(angle);
    }
    else if (cmd.startsWith("!EMOJI#")) {
        int id = getValue(cmd, 'E');
        showEmoji(id);
    }
    else if (cmd.startsWith("!RGB#")) {
        int r = getValue(cmd, 'R');
        int g = getValue(cmd, 'G');
        int b = getValue(cmd, 'B');
        setRGB(r, g, b);
    }
    else if (cmd.startsWith("!STOP#")) {
        setMotorSpeed(0, 0);
    }
    else {
        Serial.println("!ERR#UNKNOWN#END");
    }
}

// 工具函数：从字符串中提取指定键的值
// 例：!MOTOR#L50#R60#END → getValue(cmd, 'L') 返回 50
int getValue(String cmd, char key) {
    int start = cmd.indexOf(key) + 1;
    int end = cmd.indexOf('#', start);
    return cmd.substring(start, end).toInt();
}

#endif
```

---

### 3. `motor_control.cpp` — L298N 电机控制

**功能**：
- 初始化 L298N 的 4 个 GPIO（IN1~IN4）
- 接收左右速度值（-100 ~ 100）
- 正转/反转/停止
- 梯形加减速（防止急刹洒水）

**引脚分配**：
```
L298N IN1 → GPIO4
L298N IN2 → GPIO5
L298N IN3 → GPIO6
L298N IN4 → GPIO7
```

**示例**：
```cpp
// motor_control.cpp
#include "motor_control.h"
#include <Arduino.h>

#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

void initMotor() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    setMotorSpeed(0, 0);
}

void setMotorSpeed(int left, int right) {
    // 限制速度范围
    left = constrain(left, -100, 100);
    right = constrain(right, -100, 100);
    
    // 左电机
    if (left > 0) {
        analogWrite(IN1, left * 2.55);
        digitalWrite(IN2, LOW);
    } else if (left < 0) {
        digitalWrite(IN1, LOW);
        analogWrite(IN2, -left * 2.55);
    } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
    }
    
    // 右电机
    if (right > 0) {
        analogWrite(IN3, right * 2.55);
        digitalWrite(IN4, LOW);
    } else if (right < 0) {
        digitalWrite(IN3, LOW);
        analogWrite(IN4, -right * 2.55);
    } else {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
    }
}
```

---

### 4. `servo_control.cpp` — SG90 舵机控制

**功能**：
- 初始化 3 个舵机（PAN / TILT / TRAY）
- PAN：-90° ~ 90°（左右旋转）
- TILT：-45° ~ 45°（上下俯仰）
- TRAY：-15° ~ 15°（托盘倾斜补偿）
- 平滑移动（防止抖动）

**引脚分配**：
```
PAN  → GPIO8
TILT → GPIO9
TRAY → GPIO10
```

**示例**：
```cpp
// servo_control.cpp
#include "servo_control.h"
#include <Servo.h>

Servo panServo, tiltServo, trayServo;

void initServo() {
    panServo.attach(8);
    tiltServo.attach(9);
    trayServo.attach(10);
    setPan(0);
    setTilt(0);
    setTray(0);
}

void setPan(int angle) {
    angle = constrain(angle, -90, 90);
    panServo.write(angle + 90);  // 0° → 90, -90° → 0, 90° → 180
}

void setTilt(int angle) {
    angle = constrain(angle, -45, 45);
    tiltServo.write(angle + 90);
}

void setTray(int angle) {
    angle = constrain(angle, -15, 15);
    trayServo.write(angle + 90);
}

void centerServos() {
    setPan(0);
    setTilt(0);
    setTray(0);
}
```

---

### 5. `ir_receiver.cpp` — HX1838 红外遥控解码

**功能**：
- 初始化 HX1838 红外接收头
- 读取遥控器按键编码
- 按键 → 功能映射表
- 方向键强制接管（最高优先级）
- 按键防抖处理

**引脚分配**：
```
HX1838 OUT → GPIO13
```

**示例**：
```cpp
// ir_receiver.cpp
#include "ir_receiver.h"
#include <IRremote.h>

#define IR_PIN 13

IRrecv irrecv(IR_PIN);
decode_results results;

// 按键映射表（需要先解码你的遥控器）
#define KEY_UP    0xFFA25D
#define KEY_DOWN  0xFF629D
#define KEY_LEFT  0xFF22DD
#define KEY_RIGHT 0xFF02FD
#define KEY_OK    0xFFA857
#define KEY_0     0xFF6897
#define KEY_1     0xFF30CF
#define KEY_2     0xFF18E7
#define KEY_3     0xFF7A85
#define KEY_4     0xFF10EF
#define KEY_5     0xFF38C7
#define KEY_6     0xFF5AA5
#define KEY_7     0xFF42BD
#define KEY_8     0xFF4AB5
#define KEY_9     0xFF52AD
#define KEY_STAR  0xFFA857  // 你的 * 键编码
#define KEY_HASH  0xFFB04F  // 你的 # 键编码

void initIR() {
    irrecv.enableIRIn();
}

int readIR() {
    if (irrecv.decode(&results)) {
        unsigned long value = results.value;
        irrecv.resume();  // 继续接收
        return value;
    }
    return 0;
}

void handleRemoteKey(unsigned long key) {
    switch (key) {
        case KEY_UP:    handlePriorityAction(MOVE_FORWARD); break;
        case KEY_DOWN:  handlePriorityAction(MOVE_BACKWARD); break;
        case KEY_LEFT:  handlePriorityAction(MOVE_LEFT); break;
        case KEY_RIGHT: handlePriorityAction(MOVE_RIGHT); break;
        case KEY_OK:    handlePriorityAction(STOP); break;
        case KEY_0:     handlePriorityAction(MODE_SWITCH); break;
        case KEY_1:     setPan(0); setTilt(0); break;
        case KEY_2:     setPan(-15); break;
        case KEY_3:     setPan(15); break;
        case KEY_4:     setTilt(-10); break;
        case KEY_5:     setTilt(10); break;
        case KEY_6:     Serial.println("!IR#DELIVERY#END"); break;
        case KEY_8:     Serial.println("!IR#FOLLOW#END"); break;
        case KEY_STAR:  increaseSpeed(); break;
        case KEY_HASH:  decreaseSpeed(); break;
        default: break;
    }
}
```

---

### 6. `imu_sensor.cpp` — MPU6050 姿态读取

**功能**：
- 初始化 MPU6050（I2C）
- 读取 Roll（左右倾斜）和 Pitch（前后倾斜）
- 提供给主循环发送 `!IMU` 数据
- 用于防洒补偿

**引脚分配**：
```
MPU6050 SDA → GPIO21
MPU6050 SCL → GPIO22
```

**示例**：
```cpp
// imu_sensor.cpp
#include "imu_sensor.h"
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void initIMU() {
    Wire.begin(21, 22);
    mpu.initialize();
}

float getRoll() {
    // 简化版：直接从 MPU6050 读取
    // 实际需要校准和滤波
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    return atan2(ay, az) * 180.0 / PI;
}

float getPitch() {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    return atan2(-ax, sqrt(ay*ay + az*az)) * 180.0 / PI;
}
```

---

### 7. `led_display.cpp` — OLED 表情 + NeoPixel 灯效

**功能**：
- OLED SSD1306 显示表情（😊 🎯 😢 🤖）
- NeoPixel RGB 状态指示灯
- 接收 `!EMOJI` 和 `!RGB` 指令

**引脚分配**：
```
OLED SDA → GPIO21（与 MPU6050 并联）
OLED SCL → GPIO22（与 MPU6050 并联）
NeoPixel DATA → GPIO48
```

**示例**：
```cpp
// led_display.cpp
#include "led_display.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDR 0x3C
#define NEOPIXEL_PIN 48

Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
Adafruit_NeoPixel rgb(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void initLED() {
    oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    oled.clearDisplay();
    oled.display();
    
    rgb.begin();
    rgb.show();
}

void showEmoji(int id) {
    oled.clearDisplay();
    oled.setTextSize(3);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(30, 20);
    
    switch (id) {
        case 0: oled.print("😊"); break;  // 开心
        case 1: oled.print("🎯"); break;  // 锁定目标
        case 2: oled.print("😢"); break;  // 丢失
        case 3: oled.print("🤖"); break;  // 思考中
        default: oled.print("🤖"); break;
    }
    oled.display();
}

void setRGB(int r, int g, int b) {
    rgb.setPixelColor(0, rgb.Color(r, g, b));
    rgb.show();
}
```

---

### 8. `priority_manager.cpp` — 控制优先级管理

**功能**：
- 管理控制优先级：**遥控器 > 树莓派串口指令**
- 当遥控器方向键被按下时，强制接管控制权
- 停止当前所有自动动作
- 方向键松开后，可恢复之前模式

**示例**：
```cpp
// priority_manager.cpp
#include "priority_manager.h"
#include "motor_control.h"

enum PriorityMode {
    PRIORITY_REMOTE,
    PRIORITY_SERIAL
};

PriorityMode currentPriority = PRIORITY_SERIAL;

void initPriorityManager() {
    currentPriority = PRIORITY_SERIAL;
}

void handlePriorityAction(int action) {
    // 任何遥控器动作都强制切换到远程优先级
    currentPriority = PRIORITY_REMOTE;
    
    switch (action) {
        case MOVE_FORWARD: setMotorSpeed(60, 60); break;
        case MOVE_BACKWARD: setMotorSpeed(-60, -60); break;
        case MOVE_LEFT: setMotorSpeed(-50, 50); break;
        case MOVE_RIGHT: setMotorSpeed(50, -50); break;
        case STOP: setMotorSpeed(0, 0); break;
        default: break;
    }
}

void releaseRemoteControl() {
    // 在模式切换时调用，恢复串口控制
    currentPriority = PRIORITY_SERIAL;
}

bool isRemoteInControl() {
    return currentPriority == PRIORITY_REMOTE;
}
```

---

## 📊 文件依赖关系

```
main.cpp
  ├── protocol.h（指令解析）
  │   ├── motor_control.cpp（L298N 驱动）
  │   ├── servo_control.cpp（SG90 舵机）
  │   └── led_display.cpp（OLED + NeoPixel）
  ├── ir_receiver.cpp（HX1838 遥控）
  │   └── priority_manager.cpp（优先级管理）
  │       └── motor_control.cpp
  └── imu_sensor.cpp（MPU6050）
```

---

## ✅ 总结

| 类别 | 文件数量 | 说明 |
|------|:-------:|------|
| 主程序 | 1 | `main.cpp` |
| 协议解析 | 1 | `protocol.h` |
| 硬件驱动 | 4 | `motor_control.cpp`、`servo_control.cpp`、`imu_sensor.cpp`、`ir_receiver.cpp` |
| 反馈显示 | 1 | `led_display.cpp` |
| 优先级管理 | 1 | `priority_manager.cpp` |
| **合计** | **8** | |

---

## 🏷️ PlatformIO 项目结构

```
ESP32/
├── src/
│   ├── main.cpp
│   ├── motor_control.cpp
│   ├── servo_control.cpp
│   ├── ir_receiver.cpp
│   ├── imu_sensor.cpp
│   ├── led_display.cpp
│   └── priority_manager.cpp
├── include/
│   └── protocol.h
├── lib/
│   └── (第三方库：IRremote、MPU6050、Adafruit 库等)
├── platformio.ini
└── .gitignore
```

---

## 📦 `platformio.ini` 配置

```ini
[env:esp32-s3-devkitm-1]
platform = espressif32
board = esp32-s3-devkitm-1
framework = arduino
monitor_speed = 115200

lib_deps =
    arduino-libraries/IRremote
    electroniccats/MPU6050
    adafruit/Adafruit SSD1306
    adafruit/Adafruit NeoPixel
```
