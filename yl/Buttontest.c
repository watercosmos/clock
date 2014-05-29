//����Ƶ��Ϊ11.0592MHz��
#include "Buttontest.h"
void SysInit(void);
void StartTX(void);

void RxRst();
void UartProcess();
void CalcCrc(unsigned char Crcbuf);
void SysInit(void);
void delay_10ms(void);
unsigned char ScanKey();
void KeyDeal();
void ResponedProcess(void);

//------------------------------------------------------------------------------
void main(void)
{
	SysInit();
	_SEI();
	SendBuf[0]=0xAA;
        SendBuf[1]=0xAA;
        SendBuf[2]=0x00;
        SendBuf[3]=0x00;
        SendBuf[4]=0x02;
        SendBuf[5]=0x01;
	SendBuf[6]=0x0A;
	SendBuf[7]=0x01;
        SendBuf[8]=0x00;
        SendBuf[9]=0x00;
        SendBuf[10]=0x00;  
        SendBuf[11]=0x00;
        SendBuf[12]=0x00;
        SendBuf[13]=0x00;
        SendBuf[14]=0xAA;
        SendBuf[15]=0xBB;
        SendBuf[16]=0xCC;
        SendBuf[17]=0xDD;
        SendBuf[18]=0xEE;
        SendBuf[19]=0xFF;
        SendBuf[20]=0x00;
        SendBuf[21]=0x00;
        SendBuf[22]=0xEA;
        SendBuf[23]=0x5F;
				
	TxNum=24;	
	ApplyTx	=1; //�ϵ������ȷ�һ��
	
	Key1Led=Key2Led=Key3Led=Key4Led=Key5Led=Key6Led=OFF;

	while(1)
	{
		if( ApplyTx )		//����з�������
               {
			if( !bNetworkBusy ) //������߲�æ
                       {
			     StartTX();		//��ʼ����
			     ApplyTx=0;
                        }
	       }

               if(NonEmpty)
               {
                  ResponedProcess();
                 }
                
		WDI=0;
		delay_10ms();
		WDI=1;
	}
}
//------------------------------------------------------------------------------
//�Ը��־��������֡������Ӧ����������
void ResponedProcess(void)
{
  unsigned char i;
  
  if(RevBuf[2]==NewSensorDeciveID&&RevBuf[3]==NewSensorSubNetID&&(RevBuf[9]&0x3F)==0x00&&(RevBuf[10]&0xBF)==0x80)     
    {   
      //��ѯ������ģ���Ƿ�����      
        SendBuf[0]=0xAA;      //AAAA000002010A01000000000000AABBCCDDEEFF0000EA5F
	SendBuf[1]=0xAA;
	SendBuf[2]=0x00;
	SendBuf[3]=0x00;
	SendBuf[4]=NewSensorDeciveID;   
	SendBuf[5]=NewSensorSubNetID;
	SendBuf[6]=0x0A;      //���鳤��
	SendBuf[7]=NewSensorSubNetID;
        SendBuf[8]=RevBuf[8]; //�������
	SendBuf[9]=0x00;
	SendBuf[10]=0x00;  
	SendBuf[11]=0x00;
	SendBuf[12]=0x00;      //MAC
	SendBuf[13]=0x00;
	SendBuf[14]=0xAA;
	SendBuf[15]=0xBB;
        SendBuf[16]=0xCC;  
	SendBuf[17]=0xDD;
	SendBuf[18]=0xEE;
	SendBuf[19]=0xFF;
	SendBuf[20]=pucData[12]; //���ݰ汾
	SendBuf[21]=pucData[13];   
        //SendBuf[22]=0xEA;      //Crc
	//SendBuf[23]=0x5F;

        Crc	= 0xffff;       //receive over��?crc16
	for(i=2;i<22;i++)
	{
	    CalcCrc(SendBuf[i]);
	}
        
        SendBuf[22]=(unsigned char)Crc;
	SendBuf[23]=(unsigned char)(Crc/256);

        TxNum=24;	
	ApplyTx	=1; 
        NonEmpty=0;
         
      }
   else 
         if(RevBuf[2]==NewSensorDeciveID&&RevBuf[3]==NewSensorSubNetID&&(RevBuf[9]&0x3F)==0x00&&(RevBuf[10]&0xBF)==0x85)
         {
            //����̨���ô�����ģ��ժҪ��Ϣ
           
			 pucData[0]=RevBuf[0];  //0xAA
			 pucData[1]=RevBuf[1];			     
			 pucData[2]=RevBuf[2];
			 pucData[3]=RevBuf[3];
			 pucData[4]=RevBuf[4];
			 pucData[5]=RevBuf[5];
			 pucData[6]=RevBuf[6];
			 pucData[7]=RevBuf[7];			     
			 pucData[8]=RevBuf[8];
			 pucData[9]=RevBuf[9];
			 pucData[10]=RevBuf[10];           
			 pucData[11]=RevBuf[11];
			 pucData[12]=RevBuf[12];	   //tDataStamp û��ȫŪ��������) 
			 pucData[13]=RevBuf[13];			     
			 pucData[14]=RevBuf[14];           //�豸�ͺ�
			 pucData[15]=RevBuf[15];
			 pucData[16]=RevBuf[16];
			 pucData[17]=RevBuf[17];
			 pucData[18]=RevBuf[18];
			 pucData[19]=RevBuf[19];			     
			 pucData[20]=RevBuf[20];
			 pucData[21]=RevBuf[21];
			 pucData[22]=RevBuf[22];
			 pucData[23]=RevBuf[23];
   			 pucData[24]=RevBuf[24];	   
			 pucData[25]=RevBuf[25];	   	
                         pucData[26]=RevBuf[26];	   //ģ���Ƿ�����
			 pucData[27]=RevBuf[27];	   //������Ϣ��6bit��
			 

   ResBuf[0]=0xAA;
   ResBuf[1]=0xAA;
   ResBuf[2]=0x00;
   ResBuf[3]=0x00;
   ResBuf[4]=NewSensorDeciveID;
   ResBuf[5]=NewSensorSubNetID;
   ResBuf[6]=0x00;
   ResBuf[7]=NewSensorSubNetID;
   ResBuf[8]=RevBuf[8];              //�������
   ResBuf[9]=0x00;
   ResBuf[10]=0x05;
   ResBuf[11]=0x00;

   Crc=0xffff;
   for(i=2;i<12;i++)
   {
   	  CalcCrc(ResBuf[i]);
    }

   ResBuf[12]=(unsigned char)Crc;
   ResBuf[13]=(unsigned char)(Crc/256);

    SendBuf[0] = ResBuf[0];
    SendBuf[1] = ResBuf[1];
    SendBuf[2] = ResBuf[2];
    SendBuf[3] = ResBuf[3];
    SendBuf[4] = ResBuf[4];
    SendBuf[5] = ResBuf[5];
    SendBuf[6] = ResBuf[6];
    SendBuf[7] = ResBuf[7];
    SendBuf[8] = ResBuf[8];
    SendBuf[9] = ResBuf[9];
    SendBuf[10] = ResBuf[10];
    SendBuf[11] = ResBuf[11];
    SendBuf[12] = ResBuf[12];
    SendBuf[13] = ResBuf[13];
    
    TxNum=14;	
    ApplyTx=1; 
    NonEmpty=0;
    
     }

   else 
         if(RevBuf[2]==NewSensorDeciveID&&RevBuf[3]==NewSensorSubNetID&&(RevBuf[9]&0x3F)==0x00&&(RevBuf[10]&0xBF)==0x8A)
         {
          //����̨��ȡ������ģ��ժҪ��Ϣ           
           ResBuf[0]=0xAA;
           ResBuf[1]=0xAA;
           ResBuf[2]=0x00;
           ResBuf[3]=0x00;
           ResBuf[4]=NewSensorDeciveID;
           ResBuf[5]=NewSensorSubNetID;
           ResBuf[6]=0x20;
           ResBuf[7]=NewSensorSubNetID;
           ResBuf[8]=RevBuf[8];            //�������
           ResBuf[9]=0x00;
           ResBuf[10]=0x0A;
           ResBuf[11]=0x00;
           ResBuf[12]=pucData[14];
           ResBuf[13]=pucData[15];
           ResBuf[14]=pucData[16];
           ResBuf[15]=pucData[17];
           ResBuf[16]=pucData[18];
           ResBuf[17]=pucData[19];
           ResBuf[18]=pucData[20];
           ResBuf[19]=pucData[21];
           ResBuf[20]=pucData[22];
           ResBuf[21]=pucData[23];
           ResBuf[22]=pucData[24];
           ResBuf[23]=pucData[25];
           ResBuf[24]=pucData[26];        //ģ���Ƿ�����
           ResBuf[25]=0x00;               //MAC
           ResBuf[26]=0x00;
           ResBuf[27]=0xAA;
           ResBuf[28]=0xBB;
           ResBuf[29]=0xCC;
           ResBuf[30]=0xDD;
           ResBuf[31]=0xEE;
           ResBuf[32]=0xFF;
           ResBuf[33]=0x00;              //����汾
           ResBuf[34]=0x00;
           ResBuf[35]=0x00;
           ResBuf[36]=0x00;
           ResBuf[37]=0x00;
           ResBuf[38]=0x00;
           ResBuf[39]=0x00;
           ResBuf[40]=0x00;
           ResBuf[41]=0x00;
           ResBuf[42]=0x00;
           ResBuf[43]=RevBuf[27];      //���ഫ������Ϣ
           
          Crc=0xffff;
          for(i=2;i<44;i++)
          {
   	    CalcCrc(ResBuf[i]);
           }

         ResBuf[44]=(unsigned char)Crc;
         ResBuf[45]=(unsigned char)(Crc/256);
         
    SendBuf[0] = ResBuf[0];
    SendBuf[1] = ResBuf[1];
    SendBuf[2] = ResBuf[2];
    SendBuf[3] = ResBuf[3];
    SendBuf[4] = ResBuf[4];
    SendBuf[5] = ResBuf[5];
    SendBuf[6] = ResBuf[6];
    SendBuf[7] = ResBuf[7];
    SendBuf[8] = ResBuf[8];
    SendBuf[9] = ResBuf[9];
    SendBuf[10] = ResBuf[10];
    SendBuf[11] = ResBuf[11];
    SendBuf[12] = ResBuf[12];
    SendBuf[13] = ResBuf[13];
    SendBuf[14] = ResBuf[14];
    SendBuf[15] = ResBuf[15];
    SendBuf[16] = ResBuf[16];
    SendBuf[17] = ResBuf[17];
    SendBuf[18] = ResBuf[18];
    SendBuf[19] = ResBuf[19];
    SendBuf[20] = ResBuf[20];
    SendBuf[21] = ResBuf[21];
    SendBuf[22] = ResBuf[22];
    SendBuf[23] = ResBuf[23];
    SendBuf[24] = ResBuf[24];
    SendBuf[25] = ResBuf[25];
    SendBuf[26] = ResBuf[26];
    SendBuf[27] = ResBuf[27];
    SendBuf[28] = ResBuf[28];
    SendBuf[29] = ResBuf[29];
    SendBuf[30] = ResBuf[30];
    SendBuf[31] = ResBuf[31];
    SendBuf[32] = ResBuf[32];
    SendBuf[33] = ResBuf[33];
    SendBuf[34] = ResBuf[34];
    SendBuf[35] = ResBuf[35];
    SendBuf[36] = ResBuf[36];
    SendBuf[37] = ResBuf[37];
    SendBuf[38] = ResBuf[38];
    SendBuf[39] = ResBuf[39];
    SendBuf[40] = ResBuf[40];
    SendBuf[41] = ResBuf[41];
    SendBuf[42] = ResBuf[42];
    SendBuf[43] = ResBuf[43];
    SendBuf[44] = ResBuf[44];
    SendBuf[45] = ResBuf[45];
    
    TxNum=46;	
    ApplyTx=1; 
    NonEmpty=0;
    
      }
  
   else 
         if(RevBuf[2]==NewSensorDeciveID&&RevBuf[3]==NewSensorSubNetID&&(RevBuf[9]&0x3F)==0x00&&(RevBuf[10]&0xBF)==0x8B)
         {
        //���������޸ı�ģ������ID���豸ID   
        SendBuf[0]=0xAA;   
	SendBuf[1]=0xAA;
	SendBuf[2]=0x00;
	SendBuf[3]=0x00;
	SendBuf[4]=NewSensorDeciveID;   
	SendBuf[5]=NewSensorSubNetID;
	SendBuf[6]=0x08;   //���鳤��
	SendBuf[7]=NewSensorSubNetID;
        SendBuf[8]=RevBuf[8];
	SendBuf[9]=0x00;
	SendBuf[10]=0x0B;  
	SendBuf[11]=0x00;
	SendBuf[12]=0x00;   //MAC
	SendBuf[13]=0x00;
	SendBuf[14]=0xAA;
	SendBuf[15]=0xBB;
        SendBuf[16]=0xCC;  
	SendBuf[17]=0xDD;
	SendBuf[18]=0xEE;
	SendBuf[19]=0xFF;

        Crc	= 0xffff;	//receive over��?crc16
	for(i=2;i<20;i++)
	{
	    CalcCrc(SendBuf[i]);
	}
        
        SendBuf[20]=(unsigned char)Crc;
	SendBuf[21]=(unsigned char)(Crc/256);

        TxNum=22;	
	ApplyTx	=1; 
        NonEmpty=0;
         
          }
      
    else 
         if(RevBuf[2]==NewSensorDeciveID&&RevBuf[3]==NewSensorSubNetID&&(RevBuf[9]&0x3F)==0x00&&(RevBuf[10]&0xBF)==0x8C)
         {
           if(RevBuf[14]==0x00&&RevBuf[15]==0x00&&RevBuf[16]==0xAA&&RevBuf[17]==0xBB&&RevBuf[18]==0xCC&&RevBuf[19]==0xDD&&RevBuf[20]==0xEE&&RevBuf[21]==0xFF)
           {
            //���ô�����������ID���豸ID            
             NewSensorDeciveID=RevBuf[22]; 
             NewSensorSubNetID=RevBuf[23];
             //����ʱ���
             pucData[12]=RevBuf[12];
             pucData[13]=RevBuf[13];
             
   ResBuf[0]=0xAA;
   ResBuf[1]=0xAA;
   ResBuf[2]=0x00;
   ResBuf[3]=0x00;
   ResBuf[4]=NewSensorDeciveID;
   ResBuf[5]=NewSensorSubNetID;
   ResBuf[6]=0x00;
   ResBuf[7]=NewSensorSubNetID;
   ResBuf[8]=RevBuf[8];              //�������
   ResBuf[9]=0x00;
   ResBuf[10]=0x0C;
   ResBuf[11]=0x00;

   Crc=0xffff;
   for(i=2;i<12;i++)
   {
   	  CalcCrc(ResBuf[i]);
    }

   ResBuf[12]=(unsigned char)Crc;
   ResBuf[13]=(unsigned char)(Crc/256);

    SendBuf[0] = ResBuf[0];
    SendBuf[1] = ResBuf[1];
    SendBuf[2] = ResBuf[2];
    SendBuf[3] = ResBuf[3];
    SendBuf[4] = ResBuf[4];
    SendBuf[5] = ResBuf[5];
    SendBuf[6] = ResBuf[6];
    SendBuf[7] = ResBuf[7];
    SendBuf[8] = ResBuf[8];
    SendBuf[9] = ResBuf[9];
    SendBuf[10] = ResBuf[10];
    SendBuf[11] = ResBuf[11];
    SendBuf[12] = ResBuf[12];
    SendBuf[13] = ResBuf[13];
    
    TxNum=14;	
    ApplyTx=1; 
    NonEmpty=0;
    
            }
         }
     
    else 
         if(RevBuf[2]==NewSensorDeciveID&&RevBuf[3]==NewSensorSubNetID&&(RevBuf[9]&0x3F)==0x00&&(RevBuf[10]&0xBF)==0x8D)
         {
            //����/���ô�����  
            EnableSensor=RevBuf[14];      
            //����ʱ���
             pucData[12]=RevBuf[12];
             pucData[13]=RevBuf[13];
                          
   ResBuf[0]=0xAA;
   ResBuf[1]=0xAA;
   ResBuf[2]=0x00;
   ResBuf[3]=0x00;
   ResBuf[4]=NewSensorDeciveID;
   ResBuf[5]=NewSensorSubNetID;
   ResBuf[6]=0x00;
   ResBuf[7]=NewSensorSubNetID;
   ResBuf[8]=RevBuf[8];              //�������
   ResBuf[9]=0x00;
   ResBuf[10]=0x0D;
   ResBuf[11]=0x00;

   Crc=0xffff;
   for(i=2;i<12;i++)
   {
   	  CalcCrc(ResBuf[i]);
    }

   ResBuf[12]=(unsigned char)Crc;
   ResBuf[13]=(unsigned char)(Crc/256);

    SendBuf[0] = ResBuf[0];
    SendBuf[1] = ResBuf[1];
    SendBuf[2] = ResBuf[2];
    SendBuf[3] = ResBuf[3];
    SendBuf[4] = ResBuf[4];
    SendBuf[5] = ResBuf[5];
    SendBuf[6] = ResBuf[6];
    SendBuf[7] = ResBuf[7];
    SendBuf[8] = ResBuf[8];
    SendBuf[9] = ResBuf[9];
    SendBuf[10] = ResBuf[10];
    SendBuf[11] = ResBuf[11];
    SendBuf[12] = ResBuf[12];
    SendBuf[13] = ResBuf[13];
    
    TxNum=14;	
    ApplyTx=1; 
    NonEmpty=0;
            
         }
  
    else 
         if(RevBuf[2]==NewSensorDeciveID&&RevBuf[3]==NewSensorSubNetID&&(RevBuf[9]&0x3F)==0x00&&(RevBuf[10]&0xBF)==0x8E)
         {
          //����/���ô���������������Ϣ
          EnableSensorSendHeart=RevBuf[14];
          
          EnableTime=RevBuf[15];
          
          HeartIntervel[0]=RevBuf[16]; 
          HeartIntervel[1]=RevBuf[17];
          
          //����ʱ���
            pucData[12]=RevBuf[12];
            pucData[13]=RevBuf[13];
            
   ResBuf[0]=0xAA;
   ResBuf[1]=0xAA;
   ResBuf[2]=0x00;
   ResBuf[3]=0x00;
   ResBuf[4]=NewSensorDeciveID;
   ResBuf[5]=NewSensorSubNetID;
   ResBuf[6]=0x00;
   ResBuf[7]=NewSensorSubNetID;
   ResBuf[8]=RevBuf[8];              //�������
   ResBuf[9]=0x00;
   ResBuf[10]=0x0E;
   ResBuf[11]=0x00;

   Crc=0xffff;
   for(i=2;i<12;i++)
   {
   	  CalcCrc(ResBuf[i]);
    }

   ResBuf[12]=(unsigned char)Crc;
   ResBuf[13]=(unsigned char)(Crc/256);

    SendBuf[0] = ResBuf[0];
    SendBuf[1] = ResBuf[1];
    SendBuf[2] = ResBuf[2];
    SendBuf[3] = ResBuf[3];
    SendBuf[4] = ResBuf[4];
    SendBuf[5] = ResBuf[5];
    SendBuf[6] = ResBuf[6];
    SendBuf[7] = ResBuf[7];
    SendBuf[8] = ResBuf[8];
    SendBuf[9] = ResBuf[9];
    SendBuf[10] = ResBuf[10];
    SendBuf[11] = ResBuf[11];
    SendBuf[12] = ResBuf[12];
    SendBuf[13] = ResBuf[13];
    
    TxNum=14;	
    ApplyTx=1; 
    NonEmpty=0;
               
                       
          }
                             
    
     else RxRst();

  }
