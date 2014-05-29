#include <iom64a.h>
#include <iomacro.h >
#define  uchar unsigned char
#define  uint  unsigned int
#define  ulong unsigned long

void date_to_time(uchar date[],Time t);
void WriteTimeOn(void);
void WriteTimeOff(void);

//����sda��λ����
#define sda_in	    DDRF &= 0xFD	      //������������
#define sda_out	    DDRF |= 0x02	      //�����������
#define sda_setb    PORTF |= 0x02      //��������      
#define sda_clr     PORTF &= 0xFD      //��������      
#define sda_r       PINF & (0x02 )        //���ݶ�ȡ  
//ʱ��scl��λ����
#define scl_in	    DDRF &= 0xFE      //ʱ����������
#define scl_out	    DDRF |= 0x01      //ʱ���������
#define scl_setb    PORTF |= 0x01       //ʱ������      
#define scl_clr     PORTF &= 0xFE     //ʱ������      
#define scl_r       PINF & (0x01)         //ʱ�Ӷ�ȡ 
//===============
#define true  1
#define false 0
//===============
uchar   date[7]; 


//====================================================================
/*��ʱ�ӳ���*/
void delay_ms(uint time)    	       //11.0592M,time=1��ʾ��ʱ1ms
{
 uint i;
 for(;time>0;time--)
  for(i=0;i<1571;i++);
  asm("nop");
  asm("nop");
  asm("nop");
}
void delay_1us(void)        	       //11.0592M, 1us��ʱ����
  {
   asm("nop");
  }
void delay_us(uint time)     	       //11.0592M, Nus��ʱ����
  {
   uint i;
   for (i=0;i<time;i++)
   asm("nop");
  }  
//=====================================================================  


/********����SD2400��I2C����********/
uchar I2CStart(void)
{
    	sda_out;
	scl_out;
	delay_1us();
    	sda_setb;	
	scl_setb;	
	delay_us(5);
	sda_in;
	delay_us(5);
	if(!sda_r)return false;		//SDA��Ϊ�͵�ƽ������æ,�˳�
	sda_out;
	delay_1us();
	sda_clr;	
	delay_us(10);
	sda_in;    	
	delay_us(10);
	while(sda_r)return false;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	scl_clr;	
	delay_us(5);
	return true;

}


//********�ر�SD2400��I2C����****//
void I2CStop(void)
{
    scl_out;
	sda_out;
	delay_1us();
	sda_clr;
	delay_1us();
	scl_clr;
	delay_us(5);
	scl_setb;
	delay_us(2);
	sda_setb;
}


//*********���� ACK****//
void I2CAck(void)
{	
    scl_out;
	sda_out;
	delay_1us();
	scl_clr;
	delay_us(2);
	sda_clr;	
	delay_us(3);
	scl_setb;	
	delay_us(5);
	scl_clr;
        delay_us(2);
        sda_setb;
        	
}


/*********����NO ACK*********/
void I2CNoAck(void)
{	
    scl_out;
	sda_out;
	delay_1us();
	scl_clr;	
	delay_1us();
	sda_setb;
	delay_us(5);
	scl_setb;	
	delay_us(5);
	scl_clr;	
}



//*********��ȡACK�ź�*********
uchar I2CWaitAck(void) 			 //����Ϊ:1=��ACK,0=��ACK
{
	uchar errtime=255;
	scl_out;
	sda_in;
	delay_1us();
	scl_clr;	
	delay_us(5);
	scl_setb;	
	delay_1us();
	sda_in;
	delay_1us();
	while(sda_r)	
	{
		errtime--;
		delay_1us();
		if(!errtime)
		{delay_us(10);
		scl_clr;
		return false;
                 }
	}
	scl_clr;	
	return true;
}


/************MCU��SD2400����һ���ֽ�*************/             
void I2CSendByte(uchar demand)		 //���ݴӸ�λ����λ
{
	uchar bd=8;                                                        
	sda_out;
	scl_out;
		while(bd--)
		{
			scl_clr;  
			delay_1us();
			if(demand&0x80)
			{
			 sda_setb;
			}
			 else
			 {
			  sda_clr;
			 }
			demand<<=1;
			delay_us(3);
			scl_setb;
			delay_us(3);
		}
		scl_clr;	 
	
     
}


/*********MCU��SD2400����һ�ֽ�*********/
uchar I2CReceiveByte(void) 		//���ݴӸ�λ����λ//
{
	uchar bl=8;
	uchar ddata=0;
	scl_out;
	delay_1us();
	delay_1us();
	sda_in;			 	//c51��ΪSDA=1;�˿��л�Ϊ����״̬����
	delay_1us();
	while(bl--)
	{
		ddata<<=1;              //���ݴӸ�λ��ʼ��ȡ
		delay_1us();
		scl_clr;
		delay_us(5);	        //�Ӹ�λ��ʼ ddata|=SDA;ddata<<=1
		scl_setb;
		delay_us(5);	
		if(sda_r)
		{
			ddata|=0x01;
		}
		
		
	}
	scl_clr;
	return ddata;
}


