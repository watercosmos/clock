/* CRC校验算法 */
void calc_crc(unsigned char buf)
{
	unsigned char j, TT;
	crc = crc ^ buf;
	for (j = 0; j < 8; j++)	{
		TT  = crc & 1;
		crc = crc >> 1;
		crc = crc & 0x7fff;
		if (TT == 1)
			crc = crc ^ 0xa001;
		crc = crc & 0xffff;
	}
}

void set_header(unsigned char len,
				unsigned char order1,
				unsigned char order2,
				unsigned char result)
{
	tx_buf[0]  = 0xAA;
	tx_buf[1]  = 0xAA;
	tx_buf[2]  = rx_buf[2];			//目的设备ID
	tx_buf[3]  = rx_buf[3];			//目的子网ID
	tx_buf[4]  = dev_id;			//源设备ID
	tx_buf[5]  = net_id;			//源子网ID
	tx_buf[6]  = len;				//分组长度
	tx_buf[7]  = 0x00;				//转发子网ID
	tx_buf[8]  = 0x00;				//分组序号
	tx_buf[9]  = order1;			//分组优先级、命令大类
	tx_buf[10] = order2;			//命令/响应、命令小类
	tx_buf[11] = result;			//命令结果
}

void set_tail(unsigned char len)
{
	int i;

	crc = 0xFFFF;
	for (i = 2; i < len; i++)
		calc_crc(tx_buf[i]);
	tx_buf[len]     = (unsigned char)crc;
	tx_buf[len + 1] = (unsigned char)(crc / 256);
}

void tx_status(void)
{
	set_header(0x0A, 0x00, 0x00, 0x00);
	memcpy(tx_buf + 12, mac, 8);
	memcpy(tx_buf + 20, timestamp, 2);
	set_tail(22);

	tx_num = 24;
	TOTX   = 1;
}

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

void tx_abstract(void)
{
	set_header(0x1F, 0x00, 0x02, 0x00);
	memcpy(tx_buf + 12, dev_models, 12);
	tx_buf[24] = enable;
	memcpy(tx_buf + 25, mac, 8);
	memcpy(tx_buf + 33, soft_version, 10);
	set_tail(43);

	tx_num = 45;
	TOTX   = 1;
}

void tx_mac(void)
{
	set_header(0x08, 0x00, 0x03, 0x00);
	memcpy(tx_buf + 12, mac, 8);
	set_tail(20);

	tx_num = 22;
	TOTX   = 1;
}

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

void set_enable(void)
{
	enable = rx_buf[10];
	set_header(0x00, 0x00, 0x05, 0x00);
	set_tail(12);

	tx_num = 14;
	TOTX   = 1;
}

void set_time(void)
{
	memcpy(timestamp, rx_buf + 10, 2);
	memcpy(&now, rx_buf + 12, 7);
	I2CWriteDate(now);
	set_header(0x00, 0x05, 0x00, 0x00);
	set_tail(12);

	tx_num = 14;
	TOTX   = 1;
}

void tx_time(void)
{
	I2CReadDate(now);
	set_header(0x07, 0x05, 0x01, 0x00);
	memcpy(tx_buf + 12, &now, 7);
	set_tail(19);

	tx_num = 21;
	TOTX   = 1;
}

void tx_logic_sum(void)
{
	set_header(0x01, 0x05, 0x02, 0x00);
	tx_buf[12] = logic_sum;
	set_tail(13);

	tx_num = 15;
	TOTX   = 1;
}

void set_logic(void)
{
	int i;

	if (logic_sum >= MAX_LOGIC_SIZE)
		return;
	for (i = 0; i < logic_sum; i++)
		if (logic_entry[i].logic_seq == (rx_buf[13] & 0x7F))
			return;
	memcpy(timestamp, rx_buf + 10, 2);
	memcpy(logic_entry + logic_sum, rx_buf + 13, 32);
	logic_sum++;

	if (logic_entry[logic_sum - 1].enable == 1) {
	}
	set_header(0x00, 0x05, 0x03, 0x00);
	set_tail(12);

	tx_num = 14;
	TOTX   = 1;
}

void tx_logic_entry(void)
{
	int i;

	for (i = 0; i < logic_sum; i++)
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

void set_logic_enable(void)
{
	int i;
	
	for (i = 0; i < logic_sum; i++)
		if (logic_entry[i].logic_seq == (rx_buf[13] & 0x7F)) {
			memcpy(timestamp, rx_buf + 10, 2);
			logic_entry[i].enable = rx_buf[13] >> 7;
			break;
		}

	set_header(0x00, 0x05, 0x07, 0x00);
	set_tail(12);

	tx_num = 14;
	TOTX   = 1;
}

void clear_logic(void)
{
	logic_sum = 0;
	memset(logic_entry, 0, MAX_LOGIC_SIZE * sizeof(Logic));
	memcpy(timestamp, rx_buf + 10, 2);

	set_header(0x00, 0x05, 0x08, 0x00);
	set_tail(12);

	tx_num = 14;
	TOTX   = 1;
}

void del_logic(void)
{
	int i, j;

	for (i = 0; i < logic_sum; i++)
		if (logic_entry[i].logic_seq == (rx_buf[13] & 0x7F)) {
			memcpy(timestamp, rx_buf + 10, 2);
			memmove(logic_entry + i, logic_entry + i + 1,
					(logic_sum - i - 1) * sizeof(Logic));
			logic_sum--;
			for (j = i; j < logic_sum; j++)
				logic_entry[j].logic_seq--;
			memset(logic_entry + logic_sum, 0,
				   (MAX_LOGIC_SIZE - logic_sum) * sizeof(Logic));
			break;
		}

	set_header(0x00, 0x05, 0x09, 0x00);
	set_tail(12);

	tx_num = 14;
	TOTX   = 1;
}

void tx_to_switch(unsigned char net_id, unsigned char dev_id,
					unsigned char type, unsigned char id, unsigned char area_id)
{
	set_header(0x03, 0x03, 0x84, 0x00);
	tx_buf[2]  = net_id;
	tx_buf[3]  = dev_id;
	tx_buf[12] = type;
	tx_buf[13] = id;
	tx_buf[14] = area_id;
	set_tail(15);

	tx_num = 17;
	TOTX   = 1;
}
