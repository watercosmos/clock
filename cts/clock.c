/* clock.c */
#include "time.h"
#include "clock.h"
#include "table.h"
#include "timeapi.h"

volatile unsigned char filled;
volatile unsigned int  length;
int k=0;

void sys_init(void);
void start_tx(unsigned char num);
void delay_10ms(void);
void rx_rst(void);
void crc_check(void);
void calc_crc(unsigned char buf);
void rx_handler(void);
void main(void)
{
	unsigned char i, j, task_seq;

	/* 初始化 */
	sys_init();
	_SEI();

	TOTX = 1;

	/*
	 *	主循环
	 *
	 *	收帧处理
	 *	逻辑表自查
	 */
	while (1) {
		if (TOTX && !BUSY) {
			start_tx(4);
			TOTX = 0;
		}
		/*
		 *	收帧处理
		 *
		 *	完成逻辑表设置帧处理
		 *	包括添加逻辑表和修改逻辑表使能状态
		 *	若逻辑使能，序列存在
		 *	则将该逻辑下一次触发时间写入时间表
		 */
		if (filled)
			k=11;
			rx_handler();
		/*
		 *	遍历逻辑表
		 *	
		 *	若逻辑表触发
		 *	则向继电器发帧
		 *	若序列完成，则计算下一次触发时间，写入时间表
		 */
		for (i = 0; i < logic_num; i++) {
			if (logic_entry[i].trigger == 1) {
				//组帧发往继电器
			}
		}

		/* 踢狗 */
		WDI = 0;
		delay_10ms();
		WDI = 1;
	}
}

