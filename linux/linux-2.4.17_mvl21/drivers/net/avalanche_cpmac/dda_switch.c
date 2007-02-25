/*****************************************************************************
 * FILE PURPOSE:     - External Switch functions Source                                   
 ******************************************************************************   
 * FILE NAME:     dda_switch.c                                                       
 *                                                                                
 * DESCRIPTION:   External Switch IOCTL and proc entries implementation.                      
 *                                                                                
 * REVISION HISTORY:                                                              
 * 1 FEB 2004                                                             
 *                                                                                
 * (C) Copyright 2004, Texas Instruments, Inc                                     
 *******************************************************************************/ 

#include <linux/config.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>

#include "dda_switch.h"

#define PROC_END_DEBUG  0
#define procPrint if (PROC_END_DEBUG) printk


#define LAN1_PORT               1
#define LAN2_PORT               2
#define LAN3_PORT               3
#define LAN4_PORT               4

static struct proc_dir_entry *port_config_dir = NULL;
static struct proc_dir_entry *port_status_dir = NULL;
static struct proc_dir_entry *port_lan_config_file[4];
static struct proc_dir_entry *port_lan_status_file[4];

static int switch_port_read_config(char* buf, char **start, off_t offset, int count, int *eof, void *data);
static int switch_port_config(struct file *file, const char* buffer, unsigned long count, void *data);
static int switch_port_status(char* buf, char **start, off_t offset, int count, int *eof, void *data);

/*****************************************************************************************************/
/*                                    PROC FUNCTIONS                                                 */
/*****************************************************************************************************/

/******************************************************************************************************/
/*switch_create_proc_entry - Create two proc entries one for read/write named sys/dev/marvell         */
/*                           and one for read named avalanche/marvell.                                */
/*                                                                                                    */
/******************************************************************************************************/
void switch_create_proc_entry(void)
{
   int i,index =0;
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
      port_lan_config_file[index]->read_proc = switch_port_read_config;	
      port_lan_config_file[index]->write_proc = switch_port_config;		   
      
      port_lan_status_file[index] = create_proc_entry(str, 0444, port_status_dir);
      port_lan_status_file[index]->data = (void * ) (LAN1_PORT + index); 	
      port_lan_status_file[index]->read_proc = switch_port_status;	
      
   }
   return;
}

/******************************************************************************************************/
/*switch_remove_proc_entry - Remove the ExtSwitch proc entries named sys/dev/marvell                  */
/*                           and avalanche/marvell.                                                   */
/*                                                                                                    */
/******************************************************************************************************/
void switch_remove_proc_entry(void)
{
   int i,index =0;
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
}

