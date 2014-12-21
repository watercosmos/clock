#ifndef CLOCK_H
#define CLOCK_H

/* clock.h */
#include "iom64.h"
#include "ina90.h"
//#include "delay.h"
#include <stdlib.h>
#include <string.h>
#include "data.h"

#define BAUDRATE  9600           //波特率
#define F_CPU     11059200       //外接晶振频率

#define DELAY_HI  0x0e           //定义延时参数, 约3ms
#define DELAY_LO  0x66
#define TWO_MS    2000           //2ms延时参数

#define RS485EN   PORTE_Bit2
#define WDI       PORTF_Bit3     //踢狗
#define LED       PORTE_Bit6     //LED灯
#define SCL       PORTF_Bit0
#define SDA       PORTF_Bit1
#define sda_in    DDRF &= 0xFD
#define sda_out   DDRF |= 0x02

struct
{
    u8 bit0:1;
    u8 bit1:1;
    u8 bit2:1;
    u8 bit3:1;
    u8 bit4:1;
    u8 bit5:1;
    u8 bit6:1;
    u8 bit7:1;
} FlagByte;

#define BUSY      FlagByte.bit0    //网络忙标志
#define TX        FlagByte.bit1    //正在发送数据标志
#define COLLISION FlagByte.bit2    //冲突等待标志
#define TOTX      FlagByte.bit3    //请求发送标志

#define MAX_RX_BUF_SIZE 60
#define MAX_TX_BUF_SIZE 60
#define MAX_LOGIC_SIZE  20
#define MAX_TIME_SIZE   20

u8 tx_buf[MAX_TX_BUF_SIZE] = {'A', 'B', 'C', 'D'};
u8 tx_num = 4;
u8 tx_pos;
u8 tx_last;

u8 rx_buf[MAX_RX_BUF_SIZE];
u8 rx_step;
u8 rx_pos;
unsigned int crc;

u8 dev_id = 0x04;
u8 net_id = 0x01;
u8 timestamp[2] = {0};
//设备型号默认为"clock"
u8 dev_models[12] = {0x63, 0x6C, 0x6F, 0x63, 0x6B, 0x00};
u8 enable = 0x01;
u8 IICSTOP;

const u8 MAC[8] = {0x00, 0x00, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
u8 soft_version[10] = {0};

Time now;

Logic logic_entry[MAX_LOGIC_SIZE];
Time_Entry time_entry[MAX_TIME_SIZE];
u8 logic_sum;
u8 time_sum;

const u8 days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//int timer;

#endif