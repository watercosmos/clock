#include "stdio.h"
#include "stdlib.h"
#include "string.h"

unsigned short crc = 0xFFFF;

void calc_crc(unsigned char buf)
{
	unsigned char j, TT;
	crc = crc ^ buf;
	for (j = 0; j < 8; j++)
	{
		TT  = crc & 1;
		crc = crc >> 1;
		crc = crc & 0x7fff;
		if (TT == 1)
			crc = crc ^ 0xa001;
		crc = crc & 0xffff;
	}
}

int main(void)
{
	char *s;
	unsigned char d[60];
	int i;
	char more;

	s = (char *)malloc(120);
	memset(s, 0, 120);
	memset(d, 0, 60);

	puts("Please enter a hexadecimal string:\n");
	scanf("%s", s);

	for (i = 0; i < strlen(s) / 2; i++) {
		switch (s[2*i]) {
			case '0': d[i] = 0x00; break;
			case '1': d[i] = 0x10; break;
			case '2': d[i] = 0x20; break;
			case '3': d[i] = 0x30; break;
			case '4': d[i] = 0x40; break;
			case '5': d[i] = 0x50; break;
			case '6': d[i] = 0x60; break;
			case '7': d[i] = 0x70; break;
			case '8': d[i] = 0x80; break;
			case '9': d[i] = 0x90; break;
			case 'A': d[i] = 0xA0; break;
			case 'B': d[i] = 0xB0; break;
			case 'C': d[i] = 0xC0; break;
			case 'D': d[i] = 0xD0; break;
			case 'E': d[i] = 0xE0; break;
			case 'F': d[i] = 0xF0; break;
			default:
				puts("input error: not a hexadecimal string!\n");
				return 1;
		}
		switch (s[2*i+1]) {
			case '0': d[i] = d[i] | 0x00; break;
			case '1': d[i] = d[i] | 0x01; break;
			case '2': d[i] = d[i] | 0x02; break;
			case '3': d[i] = d[i] | 0x03; break;
			case '4': d[i] = d[i] | 0x04; break;
			case '5': d[i] = d[i] | 0x05; break;
			case '6': d[i] = d[i] | 0x06; break;
			case '7': d[i] = d[i] | 0x07; break;
			case '8': d[i] = d[i] | 0x08; break;
			case '9': d[i] = d[i] | 0x09; break;
			case 'A': d[i] = d[i] | 0x0A; break;
			case 'B': d[i] = d[i] | 0x0B; break;
			case 'C': d[i] = d[i] | 0x0C; break;
			case 'D': d[i] = d[i] | 0x0D; break;
			case 'E': d[i] = d[i] | 0x0E; break;
			case 'F': d[i] = d[i] | 0x0F; break;
			default:
				puts("input error: not a hexadecimal string!\n");
				return 1;
		}
	}
	for (i = 2; i < strlen(s) / 2; i++)
		calc_crc(d[i]);
	printf("\nCRC: %02X %02X\n",
		(unsigned char)crc, (unsigned char)(crc / 256));
/*
	puts("\nWould you like to continue? (y/n)\n");
	getchar();
	scanf("%c", &more);
	if (more == 'y' || more == 'Y') {
		puts("\n");
		main();
	}*/

	return 0;
}