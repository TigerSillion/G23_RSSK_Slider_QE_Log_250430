
typedef struct
{
    uint16_t start_flag;
    uint16_t datalen;
    uint32_t timestamp;
    uint16_t slider_on;
    uint16_t slider_status;
    uint16_t threshold;
    uint16_t Slider_TS0;
    uint16_t Slider_TS1;
    uint16_t Slider_TS2;
    uint16_t Slider_TS3;
    uint16_t Slider_TS4;
    uint16_t max_data_num;
    uint16_t d1;
    uint16_t d2;
    uint16_t d3;
    uint16_t dsum;
    uint16_t crc16;
    uint16_t end_flag;
} DataPacket_t;

void send_data_frame(void);
