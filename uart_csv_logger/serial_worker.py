# serial_worker.py
# 串口通信与帧同步、CRC校验模块，适配SRS协议，带详细中文注释
import serial
import threading
import crcmod

class SerialWorker(threading.Thread):
    """
    串口接收线程：负责串口数据读取、帧同步、CRC校验
    接收到完整有效帧后，回调frame_callback
    """
    def __init__(self, port, baudrate, frame_callback, error_callback=None):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.frame_callback = frame_callback  # 数据帧回调
        self.error_callback = error_callback  # 错误回调
        self._stop_flag = threading.Event()
        self.ser = None
        # CRC16-CCITT (0x1021, 初值0xFFFF, 无反转, 无异或)
        self.crc16 = crcmod.mkCrcFun(0x11021, rev=False, initCrc=0xFFFF, xorOut=0x0000)

    def run(self):
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=0.1)
            buf = bytearray()
            while not self._stop_flag.is_set():
                data = self.ser.read(1024)
                if data:
                    buf.extend(data)
                    while True:
                        # 帧同步：查找STX(0x02)和ETX(0x03)
                        if 0x02 in buf:
                            stx = buf.index(0x02)
                            if 0x03 in buf[stx+1:]:
                                etx = buf.index(0x03, stx+1)
                                frame = buf[stx+1:etx]  # 不含STX/ETX
                                # CRC校验
                                if len(frame) >= 4+2+2+2+2+2:  # 至少有头部
                                    crc_recv = int.from_bytes(frame[-2:], 'little')
                                    crc_calc = self.crc16(frame[:-2])
                                    if crc_recv == crc_calc:
                                        # 回调frame_callback，传递去除CRC的帧内容
                                        self.frame_callback(frame[:-2])
                                    else:
                                        if self.error_callback:
                                            self.error_callback('CRC校验失败')
                                buf = buf[etx+1:]
                            else:
                                break  # 等待更多数据
                        else:
                            buf.clear()
                            break
        except Exception as e:
            if self.error_callback:
                self.error_callback(str(e))
        finally:
            if self.ser and self.ser.is_open:
                self.ser.close()

    def stop(self):
        self._stop_flag.set()
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.join()