static int switch_port_read_config(char* buf, char **start, off_t offset, int count, int *eof, void *data)
{
   int len;
   unsigned int port_status = 0,Duplex,Speed;
   int PortNum = (int)data;
   
   
   MOD_INC_USE_COUNT;
   procPrint("read ....\n");
   if(switch_portLinkGet(PortNum) == PORT_LINK_DOWN)
   {
      procPrint("LINK is down Port num = %d\n",PortNum);    
      port_status = 5;
   }
   else
   {
      
      if( switch_portAutoGet(PortNum) == PORT_AUTO_YES)
      {
         procPrint("Auto negt. is enabled \n");
         port_status = 0;
         
      }else /* NO AUTO */
      {
         
         Duplex = switch_portDuplexGet(PortNum);
         Speed = switch_portSpeedGet(PortNum);
         
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
/*switch_port_config - The write call back function for the proc entry.                             */
/*                      *buffer - contain the user command :                                         */
/*                              0 - Restart Auto Negotiation.                                        */
/*                              1 - Disable Auto Negotiation and Set to Half Duplex 10 Mbps.         */
/*                              2 - Disable Auto Negotiation and Set to Full Duplex 10 Mbps.         */
/*                              3 - Disable Auto Negotiation and Set to Half Duplex 100 Mbps.        */
/*                              4 - Disable Auto Negotiation and Set to Full Duplex 100 Mbps.        */
/*                      *data - contain the Port Number                                              */
/*                      count - Number of Bytes need to read from the buffer.                        */
/******************************************************************************************************/
static int switch_port_config(struct file *file, const char* buffer, unsigned long count, void *data) 
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
		if( switch_portAutoGet(PortNum) == PORT_AUTO_YES)
		{
			procPrint("Auto negt is already enabled, Restart Auto Negt.\n");
			switch_portAutoSet(PortNum,PORT_AUTO_RESTART);
		}else
		{
			procPrint("Auto negt is disabled, Enable Auto Negt.\n");		
			switch_portAutoSet(PortNum,PORT_AUTO_YES);
		}

	}
	else if(strcmp("1", status) == 0)
	{
		switch_portAutoSet(PortNum,PORT_AUTO_NO);
		switch_portDuplexSet(PortNum,PORT_DUPLEX_HALF);
		switch_portSpeedSet(PortNum,PORT_SPEED_10);

		procPrint("Disable auto negt.And force to 10Mbps Half Duplex\n");


	}
	else if(strcmp("2", status) == 0)
	{
		switch_portAutoSet(PortNum,PORT_AUTO_NO);
		switch_portDuplexSet(PortNum,PORT_DUPLEX_FULL);
		switch_portSpeedSet(PortNum,PORT_SPEED_10);
		procPrint("Disable auto negt.And force to 10Mbps Full Duplex\n");


	}
	else if(strcmp("3", status) == 0)
	{
		switch_portAutoSet(PortNum,PORT_AUTO_NO);
		switch_portDuplexSet(PortNum,PORT_DUPLEX_HALF);
		switch_portSpeedSet(PortNum,PORT_SPEED_100);
		procPrint("Disable auto negt.And force to 100Mbps Half Duplex\n");

	}
	else if(strcmp("4", status) == 0)
	{
		switch_portAutoSet(PortNum,PORT_AUTO_NO);
		switch_portDuplexSet(PortNum,PORT_DUPLEX_FULL);
		switch_portSpeedSet(PortNum,PORT_SPEED_100);
		procPrint("Disable auto negt.And force to 100Mbps Full Duplex\n"); 
	}

	MOD_DEC_USE_COUNT;
	return len;   

}

/*****************************************************************************************************/
/*switch_port_status - The read call back function for the proc entry.                              */
/*                      *data - Port Number                                                          */
/*                      RETURN - The Speed and Duplex of the port. The Returns value can be:         */
/*                              1 - Half Duplex 10 Mbps.                                             */
/*                              2 - Full Duplex 10 Mbps.                                             */
/*                              3 - Half Duplex 100 Mbps.                                            */
/*                              4 - Full Duplex 100 Mbps.                                            */
/*                              5 - Link is Down.                                                    */
/******************************************************************************************************/
static int switch_port_status(char* buf, char **start, off_t offset, int count, int *eof, void *data) 
{
	int len;
	unsigned int port_status = 0,Duplex,Speed;
	int PortNum = (int)data;


	MOD_INC_USE_COUNT;
	procPrint("read ....\n");
	if(switch_portLinkGet(PortNum) == PORT_LINK_DOWN)
	{
		procPrint("LINK is down Port num = %d\n",PortNum);    
		port_status = 5;
	}
	else
	{

		if( switch_portAutoGet(PortNum) == PORT_AUTO_YES)
		{
			procPrint("Auto negt. is enabled \n");
		}

		Duplex = switch_portDuplexGet(PortNum);
		Speed = switch_portSpeedGet(PortNum);

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

int external_switch_ioctl(SwitchIoctlType *ioctl_cmd)
{
	int ret = 0;


	switch (ioctl_cmd->t_opcode)
	{

			/* SWITCH MANAGEMENT */

		case SWITCH_GETVERSION:		
         ret = switch_getVersion(&ioctl_cmd->msg.SWITCH_verinfo.verinfo);
			break;

		case SWITCH_SPECCAPGET:
         ret = switch_specCapsGet(&ioctl_cmd->msg.SWITCH_specialcap.SpecialCap); 
			break;

		case SWITCH_SPECCAPSET:
         ret = switch_specCapsSet(&ioctl_cmd->msg.SWITCH_specialcap.SpecialCap);
			break;

		case SWITCH_TRAILERSET: 
         ret = switch_trailerSet(ioctl_cmd->msg.PORT_config.Status);
			break;

			/* PORT MANAGEMENT */

		case PORT_LINKGET:
         ret = (int)switch_portLinkGet(ioctl_cmd->msg.PORT_config.PortNum);
	      ioctl_cmd->msg.PORT_config.Status = ret;
			break;

		case PORT_SPEEDSET:
         ret = switch_portSpeedSet(ioctl_cmd->msg.PORT_config.PortNum,(PORT_SPEED)ioctl_cmd->msg.PORT_config.Status/*Speed*/);
			break;

		case PORT_SPEEDGET:
         ret = switch_portSpeedGet(ioctl_cmd->msg.PORT_config.PortNum);
	      ioctl_cmd->msg.PORT_config.Status = ret;
			break;

      case PORT_AUTOSET:
         ret = switch_portAutoSet(ioctl_cmd->msg.PORT_config.PortNum,(PORT_AUTO)ioctl_cmd->msg.PORT_config.Status/*autoStatus*/);
         break;

		case PORT_AUTOGET:
         ret = switch_portAutoGet(ioctl_cmd->msg.PORT_config.PortNum);
         ioctl_cmd->msg.PORT_config.Status = ret; /*autoStatus*/
			break;

      case PORT_DUPLEXSET:
         ret = switch_portDuplexSet(ioctl_cmd->msg.PORT_config.PortNum,(PORT_DUPLEX)ioctl_cmd->msg.PORT_config.Status/*DuplexStatus*/);
			break;

      case PORT_DUPLEXGET:
         ret = switch_portDuplexGet(ioctl_cmd->msg.PORT_config.PortNum/*port num*/);
         ioctl_cmd->msg.PORT_config.Status = ret; /*DuplexStatus*/         
         break;

      case PORT_STATUSSET:
         ret = switch_portStatusSet(ioctl_cmd->msg.PORT_config.PortNum,ioctl_cmd->msg.PORT_config.Status);   
         break;

      case PORT_STATUSGET:
         ret = switch_portStatusGet(ioctl_cmd->msg.PORT_config.PortNum);
         ioctl_cmd->msg.PORT_config.Status = ret;
       	break;

      case PORT_FCSET:
         ret = switch_portFCSet(ioctl_cmd->msg.PORT_config.PortNum,ioctl_cmd->msg.PORT_config.Status);
         break;
         
      case PORT_FCGET:
         ret = switch_portFCGet(ioctl_cmd->msg.PORT_config.PortNum);
         ioctl_cmd->msg.PORT_config.Status = ret;
         break;
         
         /* S T A T I S T I C S */
         
      case PORT_STATISTICGET:
         ret = switch_statPortGet(ioctl_cmd->msg.PORT_counters.PortNum,&ioctl_cmd->msg.PORT_counters.Counters);
         break;
         
      case PORT_STATISTICRESET:
         ret = switch_statPortReset(ioctl_cmd->msg.PORT_counters.PortNum);
         break;
         
         /*    VLAN SUBMODULE  */
      case VLAN_CREATE:		/* Creates VLAN with the given ID. */
         ret = switch_vlanCreate(ioctl_cmd->msg.VLAN_info.vlnId);
         break; 
         
         
      case VLAN_DELETE:		/* Deletes VLAN. */
         ret = switch_vlanDelete(ioctl_cmd->msg.VLAN_info.vlnId);
         break;
         
      case VLAN_PORT_ADD:		/* Adds port to VLAN. */
         ret = switch_vlanPortAdd(ioctl_cmd->msg.VLAN_info.vlnId,ioctl_cmd->msg.VLAN_info.PortNum,ioctl_cmd->msg.VLAN_info.tag);
         break;
         
      case VLAN_PORT_DELETE:		/* Deletes port from VLAN. */
         ret = switch_vlanPortDelete(ioctl_cmd->msg.VLAN_info.PortNum,ioctl_cmd->msg.VLAN_info.vlnId);
         
         break;
         
      case VLAN_SET_DEF:		/* Sets default VLAN for the given port. */
         ret = switch_vlanDefaultSet(ioctl_cmd->msg.VLAN_info.PortNum,ioctl_cmd->msg.VLAN_info.vlnId);
         
         break;
         
      case VLAN_PORT_UPDATE:		/* Updates port status in VLAN. */
         ret = switch_vlanPortUpdate(ioctl_cmd->msg.VLAN_info.PortNum,ioctl_cmd->msg.VLAN_info.vlnId,ioctl_cmd->msg.VLAN_info.tag);
         break;
         
      case VLAN_SET_PORT_PRI:		/* Sets the priority of a port. */
         ret = switch_vlanSetPortPriority(ioctl_cmd->msg.VLAN_info.PortNum,ioctl_cmd->msg.VLAN_info.priority);
         break;

      case VLAN_GET_PORT_PRI:		/* Gets the priority of a port. */
         ret = switch_vlanGetPortPriority(ioctl_cmd->msg.VLAN_info.PortNum);
         
         break;
         
      case DELETE_MAC_ADDR:
         ret = switch_deleteMacAddrFromAtu(ioctl_cmd->msg.MAC_addr.MacAddr);
         
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
