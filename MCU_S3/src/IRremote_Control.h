#ifndef IRREMOTE_CONTROL_H
#define IRREMOTE_CONTROL_H

#include <Arduino.h>

// 按键编码定义
#define KEY_POWER   0xBA45FF00
#define KEY_MENU    0xB847FF00
#define KEY_TEST    0xBB44FF00
#define KEY_FORWARD 0xBF40FF00
#define KEY_BACK    0xE619FF00
#define KEY_LEFT    0xF807FF00
#define KEY_RIGHT   0xF609FF00
#define KEY_PLAY    0xEA15FF00
#define KEY_0       0xE916FF00
#define KEY_1       0xF30CFF00
#define KEY_2       0xE718FF00
#define KEY_3       0xA15EFF00
#define KEY_4       0xF708FF00
#define KEY_5       0xE31CFF00
#define KEY_6       0xA55AFF00
#define KEY_7       0xBD42FF00
#define KEY_8       0xAD52FF00
#define KEY_9       0xB54AFF00
#define KEY_C       0xF20DFF00
#define KEY_START   0xBC43FF00

// 按键枚举
enum RemoteKey {
  REMOTE_NONE = 0,
  REMOTE_POWER,
  REMOTE_MENU,
  REMOTE_TEST,
  REMOTE_FORWARD,
  REMOTE_BACK,
  REMOTE_LEFT,
  REMOTE_RIGHT,
  REMOTE_PLAY,
  REMOTE_0,
  REMOTE_1,
  REMOTE_2,
  REMOTE_3,
  REMOTE_4,
  REMOTE_5,
  REMOTE_6,
  REMOTE_7,
  REMOTE_8,
  REMOTE_9,
  REMOTE_C,
  REMOTE_START,
  REMOTE_UNKNOWN
};

// 类声明（不包含 IRremote.h）
class IRremoteControl {
public:
  IRremoteControl(int rxPin);
  void begin();
  bool available();
  uint32_t getRawCode();
  RemoteKey getKey();
  const char* getKeyName();
  void resume();

private:
  int _rxPin;
  uint32_t _lastCode;
  RemoteKey _lastKey;
  RemoteKey codeToKey(uint32_t code);
};

#endif // IRREMOTE_CONTROL_H