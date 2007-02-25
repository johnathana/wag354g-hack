/*****************************************************************************
 * FILE PURPOSE:     - Marvell 6063 Switch driver module Source                                   
 ******************************************************************************   
 * FILE NAME:     marvell_drv.c                                                       
 *                                                                                
 * DESCRIPTION:   Linux Marvell character driver implementation                       
 *                                                                                
 * REVISION HISTORY:                                                              
 * 1 FEB 2004                                                             
 *                                                                                
 * (C) Copyright 2004, Texas Instruments, Inc                                     
 *******************************************************************************/ 

#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/proc_fs.h>
#include <linux/ioctl.h>
#include <linux/devfs_fs_kernel.h>
#include <asm/avalanche/avalanche_map.h>

#include "DDA_switchLinux.h"
#include "pal_defs.h"

#define TI_MARVELL_VERSION                 "1.0"

#define PROC_END_DEBUG  0
#define procPrint if (PROC_END_DEBUG) printk

typedef struct {                                                    
	Uint32 switch_handle;       /* handle for switch hal */    
	devfs_handle_t dev_handle;  /* only used if devfs is there */
}SWITCH_DEV;

#define LAN1_PORT               1
#define LAN2_PORT               2
#define LAN3_PORT               3
#define LAN4_PORT               4

static struct proc_dir_entry *port_config_dir = NULL;
static struct proc_dir_entry *port_status_dir = NULL;
static struct proc_dir_entry *port_lan_config_file[4];
static struct proc_dir_entry *port_lan_status_file[4];


