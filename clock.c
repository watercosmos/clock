/* clock.c */
#include <"clock.h">

unsigned char tx_buf[20] = {'A', 'B', 'C', 'D'};
unsigned char tx_num;
unsigned char tx_pos;
unsigned char tx_last;

unsigned char rx_buf[20];
unsigned char rx_step;
unsigned char rx_pos;
unsigned char rx_now;
unsigned char crc;

volatile unsigned char filled;
volatile unsigned int  length;

void sys_init(void);
void delay_10ms(void);
void start_tx(unsigned char num);
void rx_rst(void);
void crc_check(void);
void calc_crc(unsigned char buf);

void main(void)
{
	/* 初始化 */
	sys_init();
	_SEI();

	TOTX = 1;

	/*
	 *	主循环
	 *	逻辑表自查
	 */
	while (1) {
		if (TOTX && !BUSY) {
			start_tx(4);
			TOTX = 0;
		}
		if (filled)
			rx_handler();

		/* 踢狗 */
		WDI = 0;
		delay_10ms();
		WDI = 1;
	}
}

/* 接收中断 */
#pragma vector=USART_RXC_vect
__interrupt void uart0_rx_isr(void)
{
	
	unsigned char rx;
	unsigned int backoff;

	BUSY = 1;

	/* 重启定时器1 */
	TCCR1B = 0x00;			//停止定时器	
	TCNT1H = DELAY_HI;		//设置延时参数, 以便侦听网络
	TCNT1L = DELAY_LO;
	TIMSK |= 0X04;			//定时器中断使能
	TIFR  |= 0X04;			//清溢出标志
	TCCR1B = 0x01;			//启动定时器

	if (TX) {				//接收自己发出的数据
		rx = UDR;
		TX = 0;
		if (rx != tx_last) {
			UCSRB     &= 0xF7;					//发生冲突, 停止发送
			PORTD_Bit1 = 1;						//发送口停止
			backoff    = rand()/3 + TWO_MS;		//得到一个不小于2ms的随机延时参数
			backoff    = 0xffff - backoff;
			TCCR1B     = 0x00;					//重装定时参数
			TCNT1H     = (unsigned char)(backoff / 256);
			TCNT1L     = (unsigned char)(backoff % 256);
			TCCR1B     = 0x01;					//启动定时器
			tx_num     = 0;
			COLLISION  = 1;
		}
	} else {				//接收其他模块数据
		rx_now = UDR;
		switch (rx_step) {
			case 0:
				if (rx_now == 0xAA) {
					rx_step++;
				} else
					rx_rst();
				break;
			case 1:
				if (rx_now == 0xAA) {
					rx_step++；
				} else
					rx_rst();
				break;
			case 2:
				rx_buf[rx_pos] = rx_now;	//目的设备ID
				rx_step++；
				rx_pos ++;
				break;
			case 3:
				rx_buf[rx_pos] = rx_now;	//目的子网ID
				rx_step++；
				rx_pos ++;
				break;
			case 4:
				rx_buf[rx_pos] = rx_now;	//源设备ID
				rx_step++；
				rx_pos ++;
				break;
			case 5:				//这里该加地址校验
				rx_buf[rx_pos] = rx_now;	//源子网ID
				rx_step++；
				rx_pos ++;
				break;
			case 6:
				rx_buf[rx_pos] = rx_now;	//分组长度
				length = rx_now;
				rx_step++；
				rx_pos ++;
				break;
			case 7:
				rx_buf[rx_pos] = rx_now;	//转发网关子网ID
				rx_step++；
				rx_pos ++;
				break;
			case 8:
				rx_buf[rx_pos] = rx_now;	//分组序号
				rx_step++；
				rx_pos ++;
				break;
			case 9:				//这里应该做命令类型校验（似乎和地址校验功能冲突）
				rx_buf[rx_pos] = rx_now;	//命令大类
				rx_step++；
				rx_pos ++;
				break;
			case 10:
				rx_buf[rx_pos] = rx_now;	//命令小类
				rx_step++；
				rx_pos ++;
				break;
			case 11:
				rx_buf[rx_pos] = rx_now;	//命令结果
				rx_step++；
				rx_pos ++;
				break;
			case 12:
				if (length == 0) {			//payload
					rx_step++;
					break;
				} else {
					length--;
					rx_buf[rx_pos] = rx_now;
					rx_pos ++;
				}
				break;
			case 13:
				rx_buf[rx_pos] = rx_now;	//crc
				rx_pos ++;
				crc_check();
				break;
			default:
				rx_rst();
	}

}

/* CRC校验函数 */
void crc_check(void)
{
	unsigned char i;

	crc = 0xffff;
	for (i = 0; i < rx_pos; i++)
		calc_crc(rx_buf[i]);
	if （crc == 0)
		filled = 1;
	else
		rx_rst();		//这里是否应该都执行rx_rst，因为若接收一帧后还未处理，又开始下一次接收？
}

