/* clock.h */
#include "iom64.h"
#include "ina90.h"
//#include "delay.h"
#include "stdlib.h"

#define BAUDRATE	9600			//波特率
#define F_CPU		11059200		//外接晶振频率

#define DELAY_HI	0x7e			//定义延时参数, 约3ms
#define DELAY_LO	0x66
#define TWO_MS		2000			//2ms延时参数

#define RS485EN		PORTE_Bit2
#define WDI		PORTF_Bit3		//踢狗

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
}FlagByte;

#define BUSY		FlagByte.bit0	//网络忙标志
#define TX		FlagByte.bit1	//正在发送数据标志
#define COLLISION	FlagByte.bit2	//冲突等待标志
#define TOTX		FlagByte.bit3	//请求发送标志

#define MAX_RX_BUF_SIZE			40
#define MAX_TX_BUF_SIZE			40
#define MAX_LOGIC_TABLE_SIZE	        2
#define MAX_TIME_TABLE_SIZE		2
#define MAX_TASK_TABLE_SIZE		3

unsigned char tx_buf[MAX_TX_BUF_SIZE] = {'A', 'B', 'C', 'D'};
unsigned char tx_num;
unsigned char tx_pos;
unsigned char tx_last;

unsigned char rx_buf[MAX_RX_BUF_SIZE];
unsigned char rx_step;
unsigned char rx_pos;
unsigned char rx_now;
unsigned char crc;

Time now;