static int ioctl_init(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_infoGet(SWITCH_DEV *dev,switch_ioctl_cmd_t* ioctl_cmd);
static int ioctl_PortLinkGet(switch_ioctl_cmd_t* ioctl_cmd);
static int ioctl_PortSpeedSet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortSpeedGet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortAutoSet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortAutoGet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VersionGet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortDuplexSet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortDuplexGet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_SpecCapsSet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_SpecCapsGet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_TrailerModeSet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortStatusSet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortStatusGet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortFCGet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_PortFCSet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_StatPortGet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_StatPortReset(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanCreate(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanDelete(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanPortAdd(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanPortDelete(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanCPUAdd(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanCPUDelete(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanDefaultSet(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanPortUpdate(switch_ioctl_cmd_t *ioctl_cmd);
static int ioctl_VlanSetPortPriority(switch_ioctl_cmd_t *ioctl_cmd);
static Uint32 ioctl_VlanGetPortPriority(switch_ioctl_cmd_t *ioctl_cmd);
static Uint32 ioctl_DeleteMacAddrFromTable(switch_ioctl_cmd_t *ioctl_cmd);

static SWITCH_DEV  *switch_dev = NULL;

/*****************************************************************************************************/
/*                                    PROC FUNCTIONS                                                 */
/*****************************************************************************************************/

static int marvell_port_read_config(char* buf, char **start, off_t offset, int count, int *eof, void *data)
{
   int len;
   unsigned int port_status = 0,Duplex,Speed;
   int PortNum = (int)data;
   
   
   MOD_INC_USE_COUNT;
   procPrint("read ....\n");
   if(PortLinkGet(0,PortNum) == PORT_LINK_DOWN)
   {
      procPrint("LINK is down Port num = %d\n",PortNum);    
      port_status = 5;
   }
   else
   {
      
      if( PortAutoGet(0,PortNum) == PORT_AUTO_YES)
      {
         procPrint("Auto negt. is enabled \n");
         port_status = 0;
         
      }else /* NO AUTO */
      {
         
         Duplex = PortDuplexGet(0,PortNum);
         Speed = PortSpeedGet(0,PortNum);
         
         if( Speed == PORT_SPEED_100)
         {
            if(Duplex == PORT_DUPLEX_FULL)
            {
               procPrint("Speed is 100Mbps FULL Duplex.\n");	
               port_status = 4;
            }else
            {
               procPrint("Speed is 10oMbps HALF Duplex.\n");	
               port_status = 3;		
            }
         }else
         {
            
            if(Duplex == PORT_DUPLEX_FULL)
            {
               procPrint("Speed is 10Mbps FULL Duplex.\n");	
               port_status = 2;
            }else
            {
               procPrint("Speed is 10Mbps HALF Duplex.\n");	
               port_status = 1;		
            }
         }
      }
   }

   len = sprintf(buf,"%d\n",port_status);
   
   MOD_DEC_USE_COUNT;
   return len;
   
}
/*****************************************************************************************************/
/*marvell_port_config - The write call back function for the proc entry.                             */
/*                      *buffer - contain the user command :                                         */
/*                              0 - Restart Auto Negotiation.                                        */
/*                              1 - Disable Auto Negotiation and Set to Half Duplex 10 Mbps.         */
/*                              2 - Disable Auto Negotiation and Set to Full Duplex 10 Mbps.         */
/*                              3 - Disable Auto Negotiation and Set to Half Duplex 100 Mbps.        */
/*                              4 - Disable Auto Negotiation and Set to Full Duplex 100 Mbps.        */
/*                      *data - contain the Port Number                                              */
/*                      count - Number of Bytes need to read from the buffer.                        */
/******************************************************************************************************/
static int marvell_port_config(struct file *file, const char* buffer, unsigned long count, void *data) 
{
	int len = count;
	char status[8];
	int PortNum = (int)data;

	MOD_INC_USE_COUNT;
	if(copy_from_user(status, buffer, len))
	{
		return -EFAULT;
	}
	status[len-1] = '\0';

	if(strcmp("0", status) == 0)
	{
		if( PortAutoGet(0,PortNum) == PORT_AUTO_YES)
		{
			procPrint("Auto negt is already enabled, Restart Auto Negt.\n");
			PortAutoSet(0,PortNum,PORT_AUTO_RESTART);
		}else
		{
			procPrint("Auto negt is disabled, Enable Auto Negt.\n");		
			PortAutoSet(0,PortNum,PORT_AUTO_YES);
		}

	}
	else if(strcmp("1", status) == 0)
	{
		PortAutoSet(0,PortNum,PORT_AUTO_NO);
		PortDuplexSet(0,PortNum,PORT_DUPLEX_HALF);
		PortSpeedSet(0,PortNum,PORT_SPEED_10);

		procPrint("Disable auto negt.And force to 10Mbps Half Duplex\n");


	}
	else if(strcmp("2", status) == 0)
	{
		PortAutoSet(0,PortNum,PORT_AUTO_NO);
		PortDuplexSet(0,PortNum,PORT_DUPLEX_FULL);
		PortSpeedSet(0,PortNum,PORT_SPEED_10);
		procPrint("Disable auto negt.And force to 10Mbps Full Duplex\n");


	}
	else if(strcmp("3", status) == 0)
	{
		PortAutoSet(0,PortNum,PORT_AUTO_NO);
		PortDuplexSet(0,PortNum,PORT_DUPLEX_HALF);
		PortSpeedSet(0,PortNum,PORT_SPEED_100);
		procPrint("Disable auto negt.And force to 100Mbps Half Duplex\n");

	}
	else if(strcmp("4", status) == 0)
	{
		PortAutoSet(0,PortNum,PORT_AUTO_NO);
		PortDuplexSet(0,PortNum,PORT_DUPLEX_FULL);
		PortSpeedSet(0,PortNum,PORT_SPEED_100);
		procPrint("Disable auto negt.And force to 100Mbps Full Duplex\n"); 
	}

	MOD_DEC_USE_COUNT;
	return len;   

}

/*****************************************************************************************************/
/*marvell_port_status - The read call back function for the proc entry.                              */
/*                      *data - Port Number                                                          */
/*                      RETURN - The Speed and Duplex of the port. The Returns value can be:         */
/*                              1 - Half Duplex 10 Mbps.                                             */
/*                              2 - Full Duplex 10 Mbps.                                             */
/*                              3 - Half Duplex 100 Mbps.                                            */
/*                              4 - Full Duplex 100 Mbps.                                            */
/*                              5 - Link is Down.                                                    */
/******************************************************************************************************/
static int marvell_port_status(char* buf, char **start, off_t offset, int count, int *eof, void *data) 
{
	int len;
	unsigned int port_status = 0,Duplex,Speed;
	int PortNum = (int)data;


	MOD_INC_USE_COUNT;
	procPrint("read ....\n");
	if(PortLinkGet(0,PortNum) == PORT_LINK_DOWN)
	{
		procPrint("LINK is down Port num = %d\n",PortNum);    
		port_status = 5;
	}
	else
	{

		if( PortAutoGet(0,PortNum) == PORT_AUTO_YES)
		{
			procPrint("Auto negt. is enabled \n");
		}

		Duplex = PortDuplexGet(0,PortNum);
		Speed = PortSpeedGet(0,PortNum);

		if( Speed == PORT_SPEED_100)
		{
			if(Duplex == PORT_DUPLEX_FULL)
			{
				procPrint("Speed is 100Mbps FULL Duplex.\n");	
				port_status = 4;
			}else
			{
				procPrint("Speed is 10oMbps HALF Duplex.\n");	
				port_status = 3;		
			}
		}else
		{

			if(Duplex == PORT_DUPLEX_FULL)
			{
				procPrint("Speed is 10Mbps FULL Duplex.\n");	
				port_status = 2;
			}else
			{
				procPrint("Speed is 10Mbps HALF Duplex.\n");	
				port_status = 1;		
			}
		}
	}

	len = sprintf(buf,"%d\n",port_status);

	MOD_DEC_USE_COUNT;
	return len;

}

/*********************************************************************/
/*                  IOCTL FUNCTIONS                                  */
/*********************************************************************/

static int switch_ioctl( struct inode * inode,struct file * filp,
		unsigned int cmd, unsigned long arg )
{
	int ret = 0;
	SWITCH_DEV  *sw_dev=(SWITCH_DEV*)filp->private_data;
	switch_ioctl_cmd_t *ioctl_cmd;

	if (cmd != SWITCH_IOCTL) {
		printk("%s: IOCTL cmd mismatch\n",__FUNCTION__);
		ret = -EINVAL;
	}

	ioctl_cmd = (switch_ioctl_cmd_t*)arg;

	switch (ioctl_cmd->t_opcode)
	{

		case SWITCH_START: 
			ret = ioctl_init(ioctl_cmd);
			break;

			/* SWITCH MANAGEMENT */

			/* SWITCH_INFOGET Must be call before any other IOCTL - It returns the Instance ID */
		case SWITCH_INFOGET: 
			ret = ioctl_infoGet(sw_dev,ioctl_cmd);
			break;

		case SWITCH_GETVERSION:		
			ret = ioctl_VersionGet(ioctl_cmd);
			break;

		case SWITCH_SPECCAPGET:
			ret = ioctl_SpecCapsGet(ioctl_cmd);
			break;

		case SWITCH_SPECCAPSET:
			ret = ioctl_SpecCapsSet(ioctl_cmd);
			break;

		case SWITCH_TRAILERSET: 
			ret = ioctl_TrailerModeSet(ioctl_cmd);
			break;

			/* PORT MANAGEMENT */

		case PORT_LINKGET:
			ret = ioctl_PortLinkGet(ioctl_cmd);
			break;

		case PORT_SPEEDSET:
			ret = ioctl_PortSpeedSet(ioctl_cmd);
			break;

		case PORT_SPEEDGET:
			ret = ioctl_PortSpeedGet(ioctl_cmd);
			break;

		case PORT_AUTOSET:
			ret = ioctl_PortAutoSet(ioctl_cmd);
			break;

		case PORT_AUTOGET:
			ret = ioctl_PortAutoGet(ioctl_cmd);
			break;

		case PORT_DUPLEXSET:
			ret = ioctl_PortDuplexSet(ioctl_cmd);
			break;

		case PORT_DUPLEXGET:
			ret = ioctl_PortDuplexGet(ioctl_cmd);
			break;

		case PORT_STATUSSET:
			ret = ioctl_PortStatusSet(ioctl_cmd);
			break;

		case PORT_STATUSGET:
			ret = ioctl_PortStatusGet(ioctl_cmd);
			break;

		case PORT_FCSET:
			ret = ioctl_PortFCSet(ioctl_cmd);
			break;

		case PORT_FCGET:
			ret = ioctl_PortFCGet(ioctl_cmd);
			break;

			/* S T A T I S T I C S */

		case PORT_STATISTICGET:
			ret = ioctl_StatPortGet(ioctl_cmd);
			break;

		case PORT_STATISTICRESET:
			ret = ioctl_StatPortReset(ioctl_cmd);
			break;
			/*    VLAN SUBMODULE  */
		case VLAN_CREATE:		/* Creates VLAN with the given ID. */
			ret = ioctl_VlanCreate(ioctl_cmd);
			break; 


		case VLAN_DELETE:		/* Deletes VLAN. */
			ret = ioctl_VlanDelete(ioctl_cmd);
			break;

		case VLAN_PORT_ADD:		/* Adds port to VLAN. */
			ret = ioctl_VlanPortAdd(ioctl_cmd);
			break;

		case VLAN_PORT_DELETE:		/* Deletes port from VLAN. */
			ret = ioctl_VlanPortDelete(ioctl_cmd);
			break;

		case VLAN_CPU_ADD:		/* Adds CPU to VLAN. */
			ret = ioctl_VlanCPUAdd(ioctl_cmd);
			break;

		case VLAN_CPU_DELETE:		/* Deletes CPU from VLAN. */
			ret = ioctl_VlanCPUDelete(ioctl_cmd);
			break;

		case VLAN_SET_DEF:		/* Sets default VLAN for the given port. */
			ret = ioctl_VlanDefaultSet(ioctl_cmd);
			break;

		case VLAN_PORT_UPDATE:		/* Updates port status in VLAN. */
			ret = ioctl_VlanPortUpdate(ioctl_cmd);
			break;

		case VLAN_SET_PORT_PRI:		/* Sets the priority of a port. */
			ret = ioctl_VlanSetPortPriority(ioctl_cmd);
			break;
		case VLAN_GET_PORT_PRI:		/* Gets the priority of a port. */
			ret = ioctl_VlanGetPortPriority(ioctl_cmd);
			break;
		case DELETE_MAC_ADDR:
			ret = ioctl_DeleteMacAddrFromTable(ioctl_cmd);
			break;

		default:
			ret = -EINVAL;
	}
   
   /* IOCTL - return 0 on Sucsess or -EINVAL on Error */
   if(ret == PAL_False || ret == -EINVAL)
      return -EINVAL; 
   else
	   return 0; 
}
/****************************************************************************************************
 * ioctl_init - 
 *
 * DESCRIPTION: Call to switch ADD and Switch start. Expect for the SWITCH_INIT structure.
 *
 *        
 *********************************************************************************************************/
static int ioctl_init(switch_ioctl_cmd_t *ioctl_cmd)
{
	SWITCH_INIT SwitchInfo;
   int ret =0;

	if (ioctl_cmd->msg.SWITCH_info.data_size != sizeof(SWITCH_INIT))
	{
		printk("%s: buffer size mismatch\n",__FUNCTION__);
		return PAL_False;
	}

	copy_from_user(&SwitchInfo,ioctl_cmd->msg.SWITCH_info.pSwitchInfo,
			ioctl_cmd->msg.SWITCH_info.data_size);
	
   SwitchAdd(switch_dev->switch_handle,&SwitchInfo);
	ret = SwitchDrvStart(switch_dev->switch_handle,SwitchInfo.instanceId);

	return ret;
}

/****************************************************************************************************
 * ioctl_infoGet - retrieves the initialized switches info
 *
 * DESCRIPTION:
 *		Retreives the initialized switches info.
 *		The driver expects the array of SWITCH_INIT elements.
 *		numOfSwitches is a pointer to the array size. 
 *		In case of pSwitchInfo is NULL, numOfSwitches is used as an output and contains
 *		the number of active switches that are included in the current driver instance.
 *        
 *********************************************************************************************************/
static int ioctl_infoGet(SWITCH_DEV *sw_dev,switch_ioctl_cmd_t *ioctl_cmd)
{
	int info_res;
	SWITCH_INIT SwitchInfo;
	Uint32 numOfSwitches;

	if (ioctl_cmd->msg.SWITCH_info.data_size != sizeof(SWITCH_INIT))
	{
		printk("%s: buffer size mismatch\n",__FUNCTION__);
		return PAL_False;
	}

	info_res = SwitchInfoGet(sw_dev->switch_handle,&SwitchInfo,&numOfSwitches);
	if(info_res != TRUE)
	{
		printk("SwitchInfoGet Return False \n");
		return PAL_False;
	}
	if(ioctl_cmd->msg.SWITCH_info.pSwitchInfo == NULL) /*indicate that the user need the num of switches*/
	{
		ioctl_cmd->msg.SWITCH_info.NumOfSwitches = numOfSwitches;
	}else
	{	/* Number of Switch is 1 */
		ioctl_cmd->instanceId = SwitchInfo.instanceId;
		copy_to_user(ioctl_cmd->msg.SWITCH_info.pSwitchInfo,&SwitchInfo,
				ioctl_cmd->msg.SWITCH_info.data_size);
	}
	return PAL_True;

}

/*******************************************************************************
 * ioctl_VersionGet
 *
 * DESCRIPTION:
 *       This function returns the version of the switch.
 *
 *
 *******************************************************************************/
static int ioctl_VersionGet(switch_ioctl_cmd_t *ioctl_cmd)
{

	SW_VER_INFO ver_info;
   int res;
	if(ioctl_cmd->msg.PORT_verinfo.data_size != sizeof(SW_VER_INFO))
	{
		printk("%s: buffer size mismatch\n",__FUNCTION__);
		return PAL_False;
	}

	res = SwitchGetVersion(ioctl_cmd->instanceId ,&ver_info);

	copy_to_user(ioctl_cmd->msg.PORT_verinfo.pverinfo,&ver_info, ioctl_cmd->msg.PORT_verinfo.data_size);
	return res;
}

/*******************************************************************************
 *
 * ioctl_PortLinkGet - get port link status(Up/down)
 *
 * The function retrieves port link status. 
 *
 *******************************************************************************/
static int ioctl_PortLinkGet(switch_ioctl_cmd_t* ioctl_cmd)
{ 
	int res;

	res = (int)PortLinkGet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum/*port num*/);
	ioctl_cmd->msg.PORT_config.Status = res;
   return res;
   
}

/****************************************************************************    
 * ioctl_PortSpeedSet - set port speed status(10/100)
 *
 * Set the current port to work in the given speed.
 *
 *******************************************************************************/
static int ioctl_PortSpeedSet(switch_ioctl_cmd_t *ioctl_cmd)
{ 
   int res = 0;

	res = PortSpeedSet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum/*port num*/,
			(PORT_SPEED)ioctl_cmd->msg.PORT_config.Status/*Speed*/);
	return res;

}
/****************************************************************************    
 * ioctl_PortSpeedGet - get port speed (10/100)
 *
 * Retrieves current speed parameter for the given port.
 *
 *******************************************************************************/
static int ioctl_PortSpeedGet(switch_ioctl_cmd_t *ioctl_cmd)
{ 
	int res;

	res = PortSpeedGet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum/*port num*/);
	ioctl_cmd->msg.PORT_config.Status = res;
   return res;
}

/****************************************************************************    
 * ioctl_PortAutoSet - set port auto (ENABLE/DISABLE/RESTART)
 *
 * Set autonegotiation status for the given port, which is Yes/No/Restart. 
 ********************************************************************************/
static int ioctl_PortAutoSet(switch_ioctl_cmd_t *ioctl_cmd)
{ 
   int ret = 0;

	ret = PortAutoSet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum/*port num*/,
			(PORT_AUTO)ioctl_cmd->msg.PORT_config.Status/*autoStatus*/);
	return ret;
}

/********************************************************************************    
 * ioctl_PortAutoGet - get port auto status (ENABLE/DISABLE/RESTART)
 *   
 * Get the autonegotaiation for "speed/duplex" status, which is Yes/No. 
 *********************************************************************************/
static int ioctl_PortAutoGet(switch_ioctl_cmd_t *ioctl_cmd)
{ 
	int res;

	res = PortAutoGet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum/*port num*/);
	ioctl_cmd->msg.PORT_config.Status = res; /*autoStatus*/
   return res;
}

/********************************************************************************    
 * ioctl_PortDuplexSet - set port duplex (Full/Half)
 *
 * Set the duplex state of the port to Full/Half. 
 *
 *********************************************************************************/
static int ioctl_PortDuplexSet(switch_ioctl_cmd_t *ioctl_cmd)
{ 

   int ret = 0;
	ret = PortDuplexSet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum/*port num*/,
			(PORT_DUPLEX)ioctl_cmd->msg.PORT_config.Status/*DuplexStatus*/);
	return ret;
}

/********************************************************************************    
 * ioctl_PortDuplexGet - get port duplex status (Full/Half)
 *
 * Retrieves Duplex status (half/full). 
 *********************************************************************************/
static int ioctl_PortDuplexGet(switch_ioctl_cmd_t *ioctl_cmd)
{ 
	int res;

	res = PortDuplexGet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum/*port num*/);
	ioctl_cmd->msg.PORT_config.Status = res; /*DuplexStatus*/
   return res;
}

/*******************************************************************************
 *
 * ioctl_PortStatusSet - enable the given port
 *
 * Enable/Disable a selected port
 **********************************************************************************/
static int ioctl_PortStatusSet(switch_ioctl_cmd_t *ioctl_cmd)
{
   int res = 0;
	res = PortStatusSet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum,
			ioctl_cmd->msg.PORT_config.Status);
	return res;
}

/********************************************************************************
 *
 * ioctl_PortStatusGet - Returns the port status
 *
 * RETURNS: PORT_ENABLE/PORT_DISABLE
 **********************************************************************************/
static int ioctl_PortStatusGet(switch_ioctl_cmd_t *ioctl_cmd)
{
	int res;
	res = PortStatusGet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum);
	ioctl_cmd->msg.PORT_config.Status = res;
   return res;;
}

