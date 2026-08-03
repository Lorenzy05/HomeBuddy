// ============================================================
// led_display.cpp - HomeBuddy 矢量图形表情版 (解决乱码问题)
// ============================================================

#include "led_display.h"

#ifndef TFT_GREY
#define TFT_GREY 0x8410
#endif

// ---------- 板载 NeoPixel ----------
#define NEOPIXEL_PIN   48
#define NEOPIXEL_COUNT 1

// ---------- 全局对象 ----------
TFT_eSPI tft = TFT_eSPI();
Adafruit_NeoPixel pixel(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// 状态管理
EmojiPosition emojiPos;
int currentEmoji = 0;

const char* statusMessages[] = {
    "Happy Mode",
    "Focus Mode", 
    "Sad Mode",
    "Robot Mode",
    "Love Mode",
    "Think Mode"
};

// ============================================================
// 初始化
// ============================================================

void initDisplay() {
    Serial.println("[Display] Initializing ILI9341...");
    tft.begin();
    tft.setRotation(0); // 0 = 竖屏 (240x320)
    
    // 显示启动画面
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(30, 130);
    tft.println("HomeBuddy");
    tft.setCursor(50, 160);
    tft.setTextSize(1);
    tft.println("Loading...");
    
    Serial.println("[Display] ILI9341 Ready.");
}

void initNeoPixel() {
    Serial.println("[LED] Initializing NeoPixel...");
    pixel.begin();
    pixel.setBrightness(30);
    pixel.fill(pixel.Color(0, 255, 0));
    pixel.show();
    Serial.println("[LED] NeoPixel Ready.");
}

void initLED() {
    initDisplay();
    initNeoPixel();
}

void initEmojiPosition() {
    emojiPos.x = 120.0;
    emojiPos.y = 80.0;
    emojiPos.targetX = 120.0;
    emojiPos.targetY = 80.0;
    emojiPos.minX = 40;
    emojiPos.maxX = 200;
    emojiPos.minY = 30;
    emojiPos.maxY = 130;
}

// ============================================================
// 位置动画逻辑
// ============================================================

void moveEmojiTo(int targetX, int targetY) {
    if (targetX < emojiPos.minX) targetX = emojiPos.minX;
    if (targetX > emojiPos.maxX) targetX = emojiPos.maxX;
    if (targetY < emojiPos.minY) targetY = emojiPos.minY;
    if (targetY > emojiPos.maxY) targetY = emojiPos.maxY;
    
    emojiPos.targetX = targetX;
    emojiPos.targetY = targetY;
}

void updateEmojiPosition() {
    emojiPos.x += (emojiPos.targetX - emojiPos.x) * 0.15;
    emojiPos.y += (emojiPos.targetY - emojiPos.y) * 0.15;
}

// ============================================================
// 🎨 矢量图形表情绘制函数 (代替 Emoji 字符)
// ============================================================

// ============================================================
// 🎨 大号矢量图形表情绘制 (放大一倍，充满上半屏)
// ============================================================

void drawVectorFace(int cx, int cy, int expression) {
    // 放大后的双眼参数
    int eyeDistance = 35; // 间距拉大
    int eyeR = 18;        // 眼睛半径放大

    switch (expression) {
        case 0: // 😊 Happy Mode: 大号萌系笑眼 + 弧形大嘴
            // 左眼 (带高光)
            tft.fillCircle(cx - eyeDistance, cy - 5, eyeR, TFT_CYAN);
            tft.fillCircle(cx - eyeDistance, cy - 5, eyeR - 4, TFT_YELLOW);
            tft.fillCircle(cx - eyeDistance + 4, cy - 9, 4, TFT_WHITE); // 眼中高光
            
            // 右眼 (带高光)
            tft.fillCircle(cx + eyeDistance, cy - 5, eyeR, TFT_CYAN);
            tft.fillCircle(cx + eyeDistance, cy - 5, eyeR - 4, TFT_YELLOW);
            tft.fillCircle(cx + eyeDistance + 4, cy - 9, 4, TFT_WHITE); // 眼中高光
            
            // 弧形大嘴
            tft.fillCircle(cx, cy + 18, 18, TFT_WHITE);
            tft.fillRect(cx - 22, cy + 5, 44, 18, TFT_BLACK); // 遮挡上半部分
            break;

        case 1: // 🎯 Focus Mode: 霸气准星大眼 + 厚实直线嘴
            // 大号准星眼
            tft.drawCircle(cx - eyeDistance, cy - 5, eyeR + 2, TFT_RED);
            tft.drawCircle(cx - eyeDistance, cy - 5, eyeR + 3, TFT_RED); // 加粗
            tft.drawFastHLine(cx - eyeDistance - 22, cy - 5, 44, TFT_RED);
            tft.drawFastVLine(cx - eyeDistance, cy - 27, 44, TFT_RED);
            
            tft.drawCircle(cx + eyeDistance, cy - 5, eyeR + 2, TFT_RED);
            tft.drawCircle(cx + eyeDistance, cy - 5, eyeR + 3, TFT_RED); // 加粗
            tft.drawFastHLine(cx + eyeDistance - 22, cy - 5, 44, TFT_RED);
            tft.drawFastVLine(cx + eyeDistance, cy - 27, 44, TFT_RED);
            
            // 加粗直线嘴
            tft.fillRect(cx - 18, cy + 22, 36, 4, TFT_WHITE);
            break;

        case 2: // 😢 Sad Mode: 大号委屈蓝眼 + 倒弧下巴
            tft.fillCircle(cx - eyeDistance, cy - 5, eyeR, TFT_BLUE);
            tft.fillCircle(cx - eyeDistance - 2, cy - 7, 5, TFT_WHITE);
            tft.fillCircle(cx + eyeDistance, cy - 5, eyeR, TFT_BLUE);
            tft.fillCircle(cx + eyeDistance - 2, cy - 7, 5, TFT_WHITE);
            
            // 倒弧形下垂嘴
            tft.drawCircle(cx, cy + 32, 16, TFT_WHITE);
            tft.drawCircle(cx, cy + 32, 17, TFT_WHITE); // 加粗
            tft.fillRect(cx - 20, cy + 32, 40, 20, TFT_BLACK);
            break;

        case 3: // 🤖 Robot Mode: 宽屏矩阵大眼 + 科技格子嘴
            // 宽屏大眼
            tft.fillRect(cx - eyeDistance - 15, cy - 15, 30, 22, TFT_GREEN);
            tft.drawRect(cx - eyeDistance - 16, cy - 16, 32, 24, TFT_WHITE);
            tft.fillRect(cx + eyeDistance - 15, cy - 15, 30, 22, TFT_GREEN);
            tft.drawRect(cx + eyeDistance - 16, cy - 16, 32, 24, TFT_WHITE);
            
            // 科技格子嘴
            tft.drawRect(cx - 25, cy + 18, 50, 14, TFT_CYAN);
            tft.drawRect(cx - 24, cy + 19, 48, 12, TFT_CYAN); // 加粗
            tft.drawFastVLine(cx - 12, cy + 18, 14, TFT_CYAN);
            tft.drawFastVLine(cx, cy + 18, 14, TFT_CYAN);
            tft.drawFastVLine(cx + 12, cy + 18, 14, TFT_CYAN);
            break;

        case 4: // ❤️ Love Mode: 超大粉色爱心眼
            // 左眼超大心形
            tft.fillCircle(cx - eyeDistance - 7, cy - 10, 11, TFT_MAGENTA);
            tft.fillCircle(cx - eyeDistance + 7, cy - 10, 11, TFT_MAGENTA);
            tft.fillTriangle(cx - eyeDistance - 17, cy - 7, cx - eyeDistance + 17, cy - 7, cx - eyeDistance, cy + 14, TFT_MAGENTA);
            
            // 右眼超大心形
            tft.fillCircle(cx + eyeDistance - 7, cy - 10, 11, TFT_MAGENTA);
            tft.fillCircle(cx + eyeDistance + 7, cy - 10, 11, TFT_MAGENTA);
            tft.fillTriangle(cx + eyeDistance - 17, cy - 7, cx + eyeDistance + 17, cy - 7, cx + eyeDistance, cy + 14, TFT_MAGENTA);
            
            // 萌萌小微笑
            tft.fillCircle(cx, cy + 20, 10, TFT_WHITE);
            tft.fillRect(cx - 12, cy + 12, 24, 10, TFT_BLACK);
            break;

        case 5: // 💡 Think Mode: 一大一小好奇大眼 + O型惊讶嘴
            // 左边超大好奇眼
            tft.drawCircle(cx - eyeDistance, cy - 5, eyeR + 4, TFT_YELLOW);
            tft.drawCircle(cx - eyeDistance, cy - 5, eyeR + 5, TFT_YELLOW);
            tft.fillCircle(cx - eyeDistance, cy - 5, 6, TFT_WHITE);
            
            // 右边小眯眼
            tft.drawCircle(cx + eyeDistance, cy - 5, eyeR - 4, TFT_YELLOW);
            tft.fillCircle(cx + eyeDistance, cy - 5, 3, TFT_WHITE);
            
            // O 型惊讶嘴
            tft.drawCircle(cx + 10, cy + 20, 8, TFT_WHITE);
            tft.drawCircle(cx + 10, cy + 20, 9, TFT_WHITE);
            break;
    }
}

// ============================================================
// 竖屏 UI 绘制 (240x320)
// ============================================================

void showRobotFace(int id, const char* info1, const char* info2) {
    if (id < 0 || id > 5) id = 0;
    currentEmoji = id;
    
    tft.fillScreen(TFT_BLACK);
    
    // ----- 1. 头部外框 (0 ~ 155 像素) -----
    tft.drawRect(5, 5, 230, 150, TFT_CYAN);
    tft.drawRect(7, 7, 226, 146, TFT_DARKCYAN);
    
    // 天线装饰
    tft.fillCircle(30, 10, 4, TFT_RED);
    tft.drawLine(30, 10, 30, 22, TFT_CYAN);
    tft.fillCircle(210, 10, 4, TFT_RED);
    tft.drawLine(210, 10, 210, 22, TFT_CYAN);
    
    // ----- 2. 调用图形绘制矢量机器人脸 -----
    drawVectorFace((int)emojiPos.x, (int)emojiPos.y, id);
    
    // ----- 3. 竖屏分割线 (Y = 160) -----
    tft.drawLine(10, 160, 230, 160, TFT_RED);
    
    // ----- 4. 下半部分状态面板 (165 ~ 315 像素) -----
    tft.fillRect(8, 165, 224, 148, TFT_DARKGREY);
    tft.drawRect(8, 165, 224, 148, TFT_CYAN);
    
    // 标题
    tft.setTextSize(1);
    tft.setTextColor(TFT_YELLOW, TFT_DARKGREY);
    tft.setCursor(20, 175);
    tft.println("=== ROBOT STATUS ===");
    
    // 状态信息
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setCursor(20, 205);
    tft.print("Mode: ");
    tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
    tft.print(info1);
    
    // 提示信息
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setCursor(20, 235);
    tft.print("Info: ");
    tft.setTextColor(TFT_CYAN, TFT_DARKGREY);
    tft.print(info2);
    
    // 坐标与 ID
    tft.setTextColor(TFT_GREY, TFT_DARKGREY);
    tft.setCursor(20, 270);
    tft.printf("Pos: (%d, %d)", (int)emojiPos.x, (int)emojiPos.y);
    
    tft.setCursor(150, 270);
    tft.printf("ID: %d/5", id);
    
    // 底部文字
    tft.setTextColor(TFT_GREY, TFT_DARKGREY);
    tft.setCursor(20, 295);
    tft.println("[HomeBuddy Graphic Demo]");
}

void updateNeoPixel(uint8_t r, uint8_t g, uint8_t b) {
    pixel.setPixelColor(0, pixel.Color(r, g, b));
    pixel.show();
}