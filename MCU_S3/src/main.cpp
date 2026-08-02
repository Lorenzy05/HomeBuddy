#include <Arduino.h>
#include "IRremote_Control.h"

#define IR_RX_PIN 4

IRremoteControl remote(IR_RX_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("  红外遥控器测试");
  Serial.println("========================================\n");
  
  remote.begin();
  
  Serial.println("✅ 红外接收器已初始化");
  Serial.println("📡 按遥控器任意键测试...\n");
}

void loop() {
  if (remote.available()) {
    RemoteKey key = remote.getKey();
    uint32_t raw = remote.getRawCode();
    const char* name = remote.getKeyName();
    
    Serial.print("📨 按键: ");
    Serial.print(name);
    Serial.print(" | 编码: 0x");
    Serial.print(raw, HEX);
    
    if (key == REMOTE_UNKNOWN) {
      Serial.print(" ⚠️ 未识别的按键!");
    }
    Serial.println();
    
    // 根据按键执行操作
    switch(key) {
      case REMOTE_POWER:
        Serial.println("  🔌 关机");
        break;
      case REMOTE_MENU:
        Serial.println("  📋 菜单");
        break;
      case REMOTE_FORWARD:
        Serial.println("  🔼 增加");
        break;
      case REMOTE_BACK:
        Serial.println("  🔽 减少");
        break;
      case REMOTE_LEFT:
        Serial.println("  ◀️ 左移");
        break;
      case REMOTE_RIGHT:
        Serial.println("  ▶️ 右移");
        break;
      case REMOTE_PLAY:
        Serial.println("  ▶️ 播放/暂停");
        break;
      case REMOTE_0 ... REMOTE_9:
        Serial.print("  🔢 数字 ");
        Serial.println(name);
        break;
      case REMOTE_C:
        Serial.println("  ❌ 清除");
        break;
      case REMOTE_START:
        Serial.println("  🚀 开始/停止");
        break;
      default:
        break;
    }
    Serial.println();
    
    remote.resume();
  }
}