/*****************************************************************************************
 * ioctl_PortFCSet - set flow control (On/off)
 *  
 * Set flow control to on/off. all the ports in this trunk will be switched in this mode.
 ******************************************************************************************/
static int ioctl_PortFCSet(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret=0;
	ret = PortFCSet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum,ioctl_cmd->msg.PORT_config.Status);
	return ret;
}

/**********************************************************************************    
 * ioctl_PortFCGet - get flow control status(On/off)
 *
 * The function retrieves Flow Control status of the given port.
 ***********************************************************************************/
static int ioctl_PortFCGet(switch_ioctl_cmd_t *ioctl_cmd)
{
	int res;
	res = PortFCGet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_config.PortNum);
	ioctl_cmd->msg.PORT_config.Status = res;
   return res;
}

/**********************************************************************************
 *
 * ioctl_SpecCapsSet - Passes a full configuration structure to the port
 ***********************************************************************************/
static int ioctl_SpecCapsSet(switch_ioctl_cmd_t *ioctl_cmd)
{
	SWITCH_SPECIAL_CAPS res;
   int ret =0;
	if(ioctl_cmd->msg.SWITCH_specialcap.data_size != sizeof(SWITCH_SPECIAL_CAPS))
	{
		printk("%s: buffer size mismatch\n",__FUNCTION__);
		return PAL_False;
	}
	copy_from_user(&res,ioctl_cmd->msg.SWITCH_specialcap.pSpecialCap,
			ioctl_cmd->msg.SWITCH_specialcap.data_size);
	ret = SpecCapsSet(ioctl_cmd->instanceId,&res);
	return ret;
}

