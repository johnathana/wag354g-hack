/***************************************************************************
**+----------------------------------------------------------------------+**
**|                                ****                                  |**
**|                                ****                                  |**
**|                                ******o***                            |**
**|                          ********_///_****                           |**
**|                           ***** /_//_/ ****                          |**
**|                            ** ** (__/ ****                           |**
**|                                *********                             |**
**|                                 ****                                 |**
**|                                  ***                                 |**
**|                                                                      |**
**|     Copyright (c) 1998 - 2004 Texas Instruments Incorporated         |**
**|                        ALL RIGHTS RESERVED                           |**
**|                                                                      |**
**| Permission is hereby granted to licensees of Texas Instruments       |**
**| Incorporated (TI) products to use this computer program for the sole |**
**| purpose of implementing a licensee product based on TI products.     |**
**| No other rights to reproduce, use, or disseminate this computer      |**
**| program, whether in part or in whole, are granted.                   |**
**|                                                                      |**
**| TI makes no representation or warranties with respect to the         |**
**| performance of this computer program, and specifically disclaims     |**
**| any responsibility for any damages, special or consequential,        |**
**| connected with the use of this program.                              |**
**|                                                                      |**
**+----------------------------------------------------------------------+**
***************************************************************************/

#ifndef __MARVELL_DEV_H__
#define __MARVELL_DEV_H__
#include "_tistdtypes1.h"
#include "DDC_switch.h"

#define MASK(hi, lo) (((~(~0 << (hi))) & (~0 << (lo))) | (1 << (hi)))
#define REG32_R(addr, hi, lo) \
        			 ((*(volatile unsigned int *)(addr) & MASK((hi), (lo))) >> (lo))

#define REG32_RMW(addr, hi, lo, val)\
        *(volatile unsigned int *)(addr) =    \
        ((*(volatile unsigned int *)(addr) & ~MASK((hi), (lo))) | ((val) << (lo)))
#define REG32_W(addr, val) *(volatile unsigned int *)(addr) =  val

/***************************************************************************
* Only SIOCDEVPRIVATE is used as cmd argument and hence, there is no switch 
* statement required. Function calls are based on cmd_struct.name
****************************************************************************/
#define SWITCH_DRV_IOCTL_MAGIC 'w'

#define SWITCH_IOCTL _IOWR(SWITCH_DRV_IOCTL_MAGIC, 0, switch_ioctl_cmd_t)

typedef enum
{
	SWITCH_START,
	SWITCH_INFOGET,
	SWITCH_GETVERSION,
	SWITCH_SPECCAPSET,
	SWITCH_SPECCAPGET,
	SWITCH_TRAILERSET,
	PORT_STATUSSET,
	PORT_STATUSGET,
	PORT_LINKGET,
	PORT_FCSET,
	PORT_FCGET,
	PORT_DUPLEXSET,
	PORT_DUPLEXGET,
	PORT_SPEEDSET,
	PORT_SPEEDGET,
	PORT_AUTOSET,
	PORT_AUTOGET,
	PORT_STATISTICGET,
	PORT_STATISTICRESET,
	VLAN_CREATE,
	VLAN_DELETE,
	VLAN_PORT_ADD,					
	VLAN_PORT_DELETE,
	VLAN_CPU_ADD,
	VLAN_CPU_DELETE,	
	VLAN_SET_DEF,
	VLAN_PORT_UPDATE,
	VLAN_SET_PORT_PRI,
	VLAN_GET_PORT_PRI,
   DELETE_MAC_ADDR
}switch_ioctl_opcode;



typedef struct _switch_ioctl_cmd_t{
	switch_ioctl_opcode t_opcode;
	HANDLE instanceId;
	Bool   TrailerMode;
	union {
		struct {
			PSWITCH_INIT pSwitchInfo;
			Uint32 NumOfSwitches;
			Uint32 data_size;
		}SWITCH_info;
		struct {
			Uint32 PortNum;
			Uint32 Status;
		} PORT_config;
		struct {
			Uint32 PortNum;
			Uint32 data_size;
			PMIB2_COUNTER pCounters;
		}PORT_counters;
		struct {
			Uint32 data_size;
			PSWITCH_SPECIAL_CAPS pSpecialCap;
		}SWITCH_specialcap;
		struct {
			Uint32 data_size;
			PSW_VER_INFO pverinfo;
		}PORT_verinfo;
		struct {
			Uint32 portNum;        
			Uint32 vlnId;          
			VLN_TAG tag;
			Uint32 priority;
		}VLAN_info;  
      struct {
         char MacAddr[6];
      }MAC_addr;  

	}msg;


}switch_ioctl_cmd_t;


#endif /* __MARVELL_DEV_H__ */
