#ifndef ASSEMBLE_H
#define ASSEMBLE_H

/* CRC校验算法 */
void calc_crc(u8 buf)
{
    u8 j, TT;
    crc = crc ^ buf;
    for (j = 0; j < 8; j++) {
        TT  = crc & 1;
        crc = crc >> 1;
        crc = crc & 0x7fff;
        if (TT == 1)
            crc = crc ^ 0xa001;
        crc = crc & 0xffff;
    }
}

/*
 * 重置逻辑i的所有条件
 */
void reset_condition(u8 i)
{
    if (logic_entry[i].cond1_enable)
        logic_entry[i].cond1_bool = 0;
    if (logic_entry[i].cond2_enable)
        logic_entry[i].cond2_bool = 0;
    if (logic_entry[i].cond3_enable)
        logic_entry[i].cond3_bool = 0;
    if (logic_entry[i].cond4_enable)
        logic_entry[i].cond4_bool = 0;
}

void set_header(u8 len, u8 order1, u8 order2, u8 result)
{
    tx_buf[0]  = 0xAA;
    tx_buf[1]  = 0xAA;
    tx_buf[2]  = rx_buf[2];     //目的设备ID
    tx_buf[3]  = rx_buf[3];     //目的子网ID
    tx_buf[4]  = dev_id;        //源设备ID
    tx_buf[5]  = net_id;        //源子网ID
    tx_buf[6]  = len;           //分组长度
    tx_buf[7]  = 0x00;          //转发子网ID
    tx_buf[8]  = 0x00;          //分组序号
    tx_buf[9]  = order1;        //分组优先级、命令大类
    tx_buf[10] = order2;        //命令/响应、命令小类
    tx_buf[11] = result;        //命令结果
}

void set_tail(u8 len)
{
    u8 i;

    crc = 0xFFFF;
    for (i = 2; i < len; i++)
        calc_crc(tx_buf[i]);
    tx_buf[len]     = (u8)crc;
    tx_buf[len + 1] = (u8)(crc / 256);
}

/* 响应查询是否在线命令 */
void tx_status(void)
{
    set_header(0x0A, 0x00, 0x00, 0x00);
    memcpy(tx_buf + 12, MAC, 8);
    memcpy(tx_buf + 20, timestamp, 2);
    set_tail(22);

    tx_num = 24;
    TOTX   = 1;
}

/* 设置摘要信息并响应 */
void set_abstract(void)
{
    memcpy(timestamp, rx_buf + 10, 2);
    memcpy(dev_models, rx_buf + 12, 12);
    enable = rx_buf[24];

    set_header(0x00, 0x00, 0x01, 0x00);
    set_tail(12);

    tx_num = 14;
    TOTX   = 1;
}

/* 发送摘要信息 */
void tx_abstract(void)
{
    set_header(0x1F, 0x00, 0x02, 0x00);
    memcpy(tx_buf + 12, dev_models, 12);
    tx_buf[24] = enable;
    memcpy(tx_buf + 25, MAC, 8);
    memcpy(tx_buf + 33, soft_version, 10);
    set_tail(43);

    tx_num = 45;
    TOTX   = 1;
}

/* 汇报MAC */
void tx_mac(void)
{
    set_header(0x08, 0x00, 0x03, 0x00);
    memcpy(tx_buf + 12, MAC, 8);
    set_tail(20);

    tx_num = 22;
    TOTX   = 1;
}

/* 配置新的设备、子网ID并响应 */
void set_id(void)
{
    memcpy(timestamp, rx_buf + 10, 2);
    dev_id = rx_buf[20];
    net_id = rx_buf[21];

    set_header(0x00, 0x00, 0x04, 0x00);
    set_tail(12);

    tx_num = 14;
    TOTX   = 1;
}

/* 启用/禁用模块 */
void set_enable(void)
{
    memcpy(timestamp, rx_buf + 10, 2);
    enable = rx_buf[12];
    set_header(0x00, 0x00, 0x05, 0x00);
    set_tail(12);

    tx_num = 14;
    TOTX   = 1;
}

/* 校准时间 */
void set_time(void)
{
    u8 week;
    Time t_dec;

    hex_to_dec(&now, &t_dec);
    week = calc_weekday(t_dec.year, t_dec.month, t_dec.day);
    
    memcpy(timestamp, rx_buf + 10, 2);
    memcpy(&now, rx_buf + 12, 7);
    I2CWriteDate(&now, week);
    set_header(0x00, 0x05, 0x00, 0x00);
    set_tail(12);

    tx_num = 14;
    TOTX   = 1;
}

/* 发送当前时间 */
void tx_time(void)
{
    I2CReadDate(&now);
    set_header(0x07, 0x05, 0x01, 0x00);
    memcpy(tx_buf + 12, &now, 7);
    set_tail(19);

    tx_num = 21;
    TOTX   = 1;
}

/* 读取当前逻辑总数 */
void tx_logic_sum(void)
{
    set_header(0x01, 0x05, 0x02, 0x00);
    tx_buf[12] = logic_sum;
    set_tail(13);

    tx_num = 15;
    TOTX   = 1;
}

