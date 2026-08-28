#include "ultrasonic.h"

Ultrasonic::Ultrasonic(uint8_t trigPin, uint8_t echoPin)
    : _trigPin(trigPin)
    , _echoPin(echoPin)
    , _emergencyThreshold(20.0f)
    , _safeThreshold(50.0f)
    , _lastDistance(-1.0f)
    , _lastStatus(STATUS_INVALID)
{
}

void Ultrasonic::begin() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
    digitalWrite(_trigPin, LOW);
}

float Ultrasonic::getDistance() {
    // 1. 发送触发脉冲（至少10us高电平）
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(12);
    digitalWrite(_trigPin, LOW);

    // 2. 等待 Echo 上升沿（带超时）
    unsigned long timeout = micros() + 30000; // 30ms 超时
    while (digitalRead(_echoPin) == LOW) {
        if (micros() > timeout) {
            _lastDistance = -1.0f;
            _lastStatus = STATUS_INVALID;
            return _lastDistance;
        }
    }
    unsigned long startTime = micros();

    // 3. 等待 Echo 下降沿（带超时）
    timeout = micros() + 30000;
    while (digitalRead(_echoPin) == HIGH) {
        if (micros() > timeout) {
            _lastDistance = -1.0f;
            _lastStatus = STATUS_INVALID;
            return _lastDistance;
        }
    }
    unsigned long endTime = micros();

    // 4. 计算距离
    unsigned long duration = endTime - startTime;
    float distance = duration * 0.0343f / 2.0f;

    // 限制合理范围（2cm ~ 450cm）
    if (distance < 0.5f || distance > 450.0f) {
        _lastDistance = -1.0f;
        _lastStatus = STATUS_INVALID;
        return _lastDistance;
    }

    _lastDistance = distance;
    _lastStatus = calculateStatus(distance);
    return _lastDistance;
}

DistanceStatus Ultrasonic::getStatus() {
    // 如果还没有测量过，先测一次
    if (_lastStatus == STATUS_INVALID) {
        getDistance();
    }
    return _lastStatus;
}

const char* Ultrasonic::getStatusString() {
    switch (_lastStatus) {
        case STATUS_EMERGENCY: return "EMERGENCY";
        case STATUS_SAFE:      return "SAFE";
        case STATUS_FAR:       return "FAR";
        case STATUS_INVALID:   return "INVALID";
        default:               return "UNKNOWN";
    }
}

bool Ultrasonic::isEmergency() {
    return _lastStatus == STATUS_EMERGENCY;
}

bool Ultrasonic::isSafe() {
    return _lastStatus == STATUS_SAFE;
}

bool Ultrasonic::isFar() {
    return _lastStatus == STATUS_FAR;
}

void Ultrasonic::setThresholds(float emergencyThreshold, float safeThreshold) {
    _emergencyThreshold = emergencyThreshold;
    _safeThreshold = safeThreshold;
}

// ============ 私有方法 ============

DistanceStatus Ultrasonic::calculateStatus(float distance) {
    if (distance < 0) return STATUS_INVALID;
    if (distance < _emergencyThreshold) return STATUS_EMERGENCY;
    if (distance <= _safeThreshold) return STATUS_SAFE;
    return STATUS_FAR;
}