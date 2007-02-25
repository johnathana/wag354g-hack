/*
    w83781d.c - Part of lm_sensors, Linux kernel modules for hardware
                monitoring
    Copyright (c) 1998, 1999, 2000  Frodo Looijaard <frodol@dds.nl>,
    Philip Edelbrock <phil@netroedge.com>,
    and Mark Studebaker <mdsxyz123@yahoo.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
    Supports following chips:

    Chip	#vin	#fanin	#pwm	#temp	wchipid	vendid	i2c	ISA
    as99127f	7	3	1?	3	0x20	0x12c3	yes	no
    w83781d	7	3	0	3	0x10	0x5ca3	yes	yes
    w83627hf	9	3	2	3	0x20	0x5ca3	yes	yes(LPC)
    w83782d	9	3	2-4	3	0x30	0x5ca3	yes	yes
    w83783s	5-6	3	2	1-2	0x40	0x5ca3	yes	no

*/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/ioport.h>
#include <linux/sysctl.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <linux/types.h>
#include <linux/i2c.h>
#define LM_DATE "20011118"
#define LM_VERSION "2.6.2"
#include <linux/sensors.h>
#include <linux/init.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,2,18)) || \
    (LINUX_VERSION_CODE == KERNEL_VERSION(2,3,0))
#define init_MUTEX(s) do { *(s) = MUTEX; } while(0)
#endif

#ifndef THIS_MODULE
#define THIS_MODULE NULL
#endif

/* RT Table support #defined so we can take it out if it gets bothersome */
#define W83781D_RT 1

/* Addresses to scan */
static unsigned short normal_i2c[] = { SENSORS_I2C_END };
static unsigned short normal_i2c_range[] = { 0x20, 0x2f, SENSORS_I2C_END };
static unsigned int normal_isa[] = { 0x0290, SENSORS_ISA_END };
static unsigned int normal_isa_range[] = { SENSORS_ISA_END };

/* Insmod parameters */
SENSORS_INSMOD_5(w83781d, w83782d, w83783s, w83627hf, as99127f);

static int init = 1;
MODULE_PARM(init, "i");
MODULE_PARM_DESC(init, "Set to zero to bypass chip initialization");

/* Many W83781D constants specified below */

/* Length of ISA address segment */
#define W83781D_EXTENT 8

/* Where are the ISA address/data registers relative to the base address */
#define W83781D_ADDR_REG_OFFSET 5
#define W83781D_DATA_REG_OFFSET 6

/* The W83781D registers */
/* The W83782D registers for nr=7,8 are in bank 5 */
#define W83781D_REG_IN_MAX(nr) ((nr < 7) ? (0x2b + (nr) * 2) : \
					   (0x554 + (((nr) - 7) * 2)))
#define W83781D_REG_IN_MIN(nr) ((nr < 7) ? (0x2c + (nr) * 2) : \
					   (0x555 + (((nr) - 7) * 2)))
#define W83781D_REG_IN(nr)     ((nr < 7) ? (0x20 + (nr)) : \
					   (0x550 + (nr) - 7))

#define W83781D_REG_FAN_MIN(nr) (0x3a + (nr))
#define W83781D_REG_FAN(nr) (0x27 + (nr))

#define W83781D_REG_TEMP2 0x0150
#define W83781D_REG_TEMP3 0x0250
#define W83781D_REG_TEMP2_HYST 0x153
#define W83781D_REG_TEMP3_HYST 0x253
#define W83781D_REG_TEMP2_CONFIG 0x152
#define W83781D_REG_TEMP3_CONFIG 0x252
#define W83781D_REG_TEMP2_OVER 0x155
#define W83781D_REG_TEMP3_OVER 0x255

#define W83781D_REG_TEMP 0x27
#define W83781D_REG_TEMP_OVER 0x39
#define W83781D_REG_TEMP_HYST 0x3A
#define W83781D_REG_BANK 0x4E

#define W83781D_REG_CONFIG 0x40
#define W83781D_REG_ALARM1 0x41
#define W83781D_REG_ALARM2 0x42
#define W83781D_REG_ALARM3 0x450	/* not on W83781D */

#define W83781D_REG_BEEP_CONFIG 0x4D
#define W83781D_REG_BEEP_INTS1 0x56
#define W83781D_REG_BEEP_INTS2 0x57
#define W83781D_REG_BEEP_INTS3 0x453	/* not on W83781D */

#define W83781D_REG_VID_FANDIV 0x47

#define W83781D_REG_CHIPID 0x49
#define W83781D_REG_WCHIPID 0x58
#define W83781D_REG_CHIPMAN 0x4F
#define W83781D_REG_PIN 0x4B

/* 782D/783S only */
#define W83781D_REG_VBAT 0x5D

/* PWM 782D (1-4) and 783S (1-2) only */
#define W83781D_REG_PWM1 0x5B	/* 782d and 783s/627hf datasheets disagree */
				/* on which is which; */
#define W83781D_REG_PWM2 0x5A	/* We follow the 782d convention here, */
				/* However 782d is probably wrong. */
#define W83781D_REG_PWM3 0x5E
#define W83781D_REG_PWM4 0x5F
#define W83781D_REG_PWMCLK12 0x5C
#define W83781D_REG_PWMCLK34 0x45C
static const u8 regpwm[] = { W83781D_REG_PWM1, W83781D_REG_PWM2,
	W83781D_REG_PWM3, W83781D_REG_PWM4
};
#define W83781D_REG_PWM(nr) (regpwm[(nr) - 1])

#define W83781D_REG_I2C_ADDR 0x48
#define W83781D_REG_I2C_SUBADDR 0x4A

/* The following are undocumented in the data sheets however we
   received the information in an email from Winbond tech support */
/* Sensor selection - not on 781d */
#define W83781D_REG_SCFG1 0x5D
static const u8 BIT_SCFG1[] = { 0x02, 0x04, 0x08 };
#define W83781D_REG_SCFG2 0x59
static const u8 BIT_SCFG2[] = { 0x10, 0x20, 0x40 };
#define W83781D_DEFAULT_BETA 3435

/* RT Table registers */
#define W83781D_REG_RT_IDX 0x50
#define W83781D_REG_RT_VAL 0x51

/* Conversions. Rounding and limit checking is only done on the TO_REG
   variants. Note that you should be a bit careful with which arguments
   these macros are called: arguments may be evaluated more than once.
   Fixing this is just not worth it. */
#define IN_TO_REG(val)  (SENSORS_LIMIT((((val) * 10 + 8)/16),0,255))
#define IN_FROM_REG(val) (((val) * 16) / 10)

extern inline u8 FAN_TO_REG(long rpm, int div)
{
	if (rpm == 0)
		return 255;
	rpm = SENSORS_LIMIT(rpm, 1, 1000000);
	return SENSORS_LIMIT((1350000 + rpm * div / 2) / (rpm * div), 1,
			     254);
}

#define FAN_FROM_REG(val,div) ((val)==0?-1:(val)==255?0:1350000/((val)*(div)))

#define TEMP_TO_REG(val) (SENSORS_LIMIT(((val)<0?(((val)-5)/10):\
                                                 ((val)+5)/10),0,255))
#define TEMP_FROM_REG(val) (((val)>0x80?(val)-0x100:(val))*10)

#define TEMP_ADD_TO_REG(val)   (SENSORS_LIMIT(((((val) + 2) / 5) << 7),\
                                              0,0xffff))
#define TEMP_ADD_FROM_REG(val) (((val) >> 7) * 5)

#define AS99127_TEMP_ADD_TO_REG(val) (SENSORS_LIMIT((((((val) + 2)*4)/10) \
                                               << 7),0,0xffff))
#define AS99127_TEMP_ADD_FROM_REG(val) ((((val) >> 7) * 10) / 4)

#define VID_FROM_REG(val) ((val)==0x1f?0:(val)>=0x10?510-(val)*10:\
                           205-(val)*5)
#define ALARMS_FROM_REG(val) (val)
#define PWM_FROM_REG(val) (val)
#define PWM_TO_REG(val) (SENSORS_LIMIT((val),0,255))
#define BEEPS_FROM_REG(val) (val)
#define BEEPS_TO_REG(val) ((val) & 0xffffff)

#define BEEP_ENABLE_TO_REG(val)   ((val)?1:0)
#define BEEP_ENABLE_FROM_REG(val) ((val)?1:0)

#define DIV_FROM_REG(val) (1 << (val))