//------------------------------------------------------------------------------
void delay_10ms(void)            //1ms��ʱ����
{
	unsigned int i;
	for (i=0;i<11400;i++);
}
//------------------------------------------------------------------------------
#pragma vector=TIMER0_OVF_vect	//��ʱ��0����ж�
__interrupt  void t0_ovf_isr(void)
{
	TCNT0  = 0x53;
	KeyDeal();					//ɨ�谴��
}
//------------------------------------------------------------------------------
#pragma vector=TIMER1_OVF_vect//��ʱ��1����ж�
__interrupt  void t1_ovf_isr(void)
{
	TCCR1B = 0x00;		//ֹͣ��ʱ��
	RxRst(); 			//���շ��͸�λ
	
	if(bCollisionWait)
	{
        bCollisionWait=0;
        StartTX();    	//���·�������
	}
    bNetworkBusy=0;     //������æ��־
}
//------------------------------------------------------------------------------
#pragma vector=USART_TXC_vect
__interrupt  void uart0_tx_isr(void)	
{							//�����ж�
	TxPos++;	            //��������ָ��� 1	
	if(TxPos<TxNum)  	    //�����Ƿ������
	{
		UDR=SendBuf[TxPos]; 	//��������
		SendLast=SendBuf[TxPos];//���淢�͵�����, �Ա����Ƿ��ͻ
		bTransmitting=1;        //�������ڷ������ݱ�־
	}
	else
	{
		TxNum	= 0;
		TxPos	= 0;
		RS485EN = 0;		//��ֹ����
		RxRst();   			//���շ��͸�λ
	}
}
//------------------------------------------------------------------------------
void StartTX(void)			//��ʼ��������
{       //tenumӦ�����︳ֵ
	RS485EN = 1;	        //ʹ�ܷ���	
	UCSRA|= 0x40;	//�ؼ�������
	UCSRB|= 0x08;
	UDR	=SendBuf[0];    //�����1������
        SendLast=SendBuf[0];	//���淢�͵�����,�Ա����ͻ��
	TxPos	= 0;	        //��������ָ����0
        bTransmitting=1;        //�������ڷ������ݱ�־
}
//------------------------------------------------------------------------------
#pragma vector=USART_RXC_vect
__interrupt  void uart0_rx_isr(void)
{							//�����ж�
unsigned char c;	
unsigned int iTime=0;


	bNetworkBusy =1;        //������æ��־

	c=UDR;	                //���յ�������	
 	
	TCCR1B=0x00;			//ֹͣ��ʱ��	
	TCNT1H=DELAY_HI;		//������ʱ����, �Ա���������
	TCNT1L=DELAY_LO;
	TIMSK |=0X04;			//��ʱ���ж�ʹ��
	TIFR  |=0X04;			//�������־
 	TCCR1B=0x01;			//������ʱ��

	if(bTransmitting)		//���ڷ�������
	{
		bTransmitting=0;    //������ڷ������ݱ�־
		if(c!=SendLast)   	//����Ƿ�����ͻ
		{
			UCSRB &= 0xF7;	//������ͻ, ֹͣ����
			PORTD_Bit1=1;	//���Ϳ�ֹͣ
			iTime=rand()/3+TWO_MS;//�õ�һ����С��2ms�������ʱ����
			iTime=0xffff-iTime;
			TCCR1B=0x00;	//��װ��ʱ����
			TCNT1H=(unsigned char)(iTime/256);
			TCNT1L=(unsigned char)(iTime%256);
			TCCR1B=0x01;	//������ʱ��
			TxNum =0;
			bCollisionWait=1;
                        
		}
	}
	else
	{
          
		RxNow=c;
                if(!NonEmpty)
                {
                  switch(RxStep)/*������յ�����*/
		  {
			case 0:	if(RxNow==0xAA)	//�ж���ʼ��
					{
						RxStep=1;
                                                RevBuf[RxPos]=RxNow;							
						RxPos++;
					}
					else RxRst();	//��λ����
			break;
			case 1:	if(RxNow==0xAA)	//�ж���ʼ��
					{
						RxStep=2;
						RevBuf[RxPos]=RxNow;
						RxPos++;
					 }
					else RxRst();	//��λ����
			break;
			case 2: RxStep=3;       //Ŀ���豸ID
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
			case 3: RxStep=4;       //Ŀ������ID
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
			case 4: RxStep=5;      //Դ�豸ID
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break; 
			case 5: RxStep=6;      //Դ����ID
					RevBuf[RxPos]=RxNow; 
					RxPos++;
			break;
                        case 6: RxStep=7;      //���鳤��
					RevBuf[RxPos]=RxNow; 
					RxPos++;

                                        DataLength=RxNow;
			break;
			case 7: RxStep=8;     //ת����������ID
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
			case 8: RxStep=9;     //�������
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
                        case 9: RxStep=10;     //�������
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
                        case 10: RxStep=11;     //����С��
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;	
                        case 11: RxStep=12;     //������
					RevBuf[RxPos]=RxNow; 
					RxPos++;								
			break;
                        case 12:if(DataLength==0x00)
                                  {                                      
                                      RxStep=13;
                                      RevBuf[RxPos]=RxNow;
                                      RxPos++;
                              
			           }
                                  else
                                   {                                      
                                      RxStep=12;
                                      RevBuf[RxPos]=RxNow;
                                      DataLength--;
                                      RxPos++ ;

                                    }               
			break;
                        case 13:                //CRC hign
					RevBuf[RxPos]=RxNow;
					RxPos++;
                                        UartProcess();
			break;
			case 14:               //CRC low
                                        RevBuf[RxPos]=RxNow;
					RxPos++;
                                        
					UartProcess();
			break;
			default:RxRst();		//reset RX!
			break;
		  }
	     }
       }
}
//------------------------------------------------------------------------------
void UartProcess()
{
    unsigned char i,j; 
    
       for(j=0;j<RxPos;j++)
       {
            SendBuf[TxNum]=RevBuf[j];
            TxNum++;
          
        }
          
           ApplyTx=1; 
           TxNum=0;
          
        Crc	= 0xffff;					//receive over��?crc16
	for(i=2;i<RxPos;i++)
	{
	   CalcCrc(RevBuf[i]);
                
	}
        
	if(Crc==0)						 //Ok!
	{         
            
          //NonEmpty=1;
		
	   }
        
	else RxRst();				       //reset RX! 
   
}
//------------------------------------------------------------------------------
void CalcCrc(unsigned char Crcbuf)
{
unsigned char j,TT;
	Crc=Crc ^ Crcbuf;
	for(j=0;j<8;j++)
	{
		TT=Crc&1;
		Crc=Crc>>1;
		Crc=Crc&0x7fff;
		if (TT==1)
		Crc=Crc^0xa001;
		Crc=Crc&0xffff;
	}
}
//------------------------------------------------------------------------------
void RxRst()			//receive TongBu reset
{
//	TxNum	= 0;
//	TxPos	= 0;
//	RS485EN = 0;		//����ֹͣ
	RxStep	= 0;
	RxPos	= 0;
}
//------------------------------------------------------------------------------
void SysInit(void)
{
	 _CLI();
	//IO�ڳ�ʼ��
	DDRA   = 0x00;	//A��Ϊ���������
	PORTA  = 0xFF;
	DDRB   = 0xFF;	//B��Ϊ�������������
	PORTB  = 0x00;
	DDRC   = 0xC3;	//C��Ϊ�߹���485ʹ�ܣ���Ӵ洢����
	PORTC  = 0x00;

	DDRD   = 0xF2;	//D�ڸ���λ���Ƶ� pwm����Ϊ���ڣ����ж�
	PORTD  = 0x00;

	//USART ��ʼ��
    UCSRC = 0x06;    //USART 9600 8, n,1�ޱ���
    UBRRL = (F_CPU/BAUDRATE/16-1)%256;//U2X=0ʱ�Ĺ�ʽ����
    UBRRH = (F_CPU/BAUDRATE/16-1)/256;
    UCSRA = 0x00;
    UCSRA|= 0x40;	//�ؼ�������
    UCSRB = 0xd8;	//ʹ�ܽ��� �����жϣ�ʹ�ܽ��գ�ʹ�ܷ���
	
	TCCR0  = 0x00;	//ֹͣ��ʱ��
	TCNT0  = 0x53;	//��ʼֵ
	OCR0   = 0x52;	//ƥ��ֵ
	TIMSK |= 0x01;	//�ж�����
	TIFR  |= 0X01;
	TCCR0  = 0x04;	//������ʱ��
	
	TCCR2  = 0x00;	//ֹͣ��ʱ��
	ASSR   = 0x00;	//�첽ʱ��ģʽ
	TCNT2  = 0x00;	//��ʼֵ
	OCR2   = 0x1e;	//ƥ��ֵ
	TCCR2  = 0x7D;	//������ʱ��
	RevBuf[7]=0x01;
	bNetworkBusy 	=0;
	bTransmitting 	=0;
	bCollisionWait 	=0;
	ApplyTx	 	=0;
	RS485EN		=0;
}
//------------------------------------------------------------------------------
void KeyDeal() //���̴���
{
	NowKey =ScanKey();
	switch(NowKey)
	{
		case	DownKEY1:
							SendBuf[9]=NowKey;
							TxNum=10;
							ApplyTx=1;
							if(!Key1Led) Key1Led=OFF;
							else 		 Key1Led=ON;
		break;
		case	DownKEY2:
							SendBuf[9]=NowKey;
							TxNum=10;
							ApplyTx=1;
							if(!Key2Led) Key2Led=OFF;
							else 		 Key2Led=ON;
		break;			
		case	DownKEY3:
							SendBuf[9]=NowKey;
							TxNum=10;
							ApplyTx=1;
							if(!Key3Led) Key3Led=OFF;
							else 		 Key3Led=ON;
		break;
		case	DownKEY4:
							SendBuf[9]=NowKey;
							TxNum=10;
							ApplyTx=1;
							if(!Key4Led) Key4Led=OFF;
							else 		 Key4Led=ON;
		break;
		case	DownKEY5:
							SendBuf[9]=NowKey;
							TxNum=10;
							ApplyTx=1;
							if(!Key5Led) Key5Led=OFF;
							else 		 Key5Led=ON;
		break;
		case	DownKEY6:
							SendBuf[9]=NowKey;
							TxNum=10;
							ApplyTx=1;
							if(!Key6Led) Key6Led=OFF;
							else 		Key6Led=ON;
		break;
		//���������˫��ʶ��
		default:
		break;
	}
}
//------------------------------------------------------------------------------
unsigned char ScanKey()
{
const unsigned char cScanWord[]={0x3E,0x3D,0x3B,0x37,0x2F,0x1F,0x3F};
unsigned char cKeyColumn,n,cKey;

	cKey  =0X3F;
	cKeyColumn=PINA&0X3F;

	if(cKeyColumn!=0x3F)	//�м�����
    {
      	if(g_bReEntry>20) 	//������
        {
			for(n=0;n<7;n++)
			{
				if(cScanWord[n]==cKeyColumn) break;	//���ֵ
			}
			cKey=cScanWord[n];
			
			if(g_cOldKey!=cKey)
			{
				g_cOldKey=cKey;		//��һ��ֻ�ܶ�һ��
			}
			else  			//�ؼ�
			{
				if(g_cReKeyTimes>LONGTIME) return 0x3F;
				if(g_cReKeyTimes==LONGTIME) //2S
			   	{
                    cKey=cKey|0x40;			//������ֵ
                   	g_cOldKey=0x3F;
                    g_cReKeyTimes++;
                }
				else
				{	
					if(g_cReKeyTimes<LONGTIME)
					{
						g_cReKeyTimes++;
					   	cKey=0x3F;
					}
				}
			}
          	g_bReEntry=0;
        }
      	else g_bReEntry++; //û����
    }
  	else
	{
		if(g_cOldKey!=0x3F)	//�жϵ㶯�ͷ�
		{
			g_bReEntry=0;	//��������
			g_cReKeyTimes=0;//��������������
			cKey=g_cOldKey|0x80;//������־����
			g_cOldKey=0x3F;
		}
		else //�޼�����
		{
			g_bReEntry	=0;	//��������
			g_cReKeyTimes=0;//��������������
		}
	}
	return cKey;
}
//------------------------------------------------------------------------------
//���崦����
/*void ResponedProcess(void)
{
  
  if(RevBuf[9]==0x00)
    goto Order0;
  else if(RevBuf[9]==0x04)
    goto Order1;
  else
    return;
Order0:
  k=987;
  switch(RevBuf[10])
  {
   
  case 0x80:ActResponse();break;//��Ӧ�Ƿ����߲�ѯ
  default:return;
  }
  return;
Order1:
  switch(RevBuf[10])
  {
  case 0x80:ProcAllPageSet();break;//�������е�ҳ������
  case 0x81:ReadAllPageSet();break;//��ȡ����ҳ������
  case 0x82:SetSingleKey();break;//����һ����������������
  case 0x83:ReadSingleKey();break;//��ȡһ����������������
  case 0x86:AutoPageJump();break;//����ҳ����ת
  case 0x87:SetSingleKeyModel();break;//���õ���������ģʽ
  default:return;
  }
  return;
}
//------------------------------------------------------------------------------
void ActResponse()
{
  unsigned int i;
  SendBuf[0]=0xaa;
  SendBuf[1]=0xaa;
  SendBuf[2]=RevBuf[4];//Ŀ���豸ID
  SendBuf[3]=RevBuf[5];//Ŀ������ID
  SendBuf[4]=0x02;//Դ�豸ID
  SendBuf[5]=0x01;//Դ����ID
  SendBuf[6]=0x10;//���ݳ���
  SendBuf[7]=RevBuf[5];//ת������ID
  SendBuf[8]=0x00;//�������
  SendBuf[9]=0x00;//�������
  SendBuf[10]=0x00;//����С��
  SendBuf[11]=0x00;//������
  SendBuf[12]=MAC1;//����
  SendBuf[13]=MAC2;
  SendBuf[14]=MAC3;
  SendBuf[15]=MAC4;
  SendBuf[16]=MAC5;
  SendBuf[17]=MAC6;
  SendBuf[18]=MAC7;
  SendBuf[19]=MAC8;
  SendBuf[20]=TimeStamp[0];
  SendBuf[21]=TimeStamp[1];
  Crc=0xffff;
  for(i=2;i<22;i++)
    CalcCrc(SendBuf[i]);
  SendBuf[22]=(unsigned char)Crc;
  SendBuf[23]=(unsigned char)(Crc/256);
  
  TxNum=24;
  ApplyTx=1;
  room=0;
}
//-----------------------------------------------------------------------------
void ProcAllPageSet()
{
  unsigned char PageNum,i;
  TimeStamp[0]=RevBuf[12];
  TimeStamp[1]=RevBuf[13];
  for(i=0;i<RevBuf[14];i++)
  {
    PageNum=RevBuf[14+i]/32;
    Page[PageNum].Combine=RevBuf[14+i]/2;
    Page[PageNum].Repel=RevBuf[14+i];
  }
  //��Ӧ
  SendBuf[0]=0xaa;
  SendBuf[1]=0xaa;
  SendBuf[2]=RevBuf[4];//Ŀ���豸ID
  SendBuf[3]=RevBuf[5];//Ŀ������ID
  SendBuf[4]=0x02;//Դ�豸ID
  SendBuf[5]=0x01;//Դ����ID
  SendBuf[6]=0x00;//���ݳ���
  SendBuf[7]=RevBuf[5];//ת������ID
  SendBuf[8]=0x00;//�������
  SendBuf[9]=0x04;//�������
  SendBuf[10]=0x00;//����С��
  SendBuf[11]=0x00;//������
  //����
  
  Crc=0xffff;
  for(i=0;i<12;i++)
    CalcCrc(SendBuf[i]);
  SendBuf[12]=(unsigned char)Crc;
  SendBuf[13]=(unsigned char)(Crc/256);
  
  TxNum=14;
  ApplyTx=1;
  room=0;
}
//------------------------------------------------------------------------------
void ReadAllPageSet()
{
  unsigned char NPage=1,i,j;
  SendBuf[0]=0xaa;
  SendBuf[1]=0xaa;
  SendBuf[2]=RevBuf[4];//Ŀ���豸ID
  SendBuf[3]=RevBuf[5];//Ŀ������ID
  SendBuf[4]=0x02;//Դ�豸ID
  SendBuf[5]=0x01;//Դ����ID
  SendBuf[6]=1+NPage;//���ݳ���
  SendBuf[7]=RevBuf[5];//ת������ID
  SendBuf[8]=0x00;//�������
  SendBuf[9]=0x04;//�������
  SendBuf[10]=0x01;//����С��
  SendBuf[11]=0x00;//������
  SendBuf[12]=NPage;
  for(i=0;i<NPage;i++)
    SendBuf[13+i]=(i<<5)&(Page[i].Combine<<1)&(Page[i].Repel);
  j=SendBuf[6]+12;
  Crc=0xffff;
  for(i=0;i<j;i++)
    CalcCrc(SendBuf[i]);
  SendBuf[j]=(unsigned char)Crc;
  SendBuf[j+1]=(unsigned char)(Crc/256);
  TxNum=j+2;
  ApplyTx=1;
  room=0;
}
//-----------------------------------------------------------------------------
void SetSingleKey()
{
  unsigned char i;
  unsigned char NPage=RevBuf[14]/32;
  unsigned char NKey=RevBuf[14]%32;
  unsigned char NFunc=RevBuf[16];
  TimeStamp[0]=RevBuf[12];
  TimeStamp[1]=RevBuf[13];
  if(NPage>0||NKey>3||NFunc>1)
    return;
  Page[NPage].Key[NKey].Model=RevBuf[15]%4;
  for(i=0;i<NFunc;i++)
  {
    //�м�����һ�����صĹ��ܺ�
    Page[NPage].Key[NKey].Func[i].Type=RevBuf[18+i*6];
    Page[NPage].Key[NKey].Func[i].SubNetID=RevBuf[19+i*6];
    Page[NPage].Key[NKey].Func[i].DeviceID=RevBuf[20+i*6];
    Page[NPage].Key[NKey].Func[i].Num=RevBuf[21+i*6];
    Page[NPage].Key[NKey].Func[i].Action=RevBuf[22+i*6];
  }
  //��Ӧ
  SendBuf[0]=0xaa;
  SendBuf[1]=0xaa;
  SendBuf[2]=RevBuf[4];//Ŀ���豸ID
  SendBuf[3]=RevBuf[5];//Ŀ������ID
  SendBuf[4]=0x02;//Դ�豸ID
  SendBuf[5]=0x01;//Դ����ID
  SendBuf[6]=0x00;//���ݳ���
  SendBuf[7]=RevBuf[5];//ת������ID
  SendBuf[8]=0x00;//�������
  SendBuf[9]=0x04;//�������
  SendBuf[10]=0x02;//����С��
  SendBuf[11]=0x00;
  
  Crc=0xffff;
  for(i=0;i<12;i++)
    CalcCrc(SendBuf[i]);
  SendBuf[12]=(unsigned char)Crc;
  SendBuf[13]=(unsigned char)(Crc/256);
  
  TxNum=14;
  ApplyTx=1;
  room=0;
}
void ReadSingleKey()
{
  unsigned char i,length;
  unsigned char NPage=RevBuf[12]/32;
  unsigned char NKey=RevBuf[12]%32;
  SendBuf[12]=Page[NPage].Key[NKey].Model;
  SendBuf[12]=SendBuf[12]<<2;
  SendBuf[13]=1;
  for(i=0;i<1;i++)
  {
    SendBuf[14+i*6]=i;
    SendBuf[15+i*6]=Page[NPage].Key[NKey].Func[i].Type;
    SendBuf[16+i*6]=Page[NPage].Key[NKey].Func[i].SubNetID;
    SendBuf[17+i*6]=Page[NPage].Key[NKey].Func[i].DeviceID;
    SendBuf[18+i*6]=Page[NPage].Key[NKey].Func[i].Num;
    SendBuf[19+i*6]=Page[NPage].Key[NKey].Func[i].Action;
  }
  SendBuf[0]=0xaa;
  SendBuf[1]=0xaa;
  SendBuf[2]=RevBuf[4];
  SendBuf[3]=RevBuf[5];
  SendBuf[4]=0x02;
  SendBuf[5]=0X01;
  SendBuf[6]=2+1*6;
  SendBuf[7]=RevBuf[5];
  SendBuf[8]=0x00;
  SendBuf[9]=0x04;
  SendBuf[10]=0x03;
  SendBuf[11]=0x00;
  
  Crc=0xffff;
  length=12+2+1*6;
  for(i=0;i<length;i++)
    CalcCrc(SendBuf[i]);
  SendBuf[length]=Crc%256;
  SendBuf[length+1]=Crc/256;
  TxNum=length+2;
  ApplyTx=1;
  room=0;
}
void AutoPageJump()
{
  ;
}
void SetSingleKeyModel()
{
  int i;
  unsigned char NPage=RevBuf[14]/32;
  unsigned char NKey=RevBuf[14]%32;
  TimeStamp[0]=RevBuf[12];
  TimeStamp[1]=RevBuf[13];
  Page[NPage].Key[NKey].Model=RevBuf[15]/4;
  SendBuf[0]=0xaa;
  SendBuf[1]=0xaa;
  SendBuf[2]=RevBuf[4];//Ŀ���豸ID
  SendBuf[3]=RevBuf[5];//Ŀ������ID
  SendBuf[4]=0x02;//Դ�豸ID
  SendBuf[5]=0x01;//Դ����ID
  SendBuf[6]=0x00;//���ݳ���
  SendBuf[7]=RevBuf[5];//ת������ID
  SendBuf[8]=0x00;//�������
  SendBuf[9]=0x04;//�������
  SendBuf[10]=0x07;//����С��
  SendBuf[11]=0x00;
    Crc=0xffff;
  for(i=0;i<12;i++)
    CalcCrc(SendBuf[i]);
  SendBuf[12]=(unsigned char)Crc;
  SendBuf[13]=(unsigned char)(Crc/256);
  
  TxNum=14;
  ApplyTx=1;
  room=0;
}*/