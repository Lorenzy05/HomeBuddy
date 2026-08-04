// ============================================================
// led_display.cpp - HomeBuddy 矢量图形表情版 (修复嘴巴方向)
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
    tft.setRotation(0);
    
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
    // 初始位置在屏幕中央安全区域
    emojiPos.x = 120.0;
    emojiPos.y = 80.0;
    emojiPos.targetX = 120.0;
    emojiPos.targetY = 80.0;
    
    // 设置安全范围
    emojiPos.minX = EMOJI_SAFE_MARGIN;
    emojiPos.maxX = TFT_WIDTH - EMOJI_SAFE_MARGIN;
    emojiPos.minY = EMOJI_SAFE_MARGIN + 10;
    emojiPos.maxY = 150 - EMOJI_SAFE_MARGIN;
}

// ============================================================
// 位置动画逻辑 - 带边界保护
// ============================================================

void moveEmojiTo(int targetX, int targetY) {
    // 强制限制在安全范围内
    int safeMinX = EMOJI_SAFE_MARGIN;
    int safeMaxX = TFT_WIDTH - EMOJI_SAFE_MARGIN;
    int safeMinY = EMOJI_SAFE_MARGIN + 10;
    int safeMaxY = 150 - EMOJI_SAFE_MARGIN;
    
    if (targetX < safeMinX) targetX = safeMinX;
    if (targetX > safeMaxX) targetX = safeMaxX;
    if (targetY < safeMinY) targetY = safeMinY;
    if (targetY > safeMaxY) targetY = safeMaxY;
    
    emojiPos.targetX = targetX;
    emojiPos.targetY = targetY;
}

void updateEmojiPosition() {
    emojiPos.x += (emojiPos.targetX - emojiPos.x) * 0.15;
    emojiPos.y += (emojiPos.targetY - emojiPos.y) * 0.15;
    
    // 确保插值过程中也不会超出边界
    int safeMinX = EMOJI_SAFE_MARGIN;
    int safeMaxX = TFT_WIDTH - EMOJI_SAFE_MARGIN;
    int safeMinY = EMOJI_SAFE_MARGIN + 10;
    int safeMaxY = 150 - EMOJI_SAFE_MARGIN;
    
    if (emojiPos.x < safeMinX) emojiPos.x = safeMinX;
    if (emojiPos.x > safeMaxX) emojiPos.x = safeMaxX;
    if (emojiPos.y < safeMinY) emojiPos.y = safeMinY;
    if (emojiPos.y > safeMaxY) emojiPos.y = safeMaxY;
}

// ============================================================
// 🎨 辅助绘制函数（表情组件）
// ============================================================

// ---------- 闪亮的大眼睛 ----------
void drawShinyEye(int x, int y, int r, uint16_t irisColor, uint16_t highlightColor) {
    // 眼白
    tft.fillCircle(x, y, r, TFT_WHITE);
    tft.drawCircle(x, y, r, 0x7BEF);
    
    // 虹膜
    tft.fillCircle(x + 3, y + 2, r * 0.6, irisColor);
    tft.fillCircle(x + 3, y + 2, r * 0.35, 0x0000); // 瞳孔
    
    // 高光（双高光更真实）
    tft.fillCircle(x - 4, y - 5, r * 0.25, highlightColor);
    tft.fillCircle(x - 6, y - 7, r * 0.12, highlightColor);
    
    // 下眼睑（用弧线）
    for (int angle = 180; angle <= 360; angle += 5) {
        float rad = angle * 3.14159 / 180;
        int px = x + (r * 0.7) * cos(rad);
        int py = y + r * 0.3 + (r * 0.5) * sin(rad);
        tft.drawPixel(px, py, TFT_DARKGREY);
    }
}

