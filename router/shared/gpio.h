#define GPIOIN_FILE	"/dev/gpio/in"
#define GPIOOUT_FILE	"/dev/gpio/out"
#define GPIOEN_FILE	"/dev/gpio/enable"
#define GPIODIR_FILE	"/dev/gpio/dir"

#define GPIO_7		 7	/* A jump device for calibration */
#define GPIO_12		12	/* A button device for antenna selection */

unsigned int get_gpioin(void);
int set_gpioout(unsigned int value);
int enable_gpioin(int pin);
int enable_gpioout(int pin);
