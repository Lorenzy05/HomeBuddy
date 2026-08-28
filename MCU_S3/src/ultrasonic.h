#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

// 距离状态枚举
enum DistanceStatus {
    STATUS_EMERGENCY,   // 紧急：< 20cm
    STATUS_SAFE,        // 安全：20 ~ 50cm
    STATUS_FAR,         // 远：> 50cm
    STATUS_INVALID      // 无效读数
};

class Ultrasonic {
public:
    /**
     * @brief 构造函数
     * @param trigPin 触发引脚
     * @param echoPin 回响引脚
     */
    Ultrasonic(uint8_t trigPin, uint8_t echoPin);

    /**
     * @brief 初始化传感器
     */
    void begin();

    /**
     * @brief 获取当前距离（厘米）
     * @return 距离值，无效时返回 -1.0
     */
    float getDistance();

    /**
     * @brief 获取距离状态（枚举）
     * @return DistanceStatus 枚举值
     */
    DistanceStatus getStatus();

    /**
     * @brief 获取距离状态描述（字符串）
     * @return 如 "EMERGENCY"、"SAFE"、"FAR"、"INVALID"
     */
    const char* getStatusString();

    /**
     * @brief 判断是否紧急（距离 < 20cm）
     * @return true 表示紧急
     */
    bool isEmergency();

    /**
     * @brief 判断是否安全（20 ~ 50cm）
     * @return true 表示安全
     */
    bool isSafe();

    /**
     * @brief 判断是否远（距离 > 50cm）
     * @return true 表示远
     */
    bool isFar();

    /**
     * @brief 设置阈值（修改默认的 20cm 和 50cm）
     * @param emergencyThreshold 紧急阈值（默认20cm）
     * @param safeThreshold 安全阈值（默认50cm）
     */
    void setThresholds(float emergencyThreshold, float safeThreshold);

private:
    uint8_t _trigPin;
    uint8_t _echoPin;
    float _emergencyThreshold;  // 紧急阈值，默认 20cm
    float _safeThreshold;       // 安全阈值，默认 50cm
    float _lastDistance;        // 最近一次距离
    DistanceStatus _lastStatus; // 最近一次状态

    // 内部方法：根据距离计算状态
    DistanceStatus calculateStatus(float distance);
};

#endif // ULTRASONIC_H