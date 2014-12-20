/* clock.c */
#include "clock.h"
#include "time.h"
#include "sd2400.h"
#include "assemble.h"

volatile unsigned char filled;
volatile unsigned int  length;

void sys_init(void);
void start_tx(void);
void rx_rst(void);
void crc_check(void);
void rx_handler(void);
void delay_10ms(void);
void time_loop(void);
void logic_loop(void);

void main(void)
{
    sys_init();
    _SEI();

    TOTX = 1;

    while (1) {
        if (filled)
            rx_handler();

        if (TOTX && !BUSY) {
            start_tx();
            TOTX = 0;
        }

        time_loop();
        logic_loop();

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
    unsigned char rx_now;
    unsigned int backoff;

    BUSY = 1;

    /* 重启定时器1 */
    TCCR1B = 0x00;            //停止定时器
    TCNT1H = DELAY_HI;        //设置延时参数, 以便侦听网络
    TCNT1L = DELAY_LO;
    TIMSK |= 0x04;            //定时器中断使能    
    TIFR  |= 0x04;            //清溢出标志
    TCCR1B = 0x01;            //启动定时器

    if (TX) {                 //接收自己发出的数据
        rx = UDR0;
        TX = 0;
        if (rx != tx_last) {
            UCSR0B    &= 0xF7;                     //发生冲突, 停止发送
            PORTE_Bit1 = 1;                        //发送口停止
            backoff    = rand()/3 + TWO_MS;        //得到一个不小于2ms的随机延时参数
            backoff    = 0xffff - backoff;
            TCCR1B     = 0x00;                     //重装定时参数
            TCNT1H     = (unsigned char)(backoff / 256);
            TCNT1L     = (unsigned char)(backoff % 256);
            TCCR1B     = 0x01;                     //启动定时器
            tx_num     = 0;
            COLLISION  = 1;
        }
    } else {                //接收其他模块数据
        rx_now = UDR0;
        switch (rx_step) {
            case 0:
            case 1:
                if (rx_now == 0xAA)
                    rx_step++;
                else
                    rx_rst();
                break;
            case 2:
                rx_buf[rx_pos] = rx_now;    //目的设备ID
                if (rx_now != dev_id) {
                    rx_rst();
                    break;
                }
                rx_step++;
                rx_pos ++;
                break;
            case 3:        //目的子网ID
            case 4:        //源设备ID
            case 5:        //源子网ID
            case 7:        //转发网关子网ID
            case 8:        //分组序号
            case 9:        //命令大类
            case 10:       //命令小类
            case 11:       //命令结果
                rx_buf[rx_pos] = rx_now;
                rx_step++;
                rx_pos ++;
                break;
            case 6:            //分组长度
                rx_buf[rx_pos] = rx_now;
                length = rx_now;
                rx_step++;
                rx_pos ++;
                break;
            case 12:        //payload
                rx_buf[rx_pos] = rx_now;
                rx_pos ++;
                if (length == 0)
                    rx_step++;
                else
                    length--;
                break;
            case 13:
                rx_buf[rx_pos] = rx_now;    //crc
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
        rx_rst();    //这里是否应该都执行rx_rst，因为若接收一帧后还未处理，又开始下一次接收？
}

/* 接收帧处理函数 */
void rx_handler(void)
{
    filled = 0;
    if (enable != 1 && (rx_buf[8] & 0xBF) != 0x80
                    && (rx_buf[8] & 0xBF) != 0x82
                    && (rx_buf[8] & 0xBF) != 0x85) {
        rx_rst();
        return;
    }
    if ((rx_buf[7] & 0x3F) == 0x00) {
        switch (rx_buf[8] & 0xBF) {
            case 0x80:
                tx_status();
                break;
            case 0x81:
                set_abstract();
                break;
            case 0x82:
                tx_abstract();
                break;
            case 0x83:
                tx_mac();
                break;
            case 0x84:
                if (!memcmp(MAC, rx_buf + 12, 8))
                    set_id();
                break;
            case 0x85:
                set_enable();
                break;
            default:
                break;
        }
    } else if ((rx_buf[7] & 0x3F) == 0x05) {
        switch (rx_buf[8] & 0xBF) {
            case 0x80:
                set_time();
                break;
            case 0x81:
                tx_time();
                break;
            case 0x82:
                tx_logic_sum();
                break;
            case 0x83:
                set_logic();
                break;
            case 0x84:
                tx_logic_entry();
                break;
            case 0x87:
                set_logic_enable();
                break;
            case 0x88:
                clear_logic();
                break;
            case 0x89:
                del_logic();
                break;
            default:
                break;
        }
    }
    //no need to do this for switch
    /*else if ((rx_buf[7] & 0x3F) == 0x05 && (rx_buf[8] & 0xBF) == 0xBF) {
        response_to_switch();
    }*/
    rx_rst();
}

/* 发送中断 */
#pragma vector=USART0_TXC_vect
__interrupt void uart0_tx_isr(void)
{
    tx_pos++;
    if (tx_pos < tx_num) {
        UDR0    = tx_buf[tx_pos];
        tx_last = tx_buf[tx_pos];
        TX = 1;
    } else {
        tx_num  = 0;
        tx_pos  = 0;
        RS485EN = 0;
        memset(tx_buf, 0, MAX_TX_BUF_SIZE);
        rx_rst();
    }
}

/* 定时器1溢出中断 */
#pragma vector=TIMER1_OVF_vect
__interrupt void t1_ovf_isr(void)
{
    TCCR1B = 0x00;        //停止定时器
    rx_rst();             //接收发送复位
    BUSY = 0;             //清网络忙标志
    if (COLLISION) {
        COLLISION = 0;
        start_tx();       //重新发送数据
    }
}

/* 定时器0中断 
#pragma vector=TIMER0_OVF_vect
__interrupt void t0_ovf_isr(void)
{
    timer++;

    if (timer < 100)
        return;

    timer = 0;
    time_loop();
    logic_loop();
}*/

/* 初始化函数 */
void sys_init(void)
{
    _CLI();

    DDRE  = 0x64;
    PORTE = 0x00;
    DDRF  = 0x00;        //第四位置1导致时间无法读写
    PORTF = 0x00;
    DDRD  = 0x08;
    PORTD = 0x00;

    WDTCR = 0x00;

    UCSR0C  = 0x06;       //USART0 9600 8, n,1无倍速
    UBRR0L  = (F_CPU/BAUDRATE/16-1)%256;    //U2X=0时的公式计算
    UBRR0H  = (F_CPU/BAUDRATE/16-1)/256;
    UCSR0A  = 0x00;
    UCSR0A |= 0x40;       //关键！！！
    UCSR0B  = 0xd8;       //使能接收 发送中断，使能接收，使能发送

    /* 定时器0 4ms
    TCCR0  = 0x00;        //停止定时器
    TCNT0  = 0x53;        //初始值 4 ms
    OCR0   = 0x52;        //匹配值 无效，这里是溢出中断
    TIMSK |= 0x01;        ///中断允许
    TIFR  |= 0x01;
    TCCR0  = 0x04;        //启动定时器，256分频
    */

    TCCR2  = 0x00;        //停止定时器

    TIMSK &= 0x3F;
}

/* 开始发送 */
void start_tx(void)
{
    RS485EN = 1;            //使能发送
    UCSR0A |= 0x40;         //关键
    UCSR0B |= 0x08;
    UDR0    = tx_buf[0];    //输出第1个数据
    tx_last = tx_buf[0];    //保存发送的数据,以便检测冲突否
    tx_pos  = 0;
    TX      = 1;
}

/* 接收重置 */
void rx_rst(void)
{
    rx_step = 0;
    rx_pos  = 0;
}

/* 1ms延时函数，用于踢狗 */
void delay_10ms(void)
{
  unsigned int i;
  for (i = 0; i < 11400; i++);
}

/**
 * 遍历时间表
 * 若时间匹配，则将对应逻辑的条件1置为真
 * 并计算该逻辑的下一次触发时间，加入时间表
 * 最后从时间表内删除当前时间表项
 */
void time_loop(void)
{
    unsigned char i, j;
    unsigned char n = time_sum;

    I2CReadDate(&now);
    for (i = 0; i < n; i++) {
        if (time_cmp(&now, &(time_entry[i].time)))
            continue;

        for (j = 0; j < logic_sum; j++) {
            if (logic_entry[j].logic_seq == time_entry[i].logic_seq) {
                logic_entry[j].cond1_bool = 1;
                calc_time(&(logic_entry[j].cond1),
                            logic_entry[j].logic_seq);
            }
        }
        del_time(i);
        break;
    }
}

/**
 * 遍历逻辑表
 * 若逻辑未开启，重置逻辑内的时间条件为0
 * 若逻辑触发，发送指令给继电器，并将时间条件重置为0
 */
void logic_loop(void)
{
    unsigned char i, enable = 0;

    for (i = 0; i < logic_sum; i++) {
        if (logic_entry[i].enable == 0)
            continue;

        switch(logic_entry[i].logic_operator) {
            case 0:            //逻辑与
                if (logic_entry[i].cond1_bool &&
                    logic_entry[i].cond2_bool &&
                    logic_entry[i].cond3_bool &&
                    logic_entry[i].cond4_bool)
                    enable = 1;
                break;
            case 1:            //逻辑或
                if (logic_entry[i].cond1_bool ||
                    logic_entry[i].cond2_bool ||
                    logic_entry[i].cond3_bool ||
                    logic_entry[i].cond4_bool)
                    enable = 1;
                break;
            case 2:            //逻辑与非
                if (!(logic_entry[i].cond1_bool &&
                    logic_entry[i].cond2_bool &&
                    logic_entry[i].cond3_bool &&
                    logic_entry[i].cond4_bool))
                    enable = 1;
                break;
            case 3:            //逻辑或非
                if (!(logic_entry[i].cond1_bool ||
                    logic_entry[i].cond2_bool ||
                    logic_entry[i].cond3_bool ||
                    logic_entry[i].cond4_bool))
                    enable = 1;
            default:
                break;
        }

        if (enable) {
            tx_to_switch(logic_entry[i].func_para, logic_entry[i].func_type);
            reset_condition(i);
            enable = 0;
        }
    }
}