extern inline u8 DIV_TO_REG(long val, enum chips type)
{
	int i;
	val = SENSORS_LIMIT(val, 1,
		((type == w83781d || type == as99127f) ? 8 : 128)) >> 1;
	for (i = 0; i < 6; i++) {
		if (val == 0)
			break;
		val >>= 1;
	}
	return ((u8) i);
}

/* Initial limits */
#define W83781D_INIT_IN_0 (vid==350?280:vid)
#define W83781D_INIT_IN_1 (vid==350?280:vid)
#define W83781D_INIT_IN_2 330
#define W83781D_INIT_IN_3 (((500)   * 100)/168)
#define W83781D_INIT_IN_4 (((1200)  * 10)/38)
#define W83781D_INIT_IN_5 (((-1200) * -604)/2100)
#define W83781D_INIT_IN_6 (((-500)  * -604)/909)
#define W83781D_INIT_IN_7 (((500)   * 100)/168)
#define W83781D_INIT_IN_8 300
/* Initial limits for 782d/783s negative voltages */
/* Note level shift. Change min/max below if you change these. */
#define W83782D_INIT_IN_5 ((((-1200) + 1491) * 100)/514)
#define W83782D_INIT_IN_6 ((( (-500)  + 771) * 100)/314)

#define W83781D_INIT_IN_PERCENTAGE 10

