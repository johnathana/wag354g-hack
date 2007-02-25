/*
* Abstraction interface for microcontroller connection to rest of system
*
* Copyright 2000,1 Compaq Computer Corporation.
*
* Use consistent with the GNU GPL is permitted,
* provided that this copyright notice is
* preserved in its entirety in all copies and derived works.
*
* COMPAQ COMPUTER CORPORATION MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
* AS TO THE USEFULNESS OR CORRECTNESS OF THIS CODE OR ITS
* FITNESS FOR ANY PARTICULAR PURPOSE.
*
* Author: Andrew Christian
*
*/

#ifndef __H3600_HAL_H
#define __H3600_HAL_H

#include <linux/h3600_ts.h>

/* Interface to the hardware-type specific functions */
struct h3600_hal_ops {
	/* Functions provided by the underlying hardware */
	int (*get_version)( struct h3600_ts_version * );
	int (*eeprom_read)( unsigned short address, unsigned char *data, unsigned short len );
	int (*eeprom_write)( unsigned short address, unsigned char *data, unsigned short len );
	int (*get_thermal_sensor)( unsigned short * );
	int (*set_notify_led)( unsigned char mode, unsigned char duration, 
			       unsigned char ontime, unsigned char offtime );
	int (*read_light_sensor)( unsigned char *result );
	int (*get_battery)( struct h3600_battery * );
	int (*spi_read)( unsigned short address, unsigned char *data, unsigned short len );
	int (*spi_write)( unsigned short address, unsigned char *data, unsigned short len );
	int (*codec_control)( unsigned char, unsigned char );
	int (*get_option_detect)( int *result );
	int (*set_audio_clock)( long val );

	/* Functions indirectly provided by the underlying hardware */
	int (*backlight_control)( enum flite_pwr power, unsigned char level );
	int (*contrast_control)( unsigned char level );

        /* for module use counting */ 
        struct module *owner;
};

/* Used by the device-specific hardware module to register itself */
extern int  h3600_hal_register_interface( struct h3600_hal_ops *ops );
extern void h3600_hal_unregister_interface( struct h3600_hal_ops *ops );

/* 
 * Calls into HAL from the device-specific hardware module
 * These run at interrupt time 
 */
extern void h3600_hal_keypress( unsigned char key );
extern void h3600_hal_touchpanel( unsigned short x, unsigned short y, int down );
extern void h3600_hal_option_detect( int present );

/* Callbacks registered by device drivers */
struct h3600_driver_ops {
	void (*keypress)( unsigned char key );
	void (*touchpanel)( unsigned short x, unsigned short y, int down );
	void (*option_detect)( int present );
};

extern int  h3600_hal_register_driver( struct h3600_driver_ops * );
extern void h3600_hal_unregister_driver( struct h3600_driver_ops * );


/* Calls into HAL from device drivers and other kernel modules */
extern void h3600_get_flite( struct h3600_ts_backlight *bl );
extern void h3600_get_contrast( unsigned char *contrast );
extern int  h3600_set_flite( enum flite_pwr pwr, unsigned char brightness );
extern int  h3600_set_contrast( unsigned char contrast );
extern int  h3600_toggle_frontlight( void );

extern int h3600_apm_get_power_status(unsigned char *ac_line_status, unsigned char *battery_status, 
				      unsigned char *battery_flag, unsigned char *battery_percentage, 
				      unsigned short *battery_life);

extern struct h3600_hal_ops *h3600_hal_ops;

/* Do not use these macros in driver files - instead, use the inline functions defined below */
#define CALL_HAL_INTERFACE( f, args... ) \
        ( h3600_hal_ops && h3600_hal_ops->f ? h3600_hal_ops->f(args) : -EIO )

#define CALL_HAL(f, args...) \
        { return ( h3600_hal_ops && h3600_hal_ops->f ? h3600_hal_ops->f(args) : -EIO ); }

#define HFUNC  static __inline__ int

/* The eeprom_read/write address + len has a maximum value of 512.  Both must be even numbers */
HFUNC h3600_eeprom_read( u16 addr, u8 *data, u16 len )  CALL_HAL(eeprom_read,addr,data,len)
HFUNC h3600_eeprom_write( u16 addr, u8 *data, u16 len)  CALL_HAL(eeprom_write,addr,data,len)
HFUNC h3600_spi_read( u8 addr, u8 *data, u16 len) 	CALL_HAL(spi_read,addr,data,len)
HFUNC h3600_spi_write( u8 addr, u8 *data, u16 len) 	CALL_HAL(spi_write,addr,data,len)
HFUNC h3600_get_version( struct h3600_ts_version *v )   CALL_HAL(get_version,v)
HFUNC h3600_get_thermal_sensor( u16 *thermal ) 	        CALL_HAL(get_thermal_sensor,thermal)
HFUNC h3600_set_led( u8 mode, u8 dur, u8 ont, u8 offt ) CALL_HAL(set_notify_led, mode, dur, ont, offt)
HFUNC h3600_get_light_sensor( u8 *result ) 	        CALL_HAL(read_light_sensor,result)
HFUNC h3600_get_battery( struct h3600_battery *bat )	CALL_HAL(get_battery,bat)
HFUNC h3600_get_option_detect( int *result)             CALL_HAL(get_option_detect,result)
HFUNC h3600_set_audio_clock( long value )               CALL_HAL(set_audio_clock,value)

#endif