// ---------- 眉毛 ----------
void drawEyebrow(int x, int y, int length, int slope, uint16_t color) {
    int endY = y + slope * 6;
    for (int i = -2; i <= 2; i++) {
        tft.drawLine(x - length/2, y + i, x + length/2, endY + i, color);
    }
}

// ---------- 微笑嘴（弧形上扬）- 修复版 ----------
void drawHappyMouth(int x, int y, int width) {
    int r = width / 2;
    // 微笑：弧线中心下压，形成向上开口的笑脸
    for (int i = -r; i <= r; i++) {
        int dy = sqrt(r * r - i * i) * 0.6;
        for (int j = 0; j < 3; j++) {
            tft.drawPixel(x + i, y + dy + j, TFT_WHITE);
        }
    }
    // 嘴巴内部（红色）- 稍微下移一点
    tft.fillRect(x - r * 0.6, y + 4, r * 1.2, 3, TFT_RED);
}

// ---------- 锐利专注眼 ----------
void drawSharpEye(int x, int y, int r, uint16_t color) {
    tft.fillCircle(x, y, r, TFT_WHITE);
    tft.fillCircle(x + 2, y, r * 0.55, color);
    tft.fillCircle(x + 2, y, r * 0.3, 0x0000);
    tft.fillCircle(x - 3, y - 4, 3, TFT_WHITE);
    
    tft.drawLine(x - r - 6, y - 4, x - r, y, TFT_BLACK);
    tft.drawLine(x + r + 6, y - 4, x + r, y, TFT_BLACK);
}

// ---------- 直线嘴 ----------
void drawStraightMouth(int x, int y, int width, uint16_t color) {
    tft.fillRoundRect(x - width/2, y - 2, width, 4, 2, color);
}

// ---------- 含泪眼 ----------
void drawTearfulEye(int x, int y, int r) {
    tft.fillCircle(x, y, r, 0x7BEF);
    tft.fillCircle(x + 2, y + 2, r * 0.5, TFT_BLUE);
    tft.fillCircle(x + 2, y + 2, r * 0.3, 0x0000);
    tft.fillCircle(x - 3, y - 4, 3, TFT_WHITE);
    
    tft.fillCircle(x - 2, y + r + 2, 3, TFT_CYAN);
    tft.fillCircle(x + 1, y + r + 5, 2, TFT_CYAN);
}

// ---------- 悲伤嘴（倒弧下垂）- 修复版 ----------
void drawSadMouth(int x, int y, int width) {
    int r = width / 2;
    // 悲伤：弧线中心上抬，形成向下开口的哭脸
    for (int i = -r; i <= r; i++) {
        int dy = -sqrt(r * r - i * i) * 0.5;
        tft.drawPixel(x + i, y + dy, TFT_WHITE);
        tft.drawPixel(x + i, y + dy + 1, TFT_WHITE);
    }
}

// ---------- 泪滴 ----------
void drawTeardrop(int x, int y) {
    tft.fillCircle(x, y, 4, TFT_CYAN);
    tft.fillCircle(x, y + 2, 3, 0x7BEF);
    tft.drawLine(x - 2, y + 4, x - 1, y + 8, TFT_CYAN);
    tft.drawLine(x + 2, y + 4, x + 1, y + 8, TFT_CYAN);
}

// ---------- 机器人眼 ----------
void drawRobotEye(int x, int y, int size) {
    tft.drawRoundRect(x - size, y - size * 0.7, size * 2, size * 1.4, 4, TFT_CYAN);
    tft.fillRoundRect(x - size + 2, y - size * 0.7 + 2, size * 2 - 4, size * 1.4 - 4, 3, TFT_BLACK);
    
    int ledSize = 4;
    for (int row = -1; row <= 1; row++) {
        for (int col = -2; col <= 2; col++) {
            if (row == 0 || col == 0) {
                tft.fillCircle(x + col * 6, y + row * 6, ledSize, TFT_GREEN);
                tft.fillCircle(x + col * 6, y + row * 6, ledSize - 1, 0x07E0);
            }
        }
    }
}

