/* 时间比较函数，t1早则返回负数，晚则返回正数，相等返回0 */
int time_cmp(const Time * t1, const Time * t2)
{
	if (t1->year != t2->year)
		return t1->year - t2->year;
	else if (t1->month != t2->month)
		return t1->month - t2->month;
	else if (t1->day != t2->day)
		return t1->day - t2->day;
	else if (t1->hour != t2->hour)
		return t1->hour - t2->hour;
	else if (t1->minute != t2->minute)
		return t1->minute - t2->minute;
	else if (t1->second != t2->second)
		return t1->second - t2->second;
	else
		return 0;
}

int date_cmp(const Time * t1, const unsigned char * t2)
{
	if (t1->year != *(t2 + 0))
		return t1->year - *(t2 + 0);
	else if (t1->month != *(t2 + 1))
		return t1->month - *(t2 + 1);
	else if (t1->day != *(t2 + 2))
		return t1->day - *(t2 + 2);
	else
		return 0;
}

void hex_to_dec(const Time *t, Time *ret)
{
	ret->year   = (t->year   >> 4) * 10 + (t->year   & 0x0F);
	ret->month  = (t->month  >> 4) * 10 + (t->month  & 0x0F);
	ret->day    = (t->day    >> 4) * 10 + (t->day    & 0x0F);
	ret->week   = (t->week   >> 4) * 10 + (t->week   & 0x0F);
	ret->hour   = (t->hour   >> 4) * 10 + (t->hour   & 0x0F);
	ret->minute = (t->minute >> 4) * 10 + (t->minute & 0x0F);
	ret->second = (t->second >> 4) * 10 + (t->second & 0x0F);
}

void dec_to_hex(const Time *t, Time *ret)
{
	ret->year   = (t->year   / 10 << 4) | (t->year   % 10);
	ret->month  = (t->month  / 10 << 4) | (t->month  % 10);
	ret->day    = (t->day    / 10 << 4) | (t->day    % 10);
	ret->week   = (t->week   / 10 << 4) | (t->week   % 10);
	ret->hour   = (t->hour   / 10 << 4) | (t->hour   % 10);
	ret->minute = (t->minute / 10 << 4) | (t->minute % 10);
	ret->second = (t->second / 10 << 4) | (t->second % 10);
}

/* 返回值0-6代表周一到周日 */
int calc_weekday(int y, int m, int d)
{
	int w;

	y = y + 2000;
	if (m == 1 || m == 2) {
		m += 12;
		y--;
	}
	w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;

	return w;
}

int calc_firstday(int y, int m)
{
	//计算每个月的第一天是周几
}

void del_time(int i)
{
	memmove(time_entry + i, time_entry + i + 1,
			(time_sum - i - 1) * sizeof(Time_Entry));
	time_sum--;
	memset(time_entry + time_sum, 0,
			(MAX_TIME_SIZE - time_sum) * sizeof(Time_Entry));
}

void calc_time(Time_Condition * tc, unsigned char ls)
{
	int i, x;
	Time t;

	memcpy(&t, &(tc->start_time), sizeof(Time));
	I2CReadDate(&now);

	if (time_cmp(&now, &t) < 0) {
		memcpy(time_entry + time_sum, &t, sizeof(Time));
		time_entry[time_sum].logic_seq = ls;
		time_sum++;
		if (tc->loop_num != 0)
			tc->loop_num--;
		return;
	}
	if (tc->loop_flag == 0)
			return;
	switch (tc->loop_end_flag) {
		case 0:			//无结束
			break;
		case 1:			//按重复次数
			if (tc->loop_num == 0)
				return;
		case 2:			//按结束日期
			if (date_cmp(&now, tc->end_date) == 0)
				return;
		default:
			break;
	}

	switch (tc->loop_unit) {
		case 0:			//按天循环	
			while (time_cmp(&now, &t) >= 0) {
				hex_to_dec(&t, &t_dec);
				t_dec.day += tc->interval;
				while (t_dec.day > days[t.month - 1]) {
					x = (t_dec.month == 2 && (t_dec.year % 400 == 0 ||
						 (t_dec.year % 4 == 0 && t_dec.year % 100 != 0)))
						? 1 : 0;
					t_dec.day = t_dec.day - days[t.month - 1] - x;
					if (t_dec.day == 0) {
						t_dec.day = 29;
						break;
					}
					t_dec.month++;
					if (t_dec.month > 12) {
						t_dec.month = 1;
						t_dec.year++;
					}
				}
				dec_to_hex(&t_dec, &t);
			}
			memcpy(time_entry + time_sum, &t, sizeof(Time));
			time_entry[time_sum].logic_seq = ls;
			time_sum++;
			break;
		case 1:			//按周循环
			/*while (time_cmp(&now, &t) >= 0) {
				hex_to_dec(&t, &t_dec);
				//
				dec_to_hex(&t_dec, &t);
			}*/
		case 2:			//按月循环
			break;
	}
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