/***********************************************************************************
 *
 * ioctl_SpecCapsGet - Retrieves a full configuration structure from the port
 ************************************************************************************/
static int ioctl_SpecCapsGet(switch_ioctl_cmd_t *ioctl_cmd)
{
	SWITCH_SPECIAL_CAPS res;
   int ret =0;
	if(ioctl_cmd->msg.SWITCH_specialcap.data_size != sizeof(SWITCH_SPECIAL_CAPS))
	{
		printk("%s: buffer size mismatch\n",__FUNCTION__);
		return PAL_False;
	}
	ret = SpecCapsGet(ioctl_cmd->instanceId,&res); 
	copy_to_user(ioctl_cmd->msg.SWITCH_specialcap.pSpecialCap,&res, 
			ioctl_cmd->msg.SWITCH_specialcap.data_size);

	return ret;
}

/***********************************************************************************
 *
 *  ioctl_TrailerModeSet- Set the trailer mode. 
 ************************************************************************************/
static int ioctl_TrailerModeSet(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret;
	ret = TrailerSet(ioctl_cmd->instanceId,ioctl_cmd->TrailerMode);
	return ret;
}

/***********************************************************************************
 *
 * ioctl_StatPortGet - retrieves the statistic from the given port
 *
 * Retreives the statistic from the given port.
 ************************************************************************************/
