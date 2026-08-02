#include "IRremote_Control.h"

// ⚠️ 重要：只在 .cpp 中包含 IRremote.h
#include <IRremote.h>

// ========== 构造函数 ==========
IRremoteControl::IRremoteControl(int rxPin) {
  _rxPin = rxPin;
  _lastCode = 0;
  _lastKey = REMOTE_NONE;
}

// ========== 初始化 ==========
void IRremoteControl::begin() {
  IrReceiver.begin(_rxPin, ENABLE_LED_FEEDBACK);
}

// ========== 检查按键 ==========
bool IRremoteControl::available() {
  if (IrReceiver.decode()) {
    // 过滤重复码
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      IrReceiver.resume();
      return false;
    }
    
    _lastCode = IrReceiver.decodedIRData.decodedRawData;
    _lastKey = codeToKey(_lastCode);
    return true;
  }
  return false;
}

// ========== 获取原始编码 ==========
uint32_t IRremoteControl::getRawCode() {
  return _lastCode;
}

// ========== 获取按键枚举 ==========
RemoteKey IRremoteControl::getKey() {
  return _lastKey;
}

// ========== 获取按键名称 ==========
const char* IRremoteControl::getKeyName() {
  switch(_lastKey) {
    case REMOTE_POWER:   return "POWER";
    case REMOTE_MENU:    return "MENU";
    case REMOTE_TEST:    return "TEST";
    case REMOTE_FORWARD: return "FORWARD";
    case REMOTE_BACK:    return "BACK";
    case REMOTE_LEFT:    return "LEFT (<)";
    case REMOTE_RIGHT:   return "RIGHT (>)";
    case REMOTE_PLAY:    return "PLAY";
    case REMOTE_0:       return "0";
    case REMOTE_1:       return "1";
    case REMOTE_2:       return "2";
    case REMOTE_3:       return "3";
    case REMOTE_4:       return "4";
    case REMOTE_5:       return "5";
    case REMOTE_6:       return "6";
    case REMOTE_7:       return "7";
    case REMOTE_8:       return "8";
    case REMOTE_9:       return "9";
    case REMOTE_C:       return "C";
    case REMOTE_START:   return "START/STOP";
    case REMOTE_UNKNOWN: return "UNKNOWN";
    default:             return "NONE";
  }
}

// ========== 继续接收 ==========
void IRremoteControl::resume() {
  IrReceiver.resume();
}

// ========== 编码转枚举 ==========
RemoteKey IRremoteControl::codeToKey(uint32_t code) {
  switch(code) {
    case KEY_POWER:   return REMOTE_POWER;
    case KEY_MENU:    return REMOTE_MENU;
    case KEY_TEST:    return REMOTE_TEST;
    case KEY_FORWARD: return REMOTE_FORWARD;
    case KEY_BACK:    return REMOTE_BACK;
    case KEY_LEFT:    return REMOTE_LEFT;
    case KEY_RIGHT:   return REMOTE_RIGHT;
    case KEY_PLAY:    return REMOTE_PLAY;
    case KEY_0:       return REMOTE_0;
    case KEY_1:       return REMOTE_1;
    case KEY_2:       return REMOTE_2;
    case KEY_3:       return REMOTE_3;
    case KEY_4:       return REMOTE_4;
    case KEY_5:       return REMOTE_5;
    case KEY_6:       return REMOTE_6;
    case KEY_7:       return REMOTE_7;
    case KEY_8:       return REMOTE_8;
    case KEY_9:       return REMOTE_9;
    case KEY_C:       return REMOTE_C;
    case KEY_START:   return REMOTE_START;
    default:          return REMOTE_UNKNOWN;
  }
}