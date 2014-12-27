#ifndef CLOCK_H
#define CLOCK_H

/* clock.h */
#include "iom64.h"
#include "ina90.h"
//#include "delay.h"
#include <stdlib.h>
#include <string.h>

#define BAUDRATE  9600           //波特率
#define F_CPU     11059200       //外接晶振频率

#define DELAY_HI  0x0e           //定义延时参数, 约3ms
#define DELAY_LO  0x66
#define TWO_MS    2000           //2ms延时参数

#define RS485EN   PORTE_Bit2
#define WDI       PORTF_Bit3     //踢狗
#define LED       PORTE_Bit6     //LED灯
#define SCL       PORTF_Bit0
#define SDA       PORTF_Bit1
#define sda_in    DDRF &= 0xFD
#define sda_out   DDRF |= 0x02

typedef unsigned char u8;
typedef unsigned int u16;

struct
{
    u8 bit0:1;
    u8 bit1:1;
    u8 bit2:1;
    u8 bit3:1;
    u8 bit4:1;
    u8 bit5:1;
    u8 bit6:1;
    u8 bit7:1;
} FlagByte;

/* 时间 7 byte */
typedef struct
{
    u8 year;
    u8 month;
    u8 day;
    u8 week;
    u8 hour;
    u8 minute;
    u8 second;
} Time;

/* 16 byte */
typedef struct
{                             //                 0     1     2
    u8   loop_flag:2;         //循环标识         单    循
    u8   loop_unit:2;         //循环单位         天    周    月
    u8   loop_sec_unit:2;     //二层循环单位     天    周
    u8   loop_end_flag:2;     //循环结束标识     无    有
    Time start_time;          //开始时间
    u8   interval;            //循环间隔
    u8   end_date[4];         //结束日期
    u8   loop_num;            //循环次数         无效
    u8   day_in_week;         //周中哪天         最低位为周一
    u8   week_in_month:3;     //月中哪周
    u8   day_in_month:5;      //月中哪天
} Time_Condition;

/* 5 byte */
typedef struct
{
    u8 net_id;       //传感器子网ID
    u8 dev_id;       //传感器设备ID
    u8 type;         //传感器类型
    u8 para1;        //参数1, 最小值
    u8 para2;        //参数2, 最大值
} Sensor_Condition;

/* 4 byte */
typedef struct
{
    u8 net_id;       //继电器子网ID
    u8 dev_id;       //继电器设备ID
    u8 area_id;      //区域号
    u8 id;           //场景号/序列号
} Func_Para;

/* 32 + 1 byte */
typedef struct
{
    u8 logic_seq:7;          //逻辑号
    u8 enable:1;             //逻辑使能
    u8 reserve:2;            //保留
    u8 logic_operator:2;     //逻辑运算：0-3对应与、或、与非、或非
    u8 cond4_enable:1;       //条件1-4启用/禁用
    u8 cond3_enable:1;
    u8 cond2_enable:1;
    u8 cond1_enable:1;
    u8 cond1_len;            //条件1长度
    Time_Condition cond1;    //条件1，时间参数
    u8 cond2_len;            //条件2长度
    Sensor_Condition cond2;  //条件2，传感器参数
    u8 cond3_len;            //条件3-4长度，预留
    u8 cond4_len;
    u8 func_type;            //功能类型：0-场景，1-序列
    Func_Para func_para;     //功能参数

    u8 cond4_bool:1;         //用于判断条件成立与否，非实际逻辑内容
    u8 cond3_bool:1;
    u8 cond2_bool:1;
    u8 cond1_bool:1;
    u8 reserve2:4;
} Logic;

/* 8 byte */
typedef struct
{
    Time time;      //触发时间
    u8 logic_seq;   //对应逻辑
} Time_Entry;

#define BUSY      FlagByte.bit0    //网络忙标志
#define TX        FlagByte.bit1    //正在发送数据标志
#define COLLISION FlagByte.bit2    //冲突等待标志
#define TOTX      FlagByte.bit3    //请求发送标志

const u8 MAC[8] = {0x00, 0x00, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
const u8 days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// EEPROM address 0x0001 - 0x001D  29 Byte

__root __eeprom u8 dev_id    @ 0x0001 = 0x04;   //设备ID
__root __eeprom u8 net_id    @ 0x0002 = 0x01;   //子网ID
__root __eeprom u8 enable    @ 0x0003 = 0x01;   //使能状态
__root __eeprom u8 logic_sum @ 0x0004;          //现存逻辑数
__root __eeprom u8 time_sum  @ 0x0005;          //现存时间数

__root __eeprom u8 timestamp[2]     @ 0x0006 = {0};  //时间戳
__root __eeprom u8 soft_version[10] @ 0x0008 = {0};  //版本号
__root __eeprom u8 dev_models[12]   @ 0x0014         //设备型号默认为"clock"
            = {0x63, 0x6C, 0x6F, 0x63, 0x6B, 0x00};

// EEPROM address 0x0020 - 0x051F  1280 Byte 逻辑表 最大30条
// EEPROM address 0x0520 - 0x0610   240 Byte 时间表 最大30条
#define MAX_LOGIC_SIZE  30
#define MAX_TIME_SIZE   30
#define LOGIC_ADDR  0x0020
#define TIME_ADDR   0x0520

#define MAX_RX_BUF_SIZE 60
#define MAX_TX_BUF_SIZE 60

u8 tx_buf[MAX_TX_BUF_SIZE] = {'A', 'B', 'C', 'D'};
u8 tx_num = 4;
u8 tx_pos;
u8 tx_last;

u8 rx_buf[MAX_RX_BUF_SIZE];
u8 rx_step;
u8 rx_pos;
u16 crc;

Logic logic_entry[MAX_LOGIC_SIZE];      //逻辑表
Time_Entry time_entry[MAX_TIME_SIZE];   //时间表

Time now;             //当前时间
u8 tem;               //计算星期的移位计数
u8 timer0;            //定时器0常规计数
u8 timer2;            //定时器0常规计数

int m1,m2,m3,log5;    //无意义, 但必须

u8 WAIT;
u8 ls_cond2 = 0xFF;

u8 eep_logic[32];     //临时变量, 用于EEPROM与发送接受buffer间中转
u8 eep_time[8];
u8 eep_tem[12];
u16 addr;

#endif