static int ioctl_StatPortGet(switch_ioctl_cmd_t *ioctl_cmd)
{
	int ret = 0;
	MIB2_COUNTER res;
	if(ioctl_cmd->msg.PORT_counters.data_size != sizeof(MIB2_COUNTER))
	{
		printk("%s: buffer size mismatch\n",__FUNCTION__);
		return -1;
	}

	ret = StatPortGet(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_counters.PortNum,&res);
	copy_to_user(ioctl_cmd->msg.PORT_counters.pCounters,&res,ioctl_cmd->msg.PORT_counters.data_size);

   return ret;
}

/***********************************************************************************
 *
 *  ioctl_StatPortReset - resets port statistic for the given port.
 *
 * Resets the statistic on given port.
 ************************************************************************************/
static int ioctl_StatPortReset(switch_ioctl_cmd_t *ioctl_cmd)
{	
	int res = 0;
	res = StatPortReset(ioctl_cmd->instanceId,ioctl_cmd->msg.PORT_counters.PortNum);
   return res;

}

/**************************************************************************************
 * 
 *  ioctl_VlanCreate - Create VLAN.
 *
 *  Create logical VLAN. No real updates are performed.
 ************************************************************************************/
static int ioctl_VlanCreate(switch_ioctl_cmd_t *ioctl_cmd)
{
	int ret = 0;

	ret = VlanCreate(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.vlnId);
   return ret;

}

