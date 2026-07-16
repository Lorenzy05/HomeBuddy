# 🍓 Raspberry Pi 部分 - 文件规划

---

## 📊 总览：需要 14 个文件

| # | 文件名 | 核心功能 | 行数估算 |
|:-:|--------|---------|:-------:|
| 1 | `main.py` | 程序入口，初始化所有模块，主循环 | ~80 |
| 2 | `config.py` | 所有配置参数（串口、摄像头、音频设备号） | ~40 |
| 3 | `requirements.txt` | Python 依赖列表 | ~10 |
| 4 | `motor_control.py` | 发送 `!MOTOR` 指令给 ESP32 | ~50 |
| 5 | `servo_control.py` | 发送 `!PAN` / `!TILT` / `!TRAY` 指令 | ~60 |
| 6 | `face_detector.py` | MediaPipe 人脸检测，返回人脸坐标 | ~70 |
| 7 | `face_follower.py` | PID 算法，计算速度指令 | ~60 |
| 8 | `voice_recognizer.py` | Vosk 语音识别，返回文字 | ~60 |
| 9 | `voice_speaker.py` | Edge-TTS 语音合成，播放音频 | ~60 |
| 10 | `llm_chat.py` | Ollama 对话，生成回复 | ~50 |
| 11 | `mode_manager.py` | 遥控/跟随/送货 三模切换逻辑 | ~70 |
| 12 | `delivery_scenario.py` | 送货场景：寻人→跟随→送达→TTS | ~70 |
| 13 | `tray_compensation.py` | 防洒补偿：读取 IMU → 调整托盘舵机 | ~50 |
| 14 | `test_full.py` | 端到端自动化测试脚本 | ~80 |
| **合计** | | | **~810 行** |

---

## 📁 每个文件的详细说明

### 1. `main.py` — 程序入口

**功能**：
- 程序启动入口
- 初始化所有模块（摄像头、串口、麦克风、音箱、LLM）
- 主循环：
  1. 检测人脸 → 如果是跟随模式，计算速度 → 发指令给 ESP32
  2. 检测语音指令 → 如果有，执行对应动作
  3. 读取串口反馈（IMU、电池电量、遥控器按键）
  4. 更新 OLED 表情和 NeoPixel 灯效
- 处理 Ctrl+C 安全退出（停止电机、释放摄像头）

**伪代码**：
```python
def main():
    init_all_modules()
    while True:
        # 1. 人脸检测
        cx, cy = face_detector.get_center()
        # 2. 模式判断
        if mode == "follow":
            left, right = face_follower.compute(cx, cy)
            motor_control.move(left, right)
        # 3. 语音指令
        text = voice_recognizer.listen()
        if text:
            process_voice_command(text)
        # 4. 读取串口反馈
        read_serial_feedback()
        # 5. 更新表情和灯效
        update_leds_and_oled()
```

---

### 2. `config.py` — 配置文件

**功能**：
- 串口配置：`SERIAL_PORT`、`BAUDRATE`
- 摄像头配置：`CAMERA_ID`、`FRAME_WIDTH`、`FRAME_HEIGHT`
- 音频配置：`MIC_DEVICE`、`SPEAKER_DEVICE`
- PID 参数：`KP`、`KI`、`KD`
- 速度限制：`MAX_SPEED`、`MIN_SPEED`
- 模式名称：`MODE_REMOTE`、`MODE_FOLLOW`、`MODE_DELIVERY`

**示例**：
```python
# config.py
SERIAL_PORT = "/dev/ttyUSB0"   # Windows: "COM3"
BAUDRATE = 115200

CAMERA_ID = 0
FRAME_WIDTH = 640
FRAME_HEIGHT = 480

MIC_DEVICE = "plughw:1,0"      # 用 arecord -l 查看
SPEAKER_DEVICE = "plughw:2,0"  # 用 aplay -l 查看

KP = 0.5
KI = 0.0
KD = 0.05

MAX_SPEED = 80
MIN_SPEED = 20
```