/******��SD2400ʵʱ���ݼĴ���******/
void I2CReadDate(Time times)
{
	uchar n;
	I2CStart();
	I2CSendByte(0x65); 
	I2CWaitAck();
	for(n=0;n<7;n++)
	{
		date[n]=I2CReceiveByte();
		if (n<6)        	 //���һ�����ݲ�Ӧ��
		{
			I2CAck();
                        
		}
	}
        date_to_time(date,times);
	I2CNoAck();
	delay_1us();
	I2CStop();
}


/******дSD2400ʵʱ���ݼĴ���******/
void I2CWriteDate(uchar times[7])
{		

        WriteTimeOn();

	I2CStart();
	I2CSendByte(0x64);      
	I2CWaitAck();   
    	I2CSendByte(0x00);		//����д��ʼ��ַ      
	I2CWaitAck();	
	I2CSendByte(times[0]);		// second     
	I2CWaitAck();	
	I2CSendByte(times[1]);		//minute      
	I2CWaitAck();	
	I2CSendByte(times[2]);		//hour ,��ʮ��Сʱ��     
	I2CWaitAck();	
	I2CSendByte(times[3]);		//week      
	I2CWaitAck();	
	I2CSendByte(times[4]);		//day      
	I2CWaitAck();	
	I2CSendByte(times[5]);		//month      
	I2CWaitAck();	
	I2CSendByte(times[6]);		//year      
	I2CWaitAck();	
	I2CStop();

	I2CStart();
	I2CSendByte(0x64);      
	I2CWaitAck();   
    	I2CSendByte(0x12);		//����д��ʼ��ַ      
	I2CWaitAck();		
    	I2CSendByte(0x00);		//�������ֵ����Ĵ���      
	I2CWaitAck();        
	I2CStop();  
	
	WriteTimeOff();      
}


/******дSD2400�������******/
void WriteTimeOn(void)
{		
	I2CStart();
	I2CSendByte(0x64);      
	I2CWaitAck();   
    	I2CSendByte(0x10);		//����д��ַ10H      
	I2CWaitAck();	
	I2CSendByte(0x80);		//��WRTC1=1      
	I2CWaitAck();
	I2CStop(); 
	
	I2CStart();
	I2CSendByte(0x64);      
	I2CWaitAck();   
    	I2CSendByte(0x0F);		//����д��ַ0FH      
	I2CWaitAck();	
	I2CSendByte(0x84);		//��WRTC2,WRTC3=1      
	I2CWaitAck();
	I2CStop(); 
	
}


/******дSD2400��ֹ����******/
void WriteTimeOff(void)
{		
	
	I2CStart();
	I2CSendByte(0x64);      
	I2CWaitAck();   
   	I2CSendByte(0x0F);		//����д��ַ0FH      
	I2CWaitAck();	
	I2CSendByte(0x0) ;		//��WRTC2,WRTC3=0      
	I2CWaitAck();
	I2CSendByte(0x0) ;		//��WRTC1=0(10H��ַ)      
	I2CWaitAck();
	I2CStop(); 
}

//��������ʱ������д��ʱ��ṹ��
void date_to_time(uchar date[],Time t)
{
  t.second=date[0];
  t.minute=date[1];
  t.hour=date[2];
  t.week=date[3];
  t.day=date[4];
  t.month=date[5];
  t.year=date[6];
}
/*
//test
void w_and_r()
{
  WriteTimeOn();

	I2CStart();
	I2CSendByte(0x64);      
	I2CWaitAck();   
    	I2CSendByte(0x03);		//����д��ʼ��ַ      
	I2CWaitAck();	
        //I2CSendByte(0x30);      
	//I2CWaitAck();
        //I2CSendByte(0x21);      
	//I2CWaitAck();
        I2CStart();  	
        I2CSendByte(0x65);      
	I2CWaitAck();
        
   //WriteTimeOff(); 
   
        date[2]=I2CReceiveByte();
        I2CAck();
        date[3]=I2CReceiveByte();
        I2CNoAck();
	delay_1us();
	I2CStop();
}
*/
//========================================================================= 
/*void main(void)
{  
        uchar rdate[7];
	uchar times[7]={0x00,0x30,0x95,0x02,0x15,0x04,0x14};         //Сʱע�⣬times[2]��
	I2CWriteDate(times); 		//дʵʱʱ��
 	while(1)
	{
          //w_and_r();
 		I2CReadDate(rdate); 		//��ʵʱʱ��	
         	delay_ms(1000);		//��ʱ1S

    }
}*/