/**************************************************************************************
 *
 * ioctl_VlanDelete - Destroy the existing VLAN.
 *
 * Deletes the VLAN
 *************************************************************************************/
static int ioctl_VlanDelete(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret = 0;
	ret = VlanDelete(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.vlnId);
   return ret;
}

/**************************************************************************************
 *
 * ioctl_VlanPortAdd - add port to VLAN.
 *
 * Adds port to the existing VLAN. 
 *************************************************************************************/
static int ioctl_VlanPortAdd(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret = 0;
	ret = VlanPortAdd(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.vlnId,
			ioctl_cmd->msg.VLAN_info.portNum,ioctl_cmd->msg.VLAN_info.tag);
   return ret;

}

/**************************************************************************************
 *
 * ioctl_VlanPortDelete - remove port from VLAN.
 *
 * Remove port from the existing VLAN. 
 *************************************************************************************/
static int ioctl_VlanPortDelete(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret = 0;
	ret = VlanPortDelete(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.portNum,
			ioctl_cmd->msg.VLAN_info.vlnId);
   return ret;
}

/**************************************************************************************
 *
 * ioctl_VlanCPUAdd - Adds CPU to VLAN.
 *
 * Adds CPU to VLAN.
 *************************************************************************************/
static int ioctl_VlanCPUAdd(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret =0;
	ret = VlanCPUAdd(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.vlnId,ioctl_cmd->msg.VLAN_info.tag);
   return ret;
}