// ---------- 天线 ----------
void drawAntenna(int x, int y) {
    tft.drawLine(x, y, x, y - 15, TFT_CYAN);
    tft.fillCircle(x, y - 15, 4, TFT_RED);
    tft.fillCircle(x, y - 15, 2, 0xFC00);
}

// ---------- 数码嘴 ----------
void drawDigitalMouth(int x, int y, int width, int height) {
    tft.drawRect(x - width/2, y - height/2, width, height, TFT_CYAN);
    for (int i = -width/2 + 4; i < width/2; i += 8) {
        tft.drawFastVLine(x + i, y - height/2 + 2, height - 4, TFT_CYAN);
    }
    for (int j = -height/2 + 4; j < height/2; j += 8) {
        tft.drawFastHLine(x - width/2 + 2, y + j, width - 4, TFT_CYAN);
    }
    tft.fillCircle(x, y, 4, 0x07E0);
}

// ---------- 爱心眼 ----------
void drawHeartEye(int x, int y, int size) {
    int hs = size / 2;
    tft.fillCircle(x - hs, y, hs, 0xF81F);
    tft.fillCircle(x + hs, y, hs, 0xF81F);
    tft.fillTriangle(x - size, y + hs, x + size, y + hs, x, y + size * 1.2, 0xF81F);
    tft.fillCircle(x - hs - 2, y - 2, 3, 0xFCFF);
}

// ---------- 害羞小嘴（小微笑）- 修复版 ----------
void drawShyMouth(int x, int y, int size) {
    // 轻微微笑：中心略微下压
    for (int i = -size; i <= size; i++) {
        int dy = sqrt(size * size - i * i) * 0.3;
        if (abs(i) < size * 0.7) {
            tft.drawPixel(x + i, y + dy, 0xF81F);
            tft.drawPixel(x + i, y + dy + 1, 0xF81F);
        }
    }
}

// ---------- 小爱心（装饰） ----------
void drawMiniHeart(int x, int y, int size) {
    tft.fillCircle(x - size/2, y, size/2, 0xF81F);
    tft.fillCircle(x + size/2, y, size/2, 0xF81F);
    tft.fillTriangle(x - size, y + size/2, x + size, y + size/2, x, y + size * 1.2, 0xF81F);
}

// ---------- 好奇眼 ----------
void drawCuriousEye(int x, int y, int r) {
    tft.fillCircle(x, y, r, TFT_WHITE);
    tft.fillCircle(x + 4, y + 2, r * 0.7, TFT_YELLOW);
    tft.fillCircle(x + 4, y + 2, r * 0.5, 0x0000);
    tft.fillCircle(x - 5, y - 6, r * 0.3, TFT_WHITE);
}

// ---------- 眯眼 ----------
void drawSquintEye(int x, int y, int r) {
    tft.fillRoundRect(x - r, y - 3, r * 2, 6, 3, TFT_WHITE);
    tft.fillRoundRect(x - r + 2, y - 1, r * 2 - 4, 3, 2, 0x0000);
    tft.fillCircle(x - r/2, y - 2, 2, TFT_WHITE);
}

// ---------- O型嘴 ----------
void drawOMouth(int x, int y, int r) {
    tft.drawCircle(x, y, r, TFT_WHITE);
    tft.fillCircle(x, y, r * 0.5, 0x0000);
}

// ---------- 思考气泡 ----------
void drawThoughtBubble(int x, int y) {
    tft.fillCircle(x, y, 12, TFT_WHITE);
    tft.drawCircle(x, y, 12, TFT_DARKGREY);
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.setCursor(x - 6, y - 8);
    tft.print("?");
    tft.fillCircle(x - 18, y + 15, 6, TFT_WHITE);
    tft.drawCircle(x - 18, y + 15, 6, TFT_DARKGREY);
    tft.fillCircle(x - 28, y + 22, 4, TFT_WHITE);
    tft.drawCircle(x - 28, y + 22, 4, TFT_DARKGREY);
}

