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
	char s[120];
	const char copy[13] = "\"<nul | clip";
	const char clear[16] = "echo off | clip";

	puts("Please enter a hexadecimal string (q to quit):");
	
	while (scanf("%s", s) && s[0] != 'q') {
		int i, size;
		unsigned char d[60];
		char cmd[100] = "set /p = \"";
		unsigned char error = 0;

		size = strlen(s);
		if (size % 2 != 0) {
			puts("The input number of characters is not double.\n"
				"Please try again.");
			continue;
		}
	
		for (i = 0; i < size / 2; i++) {
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
				case 'a':
				case 'A': d[i] = 0xA0; break;
				case 'b':
				case 'B': d[i] = 0xB0; break;
				case 'c':
				case 'C': d[i] = 0xC0; break;
				case 'd':
				case 'D': d[i] = 0xD0; break;
				case 'e':
				case 'E': d[i] = 0xE0; break;
				case 'f':
				case 'F': d[i] = 0xF0; break;
				default:
					error = 1;
					break;
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
				case 'a':
				case 'A': d[i] = d[i] | 0x0A; break;
				case 'b':
				case 'B': d[i] = d[i] | 0x0B; break;
				case 'c':
				case 'C': d[i] = d[i] | 0x0C; break;
				case 'd':
				case 'D': d[i] = d[i] | 0x0D; break;
				case 'e':
				case 'E': d[i] = d[i] | 0x0E; break;
				case 'f':
				case 'F': d[i] = d[i] | 0x0F; break;
				default:
					error = 1;
					break;
			}
		}
		if (error) {
			puts("Input error: Not a hexadecimal string.\n"
				"Please try again.");
			continue;
		}

		for (i = 2; i < size / 2; i++)
			calc_crc(d[i]);
		printf("CRC: %02X %02X\n",
			(unsigned char)crc, (unsigned char)(crc / 256));
	
		sprintf(s + size, "%02X", (unsigned char)crc);
		sprintf(s + size + 2, "%02X", (unsigned char)(crc / 256));
		crc = 0xFFFF;
	
		memcpy(cmd + strlen(cmd), s, strlen(s));
		strcpy(cmd + strlen(cmd), copy);
		system(clear);
		system(cmd);
	
		puts("The complete command has been copied to your clipboard.\n\n"
			"Please enter a hexadecimal string (q to quit):");
	}

	return 0;
}