/************************************************************************************
 *
 *  ioctl_VlanCPUDelete -   Deletes CPU from VLAN.
 *
 * Updates port to tagged/untagged in VLAN.
 *************************************************************************************/
static int ioctl_VlanCPUDelete(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret = 0;
	ret = VlanCPUDelete(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.vlnId);
   return ret;

}

/************************************************************************************
 *
 *  ioctl_VlanDefaultSet -   Sets default VLAN for the given port.
 *
 * Updates port to tagged/untagged in VLAN.
 *************************************************************************************/
static int ioctl_VlanDefaultSet(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret =0;
	ret = VlanDefaultSet(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.portNum,ioctl_cmd->msg.VLAN_info.vlnId);
   return ret;
}

/************************************************************************************
 *
 * ioctl_VlanPortUpdate - updates port status in VLAN
 *
 * Updates port to tagged/untagged in VLAN.
 *************************************************************************************/
static int ioctl_VlanPortUpdate(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret =0;
	ret = VlanPortUpdate(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.portNum,ioctl_cmd->msg.VLAN_info.vlnId,
			ioctl_cmd->msg.VLAN_info.tag);
   return ret;
}

/*************************************************************************************
 *
 * ioctl_VlanSetPortPriority -  Set the priority of a port
 *
 *************************************************************************************/
static int ioctl_VlanSetPortPriority(switch_ioctl_cmd_t *ioctl_cmd)
{
   int ret =0;
	ret = VlanSetPortPriority(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.portNum,
			ioctl_cmd->msg.VLAN_info.priority);
   return ret;
}

/*******************************************************************************
 *
 * ioctl_VlanGetPortPriority -  Get the priority of a port
 *************************************************************************************/
static Uint32 ioctl_VlanGetPortPriority(switch_ioctl_cmd_t *ioctl_cmd)
{
	Uint32 ret =0;
   
	ret = VlanGetPortPriority(ioctl_cmd->instanceId,ioctl_cmd->msg.VLAN_info.portNum);
   return ret;
}

/*******************************************************************************
 *
 * ioctl_DeleteMacAddrFromTable
 *************************************************************************************/
static Uint32 ioctl_DeleteMacAddrFromTable(switch_ioctl_cmd_t *ioctl_cmd)
{
	Uint32 ret =0;

	ret = DeleteMacAddrFromAtu(ioctl_cmd->instanceId,ioctl_cmd->msg.MAC_addr.MacAddr);
   return ret;
}

static int switch_open( struct inode * inode, struct file * filp )
{
	filp->private_data = switch_dev;
	return 0;
}



