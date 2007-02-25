/******************************************************************************
 * FILE PURPOSE:     - LED HAL module Header
 ******************************************************************************
 * FILE NAME:     led_hal.h
 *
 * DESCRIPTION:    LED HAL API's.
 *
 * REVISION HISTORY:
 * 11 Oct 03 - PSP TII
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __LED_HAL__
#define __LED_HAL__

/* Interface prototypes */

int avalanche_led_hal_init (int *gpio_off_value, int num_gpio_pins);
int avalanche_led_config_set (LED_CONFIG_T * led_cfg);
int avalanche_led_config_get (LED_CONFIG_T *led_cfg,int module_id,int instance, int state);
void *avalanche_led_register (const char *module_name, int instance_num);
void  avalanche_led_action (void *handle, int state_id);
int avalanche_led_unregister (void *handle);
void avalanche_led_free_all(void);
void avalanche_led_hal_exit (void);

#endif /*__LED_HAL__ */
