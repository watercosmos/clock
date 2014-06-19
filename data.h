/* 时间 7字节 */
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
	unsigned char loop_flag:2;			//循环标识
	unsigned char loop_unit:2;			//循环单位
	unsigned char loop_sec_unit:2;		//二层循环单位
	unsigned char loop_end_flag:2;		//循环结束标识
	Time          start_time;			//开始时间
	unsigned char interval;				//循环间隔
	unsigned char end_date[4];			//结束日期
	unsigned char loop_num;				//循环次数
	unsigned char day_in_week;			//周中哪天
	unsigned char day_in_month:5;		//月中哪天
	unsigned char week_in_month:3;		//月中哪周
} Time_Condition;

/* 5 byte */
typedef struct {
	unsigned char net_id;			//传感器子网ID
	unsigned char dev_id;			//传感器设备ID
	unsigned char type;				//传感器类型
	unsigned char para1;			//参数1
	unsigned char para2;			//参数2
} Sensor_Condition;

/* 4 byte */
typedef struct
{
	unsigned char net_id;			//继电器子网ID
	unsigned char dev_id;			//继电器设备ID
	unsigned char area_id;			//区域号
	unsigned char id;				//场景号/序列号
} Func_Para;

/* 32 byte */
typedef struct
{
	unsigned char enable:1;
	unsigned char logic_seq:7;
	unsigned char reserve:2;
	unsigned char logic_operator:2;
	unsigned char cond4_enable:1;
	unsigned char cond3_enable:1;
	unsigned char cond2_enable:1;
	unsigned char cond1_enable:1;
	unsigned char cond1_len;
	Time_Condition cond1;
	unsigned char cond2_len;
	Sensor_Condition cond2;
	unsigned char cond3_len;
	unsigned char cond4_len;
	unsigned char func_type;
	Func_Para func_para;
} Logic;

/*  */
typedef struct
{
	Time time;
	unsigned char logic_seq;
} Time_Entry;
