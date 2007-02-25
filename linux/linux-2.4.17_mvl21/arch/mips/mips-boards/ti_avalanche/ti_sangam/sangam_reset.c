#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include <linux/module.h>

#include <asm/avalanche/avalanche_map.h>

#define AR7_VERSION_FILE "led_mod/hardware_version"
#define AR7_RESET_FILE "led_mod/ar7reset"
#define AR7_RESET_GPIO 11
#define RESET_POLL_TIME 1
#define RESET_HOLD_TIME 4

#define tnetd73xx_gpio_ctrl(gpio_pin, pin_mode, pin_direction) avalanche_gpio_ctrl(gpio_pin, pin_mode, pin_direction)

static struct proc_dir_entry *reset_file;
static int res_state = 0;
static int count;
static struct timer_list reset_timer, *pTimer = NULL;
static ssize_t proc_read_reset_fops(struct file *filp,
                                    char *buf,size_t count , loff_t *offp);

static ssize_t proc_read_hwversion_fops(struct file *filp, char *buf,
                                        size_t count, loff_t *offp);

struct file_operations reset_fops = {
                                     read: proc_read_reset_fops
                                   };
struct file_operations hardware_version_fops = {
                                     read: proc_read_hwversion_fops
                                   };

//add by leijun
BOOL tnetd73xx_gpio_in(unsigned gpio_pin)
{
        int num = 0;
        if (gpio_pin > 27) return 0;
        num = avalanche_gpio_in_bit(gpio_pin);
        if (num) return 1;
        return 0;
}

static ssize_t proc_read_hwversion_fops(struct file *filp, char *buf,
                                        size_t count, loff_t *offp)
{
        char line[16];
        int len = 0;
        if( *offp != 0 )
                return 0;
#if 1
        avalanche_gpio_ctrl(20,GPIO_PIN,GPIO_INPUT_PIN);
        avalanche_gpio_ctrl(21,GPIO_PIN,GPIO_INPUT_PIN);
        avalanche_gpio_ctrl(22,GPIO_PIN,GPIO_INPUT_PIN);
        avalanche_gpio_ctrl(23,GPIO_PIN,GPIO_INPUT_PIN);
        avalanche_gpio_ctrl(24,GPIO_PIN,GPIO_INPUT_PIN);
        avalanche_gpio_ctrl(25,GPIO_PIN,GPIO_INPUT_PIN);
#endif
//      printk("$$$$$$$$$$$leijun 3\n");

        len = sprintf(line,  "%d%d.%d%d%d%d\n", tnetd73xx_gpio_in(20),
                        tnetd73xx_gpio_in(21), tnetd73xx_gpio_in(22),
                        tnetd73xx_gpio_in(23), tnetd73xx_gpio_in(24),
                        tnetd73xx_gpio_in(25));

        copy_to_user(buf, line, len);
//      printk("line = %d\n", line);
        *offp = len;
        return len;
}

static void hardware_version_init(void)
{
        static struct proc_dir_entry *hardware_version_file;
        hardware_version_file = create_proc_entry(AR7_VERSION_FILE, 0777, NULL);
        if(hardware_version_file == NULL)
                 return;

        hardware_version_file->owner = THIS_MODULE;
        hardware_version_file->proc_fops = &hardware_version_fops;
#if 0
        tnetd73xx_gpio_ctrl(20,GPIO_PIN,GPIO_INPUT_PIN);
        tnetd73xx_gpio_ctrl(21,GPIO_PIN,GPIO_INPUT_PIN);
        tnetd73xx_gpio_ctrl(22,GPIO_PIN,GPIO_INPUT_PIN);
        tnetd73xx_gpio_ctrl(23,GPIO_PIN,GPIO_INPUT_PIN);
        tnetd73xx_gpio_ctrl(24,GPIO_PIN,GPIO_INPUT_PIN);
        tnetd73xx_gpio_ctrl(25,GPIO_PIN,GPIO_INPUT_PIN);
#endif
        return;
}
///over

static ssize_t proc_read_reset_fops(struct file *filp,
                                 char *buf,size_t count , loff_t *offp)
{
  char * pdata = NULL;
  char line[3];
  int len = 0;
  if( *offp != 0 )
  	return 0;

  pdata = buf;
	len = sprintf(line,"%d\n", res_state );
//wwzh
//  res_state = 0; 
  copy_to_user(buf,line,len );
	*offp = len;
	return len;
}

static void reset_timer_func(unsigned long data)
{
#if 0
  count = (avalanche_gpio_in_bit(AR7_RESET_GPIO) == 0) ? count + 1: 0;
  if( count >= RESET_HOLD_TIME/RESET_POLL_TIME )
    res_state = 1;
#else
        if (tnetd73xx_gpio_in(AR7_RESET_GPIO) == 0)
                res_state = 1;
        else
                res_state = 0;
#endif
	pTimer->expires = jiffies + HZ*RESET_POLL_TIME;
	add_timer (pTimer);
  return;
}

void sangam_reset_init(void)
{
  /* Create board reset proc file */
  proc_mkdir("led_mod",NULL); 
  reset_file = create_proc_entry( AR7_RESET_FILE, 0777, NULL);
  if( reset_file == NULL)
    goto reset_file;
  reset_file->owner = THIS_MODULE;
  reset_file->proc_fops = &reset_fops;

  /* Initialise GPIO 11 for input */
  avalanche_gpio_ctrl(AR7_RESET_GPIO,GPIO_PIN,GPIO_INPUT_PIN);

  /* Create a timer which fires every seconds */
  pTimer = &reset_timer;
	init_timer( pTimer );
	pTimer->function = reset_timer_func;
	pTimer->data = 0;
  /* Start the timer */
  reset_timer_func(0);
	hardware_version_init();//leijun

  return ;

  reset_file:

    remove_proc_entry("AR7_RESET_FILE",NULL);
	return;
}









