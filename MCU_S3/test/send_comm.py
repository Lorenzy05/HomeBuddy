#!/usr/bin/env python3
"""
send_comm.py - 树莓派端串口发送工具
功能：通过串口发送指令给 ESP32，并显示回显
用法：python3 send_comm.py
"""

import serial
import serial.tools.list_ports
import time
import sys
from datetime import datetime

# ==================== 配置区 ====================
# 如果你的树莓派通过 USB 连接 ESP32，串口通常是 /dev/ttyUSB0 或 /dev/ttyACM0
# 如果通过 GPIO 串口（UART），通常是 /dev/ttyS0 或 /dev/serial0
SERIAL_PORT = "COM10"  # 根据实际修改
BAUDRATE = 115200
TIMEOUT = 1  # 秒
# ===============================================

def list_serial_ports():
    """列出所有可用串口，方便你确认端口名"""
    ports = serial.tools.list_ports.comports()
    print("可用的串口列表：")
    for port in ports:
        print(f"  {port.device} - {port.description}")
    print()

def send_command(ser, cmd):
    """发送一条指令，并等待回显"""
    # 加上换行符，匹配 ESP32 的 readLine()
    ser.write((cmd + "\n").encode())

    # 等待并读取回显
    time.sleep(0.05)  # 给 ESP32 一点处理时间
    response = ""
    while ser.in_waiting > 0:
        response += ser.read(ser.in_waiting).decode(errors="ignore")

    return response.strip()

def main():
    # 先显示可用串口
    list_serial_ports()

    print(f"尝试连接 {SERIAL_PORT} ...")
    try:
        ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=TIMEOUT)
        print(f"✅ 已连接到 {SERIAL_PORT}")
    except serial.SerialException as e:
        print(f"❌ 无法打开串口: {e}")
        print("请检查：")
        print("  1. 串口线是否插好")
        print("  2. 端口名是否正确（用上面列表中的名字）")
        print("  3. 是否有权限（试试 sudo 或把自己加入 dialout 组）")
        sys.exit(1)

    # 清空缓冲区，丢掉可能存在的垃圾数据
    ser.flushInput()
    ser.flushOutput()

    print("\n输入指令发送给 ESP32，输入 'quit' 或 'exit' 退出")
    print("提示：ESP32 会回显 'Echo: xxx'")
    print("-" * 40)

    while True:
        try:
            cmd = input(">>> ").strip()
            if cmd.lower() in ["quit", "exit", "q"]:
                print("退出程序")
                break
            if not cmd:
                continue

            # 发送指令并获取回显
            response = send_command(ser, cmd)

            # 打印带时间戳的回显
            timestamp = datetime.now().strftime("%H:%M:%S")
            if response:
                print(f"[{timestamp}] ESP32 回显: {response}")
            else:
                print(f"[{timestamp}] ⚠️ 没有收到回显，检查连接")

        except KeyboardInterrupt:
            print("\n用户中断")
            break
        except serial.SerialException as e:
            print(f"❌ 串口错误: {e}")
            break

    ser.close()
    print("串口已关闭")


if __name__ == "__main__":
    main()