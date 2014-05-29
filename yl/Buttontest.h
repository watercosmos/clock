/************************************************
�ļ���AVRUsart.h
��;��ϵͳ���ú���
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

#define BAUDRATE  9600    //������
#define F_CPU     11059200 //��Ӿ���Ƶ��
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

#define DELAY_HI  0x7e	  //������ʱ����, Լ3ms   0x7e   //����ʱ�����
#define DELAY_LO  0x66    //    0x66
#define TWO_MS    2000	  //2ms��ʱ����
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
//֡ͷ����ʽ
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
//ȷ��һ���������ܵ���������
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
    }Key[4];//ռ�����bit
  }Page[1];//ռ������bit
//MAC��ַ
#define MAC1 0X00
#define MAC2 0X00
#define MAC3 0X00
#define MAC4 0X00
#define MAC5 0X00
#define MAC6 0X00
#define MAC7 0X00
#define MAC8 0X00
//---------------------------------------------------
#define bNetworkBusy  	FlagByte.bit0//����æ��־
#define bTransmitting  	FlagByte.bit1//���ڷ������ݱ�־
#define bCollisionWait 	FlagByte.bit2//���ó�ͻ�ȴ���־
#define ApplyTx			FlagByte.bit3//�����ͱ�־
#define btest			FlagByte.bit4//�����ͱ�־

#define RS485EN			PORTC_Bit1
#define WDI			PORTC_Bit0//�߹�

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
unsigned int  g_cReKeyTimes; 	//�ؼ�����
//ȫ�ֱ���
unsigned char TxPos;	      	//��������ָ��	
unsigned char TxNum;	  		//�������ݳ���
unsigned char SendBuf[100];  	//�������ݻ�����
unsigned char SendLast;		  	//����͵�����

unsigned char RevBuf[100];  		//�������ݻ�����
unsigned char RxStep;			//���ղ���
unsigned char RxPos;			//��������ָ��
unsigned char RxNow;			//��ǰ���յ�����	
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
unsigned char pucData[200]={0};	                 //��ʱ��ȡժҪ��Ϣ 

