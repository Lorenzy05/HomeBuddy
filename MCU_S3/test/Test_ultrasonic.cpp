#include <Arduino.h>
#include "ultrasonic.h"

// 定义引脚
#define TRIG_PIN  5
#define ECHO_PIN  6

// 创建超声波对象
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

void setup() {
    Serial.begin(115200);
    ultrasonic.begin();
    Serial.println("超声波传感器已启动");
    
    // 可选：自定义阈值
    // ultrasonic.setThresholds(15.0f, 45.0f); // 改成 15cm 紧急，45cm 安全
}

void loop() {
    // 读取距离
    float dist = ultrasonic.getDistance();
    
    if (dist > 0) {
        // 方法1：用枚举判断
        switch (ultrasonic.getStatus()) {
            case STATUS_EMERGENCY:
                Serial.printf("🚨 紧急！%.2f cm，立即刹车！\n", dist);
                // 这里写你的刹车逻辑
                break;
            case STATUS_SAFE:
                Serial.printf("✅ 安全！%.2f cm，正常行驶\n", dist);
                break;
            case STATUS_FAR:
                Serial.printf("📡 较远！%.2f cm，可加速\n", dist);
                break;
            default:
                break;
        }

        // 方法2：直接用布尔函数判断（更简洁）
        if (ultrasonic.isEmergency()) {
            // 紧急处理
            Serial.println("⚠️ 紧急制动！");
        } else if (ultrasonic.isSafe()) {
            // 安全处理
        } else if (ultrasonic.isFar()) {
            // 远距离处理
        }
    } else {
        Serial.println("⚠️ 读数无效，请检查传感器");
    }

    delay(300);
}