/************************************************
文件：AVRUsart.h
用途：系统配置函数
************************************************/
#include "iom32.h"
#include "ina90.h"
//#include "delay.h"
#include "stdlib.h"
/*********************************************/
#define TRUE  1
#define FALSE 0
//#define NULL  0
#define ON  0
#define OFF 1

#define BAUDRATE  9600    //波特率
#define F_CPU     11059200 //外接晶振频率
#define LONGTIME  300
#define LenghtMax 0x14

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

#define DELAY_HI  0x7e	  //定义延时参数, 约3ms   0x7e   //具体时间计算
#define DELAY_LO  0x66    //    0x66
#define TWO_MS    2000	  //2ms延时参数
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
//帧头部格式
struct {
	unsigned char DestinationDeviceId;
	unsigned char DestinationNetId;
	unsigned char SourceDeviceId;
	unsigned char SourceNetId;
        unsigned char Length;
	unsigned char ForwardNetId;
	unsigned char PacketNumber;
	unsigned char LargeOder;
	unsigned char SmallOder;
	unsigned char OrderResults;
}Header;
//确定一个按键功能的完整表项
struct
  {
    unsigned char Combine:4;
    unsigned char Repel:1;
    struct
    {
      unsigned char Model:6;
      struct
      {
        unsigned char Type;
        unsigned char SubNetID;
        unsigned char DeviceID;
        unsigned char Num;
        unsigned char Action;
      }Func[1];
    }Key[4];//占用五个bit
  }Page[1];//占用三个bit
//MAC地址
#define MAC1 0X00
#define MAC2 0X00
#define MAC3 0X00
#define MAC4 0X00
#define MAC5 0X00
#define MAC6 0X00
#define MAC7 0X00
#define MAC8 0X00
//---------------------------------------------------
#define bNetworkBusy  	FlagByte.bit0//网络忙标志
#define bTransmitting  	FlagByte.bit1//正在发送数据标志
#define bCollisionWait 	FlagByte.bit2//设置冲突等待标志
#define ApplyTx			FlagByte.bit3//请求发送标志
#define btest			FlagByte.bit4//请求发送标志

#define RS485EN			PORTC_Bit1
#define WDI			PORTC_Bit0//踢狗

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
unsigned char SendBuf[100];  	//发送数据缓冲区
unsigned char SendLast;		  	//最后发送的数据

unsigned char RevBuf[100];  		//接收数据缓冲区
unsigned char RxStep;			//接收步骤
unsigned char RxPos;			//接收数据指针
unsigned char RxNow;			//当前接收的数据	
unsigned int  Crc;
unsigned char TimeStamp[2];

unsigned char NonEmpty=0;

unsigned char NewSensorDeciveID=0x02;
unsigned char NewSensorSubNetID=0x01;
unsigned char DataLength=0;
unsigned char EnableSensor=1;
unsigned char EnableSensorSendHeart=1;
unsigned char EnableTime=0;
unsigned char HeartIntervel[2]={0};

unsigned char ResBuf[200]={0};
unsigned char pucData[200]={0};	                 //临时存取摘要信息 

