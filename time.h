/* time.h */
#ifndef TIME_H
#define TIME_H

void I2CReadDate(Time *times);

/* 10ms延时函数 */
void delay_10ms(u8 count)
{
    u16 i, j;
    for (j = 0; j < count; j++)
        for (i = 0; i < 11400; i++);
}

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

/* 时间粗略比较函数，t1比t2晚1分钟内亦视为相等 */
int time_cmp2(const Time * t1, const Time * t2)
{
    if (t1->year != t2->year)
        return t1->year - t2->year;
    else if (t1->month != t2->month)
        return t1->month - t2->month;
    else if (t1->day != t2->day)
        return t1->day - t2->day;
    else if (t1->hour != t2->hour)
        return t1->hour - t2->hour;
    else if (t1->minute == t2->minute + 1)
        return 0;
    else if (t1->minute != t2->minute)
        return t1->minute - t2->minute;
    else if (t1->second < t2->second)
        return t1->second - t2->second;
    else
        return 0;
}

/* 日期比较函数 */
int date_cmp(const Time * t1, const u8 * t2)
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

/* 将十六进制时间按字面值转换为十进制 */
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

/* 将十进制时间按字面转换为十六进制 */
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

/* 修正日期，当日期、月份超出定值 */
void fix_date(Time *t)
{
    u8 x;

    while (t->month > 12) {
        t->month -= 12;
        t->year++;
    }

    while (t->day > days[t->month - 1]) {
        //闰年2月时，x = 1，否则为0
        x = (t->month == 2 && (t->year % 400 == 0 ||
            (t->year % 4 == 0 && t->year % 100 != 0))) ? 1 : 0;
        t->day = t->day - days[t->month - 1] - x;
        //当前日期为闰年2月29日时
        if (t->day == 0) {
            t->day = 29;
            break;
        }
        t->month++;
        if (t->month > 12) {
            t->month = 1;
            t->year++;
        }
    }
}

/* 由年月日计算星期几，返回值0-6代表周一到周日 */
u8 calc_weekday(int y, u8 m, u8 d)
{
    u8 w;

    y = y + 2000;
    if (m == 1 || m == 2) {
        m += 12;
        y--;
    }
    //Zeller’s Formula
    w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;

    return w;
}

/* 由年月日计算属于月中第几周 */
u8 which_week(u8 y, u8 m, u8 d)
{
    u8 firstday = calc_weekday(y, m, 1);

    return (d + firstday - 1) / 7 + 1;
}

/* 由年、月、周数、周中哪天计算实际日期，返回日期 */
u8 calc_date(u8 y, u8 m, u8 w, u8 day_in_week)
{
    u8 firstday_in_month = calc_weekday(y, m, 1);
    u8 firstday_in_week  = 7 * w - firstday_in_month - 6;
    
    m1 = y;
    m2 = m;
    m3 = w;

    tem = 0;
    while (!(day_in_week & 0x01)) {
        day_in_week >>= 1;
        tem++;
    }

    return firstday_in_week + tem;
}

/* 删除单条时间表项 */
void del_time(u8 i)
{
    u8 j, k;

    memmove(time_entry + i, time_entry + i + 1,
            (time_sum - i - 1) * sizeof(Time_Entry));
    time_sum--;
    EEPROM_write(ADDR_time_sum, time_sum);
    for (j = i; j < time_sum; j++) {
        WDI = 0;
        delay_10ms(1);
        WDI = 1;
        memcpy(eep_tem, time_entry + j, 8);
        for (k = 0; k < 8; k++)
            EEPROM_write(ADDR_time + j * 8 + k, eep_tem[k]);
    }
}

/* 根据逻辑内的时间参数计算下一次触发时间并加入时间表 */
void calc_time(const Time_Condition * tc, u8 ls)
{
    u8 i, diw = tc->day_in_week;    //避免修改原值
    Time t, t_dec;

    //时间表已满，舍弃
    if (time_sum >= MAX_TIME_SIZE)
        return;

    memcpy(&t, &(tc->start_time), sizeof(Time));
    I2CReadDate(&now);    //有必要吗?

    //若当前时间早于开始时间，则将开始时间加入时间表
    if (time_cmp(&now, &t) < 0) {
        u8 i;
        memcpy(time_entry + time_sum, &t, sizeof(Time));
        time_entry[time_sum].logic_seq = ls;
        memcpy(eep_tem, time_entry + time_sum, 8);
        for (i = 0; i < 8; i++)
            EEPROM_write(ADDR_time + time_sum * 8 + i,
                         eep_tem[i]);
        time_sum++;
        EEPROM_write(ADDR_time_sum, time_sum);
        return;
    }
    
    //单次逻辑直接返回
    if (tc->loop_flag == 0)
        return;

    //判断循环是否结束
    if (tc->loop_end_flag && date_cmp(&now, tc->end_date) >= 0)
        return;

    switch (tc->loop_unit) {
        case 0:            //按天循环
            while (time_cmp(&now, &t) >= 0) {
                hex_to_dec(&t, &t_dec);
                t_dec.day += tc->interval;
                fix_date(&t_dec);
                dec_to_hex(&t_dec, &t);
            }
            break;
        case 1:            //按周循环
            while (time_cmp(&now, &t) >= 0) {
                hex_to_dec(&t, &t_dec);
                t_dec.day += 7 * tc->interval;
                fix_date(&t_dec);
                t_dec.day -= calc_weekday(t_dec.year, t_dec.month, t_dec.day);
                //这里没考虑周内多天
                tem = 0;
                while (!(diw & 0x01)) {
                    diw >>= 1;
                    tem++;
                }
                t_dec.day += tem;
                fix_date(&t_dec);
                dec_to_hex(&t_dec, &t);
            }
            break;
        case 2:            //按月循环
            if (tc->loop_sec_unit == 0) {            //二层循环为天
                while (time_cmp(&now, &t) >= 0) {
                    hex_to_dec(&t, &t_dec);
                    t_dec.month += tc->interval;
                    fix_date(&t_dec);
                    t_dec.day = tc->day_in_month;
                    dec_to_hex(&t_dec, &t);
                }
            } else if (tc->loop_sec_unit == 1) {     //二层循环为周
                while (time_cmp(&now, &t) >= 0) {
                    hex_to_dec(&t, &t_dec);
                    t_dec.month += tc->interval;
                    fix_date(&t_dec);
                    t_dec.day = calc_date(t_dec.year,
                                          t_dec.month,
                                          tc->week_in_month,
                                          tc->day_in_week);
                    dec_to_hex(&t_dec, &t);
                }
            }
            break;
    }

    //判断循环是否结束
    if (tc->loop_end_flag && date_cmp(&t, tc->end_date) >= 0)
        return;

    memcpy(time_entry + time_sum, &t, sizeof(Time));
    log5 = ls;
    time_entry[time_sum].logic_seq = ls;
    memcpy(eep_tem, time_entry + time_sum, 8);
    for (i = 0; i < 8; i++)
        EEPROM_write(ADDR_time + time_sum * 8 + i, eep_tem[i]);
    time_sum++;
    EEPROM_write(ADDR_time_sum, time_sum);
}

void clear_time(void)
{
    int i,
       t_sum = time_sum;

    for (i = t_sum - 1; i >= 0; i--) {
        if (time_cmp(&now, &(time_entry[i].time)) > 0) {
            del_time(i);
            i++;
        }
    }
}

#endif