#define W83781D_INIT_IN_MIN_0 \
        (W83781D_INIT_IN_0 - W83781D_INIT_IN_0 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_0 \
        (W83781D_INIT_IN_0 + W83781D_INIT_IN_0 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MIN_1 \
        (W83781D_INIT_IN_1 - W83781D_INIT_IN_1 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_1 \
        (W83781D_INIT_IN_1 + W83781D_INIT_IN_1 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MIN_2 \
        (W83781D_INIT_IN_2 - W83781D_INIT_IN_2 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_2 \
        (W83781D_INIT_IN_2 + W83781D_INIT_IN_2 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MIN_3 \
        (W83781D_INIT_IN_3 - W83781D_INIT_IN_3 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_3 \
        (W83781D_INIT_IN_3 + W83781D_INIT_IN_3 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MIN_4 \
        (W83781D_INIT_IN_4 - W83781D_INIT_IN_4 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_4 \
        (W83781D_INIT_IN_4 + W83781D_INIT_IN_4 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MIN_5 \
        (W83781D_INIT_IN_5 - W83781D_INIT_IN_5 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_5 \
        (W83781D_INIT_IN_5 + W83781D_INIT_IN_5 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MIN_6 \
        (W83781D_INIT_IN_6 - W83781D_INIT_IN_6 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_6 \
        (W83781D_INIT_IN_6 + W83781D_INIT_IN_6 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MIN_7 \
        (W83781D_INIT_IN_7 - W83781D_INIT_IN_7 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_7 \
        (W83781D_INIT_IN_7 + W83781D_INIT_IN_7 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MIN_8 \
        (W83781D_INIT_IN_8 - W83781D_INIT_IN_8 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
#define W83781D_INIT_IN_MAX_8 \
        (W83781D_INIT_IN_8 + W83781D_INIT_IN_8 * W83781D_INIT_IN_PERCENTAGE \
         / 100)
/* Initial limits for 782d/783s negative voltages */
/* These aren't direct multiples because of level shift */
/* Beware going negative - check */
#define W83782D_INIT_IN_MIN_5_TMP \
        (((-1200 * (100 + W83781D_INIT_IN_PERCENTAGE)) + (1491 * 100))/514)
#define W83782D_INIT_IN_MIN_5 \
        ((W83782D_INIT_IN_MIN_5_TMP > 0) ? W83782D_INIT_IN_MIN_5_TMP : 0)
#define W83782D_INIT_IN_MAX_5 \
        (((-1200 * (100 - W83781D_INIT_IN_PERCENTAGE)) + (1491 * 100))/514)
#define W83782D_INIT_IN_MIN_6_TMP \
        ((( -500 * (100 + W83781D_INIT_IN_PERCENTAGE)) +  (771 * 100))/314)
#define W83782D_INIT_IN_MIN_6 \
        ((W83782D_INIT_IN_MIN_6_TMP > 0) ? W83782D_INIT_IN_MIN_6_TMP : 0)
#define W83782D_INIT_IN_MAX_6 \
        ((( -500 * (100 - W83781D_INIT_IN_PERCENTAGE)) +  (771 * 100))/314)

#define W83781D_INIT_FAN_MIN_1 3000
#define W83781D_INIT_FAN_MIN_2 3000
#define W83781D_INIT_FAN_MIN_3 3000

#define W83781D_INIT_TEMP_OVER 600
#define W83781D_INIT_TEMP_HYST 500
#define W83781D_INIT_TEMP2_OVER 600
#define W83781D_INIT_TEMP2_HYST 500
#define W83781D_INIT_TEMP3_OVER 600
#define W83781D_INIT_TEMP3_HYST 500

#ifdef MODULE
extern int init_module(void);
extern int cleanup_module(void);
#endif				/* MODULE */

/* There are some complications in a module like this. First off, W83781D chips
   may be both present on the SMBus and the ISA bus, and we have to handle
   those cases separately at some places. Second, there might be several
   W83781D chips available (well, actually, that is probably never done; but
   it is a clean illustration of how to handle a case like that). Finally,
   a specific chip may be attached to *both* ISA and SMBus, and we would
   not like to detect it double. Fortunately, in the case of the W83781D at
   least, a register tells us what SMBus address we are on, so that helps
   a bit - except if there could be more than one SMBus. Groan. No solution
   for this yet. */

/* This module may seem overly long and complicated. In fact, it is not so
   bad. Quite a lot of bookkeeping is done. A real driver can often cut
   some corners. */

/* For each registered W83781D, we need to keep some data in memory. That
   data is pointed to by w83781d_list[NR]->data. The structure itself is
   dynamically allocated, at the same time when a new w83781d client is
   allocated. */
struct w83781d_data {
	struct semaphore lock;
	int sysctl_id;
	enum chips type;

	struct semaphore update_lock;
	char valid;		/* !=0 if following fields are valid */
	unsigned long last_updated;	/* In jiffies */

	struct i2c_client *lm75;	/* for secondary I2C addresses */
	/* pointer to array of 2 subclients */

	u8 in[9];		/* Register value - 8 & 9 for 782D only */
	u8 in_max[9];		/* Register value - 8 & 9 for 782D only */
	u8 in_min[9];		/* Register value - 8 & 9 for 782D only */
	u8 fan[3];		/* Register value */
	u8 fan_min[3];		/* Register value */
	u8 temp;
	u8 temp_over;		/* Register value */
	u8 temp_hyst;		/* Register value */
	u16 temp_add[2];	/* Register value */
	u16 temp_add_over[2];	/* Register value */
	u16 temp_add_hyst[2];	/* Register value */
	u8 fan_div[3];		/* Register encoding, shifted right */
	u8 vid;			/* Register encoding, combined */
	u32 alarms;		/* Register encoding, combined */
	u32 beeps;		/* Register encoding, combined */
	u8 beep_enable;		/* Boolean */
	u8 pwm[4];		/* Register value */
	u16 sens[3];		/* 782D/783S only.
				   1 = pentium diode; 2 = 3904 diode;
				   3000-5000 = thermistor beta.
				   Default = 3435. 
				   Other Betas unimplemented */
#ifdef W83781D_RT
	u8 rt[3][32];		/* Register value */
#endif
};


#ifdef MODULE
static
#else
extern
#endif
int __init sensors_w83781d_init(void);
static int __init w83781d_cleanup(void);

static int w83781d_attach_adapter(struct i2c_adapter *adapter);
static int w83781d_detect(struct i2c_adapter *adapter, int address,
			  unsigned short flags, int kind);
static int w83781d_detach_client(struct i2c_client *client);
static int w83781d_command(struct i2c_client *client, unsigned int cmd,
			   void *arg);
static void w83781d_inc_use(struct i2c_client *client);
static void w83781d_dec_use(struct i2c_client *client);

static int w83781d_read_value(struct i2c_client *client, u16 register);
static int w83781d_write_value(struct i2c_client *client, u16 register,
			       u16 value);
static void w83781d_update_client(struct i2c_client *client);
static void w83781d_init_client(struct i2c_client *client);


static void w83781d_in(struct i2c_client *client, int operation,
		       int ctl_name, int *nrels_mag, long *results);
static void w83781d_fan(struct i2c_client *client, int operation,
			int ctl_name, int *nrels_mag, long *results);
static void w83781d_temp(struct i2c_client *client, int operation,
			 int ctl_name, int *nrels_mag, long *results);
static void w83781d_temp_add(struct i2c_client *client, int operation,
			     int ctl_name, int *nrels_mag, long *results);
static void w83781d_vid(struct i2c_client *client, int operation,
			int ctl_name, int *nrels_mag, long *results);
static void w83781d_alarms(struct i2c_client *client, int operation,
			   int ctl_name, int *nrels_mag, long *results);
static void w83781d_beep(struct i2c_client *client, int operation,
			 int ctl_name, int *nrels_mag, long *results);
static void w83781d_fan_div(struct i2c_client *client, int operation,
			    int ctl_name, int *nrels_mag, long *results);
static void w83781d_pwm(struct i2c_client *client, int operation,
			int ctl_name, int *nrels_mag, long *results);
static void w83781d_sens(struct i2c_client *client, int operation,
			 int ctl_name, int *nrels_mag, long *results);
#ifdef W83781D_RT
static void w83781d_rt(struct i2c_client *client, int operation,
		       int ctl_name, int *nrels_mag, long *results);
#endif
static u16 swap_bytes(u16 val);

static int w83781d_id = 0;

static struct i2c_driver w83781d_driver = {
	/* name */ "W83781D sensor driver",
	/* id */ I2C_DRIVERID_W83781D,
	/* flags */ I2C_DF_NOTIFY,
	/* attach_adapter */ &w83781d_attach_adapter,
	/* detach_client */ &w83781d_detach_client,
	/* command */ &w83781d_command,
	/* inc_use */ &w83781d_inc_use,
	/* dec_use */ &w83781d_dec_use
};

/* Used by w83781d_init/cleanup */
static int __initdata w83781d_initialized = 0;

/* The /proc/sys entries */
/* These files are created for each detected chip. This is just a template;
   though at first sight, you might think we could use a statically
   allocated list, we need some way to get back to the parent - which
   is done through one of the 'extra' fields which are initialized 
   when a new copy is allocated. */

/* just a guess - no datasheet */
static ctl_table as99127f_dir_table_template[] = {
	{W83781D_SYSCTL_IN0, "in0", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN1, "in1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN2, "in2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN3, "in3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN4, "in4", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN5, "in5", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN6, "in6", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_FAN1, "fan1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN2, "fan2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN3, "fan3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_TEMP1, "temp1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp},
	{W83781D_SYSCTL_TEMP2, "temp2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_TEMP3, "temp3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_VID, "vid", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_vid},
	{W83781D_SYSCTL_FAN_DIV, "fan_div", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan_div},
	{W83781D_SYSCTL_ALARMS, "alarms", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_alarms},
	{W83781D_SYSCTL_BEEP, "beep", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_beep},
	{W83781D_SYSCTL_PWM1, "pwm1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_PWM2, "pwm2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{0}
};

static ctl_table w83781d_dir_table_template[] = {
	{W83781D_SYSCTL_IN0, "in0", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN1, "in1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN2, "in2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN3, "in3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN4, "in4", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN5, "in5", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN6, "in6", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_FAN1, "fan1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN2, "fan2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN3, "fan3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_TEMP1, "temp1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp},
	{W83781D_SYSCTL_TEMP2, "temp2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_TEMP3, "temp3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_VID, "vid", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_vid},
	{W83781D_SYSCTL_FAN_DIV, "fan_div", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan_div},
	{W83781D_SYSCTL_ALARMS, "alarms", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_alarms},
	{W83781D_SYSCTL_BEEP, "beep", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_beep},
#ifdef W83781D_RT
	{W83781D_SYSCTL_RT1, "rt1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_rt},
	{W83781D_SYSCTL_RT2, "rt2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_rt},
	{W83781D_SYSCTL_RT3, "rt3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_rt},
#endif
	{0}
};

/* without pwm3-4 */
static ctl_table w83782d_isa_dir_table_template[] = {
	{W83781D_SYSCTL_IN0, "in0", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN1, "in1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN2, "in2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN3, "in3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN4, "in4", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN5, "in5", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN6, "in6", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN7, "in7", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN8, "in8", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_FAN1, "fan1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN2, "fan2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN3, "fan3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_TEMP1, "temp1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp},
	{W83781D_SYSCTL_TEMP2, "temp2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_TEMP3, "temp3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_VID, "vid", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_vid},
	{W83781D_SYSCTL_FAN_DIV, "fan_div", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan_div},
	{W83781D_SYSCTL_ALARMS, "alarms", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_alarms},
	{W83781D_SYSCTL_BEEP, "beep", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_beep},
	{W83781D_SYSCTL_PWM1, "pwm1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_PWM2, "pwm2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_SENS1, "sensor1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_sens},
	{W83781D_SYSCTL_SENS2, "sensor2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_sens},
	{W83781D_SYSCTL_SENS3, "sensor3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_sens},
	{0}
};

/* with pwm3-4 */
static ctl_table w83782d_i2c_dir_table_template[] = {
	{W83781D_SYSCTL_IN0, "in0", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN1, "in1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN2, "in2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN3, "in3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN4, "in4", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN5, "in5", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN6, "in6", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN7, "in7", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN8, "in8", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_FAN1, "fan1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN2, "fan2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN3, "fan3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_TEMP1, "temp1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp},
	{W83781D_SYSCTL_TEMP2, "temp2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_TEMP3, "temp3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_VID, "vid", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_vid},
	{W83781D_SYSCTL_FAN_DIV, "fan_div", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan_div},
	{W83781D_SYSCTL_ALARMS, "alarms", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_alarms},
	{W83781D_SYSCTL_BEEP, "beep", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_beep},
	{W83781D_SYSCTL_PWM1, "pwm1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_PWM2, "pwm2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_PWM3, "pwm3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_PWM4, "pwm4", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_SENS1, "sensor1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_sens},
	{W83781D_SYSCTL_SENS2, "sensor2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_sens},
	{W83781D_SYSCTL_SENS3, "sensor3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_sens},
	{0}
};

static ctl_table w83783s_dir_table_template[] = {
	{W83781D_SYSCTL_IN0, "in0", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	/* no in1 to maintain compatibility with 781d and 782d. */
	{W83781D_SYSCTL_IN2, "in2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN3, "in3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN4, "in4", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN5, "in5", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_IN6, "in6", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_in},
	{W83781D_SYSCTL_FAN1, "fan1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN2, "fan2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_FAN3, "fan3", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan},
	{W83781D_SYSCTL_TEMP1, "temp1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp},
	{W83781D_SYSCTL_TEMP2, "temp2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_temp_add},
	{W83781D_SYSCTL_VID, "vid", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_vid},
	{W83781D_SYSCTL_FAN_DIV, "fan_div", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_fan_div},
	{W83781D_SYSCTL_ALARMS, "alarms", NULL, 0, 0444, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_alarms},
	{W83781D_SYSCTL_BEEP, "beep", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_beep},
	{W83781D_SYSCTL_PWM1, "pwm1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_PWM2, "pwm2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_pwm},
	{W83781D_SYSCTL_SENS1, "sensor1", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_sens},
	{W83781D_SYSCTL_SENS2, "sensor2", NULL, 0, 0644, NULL, &i2c_proc_real,
	 &i2c_sysctl_real, NULL, &w83781d_sens},
	{0}
};


/* This function is called when:
     * w83781d_driver is inserted (when this module is loaded), for each
       available adapter
     * when a new adapter is inserted (and w83781d_driver is still present) */
int w83781d_attach_adapter(struct i2c_adapter *adapter)
{
	return i2c_detect(adapter, &addr_data, w83781d_detect);
}

int w83781d_detect(struct i2c_adapter *adapter, int address,
		   unsigned short flags, int kind)
{
	int i, val1, val2;
	struct i2c_client *new_client;
	struct w83781d_data *data;
	int err = 0;
	const char *type_name = "";
	const char *client_name = "";
	int is_isa = i2c_is_isa_adapter(adapter);
	enum vendor { winbond, asus } vendid;

	if (!is_isa
	    && !i2c_check_functionality(adapter,
					I2C_FUNC_SMBUS_BYTE_DATA)) goto
		    ERROR0;

	if (is_isa) {
		if (check_region(address, W83781D_EXTENT))
			goto ERROR0;
	}

	/* Probe whether there is anything available on this address. Already
	   done for SMBus clients */
	if (kind < 0) {
		if (is_isa) {

#define REALLY_SLOW_IO
			/* We need the timeouts for at least some LM78-like chips. But only
			   if we read 'undefined' registers. */
			i = inb_p(address + 1);
			if (inb_p(address + 2) != i)
				goto ERROR0;
			if (inb_p(address + 3) != i)
				goto ERROR0;
			if (inb_p(address + 7) != i)
				goto ERROR0;
#undef REALLY_SLOW_IO

			/* Let's just hope nothing breaks here */
			i = inb_p(address + 5) & 0x7f;
			outb_p(~i & 0x7f, address + 5);
			if ((inb_p(address + 5) & 0x7f) != (~i & 0x7f)) {
				outb_p(i, address + 5);
				return 0;
			}
		}
	}

	/* OK. For now, we presume we have a valid client. We now create the
	   client structure, even though we cannot fill it completely yet.
	   But it allows us to access w83781d_{read,write}_value. */

	if (!(new_client = kmalloc(sizeof(struct i2c_client) +
				   sizeof(struct w83781d_data),
				   GFP_KERNEL))) {
		err = -ENOMEM;
		goto ERROR0;
	}

	data = (struct w83781d_data *) (new_client + 1);
	new_client->addr = address;
	init_MUTEX(&data->lock);
	new_client->data = data;
	new_client->adapter = adapter;
	new_client->driver = &w83781d_driver;
	new_client->flags = 0;

	/* Now, we do the remaining detection. */

	/* The w8378?d may be stuck in some other bank than bank 0. This may
	   make reading other information impossible. Specify a force=... or
	   force_*=... parameter, and the Winbond will be reset to the right
	   bank. */
	if (kind < 0) {
		if (w83781d_read_value(new_client, W83781D_REG_CONFIG) &
		    0x80) goto ERROR1;
		val1 = w83781d_read_value(new_client, W83781D_REG_BANK);
		val2 = w83781d_read_value(new_client, W83781D_REG_CHIPMAN);
		/* Check for Winbond or Asus ID if in bank 0 */
		if ((!(val1 & 0x07)) &&
		    (((!(val1 & 0x80)) && (val2 != 0xa3) && (val2 != 0xc3))
		     || ((val1 & 0x80) && (val2 != 0x5c)
			 && (val2 != 0x12)))) goto ERROR1;
		/* If Winbond SMBus, check address at 0x48. Asus doesn't support */
		if ((!is_isa) && (((!(val1 & 0x80)) && (val2 == 0xa3)) ||
				  ((val1 & 0x80) && (val2 == 0x5c)))) {
			if (w83781d_read_value
			    (new_client, W83781D_REG_I2C_ADDR) != address)
				goto ERROR1;
		}
	}

	/* We have either had a force parameter, or we have already detected the
	   Winbond. Put it now into bank 0 and Vendor ID High Byte */
	w83781d_write_value(new_client, W83781D_REG_BANK,
			    (w83781d_read_value(new_client,
						W83781D_REG_BANK) & 0x78) |
			    0x80);

	/* Determine the chip type. */
	if (kind <= 0) {
		/* get vendor ID */
		val2 = w83781d_read_value(new_client, W83781D_REG_CHIPMAN);
		if (val2 == 0x5c)
			vendid = winbond;
		else if (val2 == 0x12)
			vendid = asus;
		else
			goto ERROR1;
		/* mask off lower bit, not reliable */
		val1 =
		    w83781d_read_value(new_client,
				       W83781D_REG_WCHIPID) & 0xfe;
		if (val1 == 0x10 && vendid == winbond)
			kind = w83781d;
		else if (val1 == 0x30 && vendid == winbond)
			kind = w83782d;
		else if (val1 == 0x40 && vendid == winbond && !is_isa)
			kind = w83783s;
		else if (val1 == 0x20 && vendid == winbond)
			kind = w83627hf;
		else if (val1 == 0x30 && vendid == asus && !is_isa)
			kind = as99127f;
		else {
			if (kind == 0)
				printk
				    ("w83781d.o: Ignoring 'force' parameter for unknown chip at"
				     "adapter %d, address 0x%02x\n",
				     i2c_adapter_id(adapter), address);
			goto ERROR1;
		}
	}

	if (kind == w83781d) {
		type_name = "w83781d";
		client_name = "W83781D chip";
	} else if (kind == w83782d) {
		type_name = "w83782d";
		client_name = "W83782D chip";
	} else if (kind == w83783s) {
		type_name = "w83783s";
		client_name = "W83783S chip";
	} else if (kind == w83627hf) {
		type_name = "w83627hf";
		client_name = "W83627HF chip";
	} else if (kind == as99127f) {
		type_name = "as99127f";
		client_name = "AS99127F chip";
	} else {
#ifdef DEBUG
		printk("w83781d.o: Internal error: unknown kind (%d)?!?",
		       kind);
#endif
		goto ERROR1;
	}

	/* Reserve the ISA region */
	if (is_isa)
		request_region(address, W83781D_EXTENT, type_name);

	/* Fill in the remaining client fields and put it into the global list */
	strcpy(new_client->name, client_name);
	data->type = kind;

	new_client->id = w83781d_id++;
	data->valid = 0;
	init_MUTEX(&data->update_lock);

	/* Tell the I2C layer a new client has arrived */
	if ((err = i2c_attach_client(new_client)))
		goto ERROR3;

	/* attach secondary i2c lm75-like clients */
	if (!is_isa) {
		if (!(data->lm75 = kmalloc(2 * sizeof(struct i2c_client),
					   GFP_KERNEL))) {
			err = -ENOMEM;
			goto ERROR4;
		}
		val1 = w83781d_read_value(new_client,
				          W83781D_REG_I2C_SUBADDR);
		data->lm75[0].addr = 0x48 + (val1 & 0x07);
		if (kind != w83783s) {
			data->lm75[1].addr = 0x48 + ((val1 >> 4) & 0x07);
			if(data->lm75[0].addr == data->lm75[1].addr) {
				printk("w83781d.o: Duplicate addresses 0x%x for subclients.\n",
					data->lm75[0].addr);
				goto ERROR5;
			}
		}
		if (kind == w83781d)
			client_name = "W83781D subclient";
		else if (kind == w83782d)
			client_name = "W83782D subclient";
		else if (kind == w83783s)
			client_name = "W83783S subclient";
		else if (kind == w83627hf)
			client_name = "W83627HF subclient";
		else if (kind == as99127f)
			client_name = "AS99127F subclient";

		for (i = 0; i <= 1; i++) {
			data->lm75[i].data = NULL;	/* store all data in w83781d */
			data->lm75[i].adapter = adapter;
			data->lm75[i].driver = &w83781d_driver;
			data->lm75[i].flags = 0;
			strcpy(data->lm75[i].name, client_name);
			data->lm75[i].id = w83781d_id++;
			if ((err = i2c_attach_client(&(data->lm75[i])))) {
				printk("w83781d.o: Subclient %d registration at address 0x%x failed.\n",
				       i, data->lm75[i].addr);
				if (i == 1)
					goto ERROR6;
				else
					goto ERROR5;
			}
			if (kind == w83783s)
				break;
		}
	} else {
		data->lm75 = NULL;
	}

	/* Register a new directory entry with module sensors */
	if ((i = i2c_register_entry(new_client,
					type_name,
					kind == as99127f ?
					   as99127f_dir_table_template :
					kind == w83781d ?
					   w83781d_dir_table_template :
					kind == w83783s ?
					   w83783s_dir_table_template :
					(is_isa || kind == w83627hf) ?
					   w83782d_isa_dir_table_template :
					   w83782d_i2c_dir_table_template,
					THIS_MODULE)) < 0) {
		err = i;
		goto ERROR7;
	}
	data->sysctl_id = i;

	/* Initialize the Winbond chip */
	w83781d_init_client(new_client);
	return 0;

/* OK, this is not exactly good programming practice, usually. But it is
   very code-efficient in this case. */

      ERROR7:
	if (!is_isa)
		i2c_detach_client(&
				  (((struct
				     w83781d_data *) (new_client->data))->
				   lm75[1]));
      ERROR6:
	if (!is_isa)
		i2c_detach_client(&
				  (((struct
				     w83781d_data *) (new_client->data))->
				   lm75[0]));
      ERROR5:
	if (!is_isa)
		kfree(((struct w83781d_data *) (new_client->data))->lm75);
      ERROR4:
	i2c_detach_client(new_client);
      ERROR3:
	if (is_isa)
		release_region(address, W83781D_EXTENT);
      ERROR1:
	kfree(new_client);
      ERROR0:
	return err;
}

int w83781d_detach_client(struct i2c_client *client)
{
	int err;

	i2c_deregister_entry(((struct w83781d_data *) (client->data))->
				 sysctl_id);

	if ((err = i2c_detach_client(client))) {
		printk
		    ("w83781d.o: Client deregistration failed, client not detached.\n");
		return err;
	}

	if(i2c_is_isa_client(client)) {
		release_region(client->addr, W83781D_EXTENT);
	} else {
		i2c_detach_client(&
				  (((struct
				     w83781d_data *) (client->data))->
				   lm75[0]));
		if((((struct w83781d_data *) (client->data))->type) != w83783s)
			i2c_detach_client(&
				  (((struct
				     w83781d_data *) (client->data))->
				   lm75[1]));
		kfree(((struct w83781d_data *) (client->data))->lm75);
	}
	kfree(client);

	return 0;
}

/* No commands defined yet */
int w83781d_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	return 0;
}

void w83781d_inc_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}

void w83781d_dec_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}

u16 swap_bytes(u16 val)
{
	return (val >> 8) | (val << 8);
}

/* The SMBus locks itself, usually, but nothing may access the Winbond between
   bank switches. ISA access must always be locked explicitly! 
   We ignore the W83781D BUSY flag at this moment - it could lead to deadlocks,
   would slow down the W83781D access and should not be necessary. 
   There are some ugly typecasts here, but the good news is - they should
   nowhere else be necessary! */
int w83781d_read_value(struct i2c_client *client, u16 reg)
{
	int res, word_sized, bank;
	struct i2c_client *cl;

	down(&(((struct w83781d_data *) (client->data))->lock));
	if (i2c_is_isa_client(client)) {
		word_sized = (((reg & 0xff00) == 0x100)
			      || ((reg & 0xff00) == 0x200))
		    && (((reg & 0x00ff) == 0x50)
			|| ((reg & 0x00ff) == 0x53)
			|| ((reg & 0x00ff) == 0x55));
		if (reg & 0xff00) {
			outb_p(W83781D_REG_BANK,
			       client->addr + W83781D_ADDR_REG_OFFSET);
			outb_p(reg >> 8,
			       client->addr + W83781D_DATA_REG_OFFSET);
		}
		outb_p(reg & 0xff, client->addr + W83781D_ADDR_REG_OFFSET);
		res = inb_p(client->addr + W83781D_DATA_REG_OFFSET);
		if (word_sized) {
			outb_p((reg & 0xff) + 1,
			       client->addr + W83781D_ADDR_REG_OFFSET);
			res =
			    (res << 8) + inb_p(client->addr +
					       W83781D_DATA_REG_OFFSET);
		}
		if (reg & 0xff00) {
			outb_p(W83781D_REG_BANK,
			       client->addr + W83781D_ADDR_REG_OFFSET);
			outb_p(0, client->addr + W83781D_DATA_REG_OFFSET);
		}
	} else {
		bank = (reg >> 8) & 0x0f;
		if (bank > 2)
			/* switch banks */
			i2c_smbus_write_byte_data(client, W83781D_REG_BANK,
						  bank);
		if (bank == 0 || bank > 2) {
			res = i2c_smbus_read_byte_data(client, reg & 0xff);
		} else {
			/* switch to subclient */
			cl =
			    &(((struct w83781d_data *) (client->data))->
			      lm75[bank - 1]);
			/* convert from ISA to LM75 I2C addresses */
			switch (reg & 0xff) {
			case 0x50: /* TEMP */
				res =
				    swap_bytes(i2c_smbus_read_word_data
					       (cl, 0));
				break;
			case 0x52: /* CONFIG */
				res = i2c_smbus_read_byte_data(cl, 1);
				break;
			case 0x53: /* HYST */
				res =
				    swap_bytes(i2c_smbus_read_word_data
					       (cl, 2));
				break;
			case 0x55: /* OVER */
			default:
				res =
				    swap_bytes(i2c_smbus_read_word_data
					       (cl, 3));
				break;
			}
		}
		if (bank > 2)
			i2c_smbus_write_byte_data(client, W83781D_REG_BANK,
						  0);
	}
	up(&(((struct w83781d_data *) (client->data))->lock));
	return res;
}

int w83781d_write_value(struct i2c_client *client, u16 reg, u16 value)
{
	int word_sized, bank;
	struct i2c_client *cl;

	down(&(((struct w83781d_data *) (client->data))->lock));
	if (i2c_is_isa_client(client)) {
		word_sized = (((reg & 0xff00) == 0x100)
			      || ((reg & 0xff00) == 0x200))
		    && (((reg & 0x00ff) == 0x53)
			|| ((reg & 0x00ff) == 0x55));
		if (reg & 0xff00) {
			outb_p(W83781D_REG_BANK,
			       client->addr + W83781D_ADDR_REG_OFFSET);
			outb_p(reg >> 8,
			       client->addr + W83781D_DATA_REG_OFFSET);
		}
		outb_p(reg & 0xff, client->addr + W83781D_ADDR_REG_OFFSET);
		if (word_sized) {
			outb_p(value >> 8,
			       client->addr + W83781D_DATA_REG_OFFSET);
			outb_p((reg & 0xff) + 1,
			       client->addr + W83781D_ADDR_REG_OFFSET);
		}
		outb_p(value & 0xff,
		       client->addr + W83781D_DATA_REG_OFFSET);
		if (reg & 0xff00) {
			outb_p(W83781D_REG_BANK,
			       client->addr + W83781D_ADDR_REG_OFFSET);
			outb_p(0, client->addr + W83781D_DATA_REG_OFFSET);
		}
	} else {
		bank = (reg >> 8) & 0x0f;
		if (bank > 2)
			/* switch banks */
			i2c_smbus_write_byte_data(client, W83781D_REG_BANK,
						  bank);
		if (bank == 0 || bank > 2) {
			i2c_smbus_write_byte_data(client, reg & 0xff,
						  value & 0xff);
		} else {
			/* switch to subclient */
			cl = &(((struct w83781d_data *) (client->data))->
			      lm75[bank - 1]);
			/* convert from ISA to LM75 I2C addresses */
			switch (reg & 0xff) {
			case 0x52: /* CONFIG */
				i2c_smbus_write_byte_data(cl, 1,
							  value & 0xff);
				break;
			case 0x53: /* HYST */
				i2c_smbus_write_word_data(cl, 2,
							  swap_bytes(value));
				break;
			case 0x55: /* OVER */
				i2c_smbus_write_word_data(cl, 3,
							  swap_bytes(value));
				break;
			}
		}
		if (bank > 2)
			i2c_smbus_write_byte_data(client, W83781D_REG_BANK,
						  0);
	}
	up(&(((struct w83781d_data *) (client->data))->lock));
	return 0;
}

/* Called when we have found a new W83781D. It should set limits, etc. */
void w83781d_init_client(struct i2c_client *client)
{
	struct w83781d_data *data = client->data;
	int vid, i;
	int type = data->type;
	u8 tmp;

	if(init && type != as99127f) { /* this resets registers we don't have
			                  documentation for on the as99127f */
		/* save this register */
		i = w83781d_read_value(client, W83781D_REG_BEEP_CONFIG);
		/* Reset all except Watchdog values and last conversion values
		   This sets fan-divs to 2, among others */
		w83781d_write_value(client, W83781D_REG_CONFIG, 0x80);
		/* Restore the register and disable power-on abnormal beep.
		   This saves FAN 1/2/3 input/output values set by BIOS. */
		w83781d_write_value(client, W83781D_REG_BEEP_CONFIG, i | 0x80);
		/* Disable master beep-enable (reset turns it on).
		   Individual beeps should be reset to off but for some reason
		   disabling this bit helps some people not get beeped */
		w83781d_write_value(client, W83781D_REG_BEEP_INTS2, 0);
	}

	vid = w83781d_read_value(client, W83781D_REG_VID_FANDIV) & 0x0f;
	vid |=
	    (w83781d_read_value(client, W83781D_REG_CHIPID) & 0x01) << 4;
	vid = VID_FROM_REG(vid);

	if ((type != w83781d) && (type != as99127f)) {
		tmp = w83781d_read_value(client, W83781D_REG_SCFG1);
		for (i = 1; i <= 3; i++) {
			if (!(tmp & BIT_SCFG1[i - 1])) {
				data->sens[i - 1] = W83781D_DEFAULT_BETA;
			} else {
				if (w83781d_read_value
				    (client,
				     W83781D_REG_SCFG2) & BIT_SCFG2[i - 1])
					data->sens[i - 1] = 1;
				else
					data->sens[i - 1] = 2;
			}
			if ((type == w83783s) && (i == 2))
				break;
		}
	}
#ifdef W83781D_RT
/*
   Fill up the RT Tables.
   We assume that they are 32 bytes long, in order for temp 1-3.
   Data sheet documentation is sparse.
   We also assume that it is only for the 781D although I suspect
   that the others support it as well....
*/

	if (init && type == w83781d) {
		u16 k = 0;
/*
    Auto-indexing doesn't seem to work...
    w83781d_write_value(client,W83781D_REG_RT_IDX,0);
*/
		for (i = 0; i < 3; i++) {
			int j;
			for (j = 0; j < 32; j++) {
				w83781d_write_value(client,
						    W83781D_REG_RT_IDX,
						    k++);
				data->rt[i][j] =
				    w83781d_read_value(client,
						       W83781D_REG_RT_VAL);
			}
		}
	}
#endif				/* W83781D_RT */

	if(init) {
		w83781d_write_value(client, W83781D_REG_IN_MIN(0),
				    IN_TO_REG(W83781D_INIT_IN_MIN_0));
		w83781d_write_value(client, W83781D_REG_IN_MAX(0),
				    IN_TO_REG(W83781D_INIT_IN_MAX_0));
		if (type != w83783s) {
			w83781d_write_value(client, W83781D_REG_IN_MIN(1),
					    IN_TO_REG(W83781D_INIT_IN_MIN_1));
			w83781d_write_value(client, W83781D_REG_IN_MAX(1),
					    IN_TO_REG(W83781D_INIT_IN_MAX_1));
		}

		w83781d_write_value(client, W83781D_REG_IN_MIN(2),
				    IN_TO_REG(W83781D_INIT_IN_MIN_2));
		w83781d_write_value(client, W83781D_REG_IN_MAX(2),
				    IN_TO_REG(W83781D_INIT_IN_MAX_2));
		w83781d_write_value(client, W83781D_REG_IN_MIN(3),
				    IN_TO_REG(W83781D_INIT_IN_MIN_3));
		w83781d_write_value(client, W83781D_REG_IN_MAX(3),
				    IN_TO_REG(W83781D_INIT_IN_MAX_3));
		w83781d_write_value(client, W83781D_REG_IN_MIN(4),
				    IN_TO_REG(W83781D_INIT_IN_MIN_4));
		w83781d_write_value(client, W83781D_REG_IN_MAX(4),
				    IN_TO_REG(W83781D_INIT_IN_MAX_4));
		if (type == w83781d || type == as99127f) {
			w83781d_write_value(client, W83781D_REG_IN_MIN(5),
					    IN_TO_REG(W83781D_INIT_IN_MIN_5));
			w83781d_write_value(client, W83781D_REG_IN_MAX(5),
					    IN_TO_REG(W83781D_INIT_IN_MAX_5));
		} else {
			w83781d_write_value(client, W83781D_REG_IN_MIN(5),
					    IN_TO_REG(W83782D_INIT_IN_MIN_5));
			w83781d_write_value(client, W83781D_REG_IN_MAX(5),
					    IN_TO_REG(W83782D_INIT_IN_MAX_5));
		}
		if (type == w83781d || type == as99127f) {
			w83781d_write_value(client, W83781D_REG_IN_MIN(6),
					    IN_TO_REG(W83781D_INIT_IN_MIN_6));
			w83781d_write_value(client, W83781D_REG_IN_MAX(6),
					    IN_TO_REG(W83781D_INIT_IN_MAX_6));
		} else {
			w83781d_write_value(client, W83781D_REG_IN_MIN(6),
					    IN_TO_REG(W83782D_INIT_IN_MIN_6));
			w83781d_write_value(client, W83781D_REG_IN_MAX(6),
					    IN_TO_REG(W83782D_INIT_IN_MAX_6));
		}
		if ((type == w83782d) || (type == w83627hf)) {
			w83781d_write_value(client, W83781D_REG_IN_MIN(7),
					    IN_TO_REG(W83781D_INIT_IN_MIN_7));
			w83781d_write_value(client, W83781D_REG_IN_MAX(7),
					    IN_TO_REG(W83781D_INIT_IN_MAX_7));
			w83781d_write_value(client, W83781D_REG_IN_MIN(8),
					    IN_TO_REG(W83781D_INIT_IN_MIN_8));
			w83781d_write_value(client, W83781D_REG_IN_MAX(8),
					    IN_TO_REG(W83781D_INIT_IN_MAX_8));
			w83781d_write_value(client, W83781D_REG_VBAT,
			    (w83781d_read_value(client, W83781D_REG_VBAT) | 0x01));
		}
		w83781d_write_value(client, W83781D_REG_FAN_MIN(1),
				    FAN_TO_REG(W83781D_INIT_FAN_MIN_1, 2));
		w83781d_write_value(client, W83781D_REG_FAN_MIN(2),
				    FAN_TO_REG(W83781D_INIT_FAN_MIN_2, 2));
		w83781d_write_value(client, W83781D_REG_FAN_MIN(3),
				    FAN_TO_REG(W83781D_INIT_FAN_MIN_3, 2));

		w83781d_write_value(client, W83781D_REG_TEMP_OVER,
				    TEMP_TO_REG(W83781D_INIT_TEMP_OVER));
		w83781d_write_value(client, W83781D_REG_TEMP_HYST,
				    TEMP_TO_REG(W83781D_INIT_TEMP_HYST));

		if (type == as99127f) {
			w83781d_write_value(client, W83781D_REG_TEMP2_OVER,
					    AS99127_TEMP_ADD_TO_REG
					    (W83781D_INIT_TEMP2_OVER));
			w83781d_write_value(client, W83781D_REG_TEMP2_HYST,
					    AS99127_TEMP_ADD_TO_REG
					    (W83781D_INIT_TEMP2_HYST));
		} else {
			w83781d_write_value(client, W83781D_REG_TEMP2_OVER,
					    TEMP_ADD_TO_REG
					    (W83781D_INIT_TEMP2_OVER));
			w83781d_write_value(client, W83781D_REG_TEMP2_HYST,
					    TEMP_ADD_TO_REG
					    (W83781D_INIT_TEMP2_HYST));
		}
		w83781d_write_value(client, W83781D_REG_TEMP2_CONFIG, 0x00);

		if (type == as99127f) {
			w83781d_write_value(client, W83781D_REG_TEMP3_OVER,
					    AS99127_TEMP_ADD_TO_REG
					    (W83781D_INIT_TEMP3_OVER));
			w83781d_write_value(client, W83781D_REG_TEMP3_HYST,
					    AS99127_TEMP_ADD_TO_REG
					    (W83781D_INIT_TEMP3_HYST));
		} else if (type != w83783s) {
			w83781d_write_value(client, W83781D_REG_TEMP3_OVER,
					    TEMP_ADD_TO_REG
					    (W83781D_INIT_TEMP3_OVER));
			w83781d_write_value(client, W83781D_REG_TEMP3_HYST,
					    TEMP_ADD_TO_REG
					    (W83781D_INIT_TEMP3_HYST));
		}
		if (type != w83783s) {
			w83781d_write_value(client, W83781D_REG_TEMP3_CONFIG,
					    0x00);
		}
		/* enable PWM2 control (can't hurt since PWM reg should have
	           been reset to 0xff) */
		if (type != w83781d) {
			w83781d_write_value(client, W83781D_REG_PWMCLK12, 0x19);
		}
	}

	/* Start monitoring */
	w83781d_write_value(client, W83781D_REG_CONFIG,
			    (w83781d_read_value(client,
						W83781D_REG_CONFIG) & 0xf7)
			    | 0x01);
}

void w83781d_update_client(struct i2c_client *client)
{
	struct w83781d_data *data = client->data;
	int i;

	down(&data->update_lock);

	if ((jiffies - data->last_updated > HZ + HZ / 2) ||
	    (jiffies < data->last_updated) || !data->valid) {

#ifdef DEBUG
		printk("Starting w83781d update\n");
#endif
		for (i = 0; i <= 8; i++) {
			if ((data->type == w83783s) && (i == 1))
				continue;	/* 783S has no in1 */
			data->in[i] =
			    w83781d_read_value(client, W83781D_REG_IN(i));
			data->in_min[i] =
			    w83781d_read_value(client,
					       W83781D_REG_IN_MIN(i));
			data->in_max[i] =
			    w83781d_read_value(client,
					       W83781D_REG_IN_MAX(i));
			if ((data->type != w83782d)
			    && (data->type != w83627hf) && (i == 6))
				break;
		}
		for (i = 1; i <= 3; i++) {
			data->fan[i - 1] =
			    w83781d_read_value(client, W83781D_REG_FAN(i));
			data->fan_min[i - 1] =
			    w83781d_read_value(client,
					       W83781D_REG_FAN_MIN(i));
		}
		if (data->type != w83781d) {
			for (i = 1; i <= 4; i++) {
				data->pwm[i - 1] =
				    w83781d_read_value(client,
						       W83781D_REG_PWM(i));
				if (((data->type == w83783s)
				     || (data->type == w83627hf)
				     || (data->type == as99127f)
				     || ((data->type == w83782d)
				        && i2c_is_isa_client(client)))
				    && i == 2)
					break;
			}
		}

		data->temp = w83781d_read_value(client, W83781D_REG_TEMP);
		data->temp_over =
		    w83781d_read_value(client, W83781D_REG_TEMP_OVER);
		data->temp_hyst =
		    w83781d_read_value(client, W83781D_REG_TEMP_HYST);
		data->temp_add[0] =
		    w83781d_read_value(client, W83781D_REG_TEMP2);
		data->temp_add_over[0] =
		    w83781d_read_value(client, W83781D_REG_TEMP2_OVER);
		data->temp_add_hyst[0] =
		    w83781d_read_value(client, W83781D_REG_TEMP2_HYST);
		if (data->type != w83783s) {
			data->temp_add[1] =
			    w83781d_read_value(client, W83781D_REG_TEMP3);
			data->temp_add_over[1] =
			    w83781d_read_value(client, W83781D_REG_TEMP3_OVER);
			data->temp_add_hyst[1] =
			    w83781d_read_value(client, W83781D_REG_TEMP3_HYST);
		}
		i = w83781d_read_value(client, W83781D_REG_VID_FANDIV);
		data->vid = i & 0x0f;
		data->vid |=
		    (w83781d_read_value(client, W83781D_REG_CHIPID) & 0x01)
		    << 4;
		data->fan_div[0] = (i >> 4) & 0x03;
		data->fan_div[1] = (i >> 6) & 0x03;
		data->fan_div[2] = (w83781d_read_value(client,
						       W83781D_REG_PIN) >>
				    6) & 0x03;
		if ((data->type != w83781d) && (data->type != as99127f)) {
			i = w83781d_read_value(client, W83781D_REG_VBAT);
			data->fan_div[0] |= (i >> 3) & 0x04;
			data->fan_div[1] |= (i >> 4) & 0x04;
			data->fan_div[2] |= (i >> 5) & 0x04;
		}
		data->alarms =
		    w83781d_read_value(client,
				       W83781D_REG_ALARM1) +
		    (w83781d_read_value(client, W83781D_REG_ALARM2) << 8);
		if ((data->type == w83782d) || (data->type == w83627hf)) {
			data->alarms |=
			    w83781d_read_value(client,
					       W83781D_REG_ALARM3) << 16;
		}
		i = w83781d_read_value(client, W83781D_REG_BEEP_INTS2);
		data->beep_enable = i >> 7;
		data->beeps = ((i & 0x7f) << 8) +
		    w83781d_read_value(client, W83781D_REG_BEEP_INTS1);
		if ((data->type != w83781d) && (data->type != as99127f)) {
			data->beeps |=
			    w83781d_read_value(client,
					       W83781D_REG_BEEP_INTS3) << 16;
		}
		data->last_updated = jiffies;
		data->valid = 1;
	}

	up(&data->update_lock);
}


/* The next few functions are the call-back functions of the /proc/sys and
   sysctl files. Which function is used is defined in the ctl_table in
   the extra1 field.
   Each function must return the magnitude (power of 10 to divide the date
   with) if it is called with operation==SENSORS_PROC_REAL_INFO. It must
   put a maximum of *nrels elements in results reflecting the data of this
   file, and set *nrels to the number it actually put in it, if operation==
   SENSORS_PROC_REAL_READ. Finally, it must get upto *nrels elements from
   results and write them to the chip, if operations==SENSORS_PROC_REAL_WRITE.
   Note that on SENSORS_PROC_REAL_READ, I do not check whether results is
   large enough (by checking the incoming value of *nrels). This is not very
   good practice, but as long as you put less than about 5 values in results,
   you can assume it is large enough. */
void w83781d_in(struct i2c_client *client, int operation, int ctl_name,
		int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	int nr = ctl_name - W83781D_SYSCTL_IN0;

	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 2;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		results[0] = IN_FROM_REG(data->in_min[nr]);
		results[1] = IN_FROM_REG(data->in_max[nr]);
		results[2] = IN_FROM_REG(data->in[nr]);
		*nrels_mag = 3;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag >= 1) {
			data->in_min[nr] = IN_TO_REG(results[0]);
			w83781d_write_value(client, W83781D_REG_IN_MIN(nr),
					    data->in_min[nr]);
		}
		if (*nrels_mag >= 2) {
			data->in_max[nr] = IN_TO_REG(results[1]);
			w83781d_write_value(client, W83781D_REG_IN_MAX(nr),
					    data->in_max[nr]);
		}
	}
}

void w83781d_fan(struct i2c_client *client, int operation, int ctl_name,
		 int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	int nr = ctl_name - W83781D_SYSCTL_FAN1 + 1;

	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		results[0] = FAN_FROM_REG(data->fan_min[nr - 1],
				  DIV_FROM_REG(data->fan_div[nr - 1]));
		results[1] = FAN_FROM_REG(data->fan[nr - 1],
			          DIV_FROM_REG(data->fan_div[nr - 1]));
		*nrels_mag = 2;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag >= 1) {
			data->fan_min[nr - 1] =
			     FAN_TO_REG(results[0],
			            DIV_FROM_REG(data->fan_div[nr-1]));
			w83781d_write_value(client,
					    W83781D_REG_FAN_MIN(nr),
					    data->fan_min[nr - 1]);
		}
	}
}

void w83781d_temp(struct i2c_client *client, int operation, int ctl_name,
		  int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 1;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		results[0] = TEMP_FROM_REG(data->temp_over);
		results[1] = TEMP_FROM_REG(data->temp_hyst);
		results[2] = TEMP_FROM_REG(data->temp);
		*nrels_mag = 3;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag >= 1) {
			data->temp_over = TEMP_TO_REG(results[0]);
			w83781d_write_value(client, W83781D_REG_TEMP_OVER,
					    data->temp_over);
		}
		if (*nrels_mag >= 2) {
			data->temp_hyst = TEMP_TO_REG(results[1]);
			w83781d_write_value(client, W83781D_REG_TEMP_HYST,
					    data->temp_hyst);
		}
	}
}

void w83781d_temp_add(struct i2c_client *client, int operation,
		      int ctl_name, int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	int nr = ctl_name - W83781D_SYSCTL_TEMP2;

	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 1;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		if (data->type == as99127f) {
			results[0] =
			    AS99127_TEMP_ADD_FROM_REG(data->
						      temp_add_over[nr]);
			results[1] =
			    AS99127_TEMP_ADD_FROM_REG(data->
						      temp_add_hyst[nr]);
			results[2] =
			    AS99127_TEMP_ADD_FROM_REG(data->temp_add[nr]);
		} else {
			results[0] =
			    TEMP_ADD_FROM_REG(data->temp_add_over[nr]);
			results[1] =
			    TEMP_ADD_FROM_REG(data->temp_add_hyst[nr]);
			results[2] = TEMP_ADD_FROM_REG(data->temp_add[nr]);
		}
		*nrels_mag = 3;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag >= 1) {
			if (data->type == as99127f)
				data->temp_add_over[nr] =
				    AS99127_TEMP_ADD_TO_REG(results[0]);
			else
				data->temp_add_over[nr] =
				    TEMP_ADD_TO_REG(results[0]);
			w83781d_write_value(client,
					    nr ? W83781D_REG_TEMP3_OVER :
					    W83781D_REG_TEMP2_OVER,
					    data->temp_add_over[nr]);
		}
		if (*nrels_mag >= 2) {
			if (data->type == as99127f)
				data->temp_add_hyst[nr] =
				    AS99127_TEMP_ADD_TO_REG(results[1]);
			else
				data->temp_add_hyst[nr] =
				    TEMP_ADD_TO_REG(results[1]);
			w83781d_write_value(client,
					    nr ? W83781D_REG_TEMP3_HYST :
					    W83781D_REG_TEMP2_HYST,
					    data->temp_add_hyst[nr]);
		}
	}
}


void w83781d_vid(struct i2c_client *client, int operation, int ctl_name,
		 int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 2;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		results[0] = VID_FROM_REG(data->vid);
		*nrels_mag = 1;
	}
}

void w83781d_alarms(struct i2c_client *client, int operation, int ctl_name,
		    int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		results[0] = ALARMS_FROM_REG(data->alarms);
		*nrels_mag = 1;
	}
}

void w83781d_beep(struct i2c_client *client, int operation, int ctl_name,
		  int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	int val;

	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		results[0] = BEEP_ENABLE_FROM_REG(data->beep_enable);
		results[1] = BEEPS_FROM_REG(data->beeps);
		*nrels_mag = 2;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag >= 2) {
			data->beeps = BEEPS_TO_REG(results[1]);
			w83781d_write_value(client, W83781D_REG_BEEP_INTS1,
					    data->beeps & 0xff);
			if ((data->type != w83781d) &&
			    (data->type != as99127f)) {
				w83781d_write_value(client,
						    W83781D_REG_BEEP_INTS3,
						    ((data-> beeps) >> 16) &
						      0xff);
			}
			val = (data->beeps >> 8) & 0x7f;
		} else if (*nrels_mag >= 1)
			val =
			    w83781d_read_value(client,
					       W83781D_REG_BEEP_INTS2) &
			    0x7f;
		if (*nrels_mag >= 1) {
			data->beep_enable = BEEP_ENABLE_TO_REG(results[0]);
			w83781d_write_value(client, W83781D_REG_BEEP_INTS2,
					    val | data->beep_enable << 7);
		}
	}
}

void w83781d_fan_div(struct i2c_client *client, int operation,
		     int ctl_name, int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	int old, old2, old3 = 0;

	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		results[0] = DIV_FROM_REG(data->fan_div[0]);
		results[1] = DIV_FROM_REG(data->fan_div[1]);
		results[2] = DIV_FROM_REG(data->fan_div[2]);
		*nrels_mag = 3;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		old = w83781d_read_value(client, W83781D_REG_VID_FANDIV);
		/* w83781d and as99127f don't have extended divisor bits */
		if ((data->type != w83781d) && data->type != as99127f) {
			old3 =
			    w83781d_read_value(client, W83781D_REG_VBAT);
		}
		if (*nrels_mag >= 3) {
			data->fan_div[2] =
			    DIV_TO_REG(results[2], data->type);
			old2 = w83781d_read_value(client, W83781D_REG_PIN);
			old2 =
			    (old2 & 0x3f) | ((data->fan_div[2] & 0x03) << 6);
			w83781d_write_value(client, W83781D_REG_PIN, old2);
			if ((data->type != w83781d) &&
			    (data->type != as99127f)) {
				old3 =
				    (old3 & 0x7f) |
				    ((data->fan_div[2] & 0x04) << 5);
			}
		}
		if (*nrels_mag >= 2) {
			data->fan_div[1] =
			    DIV_TO_REG(results[1], data->type);
			old =
			    (old & 0x3f) | ((data->fan_div[1] & 0x03) << 6);
			if ((data->type != w83781d) &&
			    (data->type != as99127f)) {
				old3 =
				    (old3 & 0xbf) |
				    ((data->fan_div[1] & 0x04) << 4);
			}
		}
		if (*nrels_mag >= 1) {
			data->fan_div[0] =
			    DIV_TO_REG(results[0], data->type);
			old =
			    (old & 0xcf) | ((data->fan_div[0] & 0x03) << 4);
			w83781d_write_value(client, W83781D_REG_VID_FANDIV,
					    old);
			if ((data->type != w83781d) &&
			    (data->type != as99127f)) {
				old3 =
				    (old3 & 0xdf) |
				    ((data->fan_div[0] & 0x04) << 3);
				w83781d_write_value(client,
						    W83781D_REG_VBAT,
						    old3);
			}
		}
	}
}

void w83781d_pwm(struct i2c_client *client, int operation, int ctl_name,
		 int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	int nr = 1 + ctl_name - W83781D_SYSCTL_PWM1;

	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		w83781d_update_client(client);
		results[0] = PWM_FROM_REG(data->pwm[nr - 1]);
		*nrels_mag = 1;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag >= 1) {
			data->pwm[nr - 1] = PWM_TO_REG(results[0]);
			w83781d_write_value(client, W83781D_REG_PWM(nr),
					    data->pwm[nr - 1]);
		}
	}
}

void w83781d_sens(struct i2c_client *client, int operation, int ctl_name,
		  int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	int nr = 1 + ctl_name - W83781D_SYSCTL_SENS1;
	u8 tmp;

	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		results[0] = data->sens[nr - 1];
		*nrels_mag = 1;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag >= 1) {
			switch (results[0]) {
			case 1:	/* PII/Celeron diode */
				tmp = w83781d_read_value(client,
						       W83781D_REG_SCFG1);
				w83781d_write_value(client,
						    W83781D_REG_SCFG1,
						    tmp | BIT_SCFG1[nr -
								    1]);
				tmp = w83781d_read_value(client,
						       W83781D_REG_SCFG2);
				w83781d_write_value(client,
						    W83781D_REG_SCFG2,
						    tmp | BIT_SCFG2[nr -
								    1]);
				data->sens[nr - 1] = results[0];
				break;
			case 2:	/* 3904 */
				tmp = w83781d_read_value(client,
						       W83781D_REG_SCFG1);
				w83781d_write_value(client,
						    W83781D_REG_SCFG1,
						    tmp | BIT_SCFG1[nr -
								    1]);
				tmp = w83781d_read_value(client,
						       W83781D_REG_SCFG2);
				w83781d_write_value(client,
						    W83781D_REG_SCFG2,
						    tmp & ~BIT_SCFG2[nr -
								     1]);
				data->sens[nr - 1] = results[0];
				break;
			case W83781D_DEFAULT_BETA:	/* thermistor */
				tmp = w83781d_read_value(client,
						       W83781D_REG_SCFG1);
				w83781d_write_value(client,
						    W83781D_REG_SCFG1,
						    tmp & ~BIT_SCFG1[nr -
								     1]);
				data->sens[nr - 1] = results[0];
				break;
			default:
				printk
				    ("w83781d.o: Invalid sensor type %ld; must be 1, 2, or %d\n",
				     results[0], W83781D_DEFAULT_BETA);
				break;
			}
		}
	}
}

#ifdef W83781D_RT
void w83781d_rt(struct i2c_client *client, int operation, int ctl_name,
		int *nrels_mag, long *results)
{
	struct w83781d_data *data = client->data;
	int nr = 1 + ctl_name - W83781D_SYSCTL_RT1;
	int i;

	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		for (i = 0; i < 32; i++) {
			results[i] = data->rt[nr - 1][i];
		}
		*nrels_mag = 32;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag > 32)
			*nrels_mag = 32;
		for (i = 0; i < *nrels_mag; i++) {
			/* fixme: no bounds checking 0-255 */
			data->rt[nr - 1][i] = results[i];
			w83781d_write_value(client, W83781D_REG_RT_IDX, i);
			w83781d_write_value(client, W83781D_REG_RT_VAL,
					    data->rt[nr - 1][i]);
		}
	}
}
#endif

int __init sensors_w83781d_init(void)
{
	int res;

	printk("w83781d.o version %s (%s)\n", LM_VERSION, LM_DATE);
	w83781d_initialized = 0;

	if ((res = i2c_add_driver(&w83781d_driver))) {
		printk
		    ("w83781d.o: Driver registration failed, module not inserted.\n");
		w83781d_cleanup();
		return res;
	}
	w83781d_initialized++;
	return 0;
}

int __init w83781d_cleanup(void)
{
	int res;

	if (w83781d_initialized >= 1) {
		if ((res = i2c_del_driver(&w83781d_driver))) {
			printk
			    ("w83781d.o: Driver deregistration failed, module not removed.\n");
			return res;
		}
		w83781d_initialized--;
	}
	return 0;
}

EXPORT_NO_SYMBOLS;

#ifdef MODULE

MODULE_AUTHOR("Frodo Looijaard <frodol@dds.nl>, "
	      "Philip Edelbrock <phil@netroedge.com>, "
	      "and Mark Studebaker <mdsxyz123@yahoo.com>");
MODULE_DESCRIPTION("W83781D driver");

int init_module(void)
{
	return sensors_w83781d_init();
}

int cleanup_module(void)
{
	return w83781d_cleanup();
}

#endif				/* MODULE */
