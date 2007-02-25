
#include <stdio.h>

unsigned char scan_number(unsigned char c)
{
	unsigned int value = 0;
	
	switch (c)
	{
		case 'a':
		case 'A':
			value = 10;
			break;
		case 'b':
		case 'B':
			value = 11;
			break;
			
		case 'c':
		case 'C':
			value = 12;
			break;
		case 'd':
		case 'D':
			value = 13;
			break;
		case 'e':
		case 'E':
			value = 14;
			break;
		case 'f':
		case 'F':
			value = 15;
			break;
		case '0':
			value = 0;
			break;
		case '1':
			value = 1;
			break;
		case '2':
			value = 2;
			break;
		case '3':
			value = 3;
			break;
		case '4':
			value = 4;
			break;
		case '5':
			value = 5;
			break;
		case '6':
			value = 6;
			break;
		case '7':
			value = 7;
			break;
		case '8':
			value = 8;
			break;
		case '9':
			value = 9;
			break;
		default:
			value = -1;
			break;
	}
	return value;
}
int strtohex(unsigned char *dst, unsigned char *src, int len)
{
	int i,n,j;
	unsigned int c = 0x00;
	n = 0;
	j = 0;

	for (i = 0; i < len; i++)
	{
		n = i + 1;
		if (n % 2)
		{
			c = scan_number(src[i]);
			c = c << 4;
			c = c & 0x000000f0;				
		}
		else
		{	
			int temp ;
			temp = scan_number(src[i]);
			c |= temp & 0x0f;
			dst[j] = c & 0xff;
			j++;
			c = 0;
		}
	}
	return 0;
}
