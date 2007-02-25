#include <stdio.h>
#include <stdlib.h>
#include "gpio.h"

static int __set_gpio(char *file, unsigned int val)
{
	FILE *fp;

	if ((fp=fopen(file, "r+"))) {
		fwrite(&val, 4, 1, fp);
		fclose(fp);
	} else
		perror(file);

	return 0;
}

static unsigned int __get_gpio(char *file)
{
	FILE *fp;
	unsigned int val = 0;

	if ((fp=fopen(file, "r"))) {
		fread(&val, 4, 1, fp);
		fclose(fp);
	} else
		perror(file);

	return val;
}

/* direct 0: out
 *        1: in */
static int __enable_gpio_pin(int pin, int direct)
{
	unsigned int dir = __get_gpio(GPIODIR_FILE);
	unsigned int en  = __get_gpio(GPIOEN_FILE);

	if (!dir || !en)
		return -1;

	if (direct)
		dir |= (1 << pin);
	else
		dir &= ~(1 << pin);

	en  |= (1 << pin);

	__set_gpio(GPIODIR_FILE, dir);
	__set_gpio(GPIOEN_FILE, en);

	return 0;
}

unsigned int get_gpioin(void)
{
	return __get_gpio(GPIOIN_FILE);
}

int set_gpioout(unsigned int value)
{
	return __set_gpio(GPIOOUT_FILE, value);
}

int enable_gpioin(int pin)
{
	return 	__enable_gpio_pin(pin, 1);
}

int enable_gpioout(int pin)
{
	return 	__enable_gpio_pin(pin, 0);
}


