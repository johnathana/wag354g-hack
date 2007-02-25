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
#include "_tistdtypes.h"
#include "pal_defs.h"

#include "ddc_cpmacDrv.h"
#include "ddc_switch.h"


typedef enum
{
	SWITCH_GETVERSION = 1,
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
	VLAN_SET_DEF,
	VLAN_PORT_UPDATE,
	VLAN_SET_PORT_PRI,
	VLAN_GET_PORT_PRI,
   DELETE_MAC_ADDR
}SwitchIoctlOpcode;

typedef struct _switchIoctlCmdType_{
   SwitchIoctlOpcode t_opcode;
   union {
      struct {
         Uint32 PortNum;
         Uint32 Status;
      }PORT_config;
      struct {
         Uint32 PortNum;
         MIB2_COUNTER Counters;
      }PORT_counters;
      struct {
         Uint32 data_size;
         SWITCH_SPECIAL_CAPS SpecialCap;
      }SWITCH_specialcap;
      struct {
         SW_VER_INFO verinfo;
      }SWITCH_verinfo;
      struct {
         Uint32 PortNum;        
         Uint32 vlnId;          
         VLN_TAG tag;
         Uint32 priority;
      }VLAN_info;  
      struct {
         char MacAddr[6];
      }MAC_addr;  
      
   }msg;
   
}SwitchIoctlType;


int external_switch_ioctl(SwitchIoctlType *userData);
void switch_create_proc_entry(void);
void switch_remove_proc_entry(void);


#endif /* __MARVELL_DEV_H__ */
