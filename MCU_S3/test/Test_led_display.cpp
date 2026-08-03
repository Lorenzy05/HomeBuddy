// ============================================================
// main.cpp - HomeBuddy 演示程序 (全自动化无触摸依赖)
// ============================================================

#include <Arduino.h>
#include "led_display.h"

unsigned long lastUpdate = 0;
int demoStep = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n========================================");
    Serial.println("  🤖 HomeBuddy Display Demo Starting");
    Serial.println("========================================");
    
    // 初始化硬件 (仅 LCD + LED)
    initLED();
    initEmojiPosition();
    
    // 初始显示
    showRobotFace(0, statusMessages[0], "No-Touch Standalone");
    updateNeoPixel(0, 255, 0); // 正常启动绿灯
    
    Serial.println("✅ LCD Display OK! Running Demo...");
}

void loop() {
    // 平滑运动插值更新
    updateEmojiPosition();
    
    // 每 2.5 秒自动变换一次表情、位置与灯光颜色
    if (millis() - lastUpdate > 2500) {
        lastUpdate = millis();
        
        demoStep = (demoStep + 1) % 6;
        currentEmoji = demoStep;
        
        // 随机移动表情位置，展示灵动感
        int randomX = random(50, 190);
        int randomY = random(40, 120);
        moveEmojiTo(randomX, randomY);
        
        // 刷新界面
        showRobotFace(currentEmoji, statusMessages[currentEmoji], "Auto Animation...");
        
        // NeoPixel 随表情变色
        switch (currentEmoji) {
            case 0: updateNeoPixel(0, 255, 0); break;   // 绿色
            case 1: updateNeoPixel(255, 0, 0); break;   // 红色
            case 2: updateNeoPixel(0, 0, 255); break;   // 蓝色
            case 3: updateNeoPixel(255, 255, 0); break; // 黄色
            case 4: updateNeoPixel(255, 0, 128); break; // 粉色
            case 5: updateNeoPixel(255, 255, 255); break;// 白色
        }
    }
    
    delay(20);
}