/* CRC校验算法 */
void calc_crc(unsigned char buf)
{
	unsigned char j, TT;
	crc = crc ^ buf;
	for (j = 0; j < 8; j++)
	{
		TT  = crc & 1;
		crc = crc >> 1;
		crc = crc & 0x7fff;
		if (TT == 1)	//??????????????????????????????????????????
		crc = crc ^ 0xa001;
		crc = crc & 0xffff;
	}
}

/* 发送中断 */
#pragma vector=USART_TXC_vect
__interrupt void uart0_tx_isr(void)
{
	tx_pos++;
	if (tx_pos < tx_num) {				//若一帧未发完，继续发下一字节
		UDR     = tx_buf[tx_pos];
		tx_last = tx_buf[tx_pos];
		TX = 1;
	} else {
		tx_num  = 0;
		tx_pos  = 0;
		RS485EN = 0;
		rx_rst();
	}
}

/* 定时器1溢出中断 */
#pragma vector=TIMER1_OVF_vect
__interrupt void t1_ovf_isr(void)
{
	TCCR1B = 0x00;		//停止定时器
	rx_rst();			//接收发送复位
	BUSY = 0;			//清网络忙标志
	if (COLLISION) {
		COLLISION = 0;
		start_tx(4);		//重新发送数据
	}
}

/* 接收帧处理函数 */
void rx_handler(void)
{
	filled = 0;
}

/* 初始化函数 */
void sys_init(void)
{
	_CLI();

	/* IO口初始化 */
	DDRA  = 0x00;		//A口为键盘输入口
	PORTA = 0xFF;
	DDRB  = 0xFF;		//B口为按键灯输出控制
	PORTB = 0x00;
	DDRC  = 0xC3;		//C口为踢狗，485使能，外接存储器口
	PORTC = 0x00;
	DDRD  = 0xF2;		//D口高四位控制灯 pwm，低为串口，外中断
	PORTD = 0x00;

	/* USART 初始化 */
	UCSRC  = 0x06;		//USART 9600 8, n,1无倍速
	UBRRL  = (F_CPU/BAUDRATE/16-1)%256;	//U2X=0时的公式计算
	UBRRH  = (F_CPU/BAUDRATE/16-1)/256;
	UCSRA  = 0x00;
	UCSRA |= 0x40;		//关键！！！
	UCSRB  = 0xd8;		//使能接收 发送中断，使能接收，使能发送
	
	TCCR0  = 0x00;		//停止定时器
	TCNT0  = 0x53;		//初始值
	OCR0   = 0x52;		//匹配值
	TIMSK |= 0x01;		//中断允许
	TIFR  |= 0X01;
	TCCR0  = 0x04;		//启动定时器
	
	TCCR2 = 0x00;		//停止定时器
	ASSR  = 0x00;		//异步时钟模式
	TCNT2 = 0x00;		//初始值
	OCR2  = 0x1e;		//匹配值
	TCCR2 = 0x7D;		//启动定时器
	//这里为什么要初始化rx_buf[7]和filled？？？？？？？？？？？
	BUSY      = 0;
	TX        = 0;
	COLLISION = 0;
	TOTX      = 0;
	RS485EN	  = 0;
}

/* 1ms延时函数，用于踢狗 */
void delay_10ms(void)
{
	unsigned int i;
	for (i = 0; i < 11400; i++);
}

/* 开始发送 */
void start_tx(unsigned char num)
{
	RS485EN = 1;			//使能发送	
	UCSRA  |= 0x40;			//关键！！！
	UCSRB  |= 0x08;
	tx_num  = num;
	UDR     = tx_buf[0];	//输出第1个数据
	tx_last = tx_buf[0];	//保存发送的数据,以便检测冲突否
	tx_pos  = 0;			//发送数据指针置0
	TX      = 1;			//设置正在发送数据标志
}

/* 接收重置 */
void rx_rst(void)
{
	rx_step = 0;
	rx_pos  = 0;
}