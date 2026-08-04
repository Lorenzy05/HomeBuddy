// ============================================================
// led_display.h - HomeBuddy 纯显示与 LED 模块 (无 Touch)
// ============================================================

#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

// ---------- 硬件配置 (竖屏 240 x 320) ----------
#define TFT_WIDTH   240
#define TFT_HEIGHT  320
#define NEOPIXEL_COUNT 1

// ---------- 表情安全边界 ----------
#define EMOJI_SAFE_MARGIN 60

// ---------- 表情位置结构体 ----------
struct EmojiPosition {
    float x;          
    float y;          
    float targetX;    
    float targetY;    
    int minX;         
    int maxX;
    int minY;
    int maxY;
};

// ---------- 外部对象引用 ----------
extern TFT_eSPI tft;
extern Adafruit_NeoPixel pixel;
extern EmojiPosition emojiPos;
extern int currentEmoji;

// ---------- 表情状态文字 ----------
extern const char* statusMessages[];

// ---------- 初始化函数 ----------
void initLED();
void initDisplay();
void initNeoPixel();
void initEmojiPosition();

// ---------- 核心显示函数 ----------
void showRobotFace(int id, const char* info1 = "Status: Happy", 
                   const char* info2 = "Demo Mode");

// ---------- 表情移动控制 ----------
void moveEmojiTo(int targetX, int targetY);
void updateEmojiPosition();

// ---------- RGB LED控制 ----------
void updateNeoPixel(uint8_t r, uint8_t g, uint8_t b);

// ---------- 表情绘制辅助函数 ----------
void drawVectorFace(int cx, int cy, int expression);
void drawShinyEye(int x, int y, int r, uint16_t irisColor, uint16_t highlightColor);
void drawEyebrow(int x, int y, int length, int slope, uint16_t color);
void drawHappyMouth(int x, int y, int width);
void drawSharpEye(int x, int y, int r, uint16_t color);
void drawStraightMouth(int x, int y, int width, uint16_t color);
void drawTearfulEye(int x, int y, int r);
void drawSadMouth(int x, int y, int width);
void drawTeardrop(int x, int y);
void drawRobotEye(int x, int y, int size);
void drawAntenna(int x, int y);
void drawDigitalMouth(int x, int y, int width, int height);
void drawHeartEye(int x, int y, int size);
void drawShyMouth(int x, int y, int size);
void drawMiniHeart(int x, int y, int size);
void drawCuriousEye(int x, int y, int r);
void drawSquintEye(int x, int y, int r);
void drawOMouth(int x, int y, int r);
void drawThoughtBubble(int x, int y);

#endif