/* 设置一条逻辑 */
void set_logic(void)
{
    u8 i, current = logic_sum,   //需要增加或修改的逻辑存储位置
          t_sum   = time_sum;

    //排除逻辑表已满、逻辑号已存在的情况
    if (logic_sum >= MAX_LOGIC_SIZE)
        return;

    //遍历逻辑表，序号已存在则将修改逻辑
    for (i = 0; i < logic_sum; i++) {
        if (logic_entry[i].logic_seq == (rx_buf[13] & 0x7F)) {
            current = i;
            break;
        }
    }

    memcpy(timestamp, rx_buf + 10, 2);
    memset(logic_entry + current, 0, sizeof(Logic));
    memcpy(logic_entry + current, rx_buf + 13, 32);

    set_header(0x00, 0x05, 0x03, 0x00);
    set_tail(12);

    tx_num = 14;
    TOTX   = 1;

    /*
     * 排除未启用的条件
     * 保证它们不影响逻辑运算结果
     */
     if (logic_entry[current].logic_operator == 0 ||
         logic_entry[current].logic_operator == 2) {
        logic_entry[i].cond1_bool = 1;
        logic_entry[i].cond2_bool = 1;
        logic_entry[i].cond3_bool = 1;
        logic_entry[i].cond4_bool = 1;
     } else {
        logic_entry[i].cond1_bool = 0;
        logic_entry[i].cond2_bool = 0;
        logic_entry[i].cond3_bool = 0;
        logic_entry[i].cond4_bool = 0;
     }

    //确定逻辑需使用哪些条件
    reset_condition(current);

    //若修改逻辑，则删除原逻辑的时间表项，否则增加逻辑总数
    if (current != logic_sum)
        for (i = 0; i < t_sum; i++) {
            if (time_entry[i].logic_seq == logic_entry[current].logic_seq) {
                del_time(i);
                break;
            }
        }
    else
        logic_sum++;

    if (logic_entry[current].enable)
        calc_time(&(logic_entry[current].cond1),
                  logic_entry[current].logic_seq);
}

/* 发送一条逻辑 */
void tx_logic_entry(void)
{
    u8 i;

    for (i = 0; i < logic_sum; i++) {
        if (logic_entry[i].logic_seq == rx_buf[10]) {
            set_header(0x21, 0x05, 0x04, 0x00);
            tx_buf[12] = logic_sum;
            memcpy(tx_buf + 13, logic_entry + i, 32);
            set_tail(45);

            tx_num = 47;
            TOTX   = 1;
            return;
        }
    }
}

/* 启用/禁用单条逻辑 */
void set_logic_enable(void)
{
    u8 i;
    
    for (i = 0; i < logic_sum; i++) {
        if (logic_entry[i].logic_seq == (rx_buf[13] & 0x7F)) {
            memcpy(timestamp, rx_buf + 10, 2);
            logic_entry[i].enable = rx_buf[13] & 0x01;
            reset_condition(i);
            break;
        }
    }

    set_header(0x00, 0x05, 0x07, 0x00);
    set_tail(12);

    tx_num = 14;
    TOTX   = 1;
}

/* 清空所有逻辑 */
void clear_logic(void)
{
    logic_sum = 0;
    time_sum  = 0;
    memset(logic_entry, 0, MAX_LOGIC_SIZE * sizeof(Logic));
    memset(time_entry, 0, MAX_TIME_SIZE * sizeof(Time_Entry));
    memcpy(timestamp, rx_buf + 10, 2);

    set_header(0x00, 0x05, 0x08, 0x00);
    set_tail(12);

    tx_num = 14;
    TOTX   = 1;
}

/* 删除单条逻辑 */
void del_logic(void)
{
    u8 i, j,
       l_sum = logic_sum,
       t_sum = time_sum;

    for (i = 0; i < l_sum; i++) {
        if (logic_entry[i].logic_seq == rx_buf[12]) {
            memcpy(timestamp, rx_buf + 10, 2);
            //将被删除逻辑后面的所有逻辑前移，
            memmove(logic_entry + i, logic_entry + i + 1,
                    (logic_sum - i - 1) * sizeof(Logic));
            logic_sum--;
            //同时其后的所有逻辑号减1以使逻辑号连续
            for (j = i; j < logic_sum; j++)
                logic_entry[j].logic_seq--;
            memset(logic_entry + logic_sum, 0,
                   (MAX_LOGIC_SIZE - logic_sum) * sizeof(Logic));
            //删除该逻辑对应的时间表项
            for (i = 0; i < t_sum; i++) {
                if (time_entry[i].logic_seq == rx_buf[12]) {
                    del_time(i);
                    break;
                }
            }
            //成功删除响应
            set_header(0x00, 0x05, 0x09, 0x00);
            set_tail(12);

            tx_num = 14;
            TOTX   = 1;
            break;
        }
    }
}

void tx_to_switch(const Logic *le)
{
    u8 tem = le->func_type;
    if (tem)
        tem++;
    set_header(0x03, 0x03, 0xBE, 0x00);
    tx_buf[2]  = le->func_para.dev_id;
    tx_buf[3]  = le->func_para.net_id;
    tx_buf[12] = tem << 4 | 0x01;
    tx_buf[13] = le->func_para.area_id;
    tx_buf[14] = le->func_para.id;
    set_tail(15);

    tx_num = 17;
    TOTX   = 1;
}

void tx_to_ctrl(u8 ls)
{
    set_header(0x01, 0x05, 0x85, 0x00);
    tx_buf[12] = ls;
    set_tail(13);
    
    tx_num = 15;
    TOTX   = 1;
}

#endif