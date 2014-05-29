/************************************************
文件：AVRUsart.h
用途：系统配置函数
************************************************/
#include "iom32.h"
#include "ina90.h"
#include "delay.h"
#include "stdlib.h"
/*********************************************/
#define TRUE  1
#define FALSE 0
//#define NULL  0
#define ON  0
#define OFF 1

#define BAUDRATE  9600//波特率
#define F_CPU     11059200 //外接晶振频率
#define LONGTIME  300


#define DownKEY1  0x3E
#define DownKEY2  0x3D
#define DownKEY3  0x3B
#define DownKEY4  0x37
#define DownKEY5  0x2F
#define DownKEY6  0x1F
#define UPKEY1	  0x3E|0x80
#define UPKEY2	  0x3D|0x80
#define UPKEY3	  0x3B|0x80
#define UPKEY4	  0x37|0x80
#define UPKEY5	  0x2F|0x80
#define UPKEY6	  0x1F|0x80

#define DELAY_HI  0x7e	//定义延时参数, 约3ms
#define DELAY_LO  0x66
#define TWO_MS    2000	//2ms延时参数
/*********************************************/
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

#define bNetworkBusy  	FlagByte.bit0//网络忙标志
#define bTransmitting  	FlagByte.bit1//正在发送数据标志
#define bCollisionWait 	FlagByte.bit2//设置冲突等待标志
#define ApplyTx			FlagByte.bit3//请求发送标志
#define btest			FlagByte.bit4//请求发送标志

#define RS485EN			PORTC_Bit1
#define WDI				PORTC_Bit0//踢狗

#define Key1Led     	PORTB_Bit0
#define Key2Led     	PORTB_Bit1
#define Key3Led     	PORTB_Bit2
#define Key4Led     	PORTB_Bit3
#define Key5Led     	PORTB_Bit4
#define Key6Led     	PORTB_Bit5


/*********************************************/
unsigned char NowKey;
unsigned char g_bReEntry;
unsigned char g_cOldKey;
unsigned int  g_cReKeyTimes; 	//重键次数
//全局变量
unsigned char TxPos;	      	//发送数据指针	
unsigned char TxNum;	  		//发送数据长度
unsigned char SendBuf[20];  	//发送数据缓冲区
unsigned char SendLast;		  	//最后发送的数据

unsigned char RevBuf[20];  		//接收数据缓冲区
unsigned char RxStep;			//接收步骤
unsigned char RxPos;			//接收数据指针
unsigned char RxNow;			//当前接收的数据	
unsigned int  Crc;