static int switch_close( struct inode * inode, struct file * filp )
{
	return 0;
}

struct file_operations switch_fops = {
owner:    THIS_MODULE,
ioctl:    switch_ioctl,
open:     switch_open,
release:  switch_close
};


int __init switch_init(void)
{
	SWITCH_INIT SwitchInfo;
   int i,index=0;
#ifdef CONFIG_MIPS_AVALANCHE_ADAM2  
	unsigned int  cpu_freq = 0;
#endif

	if((switch_dev = kmalloc(sizeof(SWITCH_DEV), GFP_KERNEL)) == NULL)
      return -ENODEV;


	/* register Marvell switch device */
	switch_dev->dev_handle = devfs_register(NULL, "Switch", DEVFS_FL_AUTO_DEVNUM, 0, 0,
			S_IFCHR | S_IRUGO | S_IWUGO, &switch_fops,NULL);

	switch_dev->switch_handle = SwitchDrvCreate();

	if(switch_dev->switch_handle == (Uint32)NULL)
		return -ENODEV;

#ifdef CONFIG_MIPS_AVALANCHE_ADAM2  
	/* put and bring the MDIO out of reset. */
	REG32_RMW(0xa8611600, 22, 22, 0);
	mdelay(200);
	REG32_RMW(0xa8611600, 22, 22, 1);
	mdelay(200);
	cpu_freq = avalanche_clkc_get_freq(CLKC_MIPS);
	printk("cpu_freq = %d\n",cpu_freq);
	/* Start MDIO */
	REG32_W(0xa8611e04,
			(1 << 30) | ((cpu_freq / 1000000) & MASK(15, 0)));
#endif /* CONFIG_MIPS_AVALANCHE_ADAM2 */


	SwitchInfo.channelNumber = 0;
	SwitchInfo.instanceId = 0;
	SwitchInfo.internalPortNum = 5;
	SwitchInfo.miiBase = 0xA8611E00;
	SwitchInfo.switchType = SWITCH_MARVELL_6063;/*2 = SWITCH_KENDIN_8995x;*/

   if(!SwitchAdd(switch_dev->switch_handle,&SwitchInfo))
      return -ENODEV;

	SwitchDrvStart(switch_dev->switch_handle,SwitchInfo.instanceId);


	/* NSP need the ability to add the port Id to the packet itself */
	TrailerSet(SwitchInfo.instanceId,TRUE);

   /**** Create Read and Write entry for each Port ****/   
   port_config_dir = proc_mkdir("sys/dev/marvell", NULL);
   /**** Create Read entry for each Port ****/
   port_status_dir = proc_mkdir("avalanche/marvell", NULL);
   
   for(i = LAN1_PORT; i<= LAN4_PORT ;i++,index++)
   {
      char str[10];
      
      sprintf(str,"%d",index + 1);
      port_lan_config_file[index] = create_proc_entry(str, 0644, port_config_dir);
      port_lan_config_file[index]->data = (void * ) (LAN1_PORT +index); 	
      port_lan_config_file[index]->read_proc = marvell_port_read_config;	
      port_lan_config_file[index]->write_proc = marvell_port_config;		   
      
      port_lan_status_file[index] = create_proc_entry(str, 0444, port_status_dir);
      port_lan_status_file[index]->data = (void * ) (LAN1_PORT + index); 	
      port_lan_status_file[index]->read_proc = marvell_port_status;	
      
   }
   return 0;
}

void switch_exit(void)
{
   int i,index=0;
   SwitchDrvDelete(switch_dev->switch_handle);
   devfs_unregister(switch_dev->dev_handle);
   kfree(switch_dev);
   
   if(port_config_dir)
   {
      for(i = LAN1_PORT; i<= LAN4_PORT ;i++,index++)
      {
         char str[10];
         
         sprintf(str,"%d",index + 1);
         remove_proc_entry(str, port_config_dir);    
         remove_proc_entry(str, port_status_dir); 
         
      }
      remove_proc_entry("sys/dev/marvell", NULL); 
      remove_proc_entry("avalanche/marvell", NULL); 
   }
   
   return;
}

void Switch_SetTrailerMode(HANDLE instanceId,Bool Mode)
{
	TrailerSet(instanceId,Mode);
}

MODULE_DESCRIPTION("Driver for MARVELL 6063 Switch");
MODULE_AUTHOR("Maintainer: Liat Guedj <liat@ti.com>");

module_init(switch_init);
module_exit(switch_exit);