// ============================================================
// 🎨 主绘制函数 - 升级版表情（修复嘴巴方向）
// ============================================================

void drawVectorFace(int cx, int cy, int expression) {
    // ===== 边界检查和修正 =====
    int minBound = 55;
    int maxBoundX = TFT_WIDTH - 55;
    int maxBoundY = 150 - 55;
    
    if (cx < minBound) cx = minBound;
    if (cx > maxBoundX) cx = maxBoundX;
    if (cy < minBound + 10) cy = minBound + 10;
    if (cy > maxBoundY) cy = maxBoundY;
    
    int eyeDist = 32;
    int eyeSize = 16;
    int faceRadius = 55;
    
    // 脸部底色
    tft.fillCircle(cx, cy + 5, faceRadius, TFT_BLACK);
    tft.drawCircle(cx, cy + 5, faceRadius, 0x7BEF);
    
    // 脸部高光
    tft.fillCircle(cx - 20, cy - 15, 30, 0x18C3);
    
    switch (expression) {
        case 0: // 😊 HAPPY - 上扬微笑
            drawShinyEye(cx - eyeDist, cy - 8, eyeSize, TFT_YELLOW, TFT_WHITE);
            drawShinyEye(cx + eyeDist, cy - 8, eyeSize, TFT_YELLOW, TFT_WHITE);
            drawEyebrow(cx - eyeDist, cy - 28, 22, 1, TFT_WHITE);
            drawEyebrow(cx + eyeDist, cy - 28, 22, 1, TFT_WHITE);
            drawHappyMouth(cx, cy + 18, 28);  // 上扬微笑 ✅
            tft.fillCircle(cx - 40, cy + 18, 8, 0xF81F);
            tft.fillCircle(cx + 40, cy + 18, 8, 0xF81F);
            break;
            
        case 1: // 🎯 FOCUS - 直线嘴
            drawSharpEye(cx - eyeDist, cy - 8, eyeSize, TFT_CYAN);
            drawSharpEye(cx + eyeDist, cy - 8, eyeSize, TFT_CYAN);
            drawEyebrow(cx - eyeDist, cy - 28, 20, 0, TFT_WHITE);
            drawEyebrow(cx + eyeDist, cy - 28, 20, 0, TFT_WHITE);
            drawStraightMouth(cx, cy + 25, 30, TFT_WHITE);
            break;
            
        case 2: // 😢 SAD - 下垂悲伤嘴
            drawTearfulEye(cx - eyeDist, cy - 8, eyeSize);
            drawTearfulEye(cx + eyeDist, cy - 8, eyeSize);
            drawEyebrow(cx - eyeDist, cy - 28, 20, -1, TFT_WHITE);
            drawEyebrow(cx + eyeDist, cy - 28, 20, -1, TFT_WHITE);
            drawSadMouth(cx, cy + 25, 20);  // 下垂嘴 ✅
            drawTeardrop(cx - 40, cy + 10);
            drawTeardrop(cx + 40, cy + 5);
            break;
            
        case 3: // 🤖 ROBOT
            drawRobotEye(cx - eyeDist, cy - 8, 20);
            drawRobotEye(cx + eyeDist, cy - 8, 20);
            drawAntenna(cx - 20, cy - faceRadius);
            drawAntenna(cx + 20, cy - faceRadius);
            drawDigitalMouth(cx, cy + 22, 34, 10);
            for (int i = 0; i < 6; i++) {
                int angle = i * 60;
                int dx = (faceRadius - 8) * cos(angle * 3.14159 / 180);
                int dy = (faceRadius - 8) * sin(angle * 3.14159 / 180);
                tft.fillCircle(cx + dx, cy + 5 + dy, 3, TFT_CYAN);
            }
            break;
            
        case 4: // ❤️ LOVE - 小微笑
            drawHeartEye(cx - eyeDist, cy - 6, 14);
            drawHeartEye(cx + eyeDist, cy - 6, 14);
            drawEyebrow(cx - eyeDist, cy - 28, 20, 1, 0xF81F);
            drawEyebrow(cx + eyeDist, cy - 28, 20, 1, 0xF81F);
            drawShyMouth(cx, cy + 22, 16);  // 害羞小微笑 ✅
            tft.fillCircle(cx - 42, cy + 20, 10, 0xF81F);
            tft.fillCircle(cx + 42, cy + 20, 10, 0xF81F);
            for (int i = -2; i <= 2; i++) {
                drawMiniHeart(cx + i * 35, cy - 45 + abs(i) * 10, 5);
            }
            break;
            
        case 5: // 💡 THINK - O型嘴
            drawCuriousEye(cx - eyeDist, cy - 8, eyeSize + 4);
            drawSquintEye(cx + eyeDist, cy - 8, eyeSize - 4);
            drawEyebrow(cx - eyeDist, cy - 30, 18, 2, TFT_WHITE);
            drawEyebrow(cx + eyeDist, cy - 26, 18, -1, TFT_WHITE);
            drawOMouth(cx + 5, cy + 20, 14);
            drawThoughtBubble(cx + 55, cy - 30);
            break;
    }
    
    // 通用高光
    tft.fillCircle(cx - 15, cy - 20, 3, 0x7BEF);
}

