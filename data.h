#ifndef DATA_H
#define DATA_H

/* 时间 7 byte */
typedef struct
{
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char week;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
} Time;

/* 16 byte */
typedef struct
{
    unsigned char loop_flag:2;        //循环标识
    unsigned char loop_unit:2;        //循环单位
    unsigned char loop_sec_unit:2;    //二层循环单位
    unsigned char loop_end_flag:2;    //循环结束标识
    Time          start_time;         //开始时间
    unsigned char interval;           //循环间隔
    unsigned char end_date[4];        //结束日期
    unsigned char loop_num;           //循环次数
    unsigned char day_in_week;        //周中哪天
    unsigned char day_in_month:5;     //月中哪天
    unsigned char week_in_month:3;    //月中哪周
} Time_Condition;

/* 5 byte */
typedef struct {
    unsigned char net_id;      //传感器子网ID
    unsigned char dev_id;      //传感器设备ID
    unsigned char type;        //传感器类型
    unsigned char para1;       //参数1
    unsigned char para2;       //参数2
} Sensor_Condition;

/* 4 byte */
typedef struct
{
    unsigned char net_id;      //继电器子网ID
    unsigned char dev_id;      //继电器设备ID
    unsigned char area_id;     //区域号
    unsigned char id;          //场景号/序列号
} Func_Para;

/* 32 byte */
typedef struct
{
    unsigned char enable:1;             //逻辑使能
    unsigned char logic_seq:7;          //逻辑号
    unsigned char reserve:2;            //保留
    unsigned char logic_operator:2;     //逻辑运算：0-3对应与或与非或非
    unsigned char cond4_enable:1;       //条件1-4启用/禁用
    unsigned char cond3_enable:1;
    unsigned char cond2_enable:1;
    unsigned char cond1_enable:1;
    unsigned char cond1_len;            //条件1长度
    Time_Condition cond1;               //条件1，即时间参数
    unsigned char cond2_len;            //条件2长度
    Sensor_Condition cond2;             //条件2，即传感器参数
    unsigned char cond3_len;            //条件3-4长度，预留
    unsigned char cond4_len;
    unsigned char func_type;            //功能类型：0-场景，1-序列
    Func_Para func_para;                //功能参数
    //用于判断条件成立与否，非实际逻辑内容
    unsigned char cond4_bool:1;
    unsigned char cond3_bool:1;
    unsigned char cond2_bool:1;
    unsigned char cond1_bool:1;
    unsigned char reserve2:4;
} Logic;

/* 8 byte */
typedef struct
{
    Time time;                    //触发时间
    unsigned char logic_seq;      //对应逻辑
} Time_Entry;

#endif