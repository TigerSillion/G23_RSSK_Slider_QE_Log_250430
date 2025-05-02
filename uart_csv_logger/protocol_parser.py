# protocol_parser.py
# 协议解析模块，支持结构化二进制协议和动态字段扩展
# 适配SRS协议说明，带详细中文注释
import struct
import json

class ProtocolParser:
    """
    协议解析器：根据外部字段配置动态解析二进制数据帧
    支持按钮、滑条等多种字段扩展，适配MCU端协议
    """
    def __init__(self, config_path):
        """
        初始化，加载字段配置
        :param config_path: 字段配置json文件路径
        """
        with open(config_path, 'r', encoding='utf-8') as f:
            self.config = json.load(f)
        self.button_fields = self.config['BUTTON_FIELDS']
        self.slider_fields = self.config['SLIDER_FIELDS']

    def parse_frame(self, frame_bytes):
        """
        解析一帧二进制数据（已去除STX/ETX，已校验CRC）
        :param frame_bytes: bytes，数据帧内容
        :return: dict，结构化数据
        """
        offset = 0
        # 解析帧头部
        timestamp, = struct.unpack_from('<I', frame_bytes, offset)
        offset += 4
        datalen, = struct.unpack_from('<H', frame_bytes, offset)
        offset += 2
        btn_count, = struct.unpack_from('<H', frame_bytes, offset)
        offset += 2
        slider_count, = struct.unpack_from('<H', frame_bytes, offset)
        offset += 2
        elem_count, = struct.unpack_from('<H', frame_bytes, offset)
        offset += 2

        result = {
            'timestamp': timestamp,
            'btn_count': btn_count,
            'slider_count': slider_count,
            'elem_count': elem_count,
            'buttons': [],
            'sliders': []
        }

        # 解析按钮数据
        for _ in range(btn_count):
            btn_data = struct.unpack_from('<' + 'H'*len(self.button_fields), frame_bytes, offset)
            offset += 2 * len(self.button_fields)
            btn_dict = dict(zip(self.button_fields, btn_data))
            result['buttons'].append(btn_dict)

        # 解析滑条数据
        for _ in range(slider_count):
            slider_data = struct.unpack_from('<' + 'H'*len(self.slider_fields), frame_bytes, offset)
            offset += 2 * len(self.slider_fields)
            slider_dict = dict(zip(self.slider_fields, slider_data))
            # 解析element数组
            elements = struct.unpack_from('<' + 'H'*elem_count, frame_bytes, offset)
            offset += 2 * elem_count
            slider_dict['elements'] = elements
            result['sliders'].append(slider_dict)

        return result

    def get_csv_header(self, btn_count, slider_count, elem_count):
        """
        动态生成CSV表头
        :param btn_count: 按钮数量
        :param slider_count: 滑条数量
        :param elem_count: 每滑条element数量
        :return: list，表头字段名
        """
        header = ['timestamp']
        for i in range(btn_count):
            for f in self.button_fields:
                header.append(f'btn{i}_{f}')
        for i in range(slider_count):
            for f in self.slider_fields:
                header.append(f'slider{i}_{f}')
            for e in range(elem_count):
                header.append(f'slider{i}_elem{e}')
        return header
