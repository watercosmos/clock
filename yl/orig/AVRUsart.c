//晶振频率为11.0592MHz　
#include "AVRUsart.h"
void SysInit(void);
void StartTX(void);

void RxRst();
void UartProcess();
void CalcCrc(unsigned char Crcbuf);
void SysInit(void);
void delay_10ms(void);
unsigned char ScanKey();
void KeyDeal();
//------------------------------------------------------------------------------
void main(void)
{
	SysInit();
	_SEI();
	SendBuf[0]=1;
	SendBuf[1]=2;
	SendBuf[2]=3;
	SendBuf[3]=4;
	SendBuf[4]=5;
	SendBuf[5]=6;
	SendBuf[6]=7;
	SendBuf[7]=8;
	
	TxNum=8;	
	ApplyTx	=1;//上电试验先发一串
	
	Key1Led=Key2Led=Key3Led=Key4Led=Key5Led=Key6Led=OFF;
	while(1)
	{
		if( ApplyTx )			//如果有发送需求
        {
			if( !bNetworkBusy ) //如果总线不忙
            {
				StartTX();		//开始发送
				ApplyTx=0;
            }
	    }
		WDI=0;
		delay_10ms();
		WDI=1;
	}
}
//------------------------------------------------------------------------------
void delay_10ms(void)            //1ms延时函数
{
	unsigned int i;
	for (i=0;i<11400;i++);
}
//------------------------------------------------------------------------------
#pragma vector=TIMER0_OVF_vect	//定时器0溢出中断
__interrupt  void t0_ovf_isr(void)
{
	TCNT0  = 0x53;
	KeyDeal();					//扫描按键
}
//------------------------------------------------------------------------------
#pragma vector=TIMER1_OVF_vect//定时器1溢出中断
__interrupt  void t1_ovf_isr(void)
{
	TCCR1B = 0x00;		//停止定时器
	RxRst(); 			//接收发送复位
	
	if(bCollisionWait)
	{
        bCollisionWait=0;
        StartTX();    	//重新发送数据
	}
    bNetworkBusy=0;     //清网络忙标志
}
//------------------------------------------------------------------------------
#pragma vector=USART_TXC_vect
__interrupt  void uart0_tx_isr(void)	
{							//发送中断
	TxPos++;	            //发送数据指针加 1	
	if(TxPos<TxNum)  	    //数据是否发送完成
	{
		UDR=SendBuf[TxPos]; 	//发送数据
		SendLast=SendBuf[TxPos];//保存发送的数据, 以便检测是否冲突
		bTransmitting=1;        //设置正在发送数据标志
	}
	else
	{
		TxNum	= 0;
		TxPos	= 0;
		RS485EN = 0;		//禁止发送
		RxRst();   			//接收发送复位
	}
}
//------------------------------------------------------------------------------
void StartTX(void)			//开始发送数据
{
	RS485EN = 1;	        //使能发送	
	UCSRA|= 0x40;	//关键！！！
	UCSRB|= 0x08;
	UDR	=SendBuf[0];    	//输出第1个数据
    SendLast=SendBuf[0];	//保存发送的数据,以便检测冲突否
	TxPos	= 0;	        //发送数据指针置0
    bTransmitting=1;        //设置正在发送数据标志
}
//------------------------------------------------------------------------------
#pragma vector=USART_RXC_vect
__interrupt  void uart0_rx_isr(void)
{							//接收中断
unsigned char c;	
unsigned int iTime=0;

	bNetworkBusy =1;        //置网络忙标志

	c=UDR;	                //接收到的数据	
 	
	TCCR1B=0x00;			//停止定时器	
	TCNT1H=DELAY_HI;		//设置延时参数, 以便侦听网络
	TCNT1L=DELAY_LO;
	TIMSK |=0X04;			//定时器中断使能
	TIFR  |=0X04;			//清溢出标志
 	TCCR1B=0x01;			//启动定时器

	if(bTransmitting)		//正在发送数据
	{
		bTransmitting=0;    //清除正在发送数据标志
		if(c!=SendLast)   	//检测是否发生冲突
		{
			UCSRB &= 0xF7;	//发生冲突, 停止发送
			PORTD_Bit1=1;	//发送口停止
			iTime=rand()/3+TWO_MS;//得到一个不小于2ms的随机延时参数
			iTime=0xffff-iTime;
			TCCR1B=0x00;	//重装定时参数
			TCNT1H=(unsigned char)(iTime/256);
			TCNT1L=(unsigned char)(iTime%256);
			TCCR1B=0x01;	//启动定时器
			TxNum =0;
			bCollisionWait=1;
		}
	}
	else
	{
		RxNow=c;
		switch(RxStep)/*处理接收的数据*/
		{
			case 0:	if(RxNow==0xAA)	//判断起始符
					{
						RevBuf[RxPos]=RxNow;	
						RxStep=1;
						RxPos++;
					}
					else RxRst();	//复位接收
			break;
			case 1:	if(RxNow==0xAA)	//判断起始符
					{
						RxStep=2;
						RevBuf[RxPos]=RxNow;
						RxPos++;
					}
					else RxRst();	//复位接收
			break;
			case 2: RxStep=3;  //address hign
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
			case 3: RxStep=4; //address low
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
			case 4: RxStep=5; //Lenght hign
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
			case 5: RxStep=7;
					RevBuf[RxPos]=RxNow; //Lenght low
					RxPos++;
			//					if((RevBuf[1]==0x10)||(RevBuf[1]==0x11))
			//					{
			//						RxStep=6;
			//						RxDataLenght=RxNow*2;
			//					}
			//					else	RxStep=7;
			break;
			//			case 6://xinxi data!
			//					RevBuf[RxPos]=RxNow;
			//					RxDataLenght--;
			//					if(RxDataLenght==0)	RxStep=7;
			//					RxPos++;
			//			break;
			case 7: RxStep=8; //CRC low
					RevBuf[RxPos]=RxNow;
					RxPos++;
			break;
			case 8: RevBuf[RxPos]=RxNow;//CRC hign
					RxPos++;
					UartProcess();
			break;
			default:RxRst();		//reset RX!
			break;
		}
	}
}
//------------------------------------------------------------------------------
void UartProcess()
{
unsigned char i;

	Crc	= 0xffff;					//receive over￡?crc16
	for(i=0;i<RxPos;i++)
	{
		CalcCrc(RevBuf[i]);
	}
	if(Crc==0)						//Ok!
	{
		switch (RevBuf[2])	//judge function
		{
			case 0x03:	SendBuf[0]=RevBuf[0];
						SendBuf[1]=RevBuf[1];
						SendBuf[2]=RevBuf[2];
						OCR2=SendBuf[3]=RevBuf[3];//试验pwm调亮度
						SendBuf[4]=RevBuf[4];
						SendBuf[5]=RevBuf[5];
						TxNum=6;
						ApplyTx=1;
			break;
			case 0x10:	SendBuf[0]=RevBuf[0];
						SendBuf[1]=RevBuf[1];
						SendBuf[2]=RevBuf[2];
						SendBuf[3]=RevBuf[3];
						SendBuf[4]=RevBuf[4];
						SendBuf[5]=RevBuf[5];
						Crc	= 0xffff;	//crc16
						for(i=0;i<6;i++)
						{
							SendBuf[i]=RevBuf[i];
							CalcCrc(SendBuf[i]);
						}
						SendBuf[6]=(unsigned char)Crc;
						SendBuf[7]=(unsigned char)(Crc/256);
						TxNum=8;
						ApplyTx=1;
			break;
		    default:	RxRst();	//reset RX!
		    break;
		}
	}
	else RxRst();					//reset RX!
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
//	RS485EN = 0;		//发送停止
	RxStep	= 0;
	RxPos	= 0;
}
//------------------------------------------------------------------------------
void SysInit(void)
{
	 _CLI();
	//IO口初始化
	DDRA   = 0x00;	//A口为键盘输入口
	PORTA  = 0xFF;
	DDRB   = 0xFF;	//B口为按键灯输出控制
	PORTB  = 0x00;
	DDRC   = 0xC3;	//C口为踢狗，485使能，外接存储器口
	PORTC  = 0x00;

	DDRD   = 0xF2;	//D口高四位控制灯 pwm，低为串口，外中断
	PORTD  = 0x00;

	//USART 初始化
    UCSRC = 0x06;    //USART 9600 8, n,1无倍速
    UBRRL = (F_CPU/BAUDRATE/16-1)%256;//U2X=0时的公式计算
    UBRRH = (F_CPU/BAUDRATE/16-1)/256;
    UCSRA = 0x00;
    UCSRA|= 0x40;	//关键！！！
    UCSRB = 0xd8;	//使能接收 发送中断，使能接收，使能发送
	
	TCCR0  = 0x00;	//停止定时器
	TCNT0  = 0x53;	//初始值
	OCR0   = 0x52;	//匹配值
	TIMSK |= 0x01;	//中断允许
	TIFR  |= 0X01;
	TCCR0  = 0x04;	//启动定时器
	
	TCCR2  = 0x00;	//停止定时器
	ASSR   = 0x00;	//异步时钟模式
	TCNT2  = 0x00;	//初始值
	OCR2   = 0x1e;	//匹配值
	TCCR2  = 0x7D;	//启动定时器
	
	bNetworkBusy 	=0;
	bTransmitting 	=0;
	bCollisionWait 	=0;
	ApplyTx	 	=0;
	RS485EN		=0;
}
//------------------------------------------------------------------------------
void KeyDeal() //键盘处理
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
		//可以添加有双键识别
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

	if(cKeyColumn!=0x3F)	//有键按下
    {
      	if(g_bReEntry>20) 	//已消抖
        {
			for(n=0;n<7;n++)
			{
				if(cScanWord[n]==cKeyColumn) break;	//求键值
			}
			cKey=cScanWord[n];
			
			if(g_cOldKey!=cKey)
			{
				g_cOldKey=cKey;		//按一次只能动一次
			}
			else  			//重键
			{
				if(g_cReKeyTimes>LONGTIME) return 0x3F;
				if(g_cReKeyTimes==LONGTIME) //2S
			   	{
                    cKey=cKey|0x40;			//长键键值
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
      	else g_bReEntry++; //没消抖
    }
  	else
	{
		if(g_cOldKey!=0x3F)	//判断点动释放
		{
			g_bReEntry=0;	//消抖清零
			g_cReKeyTimes=0;//长按键控制清零
			cKey=g_cOldKey|0x80;//按键标志设置
			g_cOldKey=0x3F;
		}
		else //无键按下
		{
			g_bReEntry	=0;	//消抖清零
			g_cReKeyTimes=0;//长按键控制清零
		}
	}
	return cKey;
}
//------------------------------------------------------------------------------