---

### 3. `requirements.txt` — Python 依赖

**功能**：列出所有需要的 Python 库，方便 `pip install -r requirements.txt` 一键安装

```
opencv-python
mediapipe
pyserial
vosk
edge-tts
pygame
pyaudio
ollama
```

---

### 4. `motor_control.py` — 电机控制

**功能**：
- 向 ESP32 发送 `!MOTOR#L{left}#R{right}#END` 指令
- 包含 `move(left, right)` 方法
- 包含 `stop()` 方法（调用 `move(0, 0)`）
- 速度值范围：-100 ~ 100
- 自动限制速度在 ±100 范围内

**示例**：
```python
class MotorController:
    def __init__(self, ser):
        self.ser = ser
    
    def move(self, left, right):
        left = max(-100, min(100, left))
        right = max(-100, min(100, right))
        cmd = f"!MOTOR#L{left}#R{right}#END\n"
        self.ser.write(cmd.encode())
    
    def stop(self):
        self.move(0, 0)
```

---

### 5. `servo_control.py` — 舵机云台控制

**功能**：
- 向 ESP32 发送 `!PAN#{angle}#END`（左右旋转）
- 向 ESP32 发送 `!TILT#{angle}#END`（上下俯仰）
- 向 ESP32 发送 `!TRAY#{angle}#END`（托盘倾斜补偿）
- 角度范围：PAN: -90~90，TILT: -45~45，TRAY: -15~15
- 包含 `center()` 方法（一键回中）

**示例**：
```python
class ServoController:
    def __init__(self, ser):
        self.ser = ser
    
    def pan(self, angle):
        angle = max(-90, min(90, angle))
        self.ser.write(f"!PAN#{angle}#END\n".encode())
    
    def tilt(self, angle):
        angle = max(-45, min(45, angle))
        self.ser.write(f"!TILT#{angle}#END\n".encode())
    
    def tray(self, angle):
        angle = max(-15, min(15, angle))
        self.ser.write(f"!TRAY#{angle}#END\n".encode())
    
    def center(self):
        self.pan(0)
        self.tilt(0)
```

---

### 6. `face_detector.py` — 人脸检测

**功能**：
- 初始化 MediaPipe FaceDetection
- 读取摄像头每一帧
- 检测人脸，返回人脸中心坐标 `(cx, cy)`
- 如果没有检测到人脸，返回 `(None, None)`
- 坐标范围：`cx: 0~1`（0=最左，1=最右），`cy: 0~1`（0=最上，1=最下）

**示例**：
```python
class FaceDetector:
    def __init__(self, camera_id=0):
        self.cap = cv2.VideoCapture(camera_id)
        self.detection = mp.solutions.face_detection.FaceDetection(
            min_detection_confidence=0.7
        )
    
    def get_center(self):
        ret, frame = self.cap.read()
        if not ret:
            return (None, None)
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = self.detection.process(rgb)
        if results.detections:
            bbox = results.detections[0].location_data.relative_bounding_box
            cx = bbox.xmin + bbox.width / 2
            cy = bbox.ymin + bbox.height / 2
            return (cx, cy)
        return (None, None)
```

---

### 7. `face_follower.py` — 人脸跟随算法（PID）

**功能**：
- 接收人脸中心坐标 `(cx, cy)`
- 计算误差：`error = cx - 0.5`
- PID 控制，输出左右轮速度
- 目标：让人脸保持在画面中央
- 如果人脸在中央且够大 → 前进
- 如果人脸在左侧 → 左转
- 如果人脸在右侧 → 右转

