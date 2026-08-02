import serial
import time

def read_com_port(port='COM8', baudrate=115200, timeout=1):
    """
    读取串口消息
    :param port: COM端口号，如 'COM3'
    :param baudrate: 波特率，ESP32通常用115200
    :param timeout: 读取超时时间（秒）
    """
    try:
        # 打开串口
        ser = serial.Serial(port, baudrate, timeout=timeout)
        print(f"成功打开 {port}，波特率 {baudrate}")
        print("开始监听消息... (按 Ctrl+C 退出)")
        
        while True:
            # 读取一行数据（以换行符结束）
            if ser.in_waiting > 0:  # 检查是否有数据
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    # 获取当前时间戳
                    timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
                    print(f"[{timestamp}] {line}")
            
            time.sleep(0.01)  # 避免CPU占用过高
            
    except serial.SerialException as e:
        print(f"串口错误: {e}")
        print("请检查:")
        print("1. COM端口号是否正确")
        print("2. 设备是否已连接")
        print("3. 端口是否被其他程序占用")
    except KeyboardInterrupt:
        print("\n程序已停止")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("串口已关闭")

if __name__ == "__main__":
    # 修改为你的COM端口号
    read_com_port(port='COM8', baudrate=115200)