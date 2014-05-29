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

	/* ��ʼ�� */
	sys_init();
	_SEI();

	TOTX = 1;

	/*
	 *	��ѭ��
	 *
	 *	��֡����
	 *	�߼����Բ�
	 */
	while (1) {
		if (TOTX && !BUSY) {
			start_tx(4);
			TOTX = 0;
		}
		/*
		 *	��֡����
		 *
		 *	����߼�������֡����
		 *	��������߼�����޸��߼���ʹ��״̬
		 *	���߼�ʹ�ܣ����д���
		 *	�򽫸��߼���һ�δ���ʱ��д��ʱ���
		 */
		if (filled)
			k=11;
			rx_handler();
		/*
		 *	�����߼���
		 *	
		 *	���߼�����
		 *	����̵�����֡
		 *	��������ɣ��������һ�δ���ʱ�䣬д��ʱ���
		 */
		for (i = 0; i < logic_num; i++) {
			if (logic_entry[i].trigger == 1) {
				//��֡�����̵���
			}
		}

		/* �߹� */
		WDI = 0;
		delay_10ms();
		WDI = 1;
	}
}

/* �����ж� */
#pragma vector=USART0_RXC_vect
__interrupt void uart0_rx_isr(void)
{
	
	unsigned char rx;
	unsigned int backoff;

	BUSY = 1;

	/* ������ʱ��1 */
	TCCR1B = 0x00;			//ֹͣ��ʱ��
	TCNT1H = DELAY_HI;		//������ʱ����, �Ա���������
	TCNT1L = DELAY_LO;
	TIMSK |= 0x04;			//��ʱ���ж�ʹ��	
	TIFR  |= 0x04;			//�������־
	TCCR1B = 0x01;			//������ʱ��

	if (TX) {				//�����Լ�����������
		rx = UDR0;
		TX = 0;
		if (rx != tx_last) {
			UCSR0B    &= 0xF7;					//������ͻ, ֹͣ����
			PORTD_Bit1 = 1;						//���Ϳ�ֹͣ
			backoff    = rand()/3 + TWO_MS;		//�õ�һ����С��2ms�������ʱ����
			backoff    = 0xffff - backoff;
			TCCR1B     = 0x00;					//��װ��ʱ����
			TCNT1H     = (unsigned char)(backoff / 256);
			TCNT1L     = (unsigned char)(backoff % 256);
			TCCR1B     = 0x01;					//������ʱ��
			tx_num     = 0;
			COLLISION  = 1;
		}
	} else {				//��������ģ������
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
				rx_buf[rx_pos] = rx_now;	//Ŀ���豸ID
				rx_step++;
				rx_pos ++;
				break;
			case 3:
				rx_buf[rx_pos] = rx_now;	//Ŀ������ID
				rx_step++;
				rx_pos ++;
				break;
			case 4:
				rx_buf[rx_pos] = rx_now;	//Դ�豸ID
				rx_step++;
				rx_pos ++;
				break;
			case 5:				//����üӵ�ַУ��
				rx_buf[rx_pos] = rx_now;	//Դ����ID
				rx_step++;
				rx_pos ++;
				break;
			case 6:
                          k=15;
				rx_buf[rx_pos] = rx_now;	//���鳤��
				length = rx_now;
				rx_step++;
				rx_pos ++;
				break;
			case 7:
				rx_buf[rx_pos] = rx_now;	//ת����������ID
				rx_step++;
				rx_pos ++;
				break;
			case 8:
				rx_buf[rx_pos] = rx_now;	//�������
				rx_step++;
				rx_pos ++;
				break;
			case 9:				//����Ӧ������������У�飨�ƺ��͵�ַУ�鹦�ܳ�ͻ��
				rx_buf[rx_pos] = rx_now;	//�������
				rx_step++;
				rx_pos ++;
				break;
			case 10:
				rx_buf[rx_pos] = rx_now;	//����С��
				rx_step++;
				rx_pos ++;
				break;
			case 11:
				rx_buf[rx_pos] = rx_now;	//������
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

/* CRCУ�麯�� */
void crc_check(void)
{
	unsigned char i;

	crc = 0xffff;
	for (i = 0; i < rx_pos; i++)
		calc_crc(rx_buf[i]);
	if (crc == 0)
		filled = 1;
	else
		rx_rst();		//�����Ƿ�Ӧ�ö�ִ��rx_rst����Ϊ������һ֡��δ�����ֿ�ʼ��һ�ν��գ�
}

/* CRCУ���㷨 */
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

/* �����ж� */
#pragma vector=USART0_TXC_vect
__interrupt void uart0_tx_isr(void)
{
	tx_pos++;
	if (tx_pos < tx_num) {				//��һ֡δ���꣬��������һ�ֽ�
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

/* ��ʱ��1����ж� */
#pragma vector=TIMER1_OVF_vect
__interrupt void t1_ovf_isr(void)
{
	TCCR1B = 0x00;		//ֹͣ��ʱ��
	rx_rst();			//���շ��͸�λ
	BUSY = 0;			//������æ��־
	if (COLLISION) {
		COLLISION = 0;
		start_tx(4);		//���·�������
	}
}

/* ����֡������ */

void rx_handler(void)
{
	unsigned char i;
	//bool check = FALSE;

	filled = 0;

	if ((rx_buf[8] & 0x3F) == 0x03) {
		/*
		 *	���յ������߼�������
		 *	����С�� = 3
		 *	���߼�����������������
		 *	��������յ���֡�����������һ���������߼�����
		 */
          k=33;
		/*for (i = 0; i < logic_num; i++)
			if ((rx_buf[11] & 0x7f) == logic_entry[i].logic_seq) {
				//check = TRUE;
				break;
			}
			*//*
		if (check && logic_entry[i].enable != ����֡���߼���ʹ��λ) {
			logic_entry[i].enable = ����֡���߼���ʹ��λ;
			logic_init(logic_entry[i]);
		}
		*/
		 if (logic_num < MAX_LOGIC_TABLE_SIZE) {	//�µ��߼����߼���δ����������߼���
			memcpy(&logic_entry[logic_num], &rx_buf[10], sizeof(Logic_Entry));
                        
                        rx_buf[0]=logic_entry[logic_num].logic_sum;
                      
                        start_tx(1);
			//logic_init(logic_entry[logic_num]);
			logic_num++;
		}
	} else if ((rx_buf[8] & 0x3F) == 0x07) {
		/*
		 *	���յ��������е�����
		 *	����С�� = 7
		 *	�����б���������������
		 *	��������յ���֡�����������һ�����������б���
		 *	δ�����ظ�����
		 */
		/*memcpy(&task_entry[task_num], &rx_buf[10], sizeof(Task_Entry));
		task_num++;*/
	}
	rx_rst();
}

/* �߼���ʼ�������߼�������ʹ��λ�ı�ʱ���� */
/*
void logic_init(Logic_Entry le)
{
	unsigned char i;
	int ret;

	if (le.enable == 0) {
		//��ʱ�����ɾ�����߼�
		for (i = 0; i < time_num; i++)
			if (time_entry[i].logic_seq == le.logic_seq) {
				del_time_entry(i);
				i--;
			}
	} else {
		I2CReadDate(now);
		ret = time_cmp(now, le.cond2.start_time);
		if (ret > 0) {
			*//*if (le.cond2.loop_flag == 0)		//��������
				return;
			else (le.cond2.loop_flag == 1) {	//ѭ������
				switch (le.cond2.loop_end_flag) {
					case 1:		//��ʱ�����
						ret = date_cmp(&now, le.cond2.end_date)
						if (ret > 0)
							return;
					case 2:		//��ѭ����������
						//
					case 0:		//�޽���
					default:
						switch (le.cond2.loop_unit) {
							case 0:		//ѭ����
								do {
									if (now.month + le.cond2.interval)
								}
							case 1:		//ѭ����
							case 2:		//ѭ����
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
/* ��ʼ������ */
void sys_init(void)
{
	_CLI();
    
    DDRE |= 0x64;
    DDRF |= 0x08;
	/* USART0 ��ʼ�� */
	UCSR0C  = 0x06;		//USART0 9600 8, n,1�ޱ���
	UBRR0L  = (F_CPU/BAUDRATE/16-1)%256;	//U2X=0ʱ�Ĺ�ʽ����
	UBRR0H  = (F_CPU/BAUDRATE/16-1)/256;
	UCSR0A  = 0x00;
	UCSR0A |= 0x40;		//�ؼ�������
	UCSR0B= 0xd8;		//ʹ�ܽ��� �����жϣ�ʹ�ܽ��գ�ʹ�ܷ���

	TCCR0  = 0x00;		//ֹͣ��ʱ��
	TCNT0  = 0x53;		//��ʼֵ
	OCR0   = 0x52;		//ƥ��ֵ
	TIMSK |= 0x01;		///�ж�����
	TIFR  |= 0x01;
	TCCR0  = 0x04;		//������ʱ��

	TCCR2 = 0x00;		//ֹͣ��ʱ��
	
	TIMSK &= 0x3F;
	
	//����ΪʲôҪ��ʼ��rx_buf[7]��filled����������������������
	/*
	BUSY      = 0;
	TX        = 0;
	COLLISION = 0;
	TOTX      = 0;
	RS485EN	  = 0;
	*/
}

/* 1ms��ʱ�����������߹� */
void delay_10ms(void)
{
	unsigned int i;
	for (i = 0; i < 11400; i++);
}

/* ��ʼ���� */
void start_tx(unsigned char num)
{
	RS485EN = 1;			//ʹ�ܷ���
	UCSR0A |= 0x40;			//�ؼ�������
	UCSR0B |= 0x08;
	tx_num  = num;
	UDR0     = tx_buf[0];	//�����1������
	tx_last = tx_buf[0];	//���淢�͵�����,�Ա����ͻ��
	tx_pos  = 0;
	TX      = 1;
}

/* �������� */
void rx_rst(void)
{
	rx_step = 0;
	rx_pos  = 0;
}