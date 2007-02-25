/*******************************************************************************  
 * FILE PURPOSE:     Header file for I2C adapter module                                  
 *******************************************************************************  
 * FILE NAME:       i2c-adap.h                                                     
 *                                                                                
 * DESCRIPTION:     Header file for Avalanche I2C adapter module                  
 *                                                                                
 * REVISION HISTORY:                                                              
 *                                                                                
 * Date           Description                               Author                
 *-----------------------------------------------------------------------------   
 * 6 Jan 2004    Initial Creation                          Sharath Kumar          
 *                                                                                
 * (C) Copyright 2003, Texas Instruments, Inc                                     
 ******************************************************************************/  
#ifndef I2C_ADAP_H
#define I2C_ADAP_H

/*
 * This struct contains the hw-dependent values of TI  IIC adapter
 */

struct iic_avalanche {
	unsigned long iic_base;
	int iic_irq;
	int iic_clock;
	int iic_own;
	int index;
};

extern struct iic_ti iic_ti_priv_data;

#endif /* I2C_ADAP_H */
