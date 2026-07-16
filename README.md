# 🏠 HomeBuddy - Home Service Robot · Ultimate Project Plan

---

![Imagination Product](Product.png)

## 📖 Table of Contents

1. [Project Overview](#1-project-overview)
2. [System Architecture](#2-system-architecture)
3. [Product List](#3-product-list)
4. [Wiring Scheme](#4-wiring-scheme)
5. [Communication Protocol](#5-communication-protocol)
6. [Control Methods](#6-control-methods)
7. [Function Modules](#7-function-modules)
8. [Implementation Tasks](#8-implementation-tasks)
9. [Acceptance Criteria](#9-acceptance-criteria)
10. [Risks & Mitigations](#10-risks--mitigations)
11. [Future Extensions](#11-future-extensions)

---

## 1. Project Overview

| Item | Description |
|------|-------------|
| 🏠 Project Name | **HomeBuddy** |
| 🎯 Positioning | Home intelligent service robot |
| ✨ Core Functions | 🗣️ Voice Chat · 👁️ Face Following · 🎮 Remote Control · 📦 Auto Delivery |
| 🧠 Architecture | Raspberry Pi (Brain) + ESP32-S3 (Executor) |
| 🎛️ Control Methods | Voice 🎤 + Remote 📟 (Dual Mode) |

### 🧭 Design Principles

| Principle | Description |
|-----------|-------------|
| 🔹 Separation of Concerns | Raspberry Pi handles AI, ESP32 handles hardware |
| 🔹 Wired Communication | UART serial, stable and reliable |
| 🔹 Dual Control | Voice + Remote, switch anytime |
| 🔹 Safety First | Remote can take over anytime for emergency stop |
| 🔹 Plug & Play | All audio devices are USB, no extra drivers needed |

---

## 2. System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      👤 User Interaction Layer                  │
│  ┌─────────────────────┐  ┌─────────────────────┐               │
│  │      🎤 Voice       │  │     📟 Remote       │              │
│  │     Control         │  │     Control         │               │
│  │  (Natural Language) │  │  (IR Physical Keys) │               │
│  └──────────┬──────────┘  └──────────┬──────────┘               │
│             └────────────┬────────────┘                         │
└──────────────────────────┼──────────────────────────────────────┘
                           │
┌──────────────────────────┼────────────────────────────────────┐
│                          ▼                                    │
│              🧠 Raspberry Pi 4B (Brain / 8GB)                 │
│  ┌────────────────────────────────────────────────────────┐   │
│  │  📦 Software Functions:                               │    │
│  │  🎤 Vosk STT (Speech Recognition)                     │    │
│  │  🔊 Edge-TTS (Speech Synthesis)                       │    │
│  │  🤖 Ollama (Local LLM / Gemma 2B)                     │    │
│  │  👁️ MediaPipe (Face Detection)                        │    │
│  │  🎯 Face Following Algorithm (PID Control)            │    │
│  │  🔄 Mode Management (Remote / Follow / Delivery)      │    │ 
│  └──────────────────┬─────────────────────────────────────┘   │
│                     │ 🔗 UART Serial (GPIO14/15)              │
│                     │ 📨 Command Protocol                     │
│                     │ 🔋 Power: Power Bank (USB-C)            │
└─────────────────────┼────────────────────────────────────────┘
                      │
┌─────────────────────┼────────────────────────────────────────┐
│                     ▼                                        │
│            ⚙️ ESP32-S3 (Executor / Sub-brain)                │
│  ┌────────────────────────────────────────────────────────┐  │
│  │  🔧 Hardware Control:                                │   │
│  │  🚗 L298N → DC Motors ×2 (Wheels)                    │   │
│  │  🦾 SG90 Servos ×3-4 (Pan-Tilt / Tray / Head)        │   │
│  │  🧭 MPU6050 (IMU / Anti-spill Compensation)          │   │
│  │  📟 HX1838 (IR Remote Receiver)                      │   │
│  │  🖥️ OLED SSD1306 (Face/Expression Display)           │   │
│  │  💡 NeoPixel (RGB Status LED)                        │   │
│  │  🔋 ADC (Battery Monitoring)                         │   │
│  └────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
```

### 🎵 Audio Solution

```
┌─────────────────────────────────────────────────────┐
│           Raspberry Pi USB Audio Solution           │
│  ┌───────────────┐    ┌───────────────┐             │
│  │  🎤 STJF USB  │    │  🔊 Dual      │            │
│  │  Microphone   │    │  USB Speaker  │             │
│  │  (Audio In)   │    │  (Audio Out)  │             │
│  └───────┬───────┘    └───────┬───────┘             │
│          │                    │                     │
│          └────────┬───────────┘                     │
│                   ▼                                 │
│          ┌─────────────────┐                        │
│          │  Raspberry Pi   │                        │
│          │  USB Ports      │                        │
│          │  (Plug & Play)  │                        │
│          └─────────────────┘                        │
└─────────────────────────────────────────────────────┘
```

---

## 3. Product List

### 📦 Core Electronics

| # | Component | Model | Qty |
|:-:|-----------|-------|:---:|
| 1 | Main Board (Brain) | Raspberry Pi 4B 8GB + 128GB SD | 1 |
| 2 | Main Board (Executor) | ESP32-S3 DevKit | 1 |
| 3 | Camera | USB Webcam 1080P | 1 |
| 4 | Microphone | STJF mini USB Microphone | 1 |
| 5 | Speaker | Dual Horn USB Speaker | 1 |
| 6 | Power Bank | USB-C Output (5V/3A) | 1 |
| 7 | Servo | SG90 9g | 3-4 |
| 8 | IMU Sensor | MPU6050 | 1 |
| 9 | Motor Driver | L298N | 1 |
| 10 | Display | OLED SSD1306 128x64 | 1 |
| 11 | RGB LED | NeoPixel | 1 |
| 12 | IR Receiver | HX1838 + Remote Control | 1 |

### 🧩 Structure & Mechanics

| # | Component | Spec | Qty |
|:-:|-----------|------|:---:|
| 1 | Chassis | 30×25cm Acrylic / 3D Printed | 1 |
| 2 | Rubber Wheels | Diameter 8-10cm | 2 |
| 3 | Caster Wheel | Metal bracket | 1 |
| 4 | LiPo Battery | 12V 3S 18650 (with protection) | 1 |
| 5 | Standoff + Screw Kit | M2.5/M3 various lengths | 1 set |
| 6 | Jumper Wire Set | M/M, M/F, F/F | 1 set |
| 7 | USB Extension Cable | Male to Female, 1m | 1 |
| 8 | Cable Ties + Nano Tape | Assorted | 1 pack |
| 9 | 3D Printing Filament | PLA / PETG (if self-printing) | 1 spool |

### 🧪 Spare / Optional

| # | Component | Model | Qty | Note |
|:-:|-----------|-------|:---:|------|
| 1 | I2S Microphone | INMP441 | 1 | Spare |
| 2 | I2S Audio Amp | MAX98357A | 1 | Spare |

---

## 4. Wiring Scheme

### 4.1 🌲 Raspberry Pi Side

| Port | → Connected To | Description |
|------|---------------|-------------|
| USB-A | 📷 USB Webcam | Video Input |
| USB-A | 🎤 STJF USB Microphone | Audio Input (Speech Recognition) |
| USB-A | 🔊 Dual USB Speaker | Audio Output (TTS) |
| USB-C | 🔋 Power Bank | Raspberry Pi Power |
| GPIO14 (TX) | ⚙️ ESP32 RX | Serial TX |
| GPIO15 (RX) | ⚙️ ESP32 TX | Serial RX |
| GND | ⚙️ ESP32 GND | Common Ground |

### 4.2 ⚙️ ESP32 Side

| Pin | → Connected To | Description |
|-----|---------------|-------------|
| UART RX | 🌲 Raspberry Pi TX | Receive Commands |
| UART TX | 🌲 Raspberry Pi RX | Send Feedback |
| GND | 🌲 Raspberry Pi GND | Common Ground |
| GPIO4 | 🚗 L298N IN1 | Left Motor A |
| GPIO5 | 🚗 L298N IN2 | Left Motor B |
| GPIO6 | 🚗 L298N IN3 | Right Motor A |
| GPIO7 | 🚗 L298N IN4 | Right Motor B |
| GPIO8 (PWM) | 🦾 SG90 Servo 1 | Pan (Left/Right) |
| GPIO9 (PWM) | 🦾 SG90 Servo 2 | Tilt (Up/Down) |
| GPIO10 (PWM) | 🦾 SG90 Servo 3 | Tray Compensation |
| GPIO13 | 📟 HX1838 OUT | IR Receiver |
| GPIO21 (SDA) | 🧭 MPU6050 SDA | I2C Data |
| GPIO22 (SCL) | 🧭 MPU6050 SCL | I2C Clock |
| GPIO21 (SDA) | 🖥️ OLED SDA | Parallel with MPU6050 |
| GPIO22 (SCL) | 🖥️ OLED SCL | Parallel with MPU6050 |
| GPIO48 | 💡 NeoPixel DATA | RGB Status LED |
| GPIO32 (ADC) | 🔋 Battery Voltage Divider | Battery Monitoring |
| VIN (5V) | 🔋 Power Bank (USB-A) | ESP32 Power |

### 4.3 🔋 Power System

```
🔋 Power Bank
    ├── USB-C → 🌲 Raspberry Pi (5V)
    └── USB-A → ⚙️ ESP32-S3 (5V)

🔋 12V LiPo Battery Pack (3S 18650, Independent)
    └── 🚗 Direct → L298N (Motor Driver, 12V Input)
```

| Power Source | Powers | Description |
|--------------|--------|-------------|
| 🔋 Power Bank | 🌲 Raspberry Pi + USB Peripherals + ⚙️ ESP32 | 5V output, stable |
| 🔋 12V LiPo Pack | 🚗 L298N + Motors | Independent, high current isolated from control circuit |

> ✅ **Advantage**: Raspberry Pi + ESP32 power is completely isolated from motors, preventing voltage drops during motor startup that could cause resets.

---

## 5. Communication Protocol

### 5.1 🌲 Raspberry Pi → ⚙️ ESP32 (Commands)

| Command Format | Parameters | Function |
|----------------|------------|----------|
| `!MOTOR#L{left}#R{right}#END` | -100 ~ 100 | 🚗 Differential drive control |
| `!PAN#{angle}#END` | -90 ~ 90 | 🔄 Pan servo control |
| `!TILT#{angle}#END` | -45 ~ 45 | 🔄 Tilt servo control |
| `!TRAY#{angle}#END` | -15 ~ 15 | 📦 Tray tilt compensation |
| `!EMOJI#{ID}#END` | 0=😊 1=🎯 2=😢 3=🤖 | 🖥️ Expression switch |
| `!RGB#R{red}#G{green}#B{blue}#END` | 0~255 | 💡 RGB LED color |
| `!MODE#{mode}#END` | 0=Remote 1=Follow 2=Delivery | 🔄 Mode switch |
| `!STOP#END` | — | 🛑 Emergency stop |

### 5.2 ⚙️ ESP32 → 🌲 Raspberry Pi (Feedback)

| Feedback Format | Frequency | Function |
|-----------------|:---------:|----------|
| `!IMU#ROLL{angle}#PITCH{angle}#END` | 10Hz | 🧭 IMU data |
| `!BATTERY#{voltage}#END` | 1Hz | 🔋 Motor battery voltage |
| `!IR#{key_code}#END` | Event | 📟 Remote key press |

---

## 6. Control Methods

### 🎛️ Dual Control Modes

| Mode | Trigger | Description |
|------|---------|-------------|
| 🎮 Remote Mode | IR Remote | Manual control of movement and pan-tilt |
| 👁️ Follow Mode | Voice / Remote toggle | Automatic face following |
| 📦 Delivery Mode | Voice "bring me water" | Auto find person + deliver |

### 📟 IR Remote Key Mapping (17-key Standard)

| Key | Function | Priority |
|-----|----------|:--------:|
| **Direction Pad** | | |
| ↑ | 🚗 Forward | ⭐⭐⭐ |
| ↓ | 🚗 Backward | ⭐⭐⭐ |
| ← | 🔄 Turn Left | ⭐⭐⭐ |
| → | 🔄 Turn Right | ⭐⭐⭐ |
| **Control Pad** | | |
| OK | 🛑 Emergency Stop | ⭐⭐⭐ |
| 0 | 🔄 Mode Switch | ⭐⭐ |
| 1 | 🎯 Pan-Tilt Center | ⭐⭐ |
| 2 | 🔄 Pan Left 15° | ⭐⭐ |
| 3 | 🔄 Pan Right 15° | ⭐⭐ |
| 4 | 🔄 Tilt Up 15° | ⭐⭐ |
| 5 | 🔄 Tilt Down 15° | ⭐⭐ |
| **Shortcuts** | | |
| 6 | 📦 Delivery Command | ⭐⭐ |
| 7 | 👋 Say Hello (TTS) | ⭐ |
| 8 | 👁️ Start Face Following | ⭐⭐ |
| 9 | 🏠 Return to Charging Station | ⭐ |
| * | ⏩ Increase Speed | ⭐ |
| # | ⏪ Decrease Speed | ⭐ |

### ⚡ Control Priority

```
🥇 Highest: Remote Direction Keys + OK (always active)
🥈 Medium: Remote Function Keys
🥉 Normal: Voice Commands + Auto Following
```

> 🔒 **Safety Mechanism**: Pressing a direction key → pauses all auto modes → manual takeover

---

## 7. Function Modules

| Module | Chip | Function | Priority |
|--------|------|----------|:--------:|
| 🎤 Speech Recognition | RPi | STJF USB Mic → Vosk STT | 🔴 P0 |
| 🔊 Speech Synthesis | RPi | Edge-TTS → Dual USB Speaker | 🔴 P0 |
| 🤖 Smart Chat | RPi | Ollama (Gemma 2B) | 🔴 P0 |
| 👁️ Face Detection | RPi | MediaPipe + USB Webcam | 🔴 P0 |
| 🎯 Face Following | RPi | PID Control + Speed Mapping | 🔴 P0 |
| 📟 IR Remote | ESP32 | HX1838 Signal Decoding | 🔴 P0 |
| 🚗 Motor Control | ESP32 | L298N PWM Speed Control | 🔴 P0 |
| 🦾 Servo Pan-Tilt | ESP32 | SG90 Angle Control | 🔴 P0 |
| 🔄 Mode Management | RPi | 3-mode switching logic | 🟡 P1 |
| 🧭 IMU Sensing | ESP32 | MPU6050 Reading | 🟡 P1 |
| 📦 Anti-spill | ESP32 | IMU → Servo Compensation | 🟡 P1 |
| 🖥️ Expression Display | ESP32 | OLED Dynamic Faces | 🟡 P1 |
| 💡 Status LED | ESP32 | NeoPixel RGB | 🟡 P1 |
| 🔋 Battery Monitor | ESP32 | ADC Voltage Sampling | 🟢 P2 |

**Priority Legend**:
- 🔴 P0 = Core functions, must implement
- 🟡 P1 = Important, improves experience
- 🟢 P2 = Nice-to-have, future iteration

---

## 8. Implementation Tasks

### 🔧 Hardware Build Tasks

| # | Task | Description |
|:-:|------|-------------|
| 1 | 🛞 Chassis Fabrication | Buy acrylic or 3D print chassis, size 30×25cm |
| 2 | 🔩 Motor Installation | Mount L298N + DC motors + wheels to chassis |
| 3 | 🧱 Layer Stacking | Use standoffs: Bottom = battery+L298N, Middle = ESP32, Top = Raspberry Pi |
| 4 | 📷 Camera Mount | Mount USB Webcam on servo pan-tilt |
| 5 | 🔌 Wiring | Connect all hardware per wiring scheme |
| 6 | 🔋 Power Test | Verify Power Bank powers Pi/ESP32, LiPo powers L298N |
| 7 | 📟 IR Decoding | Flash IRrecv example, read each remote key's code |

### 💻 Software Development Tasks (In Order)

| # | Module | Description |
|:-:|--------|-------------|
| 1 | ⚙️ ESP32 Serial | Implement `!MOTOR` / `!PAN` / `!TILT` command parsing |
| 2 | ⚙️ ESP32 Motor Test | Manual commands to verify L298N forward/reverse |
| 3 | ⚙️ ESP32 Servo Test | Manual commands to verify SG90 angle control |
| 4 | ⚙️ ESP32 IR Decode | Map remote keys to actions |
| 5 | ⚙️ ESP32 MPU6050 | Read IMU data and send via serial to Raspberry Pi |
| 6 | ⚙️ ESP32 OLED + NeoPixel | Expression switching + status LED effects |
| 7 | 🌲 RPi USB Audio | Test recording (mic) and playback (speaker) |
| 8 | 🌲 RPi Vosk STT | Offline speech recognition |
| 9 | 🌲 RPi Edge-TTS | Text-to-speech via speaker |
| 10 | 🌲 RPi MediaPipe | USB Webcam face detection (draw bounding box) |
| 11 | 🌲 RPi Face Following | Calculate face offset → PID → send `!MOTOR` to ESP32 |
| 12 | 🌲 RPi Ollama | Integrate local LLM chat capability |
| 13 | 🔄 Mode Manager | Implement Remote / Follow / Delivery 3-mode switching |
| 14 | 🧪 Full System Test | Voice trigger → Face follow → Delivery → TTS response |

### 🧪 Testing & Debugging Tasks

| # | Task | Description |
|:-:|------|-------------|
| 1 | 🚗 Remote Movement Test | Control forward/backward/turn with remote, check response speed |
| 2 | 🎯 Face Following Test | Person moves, robot follows automatically |
| 3 | 🗣️ Voice Interaction Test | Say "hello" → recognition → LLM reply → TTS |
| 4 | 📦 Delivery Scenario Test | Say "bring me water" → find person → deliver → voice alert |
| 5 | 🧊 Anti-spill Test | Emergency stop, observe tray stays level |
| 6 | 🔋 Battery Life Test | Continuous run, record runtime |
| 7 | 📟 IR Interference Test | Test remote in bright light, at various angles |
| 8 | 🎤 Audio Noise Test | Voice recognition with motors running |

---

## 🔩 Part 1: Hardware Platform Setup (7 Tasks)

| # | Task | Description | Status |
|:-:|------|-------------|:------:|
| 1.1 | Chassis Fabrication | 30×25cm acrylic or 3D printed, mark all mounting holes | ❌ |
| 1.2 | Motor + Wheel Installation | Mount L298N, 2x DC motors, rubber wheels, caster wheel | ❌ |
| 1.3 | Layer Stacking | Standoffs: Bottom (battery + L298N), Middle (ESP32), Top (Raspberry Pi) | ❌ |
| 1.4 | Camera + Pan-Tilt Mount | Mount USB Webcam on SG90 servo pan-tilt | ❌ |
| 1.5 | Secure All Hardware | Use cable ties + nano tape to secure all modules | ❌ |
| 1.6 | Power System Test | Power Bank → Pi + ESP32; LiPo → L298N | ❌ |
| 1.7 | Power-On Verification | All boards LED on, wheels spin freely by hand | ❌ |

---

## ⚙️ Part 2: Executor Firmware Development (8 Tasks)

| # | Task | Description | Status |
|:-:|------|-------------|:------:|
| 2.1 | Serial Communication | Raspberry Pi ↔ ESP32 UART, send and receive data | ❌ |
| 2.2 | Motor Control | `!MOTOR#L50#R50#END` → wheels spin | ❌ |
| 2.3 | Servo Pan-Tilt | `!PAN#30#END` → servo moves to specified angle | ❌ |
| 2.4 | OLED Expression | `!EMOJI#1#END` → displays 😊 🎯 😢 🤖 | ❌ |
| 2.5 | NeoPixel LED | `!RGB#R255#G0#B0#END` → lights up red | ❌ |
| 2.6 | MPU6050 Reading | Send `!IMU#ROLL#PITCH#END` every 100ms | ❌ |
| 2.7 | ADC Battery Monitor | Send `!BATTERY#3.7#END` | ❌ |
| 2.8 | Protocol Documentation | Record all commands in `docs/protocol.md` | ❌ |

---

## 🎮 Part 3: Remote Control (7 Tasks)

| # | Task | Description | Status |
|:-:|------|-------------|:------:|
| 3.1 | IR Decoding | Read and record each remote key's code | ❌ |
| 3.2 | Key Mapping | Map key codes → functions (forward/back/turn/stop/pan-tilt) | ❌ |
| 3.3 | Direction Control | ↑↓←→ control movement, OK = emergency stop | ❌ |
| 3.4 | Pan-Tilt Control | 1 = center, 2/3 = pan left/right, 4/5 = tilt up/down | ❌ |
| 3.5 | Speed Adjustment | \* = speed up, # = slow down, 3 speed levels | ❌ |
| 3.6 | Priority Mechanism | Pressing direction key → immediately pauses all auto modes | ❌ |
| 3.7 | Mode Switch | 0 key toggles: Remote ↔ Follow ↔ Delivery | ❌ |

---

## 🧠 Part 4: Raspberry Pi AI Core (8 Tasks)

| # | Task | Description | Status |
|:-:|------|-------------|:------:|
| 4.1 | USB Webcam Test | `cv2.VideoCapture(0)` displays image normally | ❌ |
| 4.2 | Face Detection | MediaPipe draws face box, outputs center coordinates (cx, cy) | ❌ |
| 4.3 | USB Microphone Test | `arecord` recording test, confirm device ID | ❌ |
| 4.4 | Speech Recognition (STT) | Vosk offline Chinese recognition | ❌ |
| 4.5 | USB Speaker Test | `aplay` playback test, confirm device ID | ❌ |
| 4.6 | Speech Synthesis (TTS) | Edge-TTS → speaker playback | ❌ |
| 4.7 | Large Language Model | Ollama + Gemma 2B local chat | ❌ |
| 4.8 | Face Following Algorithm | Face offset → PID → generate `!MOTOR` commands | ❌ |

---

## 🎯 Part 5: System Integration (9 Tasks)

| # | Task | Description | Status |
|:-:|------|-------------|:------:|
| 5.1 | Pi ↔ ESP32 Integration | Face following → serial `!MOTOR` → ESP32 executes | ❌ |
| 5.2 | Face Following Test | Person walks, robot follows | ❌ |
| 5.3 | Voice → Action | "Turn left" → Pi → ESP32 → left turn | ❌ |
| 5.4 | Remote → Pi | Press 8 on remote → triggers face following | ❌ |
| 5.5 | Delivery Scenario | "Bring me water" → find person → follow → arrive → stop → TTS | ❌ |
| 5.6 | Anti-spill Compensation | Emergency stop → MPU6050 → tray servo counter-compensation | ❌ |
| 5.7 | 3-Mode Switching | Remote ↔ Follow ↔ Delivery, smooth switching | ❌ |
| 5.8 | End-to-End Test Script | `test_full.py` runs all features automatically | ❌ |
| 5.9 | User Manual | Write `docs/user-manual.md` | ❌ |

---

## 🏁 Release: v1.0 Final (5 Tasks)

| # | Task | Description | Status |
|:-:|------|-------------|:------:|
| R.1 | Full Regression Test | Re-run all Part 1-5 functionality | ❌ |
| R.2 | Update README | Add usage instructions + photos | ❌ |
| R.3 | Update User Manual | Confirm all function descriptions are accurate | ❌ |
| R.4 | Fix All Known Bugs | Ensure no known issues remain | ❌ |
| R.5 | Code Cleanup | Remove debug code, add complete comments | ❌ |

---

## 📊 Progress Summary

| Part | Name | Tasks | Tag | Progress |
|:----:|------|:-----:|-----|:--------:|
| 1 | 🔩 Hardware Platform | 7 | `v0.1-hardware` | 0 / 7 |
| 2 | ⚙️ Executor Firmware | 8 | `v0.2-executor` | 0 / 8 |
| 3 | 🎮 Remote Control | 7 | `v0.3-remote` | 0 / 7 |
| 4 | 🧠 AI Core | 8 | `v0.4-ai` | 0 / 8 |
| 5 | 🎯 System Integration | 9 | `v1.0-beta` | 0 / 9 |
| Release | 🏁 v1.0 Final | 5 | `v1.0` | 0 / 5 |
| **Total** | | **44** | | **0 / 44** |

---

## 🔄 How to Use This List

```
1. Start with Part 1, Task 1.1
2. Complete the task
3. Change ❌ to ✅
4. Move to next task
5. When all tasks in a Part are ✅, run Acceptance Criteria check
6. If all pass → git tag v0.x-xxx
7. Move to next Part
```

---



















## 9. Acceptance Criteria

### ✅ Functional Acceptance

| Function | Acceptance Criteria | Status |
|----------|---------------------|:------:|
| 🎮 Remote Movement | Direction keys control smooth movement | ⬜ Pending |
| 🔄 Pan-Tilt Control | Camera rotates smoothly | ⬜ Pending |
| 👁️ Face Detection | Face box drawn on screen with coordinates | ⬜ Pending |
| 🎯 Face Following | Robot follows person moving around room | ⬜ Pending |
| 🎤 Speech Recognition | STJF mic accurately recognizes Chinese commands | ⬜ Pending |
| 🔊 Speech Synthesis | Dual speaker plays TTS clearly | ⬜ Pending |
| 📦 Auto Delivery | "Bring water" → find person → deliver | ⬜ Pending |
| 📟 IR Remote | All keys function correctly | ⬜ Pending |
| 🧊 Anti-spill | Tray compensation effective during sudden stops | ⬜ Pending |
| 🖥️ Expression Display | OLED switches between expressions | ⬜ Pending |
| 💡 Status LED | RGB shows different colors for different states | ⬜ Pending |
| 🔄 Mode Switching | Remote/Follow/Delivery switch smoothly | ⬜ Pending |
| 🔋 Dual Power | Power Bank + LiPo work stable together | ⬜ Pending |

### 📊 Performance Acceptance

| Metric | Requirement | Notes |
|--------|-------------|-------|
| 🔋 Battery Life | ≥ 2 hours (continuous movement) | Depends on LiPo capacity |
| 🔋 Pi Power | Power Bank independent | No motor interference |
| 📦 Max Payload | ≥ 500g | 1 bottle of water + snacks |
| 🏎️ Max Speed | 0.5 m/s | Safe indoor speed |
| ⏱️ Following Latency | < 200ms | Should not feel laggy |
| 🎤 Speech Recognition | > 80% accuracy | In quiet environment |
| 📟 Remote Range | ≥ 8 meters | Sufficient indoors |
| 🔊 Speaker Volume | Clearly audible at 5 meters | Dual speaker advantage |

---

## 10. Risks & Mitigations

| ⚠️ Risk | 📌 Impact | 🛠️ Mitigation |
|----------|-----------|----------------|
| 🔋 Power Bank insufficient output | Pi throttles/reboots | Use Power Bank that supports 5V/3A output |
| 🔋 Motor startup voltage drop | ESP32 resets | Isolate Pi/ESP32 power from motors |
| 🎤 USB Mic latency | Voice interaction lag | Lower sample rate / ALSA low-latency config |
| 🔊 USB Speaker no sound | TTS fails | Check ALSA default output device |
| 📡 Not enough USB ports | Can't plug all devices | Pi 4B has 4 USB ports, enough; or use USB HUB |
| 💧 Spilled drink | Core function fails | Ramp up/down speed + MPU6050 hardware compensation |
| 🌀 Face following jitter | Poor experience | Tune PID parameters, reduce sensitivity |
| ☀️ IR interference from strong light | Remote fails | Avoid direct sunlight on receiver |

---

## 11. Future Extensions

| Feature | Hardware Needed | Priority |
|---------|-----------------|:--------:|
| 🚧 Obstacle Avoidance | HC-SR04 Ultrasonic Sensor | ⭐⭐⭐ |
| 🗺️ Mapping & Navigation | RPLIDAR Laser Scanner | ⭐⭐ |
| 🔋 Auto Recharging | Charging Dock + IR Guidance | ⭐⭐ |
| 📹 Remote Monitoring | Camera streaming (already have) | ⭐⭐⭐ |
| ✋ Gesture Recognition | Existing Camera | ⭐⭐ |
| 📦 Object Recognition | Existing Camera + YOLO | ⭐⭐ |
| 🌍 Multi-language Support | Change Vosk model | ⭐ |

---

## 📌 Appendices

### A. Audio Device Configuration Notes

| Device | Configuration Method |
|--------|---------------------|
| 🎤 STJF USB Microphone | `arecord -l` to find device, specify in Vosk |
| 🔊 Dual USB Speaker | `aplay -l` to find device, specify in Pygame |
| 🎵 Volume Control | Use `alsamixer` to adjust USB speaker volume |

**Raspberry Pi USB Audio Test Commands**:
```bash
# Test recording (STJF Mic)
arecord -D plughw:1,0 -d 5 -f S16_LE -r 16000 test.wav

# Test playback (Dual Speaker)
aplay -D plughw:2,0 test.wav
```

### B. Reference Resources

| Resource | Link / Description |
|----------|-------------------|
| Vosk Chinese Model | https://alphacephei.com/vosk/models |
| Ollama | https://ollama.ai |
| MediaPipe | https://mediapipe.dev |
| IRremote Library | Search "IRremote" in Arduino Library Manager |
| ALSA Configuration | `/etc/asound.conf` to set default sound card |

### C. Document Version History

| Version | Date | Changes |
|---------|------|---------|
| v1.0 | 2026-07-01 | Initial release |
| v1.1 | 2026-07-01 | Integrated STJF USB mic + Dual USB speaker + Power Bank |
| v1.2 | 2026-07-01 | Procurement via buy/3D print; removed mobile App; simplified power; restructured tasks |
| v1.3 | 2026-07-04 | Product List consolidated: merged Owned + To Purchase + Spare into single list |

---

**🏠 HomeBuddy · Smarter Home, Warmer Life** ❤️
