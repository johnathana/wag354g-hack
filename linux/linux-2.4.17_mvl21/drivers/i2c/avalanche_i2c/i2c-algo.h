/******************************************************************************* 
 * FILE PURPOSE:     Header for I2C algorthmic module                              
 ******************************************************************************* 
 * FILE NAME:       i2c_algo.h                                                  
 *                                                                               
 * DESCRIPTION:     Header file for Avalanche I2C algorithmic module             
 *                                                                               
 * REVISION HISTORY:                                                             
 *                                                                               
 * Date           Description                               Author               
 *-----------------------------------------------------------------------------  
 * 6 Jan 2004    Initial Creation                          Sharath Kumar         
 *                                                                               
 * (C) Copyright 2003, Texas Instruments, Inc                                    
 ******************************************************************************/ 
#ifndef I2C_ALGO_H
#define I2C_ALGO_H 

#include <linux/i2c.h>

struct i2c_algo_iic_data {
	void *data;		/* private data for lowlevel routines	*/
	void (*setiic) (void *data, int ctl, int val);
	int  (*getiic) (void *data, int ctl);
	int  (*getown) (void *data);
	int  (*getclock) (void *data);
	void (*waitforpin) (void);

	/* local settings */
	int udelay;
	int mdelay;
	int timeout;
};

#define I2C_IIC_ADAP_MAX	16

int i2c_avalanche_add_bus(struct i2c_adapter *);
int i2c_avalanche_del_bus(struct i2c_adapter *);

#endif /* I2C_ALGO_H */