**伪代码**：
```python
class FaceFollower:
    def __init__(self, kp=0.5, ki=0.0, kd=0.05):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.last_error = 0
        self.integral = 0
    
    def compute(self, cx, cy, face_width):
        if cx is None:
            return (0, 0)
        
        error = cx - 0.5  # 目标：人脸在画面中央
        
        # PID 计算转向量
        self.integral += error
        derivative = error - self.last_error
        turn = self.kp * error + self.ki * self.integral + self.kd * derivative
        self.last_error = error
        
        # 如果人脸够大 → 前进
        if face_width > 0.15:
            base_speed = 50
        else:
            base_speed = 20  # 太远就慢慢走
        
        left = base_speed - turn * 80
        right = base_speed + turn * 80
        
        return (int(left), int(right))
```

---

### 8. `voice_recognizer.py` — 语音识别（STT）

**功能**：
- 初始化 Vosk 中文模型
- 从 USB 麦克风录音
- 实时语音识别，返回识别文字
- 支持连续识别和单次识别两种模式
- 包含唤醒词检测（如 "Hey HomeBuddy"）

**示例**：
```python
class VoiceRecognizer:
    def __init__(self, model_path="vosk-model-cn", device="plughw:1,0"):
        self.model = vosk.Model(model_path)
        self.recognizer = vosk.KaldiRecognizer(self.model, 16000)
        self.audio = pyaudio.PyAudio()
        self.stream = self.audio.open(
            format=pyaudio.paInt16,
            channels=1,
            rate=16000,
            input=True,
            input_device_index=self._get_device_index(device),
            frames_per_buffer=8000
        )
    
    def listen_once(self, timeout=5):
        # 监听 5 秒，返回识别的文字
        pass
    
    def listen_continuous(self, callback):
        # 持续监听，有识别结果就调用 callback
        pass
```

---

### 9. `voice_speaker.py` — 语音合成（TTS）

**功能**：
- 使用 Edge-TTS 生成语音（在线）
- 或使用 espeak 生成语音（离线）
- 通过 USB 音箱播放音频
- 包含 `speak(text)` 方法
- 支持打断（播放过程中停止）

**示例**：
```python
class VoiceSpeaker:
    def __init__(self, device="plughw:2,0"):
        self.device = device
        self.is_playing = False
    
    def speak(self, text):
        # 使用 edge-tts 生成音频
        import edge_tts
        import pygame
        # ... 生成并播放
        self.is_playing = True
        # ... 播放完成后设置为 False
    
    def stop(self):
        # 停止当前播放
        pygame.mixer.music.stop()
        self.is_playing = False
```

---

### 10. `llm_chat.py` — 大语言模型对话

**功能**：
- 连接本地 Ollama 服务
- 加载 Gemma 2B 或 Phi-3 模型
- 发送用户消息，返回 LLM 回复
- 维护对话历史（上下文）
- 超时处理（LLM 响应慢时不卡死）

**示例**：
```python
class ChatLLM:
    def __init__(self, model="gemma:2b", ollama_url="http://localhost:11434"):
        self.model = model
        self.url = ollama_url
        self.history = []
    
    def chat(self, user_message):
        self.history.append({"role": "user", "content": user_message})
        response = requests.post(
            f"{self.url}/api/chat",
            json={"model": self.model, "messages": self.history}
        )
        reply = response.json()["message"]["content"]
        self.history.append({"role": "assistant", "content": reply})
        return reply
    
    def clear_history(self):
        self.history = []
```

---

### 11. `mode_manager.py` — 模式管理

**功能**：
- 管理三种模式：遥控、跟随、送货
- 提供 `switch_mode(mode)` 方法
- 模式切换时执行对应的初始化和清理动作
- 当前模式状态查询
- 模式切换回调（如切换模式时播报 TTS）

**伪代码**：
```python
class ModeManager:
    MODES = ["remote", "follow", "delivery"]
    
    def __init__(self):
        self.current_mode = "remote"
    
    def switch_to(self, mode):
        if mode not in self.MODES:
            return False
        # 退出当前模式
        self._exit_mode(self.current_mode)
        # 进入新模式
        self.current_mode = mode
        self._enter_mode(mode)
        return True
    
    def _enter_mode(self, mode):
        if mode == "follow":
            # 启动人脸跟随
            pass
        elif mode == "delivery":
            # 启动送货场景
            pass
    
    def get_mode(self):
        return self.current_mode
```

