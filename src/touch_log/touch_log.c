#include "r_smc_entry.h"
#include "touch_log.h"
#define STX 0x02
#define ETX 0x03

uint16_t crc16_ccitt(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < len; i++) {
        crc ^= ((uint16_t)data[i] << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

DataPacket_t g_data_packet;


void send_data_frame(void)
{

	uint8_t buf[128];  // buffer
	    uint16_t idx = 0;

	    // 所有 uint16_t 数据字段（除了 start_flag, datalen, timestamp, crc16, end_flag）
	    uint16_t fields[] = {
	    		g_data_packet.slider_on,
	        g_data_packet.slider_status,
	        g_data_packet.threshold,
	        g_data_packet.Slider_TS0, g_data_packet.Slider_TS1, g_data_packet.Slider_TS2,
	        g_data_packet.Slider_TS3, g_data_packet.Slider_TS4,
	        g_data_packet.max_data_num,
	        g_data_packet.d1, g_data_packet.d2, g_data_packet.d3,
	        g_data_packet.dsum
	        // 👉 未来这里增加更多字段，只写一行
	    };

	    // 自动计算 datalen
	    g_data_packet.datalen =
	          2 // datalen 本身
	        + 4 // timestamp
	        + sizeof(fields)  // 所有 uint16_t 字段
	        + 2 // crc16
	        + 2; // end_flag

	    // 更新 start_flag, end_flag
	    g_data_packet.start_flag = 0x55AA;
	    g_data_packet.end_flag = 0xAA55;

	    // 拼 start_flag
	    buf[idx++] = g_data_packet.start_flag & 0xFF;
	    buf[idx++] = (g_data_packet.start_flag >> 8) & 0xFF;

	    // 拼 datalen
	    buf[idx++] = g_data_packet.datalen & 0xFF;
	    buf[idx++] = (g_data_packet.datalen >> 8) & 0xFF;

	    // 拼 timestamp
	    buf[idx++] = g_data_packet.timestamp & 0xFF;
	    buf[idx++] = (g_data_packet.timestamp >> 8) & 0xFF;
	    buf[idx++] = (g_data_packet.timestamp >> 16) & 0xFF;
	    buf[idx++] = (g_data_packet.timestamp >> 24) & 0xFF;

	    // 拼 fields[]
	    for (int i = 0; i < sizeof(fields)/sizeof(fields[0]); i++) {
	        buf[idx++] = fields[i] & 0xFF;
	        buf[idx++] = (fields[i] >> 8) & 0xFF;
	    }

	    // 计算 CRC16: 从 datalen 开始到 dsum
	    uint16_t crc_start_idx = 2; // datalen index
	    uint16_t crc_len = idx - crc_start_idx;  // 不含 start_flag
	    g_data_packet.crc16 = crc16_ccitt(&buf[crc_start_idx], crc_len);

	    // 拼 crc16
	    buf[idx++] = g_data_packet.crc16 & 0xFF;
	    buf[idx++] = (g_data_packet.crc16 >> 8) & 0xFF;

	    // 拼 end_flag
	    buf[idx++] = g_data_packet.end_flag & 0xFF;
	    buf[idx++] = (g_data_packet.end_flag >> 8) & 0xFF;

	    // 最终长度

    R_Config_UARTA1_Send(buf, idx);
}

