# uart_csv_logger.py
# 主界面，集成协议解析、串口通信、数据可视化、CSV保存等功能
# 适配SRS需求，详细中文注释
import sys
import os
import csv
from PyQt5 import QtWidgets, QtCore
import pyqtgraph as pg
import serial.tools.list_ports
from protocol_parser import ProtocolParser
from serial_worker import SerialWorker

class MainWindow(QtWidgets.QMainWindow):
    """
    主窗口类，负责UI、数据流转、可视化
    """
    def __init__(self):
        super().__init__()
        self.setWindowTitle('UART结构化数据监控工具')
        self.resize(1200, 800)
        # 协议字段配置文件路径
        self.config_path = os.path.join(os.path.dirname(__file__), 'configs', 'fields_config.json')
        self.parser = ProtocolParser(self.config_path)
        self.serial_worker = None
        self.data_buffer = []  # 存储所有帧数据
        self.csv_header = []   # 当前表头
        self.frame_count = 0
        self.crc_error_count = 0
        self.init_ui()
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_plot)

    def init_ui(self):
        # 顶部：串口配置
        top_layout = QtWidgets.QHBoxLayout()
        self.port_cb = QtWidgets.QComboBox()
        self.refresh_ports()
        self.baud_cb = QtWidgets.QComboBox()
        self.baud_cb.addItems(['115200', '57600', '38400', '19200', '9600'])
        self.baud_cb.setCurrentText('115200')
        self.connect_btn = QtWidgets.QPushButton('连接串口')
        self.connect_btn.clicked.connect(self.connect_serial)
        self.status_label = QtWidgets.QLabel('未连接')
        self.start_btn = QtWidgets.QPushButton('开始接收')
        self.start_btn.setEnabled(False)
        self.start_btn.clicked.connect(self.start_receiving)
        self.stop_btn = QtWidgets.QPushButton('停止接收')
        self.stop_btn.setEnabled(False)
        self.stop_btn.clicked.connect(self.stop_receiving)
        self.save_btn = QtWidgets.QPushButton('保存CSV')
        self.save_btn.setEnabled(False)
        self.save_btn.clicked.connect(self.save_csv)
        top_layout.addWidget(QtWidgets.QLabel('串口:'))
        top_layout.addWidget(self.port_cb)
        top_layout.addWidget(QtWidgets.QLabel('波特率:'))
        top_layout.addWidget(self.baud_cb)
        top_layout.addWidget(self.connect_btn)
        top_layout.addWidget(self.status_label)
        top_layout.addWidget(self.start_btn)
        top_layout.addWidget(self.stop_btn)
        top_layout.addWidget(self.save_btn)

        # 中部：曲线区
        self.plot_widget = pg.PlotWidget(title="实时曲线")
        self.plot_curves = {}  # 字段名->曲线对象
        self.plot_widget.addLegend()

        # 下部：表格区
        self.table = QtWidgets.QTableWidget()
        self.table.setColumnCount(1)
        self.table.setRowCount(1)
        self.table.setHorizontalHeaderLabels(['数据'])
        self.table.horizontalHeader().setSectionResizeMode(QtWidgets.QHeaderView.Stretch)

        # 主布局
        main_layout = QtWidgets.QVBoxLayout()
        main_layout.addLayout(top_layout)
        main_layout.addWidget(self.plot_widget, stretch=2)
        main_layout.addWidget(self.table, stretch=1)
        central = QtWidgets.QWidget()
        central.setLayout(main_layout)
        self.setCentralWidget(central)

    def refresh_ports(self):
        """刷新串口列表"""
        self.port_cb.clear()
        ports = serial.tools.list_ports.comports()
        for p in ports:
            self.port_cb.addItem(p.device)

    def connect_serial(self):
        """连接串口，启动串口接收线程"""
        port = self.port_cb.currentText()
        baud = int(self.baud_cb.currentText())
        if not port:
            self.status_label.setText('未选择串口')
            return
        # 启动串口线程
        self.serial_worker = SerialWorker(port, baud, self.on_frame_received, self.on_serial_error)
        self.serial_worker.start()
        self.status_label.setText('已连接')
        self.start_btn.setEnabled(True)

    def start_receiving(self):
        """开始接收数据"""
        self.data_buffer.clear()
        self.frame_count = 0
        self.crc_error_count = 0
        self.csv_header = []
        self.table.setRowCount(0)
        self.plot_curves.clear()
        self.plot_widget.clear()
        self.timer.start(100)  # 10Hz刷新
        self.start_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)
        self.save_btn.setEnabled(False)

    def stop_receiving(self):
        """停止接收数据"""
        self.timer.stop()
        self.start_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self.save_btn.setEnabled(True)

    def save_csv(self):
        """保存数据为CSV文件"""
        if not self.data_buffer:
            QtWidgets.QMessageBox.warning(self, '提示', '没有数据可保存!')
            return
        path, _ = QtWidgets.QFileDialog.getSaveFileName(self, '保存CSV', '', 'CSV Files (*.csv)')
        if not path:
            return
        with open(path, 'w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerow(self.csv_header)
            for row in self.data_buffer:
                writer.writerow(row)
        QtWidgets.QMessageBox.information(self, '提示', '保存成功!')

    def on_frame_received(self, frame_bytes):
        """串口线程回调：收到一帧有效数据，解析并显示"""
        parsed = self.parser.parse_frame(frame_bytes)
        # 动态生成表头
        if not self.csv_header:
            self.csv_header = self.parser.get_csv_header(parsed['btn_count'], parsed['slider_count'], parsed['elem_count'])
            self.table.setColumnCount(len(self.csv_header))
            self.table.setHorizontalHeaderLabels(self.csv_header)
        # 展开数据为一行
        row = [parsed['timestamp']]
        for btn in parsed['buttons']:
            row.extend(btn.values())
        for slider in parsed['sliders']:
            row.extend([slider[k] for k in self.parser.slider_fields])
            row.extend(slider['elements'])
        self.data_buffer.append(row)
        self.frame_count += 1
        # 状态栏
        self.status_label.setText(f"已连接 | 帧数: {self.frame_count} | CRC错误: {self.crc_error_count}")
        # 更新表格
        self.table.insertRow(0)
        for col, val in enumerate(row):
            self.table.setItem(0, col, QtWidgets.QTableWidgetItem(str(val)))
        # 实时曲线数据
        for idx, name in enumerate(self.csv_header[1:]):  # 跳过timestamp
            if name not in self.plot_curves:
                curve = self.plot_widget.plot(name=name, pen=pg.intColor(idx))
                self.plot_curves[name] = curve
            ydata = [r[idx+1] for r in self.data_buffer[-1000:]]  # 最多1000帧
            xdata = [r[0] for r in self.data_buffer[-1000:]]
            self.plot_curves[name].setData(x=xdata, y=ydata)

    def update_plot(self):
        """定时刷新曲线"""
        if self.data_buffer and self.csv_header:
            for idx, name in enumerate(self.csv_header[1:]):
                if name in self.plot_curves:
                    ydata = [r[idx+1] for r in self.data_buffer[-1000:]]
                    xdata = [r[0] for r in self.data_buffer[-1000:]]
                    self.plot_curves[name].setData(x=xdata, y=ydata)

    def on_serial_error(self, msg):
        """串口线程错误回调"""
        self.crc_error_count += 1
        self.status_label.setText(f"串口错误: {msg} | CRC错误: {self.crc_error_count}")

    def closeEvent(self, event):
        if self.serial_worker:
            self.serial_worker.stop()
        event.accept()

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    win = MainWindow()
    win.show()
    sys.exit(app.exec_())
