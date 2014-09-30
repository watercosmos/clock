/* clock.h */
#include "iom64.h"
#include "ina90.h"
//#include "delay.h"
#include "stdlib.h"
#include "string.h"
#include "data.h"

#define BAUDRATE	9600			//波特率
#define F_CPU		11059200		//外接晶振频率

#define DELAY_HI	0x0e			//定义延时参数, 约3ms
#define DELAY_LO	0x66
#define TWO_MS		2000			//2ms延时参数

#define RS485EN		PORTE_Bit2
#define WDI			PORTF_Bit3		//踢狗
#define LED			PORTE_Bit6		//LED灯
#define SCL			PORTF_Bit0
#define SDA			PORTF_Bit1
#define sda_in		DDRF &= 0xFD
#define sda_out		DDRF |= 0x02

struct
{
	unsigned char 	bit0:1;
	unsigned char	bit1:1;
	unsigned char	bit2:1;
	unsigned char	bit3:1;
	unsigned char	bit4:1;
	unsigned char	bit5:1;
	unsigned char	bit6:1;
	unsigned char	bit7:1;
} FlagByte;

#define BUSY		FlagByte.bit0	//网络忙标志
#define TX			FlagByte.bit1	//正在发送数据标志
#define COLLISION	FlagByte.bit2	//冲突等待标志
#define TOTX		FlagByte.bit3	//请求发送标志

#define MAX_RX_BUF_SIZE			60
#define MAX_TX_BUF_SIZE			60
#define MAX_LOGIC_SIZE			20
#define MAX_TIME_SIZE			20

unsigned char tx_buf[MAX_TX_BUF_SIZE] = {'A', 'B', 'C', 'D'};
unsigned char tx_num = 4;
unsigned char tx_pos;
unsigned char tx_last;

unsigned char rx_buf[MAX_RX_BUF_SIZE];
unsigned char rx_step;
unsigned char rx_pos;
unsigned int crc;

unsigned char dev_id = 0x04;
unsigned char net_id = 0x01;
unsigned char timestamp[2] = {0};
unsigned char dev_models[12] = {0};
unsigned char enable = 0x01;

unsigned char mac[8] = {0x00, 0x00, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
unsigned char soft_version[10] = {0};

Time now = {0x0E, 0x06, 0x03, 0x01, 0x0C, 0x1A, 0x00};
Time t_dec;

Logic logic_entry[MAX_LOGIC_SIZE];
Time_Entry time_entry[MAX_TIME_SIZE];
unsigned char logic_sum;
unsigned char time_sum;

int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
