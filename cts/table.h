/* �߼����ڵ���������Ԥ���������� */
//typedef struct {} Condition1;
//typedef struct {} Condition3;
//typedef struct {} Condition4;

/* ʱ���������� 16�ֽ�*/
typedef struct
{
	unsigned char loop_flag:2;			//ѭ����ʶ
	unsigned char loop_unit:2;			//ѭ����λ
	unsigned char loop_sec_unit:2;		//����ѭ����λ
	unsigned char loop_end_flag:2;		//ѭ��������ʶ
	unsigned char interval;				//ѭ�����
	unsigned char loop_num;				//ѭ������
	Time          start_time;		//��ʼʱ��
	unsigned char end_date[4];			//��������
	unsigned char day_in_week;			//��������
	unsigned char day_in_month:5;		//��������
	unsigned char week_in_month:3;		//��������
} Time_Condition;

/* ���� */
typedef struct
{
	unsigned char scence;			//����
	unsigned char runtime[2];		//����ִ��ʱ��
} Scence;

/* ���б��� 19�ֽ� */
typedef struct
{
	unsigned char device_id;			//�̵���ID
	unsigned char task_seq;				//���к�
	unsigned char scence_sum;			//��������
	Scence scence_seq[8];
} Task_Entry;

/* ʱ����� 8�ֽ� */
typedef struct
{
	Time time;							//����ʱ��
	unsigned char logic_seq;			//�߼���
} Time_Entry;

/* �߼������ڵĹ��ܲ��� 4�ֽ� */
typedef struct
{
	unsigned char device_id;			//�м����豸ID
	unsigned char subnet_id;			//�м�������ID
	unsigned char task_seq;				//���к�
	unsigned char scence_seq;			//�����
} Func_Para;

/* �߼����� 24�ֽڣ�Ŀǰ�� */
typedef struct
{
	unsigned char enable:1;				//ʹ��λ
	unsigned char trigger:1;			//����λ
	unsigned char logic_seq:6;			//�߼���
	unsigned char logic_sum;			//�߼�����
	unsigned char reserved:2;			//����λ
	unsigned char logic_operator:2;		//�߼������
	unsigned char cond4_enable:1;		//����1����λ
	unsigned char cond3_enable:1;		//����2����λ
	unsigned char cond2_enable:1;		//����3����λ
	unsigned char cond1_enable:1;		//����4����λ
	//Condition1      cond1;				//����1����
	Time_Condition  cond2;				//����2����
	//Condition3      cond3;				//����3����
	//Condition4      cond4;				//����4����
	unsigned char function_type;		//�������ͣ�δʹ�ã�
	Func_Para fp;						//���ܲ���
} Logic_Entry;

Logic_Entry logic_entry[MAX_LOGIC_TABLE_SIZE];		//�߼���
Time_Entry  time_entry [MAX_TIME_TABLE_SIZE];		//ʱ���
Task_Entry  task_entry [MAX_TASK_TABLE_SIZE];		//���б�

unsigned char logic_num;	//�߼���������Ŀ��
unsigned char time_num;		//ʱ���������Ŀ��
unsigned char task_num;		//���б�������Ŀ��

/* ɾ��ʱ����� */
void del_time_entry(unsigned char x)
{
	unsigned char i;
	for (i = x; i < (time_num - 1); i++)
		* (time_entry + i) = * (time_entry + i + 1);
	time_num--;
}

/* ���ʱ����� */
void add_time_entry(Time_Condition tc)
{
	//
}

/* ��ʱ�����������һ�δ���ʱ�� */
void calc_time(Time_Condition tc)
{
	if (tc.loop_flag == 0) {
		//
	} else {
		//
	}
}