---

### 12. `delivery_scenario.py` — 送货场景

**功能**：
- 触发条件：语音 "送水给我" 或 遥控器按 6
- 流程：
  1. 寻找目标人脸（原地旋转扫描）
  2. 锁定人脸 → 切换到跟随模式
  3. 跟随前进
  4. 到达目标（人脸足够大）→ 停止
  5. 播报 TTS："水送到了，主人请拿好"
  6. 回到待机模式
- 超时处理：如果 30 秒找不到人脸，播报 "没找到你"

**伪代码**：
```python
class DeliveryScenario:
    def __init__(self, face_detector, face_follower, motor, speaker):
        self.face_detector = face_detector
        self.face_follower = face_follower
        self.motor = motor
        self.speaker = speaker
    
    def run(self):
        self.speaker.speak("好的，马上送到")
        # 1. 寻找人脸（原地旋转）
        found = self._search_face(timeout=30)
        if not found:
            self.speaker.speak("没找到你")
            return
        # 2. 跟随
        self._follow_to_target()
        # 3. 到达
        self.motor.stop()
        self.speaker.speak("水送到了，主人请拿好")
    
    def _search_face(self, timeout):
        # 原地旋转扫描
        pass
    
    def _follow_to_target(self):
        # 跟随直到人脸足够大
        pass
```

---

### 13. `tray_compensation.py` — 防洒补偿

**功能**：
- 从 ESP32 读取 MPU6050 姿态数据（`!IMU#ROLL#PITCH#END`）
- 检测车身倾斜角度
- 计算托盘舵机补偿角度
- 发送 `!TRAY#{angle}#END` 指令
- 实时补偿，保持托盘水平

**伪代码**：
```python
class TrayCompensation:
    def __init__(self, servo_controller):
        self.servo = servo_controller
        self.roll = 0
        self.pitch = 0
    
    def update(self, roll, pitch):
        # 反向补偿
        comp_roll = -roll * 0.8
        comp_pitch = -pitch * 0.8
        # 限制角度
        comp_roll = max(-15, min(15, comp_roll))
        comp_pitch = max(-15, min(15, comp_pitch))
        self.servo.tray(comp_roll)
```

---

### 14. `test_full.py` — 端到端测试

**功能**：
- 一键运行所有功能测试
- 测试内容：
  1. 串口通信是否正常
  2. 电机是否能转动
  3. 舵机是否能转动
  4. 人脸检测是否正常
  5. 语音识别是否正常
  6. 语音合成是否正常
  7. 人脸跟随是否正常
  8. 送货场景是否正常
- 输出测试报告（通过/失败）

---

## 📊 文件依赖关系

```
main.py
  ├── config.py
  ├── motor_control.py
  ├── servo_control.py
  ├── face_detector.py
  │   └── face_follower.py
  ├── voice_recognizer.py
  ├── voice_speaker.py
  ├── llm_chat.py
  ├── mode_manager.py
  │   ├── delivery_scenario.py
  │   └── tray_compensation.py
  └── test_full.py (独立，用于测试)

所有文件 → 依赖 config.py
```

---

## ✅ 总结

| 类别 | 文件数量 | 说明 |
|------|:-------:|------|
| 核心文件 | 3 | `main.py`、`config.py`、`requirements.txt` |
| 控制类 | 2 | `motor_control.py`、`servo_control.py` |
| AI 视觉类 | 2 | `face_detector.py`、`face_follower.py` |
| AI 语音类 | 2 | `voice_recognizer.py`、`voice_speaker.py` |
| AI 对话类 | 1 | `llm_chat.py` |
| 场景管理 | 3 | `mode_manager.py`、`delivery_scenario.py`、`tray_compensation.py` |
| 测试 | 1 | `test_full.py` |
| **合计** | **14** | |

---