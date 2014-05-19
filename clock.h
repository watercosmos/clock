/* clock.h */
#include "iom32.h"
#include "ina90.h"
#include "delay.h"
#include "stdlib.h"

#define BAUDRATE	9600			//波特率
#define F_CPU		11059200		//外接晶振频率

#define DELAY_HI	0x7e			//定义延时参数, 约3ms
#define DELAY_LO	0x66
#define TWO_MS		2000			//2ms延时参数

#define RS485EN		PORTC_Bit1
#define WDI			PORTC_Bit0		//踢狗

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
} FlagByte;

#define BUSY		FlagByte.bit0	//网络忙标志
#define TX			FlagByte.bit1	//正在发送数据标志
#define COLLISION	FlagByte.bit2	//冲突等待标志
#define TOTX		FlagByte.bit3	//请求发送标志

/* 逻辑表内的其他三个预留条件参数 */
typedef struct {} condition1;
typedef struct {} condition3;
typedef struct {} condition4;

/* 时间条件参数 16字节*/
typedef struct
{
	unsigned char loop_flag:2;			//循环标识
	unsigned char loop_unit:2;			//循环单位
	unsigned char loop_sec_unit:2;		//二层循环单位
	unsigned char loop_end_flag:2;		//循环结束标识
	unsigned char interval;				//循环间隔
	unsigned char loop_num;				//循环次数
	unsigned char start_date[4];		//开始日期
	unsigned char start_time[3];		//开始时间
	unsigned char end_date[4];			//结束日期
	unsigned char day_in_week;			//周中哪天
	unsigned char day_in_month:5;		//月中哪天
	unsigned char week_in_month:3;		//月中哪周
} time_condition;

/* 序列表项 19字节 */
typedef struct
{
	unsigned char device_id;			//继电器ID
	unsigned char task_num;				//序列号
	unsigned char scence_sum;			//场景总数
	unsigned char scence_num[8];		//场景号
	unsigned char scence_runtime[8];	//场景执行时间
} task_entry;

/* 时间表项 8字节 */
typedef struct
{
	unsigned char time[7];				//触发时间
	unsigned char logic_num;			//逻辑号
} time_entry;

/* 逻辑表项内的功能参数 4字节 */
typedef struct
{
	unsigned char device_id;			//中继器设备ID
	unsigned char subnet_id;			//中继器子网ID
	unsigned char task_num;				//序列号
	unsigned char scence_num;			//步骤号
} func_para;

/* 逻辑表项 24字节（目前） */
typedef struct
{
	unsigned char enable:1;				//使能位
	unsigned char trigger:1;			//触发位
	unsigned char logic_num:6;			//逻辑号
	unsigned char logic_sum;			//逻辑总数
	unsigned char reserved:2;			//保留位
	unsigned char logic_operator:2;		//逻辑运算符
	unsigned char cond4_enable:1;		//条件1触发位
	unsigned char cond3_enable:1;		//条件2触发位
	unsigned char cond2_enable:1;		//条件3触发位
	unsigned char cond1_enable:1;		//条件4触发位
	condition1      cond1;				//条件1参数
	time_condition  cond2;				//条件2参数
	condition3      cond3;				//条件3参数
	condition4      cond4;				//条件4参数
	unsigned char function_type;		//功能类型（未使用）
	func_para fp;						//功能参数
} logic_entry;