/* 接收中断 */
#pragma vector=USART0_RXC_vect
__interrupt void uart0_rx_isr(void)
{
	
	unsigned char rx;
	unsigned int backoff;

	BUSY = 1;

	/* 重启定时器1 */
	TCCR1B = 0x00;			//停止定时器
	TCNT1H = DELAY_HI;		//设置延时参数, 以便侦听网络
	TCNT1L = DELAY_LO;
	TIMSK |= 0x04;			//定时器中断使能	
	TIFR  |= 0x04;			//清溢出标志
	TCCR1B = 0x01;			//启动定时器

	if (TX) {				//接收自己发出的数据
		rx = UDR0;
		TX = 0;
		if (rx != tx_last) {
			UCSR0B    &= 0xF7;					//发生冲突, 停止发送
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
		rx_now = UDR0;
		switch (rx_step) {
			case 0:
                          k=12;
				if (rx_now == 0xAA) {
					rx_step++;
				} else{
                                  k=98;
					rx_rst();
                                }
				break;
			case 1:
                          k=13;
				if (rx_now == 0xAA) {
					rx_step++;
				} else
					rx_rst();
				break;
			case 2:
                          k=14;
				rx_buf[rx_pos] = rx_now;	//目的设备ID
				rx_step++;
				rx_pos ++;
				break;
			case 3:
				rx_buf[rx_pos] = rx_now;	//目的子网ID
				rx_step++;
				rx_pos ++;
				break;
			case 4:
				rx_buf[rx_pos] = rx_now;	//源设备ID
				rx_step++;
				rx_pos ++;
				break;
			case 5:				//这里该加地址校验
				rx_buf[rx_pos] = rx_now;	//源子网ID
				rx_step++;
				rx_pos ++;
				break;
			case 6:
                          k=15;
				rx_buf[rx_pos] = rx_now;	//分组长度
				length = rx_now;
				rx_step++;
				rx_pos ++;
				break;
			case 7:
				rx_buf[rx_pos] = rx_now;	//转发网关子网ID
				rx_step++;
				rx_pos ++;
				break;
			case 8:
				rx_buf[rx_pos] = rx_now;	//分组序号
				rx_step++;
				rx_pos ++;
				break;
			case 9:				//这里应该做命令类型校验（似乎和地址校验功能冲突）
				rx_buf[rx_pos] = rx_now;	//命令大类
				rx_step++;
				rx_pos ++;
				break;
			case 10:
				rx_buf[rx_pos] = rx_now;	//命令小类
				rx_step++;
				rx_pos ++;
				break;
			case 11:
				rx_buf[rx_pos] = rx_now;	//命令结果
				rx_step++;
				rx_pos ++;
				break;
			case 12:
                          k=22;
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
                          k=23;
				rx_buf[rx_pos] = rx_now;	//crc
				rx_pos ++;
				crc_check();
				break;
			default:
				rx_rst();
	}

}
}

/* CRC校验函数 */
void crc_check(void)
{
	unsigned char i;

	crc = 0xffff;
	for (i = 0; i < rx_pos; i++)
		calc_crc(rx_buf[i]);
	if (crc == 0)
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
#pragma vector=USART0_TXC_vect
__interrupt void uart0_tx_isr(void)
{
	tx_pos++;
	if (tx_pos < tx_num) {				//若一帧未发完，继续发下一字节
		UDR0     = tx_buf[tx_pos];
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
	unsigned char i;
	//bool check = FALSE;

	filled = 0;

	if ((rx_buf[8] & 0x3F) == 0x03) {
		/*
		 *	接收到设置逻辑的命令
		 *	命令小类 = 3
		 *	若逻辑表已满，不做处理
		 *	这里假设收到的帧的数据域就是一条完整的逻辑表项
		 */
          k=33;
		/*for (i = 0; i < logic_num; i++)
			if ((rx_buf[11] & 0x7f) == logic_entry[i].logic_seq) {
				//check = TRUE;
				break;
			}
			*//*
		if (check && logic_entry[i].enable != 接收帧内逻辑的使能位) {
			logic_entry[i].enable = 接收帧内逻辑的使能位;
			logic_init(logic_entry[i]);
		}
		*/
		 if (logic_num < MAX_LOGIC_TABLE_SIZE) {	//新的逻辑，逻辑表未满，则加入逻辑表
			memcpy(&logic_entry[logic_num], &rx_buf[10], sizeof(Logic_Entry));
                        
                        rx_buf[0]=logic_entry[logic_num].logic_sum;
                      
                        start_tx(1);
			//logic_init(logic_entry[logic_num]);
			logic_num++;
		}
	} else if ((rx_buf[8] & 0x3F) == 0x07) {
		/*
		 *	接收到设置序列的命令
		 *	命令小类 = 7
		 *	若序列表已满，不做处理
		 *	这里假设收到的帧的数据域就是一条完整的序列表项
		 *	未考虑重复表项
		 */
		/*memcpy(&task_entry[task_num], &rx_buf[10], sizeof(Task_Entry));
		task_num++;*/
	}
	rx_rst();
}

/* 逻辑初始函数，逻辑新增或使能位改变时触发 */
/*
void logic_init(Logic_Entry le)
{
	unsigned char i;
	int ret;

	if (le.enable == 0) {
		//从时间表内删除该逻辑
		for (i = 0; i < time_num; i++)
			if (time_entry[i].logic_seq == le.logic_seq) {
				del_time_entry(i);
				i--;
			}
	} else {
		I2CReadDate(now);
		ret = time_cmp(now, le.cond2.start_time);
		if (ret > 0) {
			*//*if (le.cond2.loop_flag == 0)		//单次任务
				return;
			else (le.cond2.loop_flag == 1) {	//循环任务
				switch (le.cond2.loop_end_flag) {
					case 1:		//以时间结束
						ret = date_cmp(&now, le.cond2.end_date)
						if (ret > 0)
							return;
					case 2:		//以循环次数结束
						//
					case 0:		//无结束
					default:
						switch (le.cond2.loop_unit) {
							case 0:		//循环月
								do {
									if (now.month + le.cond2.interval)
								}
							case 1:		//循环周
							case 2:		//循环日
						}
				}
			}
			return;*//*
		}
		else if (ret < 0) {
			//
		}
	}
}
*/
/* 初始化函数 */
void sys_init(void)
{
	_CLI();
    
    DDRE |= 0x64;
    DDRF |= 0x08;
	/* USART0 初始化 */
	UCSR0C  = 0x06;		//USART0 9600 8, n,1无倍速
	UBRR0L  = (F_CPU/BAUDRATE/16-1)%256;	//U2X=0时的公式计算
	UBRR0H  = (F_CPU/BAUDRATE/16-1)/256;
	UCSR0A  = 0x00;
	UCSR0A |= 0x40;		//关键！！！
	UCSR0B= 0xd8;		//使能接收 发送中断，使能接收，使能发送

	TCCR0  = 0x00;		//停止定时器
	TCNT0  = 0x53;		//初始值
	OCR0   = 0x52;		//匹配值
	TIMSK |= 0x01;		///中断允许
	TIFR  |= 0x01;
	TCCR0  = 0x04;		//启动定时器

	TCCR2 = 0x00;		//停止定时器
	
	TIMSK &= 0x3F;
	
	//这里为什么要初始化rx_buf[7]和filled？？？？？？？？？？？
	/*
	BUSY      = 0;
	TX        = 0;
	COLLISION = 0;
	TOTX      = 0;
	RS485EN	  = 0;
	*/
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
	UCSR0A |= 0x40;			//关键！！！
	UCSR0B |= 0x08;
	tx_num  = num;
	UDR0     = tx_buf[0];	//输出第1个数据
	tx_last = tx_buf[0];	//保存发送的数据,以便检测冲突否
	tx_pos  = 0;
	TX      = 1;
}

/* 接收重置 */
void rx_rst(void)
{
	rx_step = 0;
	rx_pos  = 0;
}