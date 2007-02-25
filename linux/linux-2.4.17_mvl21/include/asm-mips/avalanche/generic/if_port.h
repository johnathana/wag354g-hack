/*******************************************************************************   
 * FILE PURPOSE:    Interface port id Header file                                      
 *******************************************************************************   
 * FILE NAME:       if_port.h                                                   
 *                                                                                 
 * DESCRIPTION:     Header file carrying information about port ids of interfaces                             
 *                                                                                 
 *                                                                                 
 * (C) Copyright 2003, Texas Instruments, Inc                                      
 ******************************************************************************/   
#ifndef _IF_PORT_H_
#define _IF_PORT_H_

#define AVALANCHE_CPMAC_LOW_PORT_ID         0
#define AVALANCHE_CPMAC_HIGH_PORT_ID        1    
#define AVALANCHE_USB_PORT_ID               2
#define AVALANCHE_WLAN_PORT_ID              3


#define AVALANCHE_MARVELL_BASE_PORT_ID      4

/* The marvell ports occupy port ids from  4 to 8 */
/* so the next port id number should start at 9   */


#endif /* _IF_PORT_H_ */