// ============================================================
// 竖屏 UI 绘制
// ============================================================

void showRobotFace(int id, const char* info1, const char* info2) {
    if (id < 0 || id > 5) id = 0;
    currentEmoji = id;
    
    tft.fillScreen(TFT_BLACK);
    
    // 头部外框
    tft.drawRect(5, 5, 230, 150, TFT_CYAN);
    tft.drawRect(7, 7, 226, 146, TFT_DARKCYAN);
    
    // 天线装饰
    tft.fillCircle(30, 10, 4, TFT_RED);
    tft.drawLine(30, 10, 30, 22, TFT_CYAN);
    tft.fillCircle(210, 10, 4, TFT_RED);
    tft.drawLine(210, 10, 210, 22, TFT_CYAN);
    
    // 绘制表情
    drawVectorFace((int)emojiPos.x, (int)emojiPos.y, id);
    
    // 分割线
    tft.drawLine(10, 160, 230, 160, TFT_RED);
    
    // 状态面板
    tft.fillRect(8, 165, 224, 148, TFT_DARKGREY);
    tft.drawRect(8, 165, 224, 148, TFT_CYAN);
    
    tft.setTextSize(1);
    tft.setTextColor(TFT_YELLOW, TFT_DARKGREY);
    tft.setCursor(20, 175);
    tft.println("=== ROBOT STATUS ===");
    
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setCursor(20, 205);
    tft.print("Mode: ");
    tft.setTextColor(TFT_GREEN, TFT_DARKGREY);
    tft.print(info1);
    
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setCursor(20, 235);
    tft.print("Info: ");
    tft.setTextColor(TFT_CYAN, TFT_DARKGREY);
    tft.print(info2);
    
    tft.setTextColor(TFT_GREY, TFT_DARKGREY);
    tft.setCursor(20, 270);
    tft.printf("Pos: (%d, %d)", (int)emojiPos.x, (int)emojiPos.y);
    
    tft.setCursor(150, 270);
    tft.printf("ID: %d/6", id + 1);
    
    tft.setTextColor(TFT_GREY, TFT_DARKGREY);
    tft.setCursor(20, 295);
    tft.println("[HomeBuddy Graphic Demo]");
}

void updateNeoPixel(uint8_t r, uint8_t g, uint8_t b) {
    pixel.setPixelColor(0, pixel.Color(r, g, b));
    pixel.show();
}