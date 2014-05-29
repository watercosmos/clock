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

Time now;

/* 时间比较函数，t1早则返回负数，晚则返回正数，相等返回0 */
int time_cmp(Time t1, Time t2)
{
	if (t1.year != t2.year)
		return t1.year - t2.year;
	else if (t1.month != t2.month)
		return t1.month - t2.month;
	else if (t1.day != t2.day)
		return t1.day - t2.day;
	else if (t1.hour != t2.hour)
		return t1.hour - t2.hour;
	else if (t1.minute != t2.minute)
		return t1.minute - t2.minute;
	else if (t1.second != t2.second)
		return t1.second - t2.second;
	else
		return 0;
}

/* 日期比较函数，t1早则返回负数，晚则返回正数，相等返回0 */
/*int date_cmp(void * d1, void * d2)
{
	if (*(unsigned char *)d1 != *(unsigned char *)d2)
		return *(unsigned char *)d1 - *(unsigned char *)d2;
	else if (*(unsigned char *)(d1 + sizeof(unsigned char)) != \
			 *(unsigned char *)(d1 + sizeof(unsigned char)))
		return *(unsigned char *)(d1 + sizeof(unsigned char)) - \
			 *(unsigned char *)(d1 + sizeof(unsigned char));
	else if (*(unsigned char *)(d1 + 2 * sizeof(unsigned char)) != \
			 *(unsigned char *)(d1 + 2 * sizeof(unsigned char)))
		return *(unsigned char *)(d1 + 2 * sizeof(unsigned char)) - \
			 *(unsigned char *)(d1 + 2 * sizeof(unsigned char));
}*/