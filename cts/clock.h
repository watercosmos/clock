/* clock.h */
#include "iom64.h"
#include "ina90.h"
//#include "delay.h"
#include "stdlib.h"

#define BAUDRATE	9600			//������
#define F_CPU		11059200		//��Ӿ���Ƶ��

#define DELAY_HI	0x7e			//������ʱ����, Լ3ms
#define DELAY_LO	0x66
#define TWO_MS		2000			//2ms��ʱ����

#define RS485EN		PORTE_Bit2
#define WDI		PORTF_Bit3		//�߹�

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

#define BUSY		FlagByte.bit0	//����æ��־
#define TX		FlagByte.bit1	//���ڷ������ݱ�־
#define COLLISION	FlagByte.bit2	//��ͻ�ȴ���־
#define TOTX		FlagByte.bit3	//�����ͱ�־

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