/* 逻辑表内的其他三个预留条件参数 */
typedef struct {} Condition1;
typedef struct {} Condition3;
typedef struct {} Condition4;

/* 时间条件参数 16字节*/
typedef struct
{
	unsigned char loop_flag:2;			//循环标识
	unsigned char loop_unit:2;			//循环单位
	unsigned char loop_sec_unit:2;		//二层循环单位
	unsigned char loop_end_flag:2;		//循环结束标识
	unsigned char interval;				//循环间隔
	unsigned char loop_num;				//循环次数
	Time          start_time[3];		//开始时间
	unsigned char end_date[4];			//结束日期
	unsigned char day_in_week;			//周中哪天
	unsigned char day_in_month:5;		//月中哪天
	unsigned char week_in_month:3;		//月中哪周
} Time_Condition;

/* 场景 */
typedef struct
{
	unsigned char scence;			//场景
	unsigned char runtime[2];		//场景执行时间
} Scence;

/* 序列表项 19字节 */
typedef struct
{
	unsigned char device_id;			//继电器ID
	unsigned char task_seq;				//序列号
	unsigned char scence_sum;			//场景总数
	Scence scence_seq[8];
} Task_Entry;

/* 时间表项 8字节 */
typedef struct
{
	Time time;							//触发时间
	unsigned char logic_seq;			//逻辑号
} Time_Entry;

/* 逻辑表项内的功能参数 4字节 */
typedef struct
{
	unsigned char device_id;			//中继器设备ID
	unsigned char subnet_id;			//中继器子网ID
	unsigned char task_seq;				//序列号
	unsigned char scence_seq;			//步骤号
} Func_Para;

/* 逻辑表项 24字节（目前） */
typedef struct
{
	unsigned char enable:1;				//使能位
	unsigned char trigger:1;			//触发位
	unsigned char logic_seq:6;			//逻辑号
	unsigned char logic_sum;			//逻辑总数
	unsigned char reserved:2;			//保留位
	unsigned char logic_operator:2;		//逻辑运算符
	unsigned char cond4_enable:1;		//条件1触发位
	unsigned char cond3_enable:1;		//条件2触发位
	unsigned char cond2_enable:1;		//条件3触发位
	unsigned char cond1_enable:1;		//条件4触发位
	Condition1      cond1;				//条件1参数
	Time_Condition  cond2;				//条件2参数
	Condition3      cond3;				//条件3参数
	Condition4      cond4;				//条件4参数
	unsigned char function_type;		//功能类型（未使用）
	Func_Para fp;						//功能参数
} Logic_Entry;

Logic_Entry logic_entry[MAX_LOGIC_TABLE_SIZE];		//逻辑表
Time_Entry  time_entry [MAX_TIME_TABLE_SIZE];		//时间表
Task_Entry  task_entry [MAX_TASK_TABLE_SIZE];		//序列表

unsigned char logic_num;	//逻辑表现有条目数
unsigned char time_num;		//时间表现有条目数
unsigned char task_num;		//序列表现有条目数

/* 删除时间表项 */
void del_time_entry(unsigned char x)
{
	unsigned char i;
	for (i = x; i < (time_num - 1); i++)
		* (time_entry + i) = * (time_entry + i + 1);
	time_num--;
}

/* 添加时间表项 */
void add_time_entry(Time_Condition tc)
{
	//
}

/* 由时间参数计算下一次触发时间 */
void calc_time(Time_Condition tc)
{
	if (tc.loop_flag == 0) {
		//
	} else {
		//
	}
}