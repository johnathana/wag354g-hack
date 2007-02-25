/**************************************************************************
 * FILE PURPOSE	:   WAN Bridge Configuration.	
 **************************************************************************
 * FILE NAME	:   wb_config.c
 *
 * DESCRIPTION	:
 *  This file contains functions that are necessary to configure and control
 *  the WAN Bridge.
 *
 *	CALL-INs:
 *
 *	CALL-OUTs:
 *
 *	User-Configurable Items:
 *
 *	(C) Copyright 2004, Texas Instruments, Inc.
 *************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <net/wb.h>
#include <asm/mips-boards/prom.h>

/**************************** STATIC FUNCTIONS ****************************/

/* Proc Entry. */
static int wb_write_bridge_info(struct file*file, const char*buffer,unsigned long count,void* data);
static int wb_read_bridge_info(char* buf, char**start, off_t offset, int count, int *eof, void *data);

/* Command handlers */
static int wb_add_bridge (char *argv[], int argc);
static int wb_del_bridge (char *argv[], int argc);
static int wb_del_interface (char *argv[], int argc);
static int wb_add_interface (char *argv[], int argc);
static int wb_show_bridge (char *argv[], int argc);
static char *wb_get_mode_string(WAN_BRIDGE_MODE wb_mode);
static int wb_debug (char *argv[], int argc);
static void wb_show_help(void);

/**************************** GLOBALS ************************************/
LIST_HEAD (wan_bridge_list);
int      wb_malloc_counter = 0;
int      wb_free_counter   = 0;
char*    wan_bridge_device_mac_string; 
int      wbDbgLevel        = WB_LOG_FATAL;
int      gError = 0;

/**************************************************************************
 * FUNCTION NAME : wb_write_bridge_info
 **************************************************************************
 * DESCRIPTION   :
 *  The function is the write handler for the WAN Bridge proc entry. This 
 *  will be called by the system, when the user does an "echo" to the proc
 *  entry. 
 * 
 * RETURNS       :
 *      0   -   Success.
 *      < 0 -   Error.
 ***************************************************************************/
static int wb_read_bridge_info(char* buf, char**start, off_t offset, int count, int *eof, void *data)
{
    int                 len = 0;

    /* We use the read proc file system to return the error code. This was added because
     * returning an error, i.e. negative value on write was not getting propogated to the
     * user space cfgmgr module. This seemed like an issue with proc fs. */
#if 0
    len += sprintf (buf+len,"This is a WRITE ONLY PROC ENTRY.\n");
    len += sprintf (buf+len,"echo help on the proc entry to see the list of commands\n");
#else
    len += sprintf (buf+len,"%d\n",gError);
#endif

    /* Return the length. */
    return len;
}

/**************************************************************************
 * FUNCTION NAME : wb_write_bridge_info
 **************************************************************************
 * DESCRIPTION   :
 *  The function is the write handler for the WAN Bridge proc entry. This 
 *  will be called by the system, when the user does an "echo" to the proc
 *  entry. 
 * 
 * RETURNS       :
 *      0   -   Success.
 *      < 0 -   Error.
 ***************************************************************************/
static int wb_write_bridge_info 
(
    struct file*    file,
    const char*     buffer,
    unsigned long   count,
    void*           data
)
{
	char    bridge_operation[100];
	char*	argv[10];
	int		argc = 0;
	char*	ptr_cmd;
    char*   delimitters = " \n\t";

	/* Validate the length of data passed. */
	if (count > 100)
		count = 100; 

    /* Initialize the buffer before using it. */
    memset ((void *)&bridge_operation[0], 0, sizeof(bridge_operation));
    memset ((void *)&argv[0], 0, sizeof(argv));
	
	/* Copy from user space. */	
	if (copy_from_user (&bridge_operation, buffer, count))
		return -EFAULT;

	/* Tokenize the command. Check if there was a NULL entry. If so be the case the
     * user did not know how to use the entry. Print the help screen. */
	ptr_cmd = strtok (bridge_operation, delimitters);
    if (ptr_cmd == NULL)
    {
        /* Show the help screen. */ 
        wb_show_help();
        return count;
    }
	do
	{
        /* Extract the first command. */
		argv[argc++] = ptr_cmd;

        /* Validate if the user entered more commands.*/
		if (argc >=10)
		{
			wblogMsg (WB_LOG_FATAL,"ERROR: Incorrect too many parameters dropping the command.",0,0,0,0,0);
			return -EFAULT;
		}

        /* Get the next valid command. */
		ptr_cmd = strtok (NULL, delimitters);
	} while (ptr_cmd != NULL);

    /* DEBUG TEST CODE -----*/
    {
        int i = 0;
        wblogMsg (WB_LOG_DEBUG,"Detected %d arguments.\n", argc,0,0,0,0);
        for (i = 0; i < argc; i++)
           wblogMsg (WB_LOG_DEBUG,"Argument %d = %s.\n", i, (int)argv[i],0,0,0);
    }
    
    /* Process the command. This has to be the first argument. Check with the list of 
     * supported commands. */   
    if (strncmp(argv[0], "addbr", strlen("addbr")) == 0)
    {
        /* Add and create a new WAN Bridge. */
        gError = wb_add_bridge (argv, argc);
        if (gError < 0)
            return gError;
        else
            return count;
    }

    if (strncmp(argv[0], "delbr", strlen("delbr")) == 0)
    {
        /* Deletes an existing WAN Bridge. */
        gError = wb_del_bridge (argv, argc);
        if (gError < 0)
            return gError;
        else
            return count;
    }

    if (strncmp(argv[0], "show", strlen("show")) == 0)
    {
        /* Display all the information about the WAN Bridge. */
        gError = wb_show_bridge (argv, argc);
        if (gError < 0)
            return gError;
        else
            return count;
    }

    if (strncmp(argv[0], "addif", strlen("addif")) == 0)
    {
        /* Add and create a new WAN Bridge device interface. */
        gError = wb_add_interface (argv, argc);
        if (gError < 0)
            return gError;
        else
            return count;
    }

    if (strncmp(argv[0], "delif", strlen("delif")) == 0)
    {
        /* Deletes a WAN Bridge interface. */
        gError = wb_del_interface (argv, argc);
        if (gError < 0)
            return gError;
        else
            return count;
    }

    if (strncmp(argv[0], "help", strlen("help")) == 0)
    {
        /* Show the help screen. */
        wb_show_help();
        return count;
    }

    if (strncmp(argv[0], "debug", strlen("debug")) == 0)
    {
        /* Show the help screen. */
        gError = wb_debug (argv, argc);
        if (gError < 0)
            return gError;
        else
            return count;
    }

    /* Set the Error code. */
    gError = -EFAULT;
    return gError;
}

/**************************************************************************
 * FUNCTION NAME : wb_init
 **************************************************************************
 * DESCRIPTION   :
 *  Startup code for the WAN Bridge. The function creates the proc entries
 *  for initialization.
 * 
 * RETURNS       :
 *      0   -   Success.
 *      < 0 -   Error.
 ***************************************************************************/
static int __init wb_init (void)
{
    struct proc_dir_entry*  ptr_dir_entry;

    /* Indicate to the world that the WAN Bridge module is initializing. */
    printk ("Initializing the WAN Bridge.\n");

    /* Create the proc entry for configuration of the WAN bridge and devices. */
    ptr_dir_entry = create_proc_entry("avalanche/wan_bridge", 0644, NULL);
    if (ptr_dir_entry == NULL)
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: Unable to create the proc entry for bridge creation.\n",0,0,0,0,0);
        return -1;
    }
    ptr_dir_entry->data         = NULL;
    ptr_dir_entry->read_proc    = wb_read_bridge_info;
    ptr_dir_entry->write_proc   = wb_write_bridge_info;
    ptr_dir_entry->owner        = NULL;

    /* Get the WAN Bridge Address. All WAN Bridge devices use the same MAC Address. */
    wan_bridge_device_mac_string = prom_getenv("wan_br_mac");
    if(!wan_bridge_device_mac_string)
    {
        wan_bridge_device_mac_string = "00.E0.A6.66.51.EB";
        printk("Please set the MAC Address for the WAN Bridge.\n");
        printk("Set the Environment variable 'wan_br_mac'. \n");
        printk("MAC Address should be in the following format: xx.xx.xx.xx.xx.xx\n");
    }

    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_deinit
 **************************************************************************
 * DESCRIPTION   :
 *  Deinitialization / Cleanup of the WAN Bridge module. Currently we do
 *  not support this.
 ***************************************************************************/
static void __exit wb_deinit(void)
{
    return;
}

/**************************************************************************
 * FUNCTION NAME : wb_show_bridge
 **************************************************************************
 * DESCRIPTION   :
 * 	Command Handler for displaying information about all WAN Bridges present
 * 	in the system. 
 *
 * RETURNS      :
 *  Always returns 0.
 ***************************************************************************/
static int wb_show_bridge (char *argv[], int argc)
{
    struct list_head*   ptr_temp_bridge;
    struct list_head*   ptr_temp_device;
    WAN_BRIDGE_DEVICE*  ptr_bridge_dev;
    WAN_BRIDGE*         ptr_wan_bridge;

    wblogMsg (WB_LOG_DEBUG, "DEBUG: Malloc Counter:%d.\n",wb_malloc_counter,0,0,0,0);
    wblogMsg (WB_LOG_DEBUG, "DEBUG: Free Counter  :%d.\n",wb_free_counter,0,0,0,0);

    printk ("WAN Bridge Information :\n\n");

    /* Cycle through the list of Bridges, to see if there is one with the existing name */
    list_for_each(ptr_temp_bridge, &wan_bridge_list)
    {
        /* Get the WAN Bridge Information. */
        ptr_wan_bridge = (WAN_BRIDGE *)ptr_temp_bridge;

        /* Print the WAN Bridge Information. */
        printk ("WAN Bridge Name:%s Src Interface:%s\n",
                ptr_wan_bridge->name, ptr_wan_bridge->ptr_src_device->name);
        printk ("WAN Bridge Mode: %s.\n", wb_get_mode_string(ptr_wan_bridge->wb_mode));

        /* Display the various statistics for the bridge. */
        printk ("Memory Errors              : %d.\n", ptr_wan_bridge->memory_errors);
        printk ("Rx Packets Dropped         : %d.\n", ptr_wan_bridge->rx_packet_dropped);
        printk ("Non-VLAN Packets received  : %d.\n", ptr_wan_bridge->non_vlan_packets_rxed);
        printk ("Tx Packets Dropped         : %d.\n", ptr_wan_bridge->tx_packet_dropped);

        /* Display the Interface information.*/
        printk ("Interfaces Attached are :-\n");
        if (list_empty(&ptr_wan_bridge->device_list) == 1)
        {
            /* There are none attached. */
            printk (" No Interfaces attached to the bridge.\n");
        }
        else
        {
            /* Print a list of interfaces attached to the BRIDGE. */
            list_for_each (ptr_temp_device, &ptr_wan_bridge->device_list)
            {
                /* Get the bridge device information. */
                ptr_bridge_dev = (WAN_BRIDGE_DEVICE *)ptr_temp_device;
                printk ("   Name: %s VLAN Id: %d User Priority:%d\n", 
                        ptr_bridge_dev->ptr_net_device->name, 
                        ptr_bridge_dev->vlan_key,
                        ptr_bridge_dev->user_priority);
            }
        }
        printk ("*********************************************\n");
    }
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_add_bridge
 **************************************************************************
 * DESCRIPTION   :
 * 	Command Handler for creating a WAN bridge.
 *
 * RETURNS       :
 *      < 0     - Error.
 *       0      - Success.
 ***************************************************************************/
static int wb_add_bridge (char *argv[], int argc)
{
    WAN_BRIDGE*         ptr_wan_bridge;
    struct net_device*  ptr_src_device;
    struct list_head*   ptr_temp; 
    WAN_BRIDGE_MODE     wb_mode;

    /****************************** BASIC VALIDATIONS ***************************/

    /* Validate the number of arguments. */
    if (argc != 4)
    {
        wb_show_help(); 
        return -EINVAL;
    }

    /* ARGUMENT 1: BRIDGE NAME VALIDATION.
     * Cycle through the list of Bridges, to see if there is one with the existing name */
    list_for_each(ptr_temp, &wan_bridge_list)
    {
        /* Get the WAN Bridge Information and check for the name. */
        ptr_wan_bridge = (WAN_BRIDGE *)ptr_temp;
        if (strcmp(ptr_wan_bridge->name, argv[1]) == 0)
        {
            wblogMsg(WB_LOG_FATAL,"ERROR: WAN Bridge %s already exists\n",(int)ptr_wan_bridge->name,0,0,0,0);
            return -EINVAL;
        }
    }

    /* ARGUMENT 2: INTERFACE NAME VALIDATION.
     * Make sure that the source interface name passed by the user exists. */
    ptr_src_device = dev_get_by_name (argv[2]);
    if (ptr_src_device == NULL)
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: Interface %s does not exist.\n",(int)argv[2],0,0,0,0);
        return -EINVAL;
    }

    /* ARGUMENT 3: WAN Bridge Mode Validation.
     * Make sure that the user entered a valid mode. */
    wb_mode = (WAN_BRIDGE_MODE)simple_strtoul(argv[3], NULL, 0);
#ifdef WAN_BRIDGE_HALF_BRIDGE_MODE_SUPPORT
    if ((wb_mode != WAN_BRIDGE_NON_VLAN_MODE) && (wb_mode != WAN_BRIDGE_VLAN_MODE) &&
        (wb_mode != WAN_BRIDGE_HALF_BRIDGE_MODE))
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: WAN Bridge Mode is incorrect can only be 0,1 or 2.\n",0,0,0,0,0);
        return -EINVAL;
    }
#else
    if ((wb_mode != WAN_BRIDGE_NON_VLAN_MODE) && (wb_mode != WAN_BRIDGE_VLAN_MODE))
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: WAN Bridge Mode is incorrect can only be 0 or 1.\n",0,0,0,0,0);
        return -EINVAL;
    }
#endif

    /************************ End of BASIC VALIDATIONS ******************************/

    /* Allocate memory for the new WAN Bridge. */
    ptr_wan_bridge = (WAN_BRIDGE *)wb_malloc(sizeof(WAN_BRIDGE));
    if (ptr_wan_bridge == NULL)
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: Unable to allocate memory for the WAN Bridge.\n",0,0,0,0,0);
        return -ENOMEM;
    }

    /* Initialize the memory. */
    memset ((void *)ptr_wan_bridge, 0, sizeof(WAN_BRIDGE));

    /* Fill up the fields for the WAN Bridge. */
    strcpy(ptr_wan_bridge->name, argv[1]);
    ptr_wan_bridge->ptr_src_device  = ptr_src_device;
    ptr_wan_bridge->wb_state        = WAN_BRIDGE_NOT_READY;
    ptr_wan_bridge->wb_mode         = wb_mode;

    /* Initialize the device list. */
    INIT_LIST_HEAD (&ptr_wan_bridge->device_list);

    /* Do an IOCTL on the source interface and pass the WAN Bridge handle. */
    if (ptr_src_device->do_ioctl == NULL)
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: Src Interface:%s does NOT support WAN Bridge\n",
                  (int)argv[2],0,0,0,0);
        wb_free(ptr_wan_bridge);
        return -EINVAL;
    }
    else
    {
        struct ifreq ifr;

        /* Initialize the structure. */
        memset ((void *)&ifr, 0, sizeof (ifr));
        ifr.ifr_ifru.ifru_data = (char *)ptr_wan_bridge;

        /* Call the IOCTL Routine. */
        if (ptr_src_device->do_ioctl (ptr_src_device, (void *)&ifr, TIWAN_ADD_WAN_BRIDGE_IOCTL) < 0)
        {
            wblogMsg (WB_LOG_FATAL,"ERROR: Src Interface:%s does NOT support WAN Bridge\n",
                      (int)argv[2],0,0,0,0);
            wb_free(ptr_wan_bridge);
            return -EINVAL;
        }
    }

    /* DEBUG Message. */
    wblogMsg (WB_LOG_DEBUG,"DEBUG: Src Interface:%s registered with WAN Bridge:0x%p\n",
              (int)argv[2],(int)ptr_wan_bridge,0,0,0);

    /* TODO: Add the created bridge to the global list. Need to verify if a critical
     * section is required. */
    list_add_tail((struct list_head *)&ptr_wan_bridge->bridge_list,&wan_bridge_list);

    wblogMsg (WB_LOG_DEBUG, "DEBUG: WAN Bridge succesfully created.\n",0,0,0,0,0);
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_del_bridge
 **************************************************************************
 * DESCRIPTION   :
 * 	Command Handler for deleting an exisiting WAN bridge.
 *
 * RETURNS       :
 *      < 0     - Error.
 *       0      - Success.
 ***************************************************************************/
static int wb_del_bridge (char *argv[], int argc)
{
    WAN_BRIDGE*         ptr_wan_bridge = NULL;
    WAN_BRIDGE_DEVICE*  ptr_wan_device;
    int                 found = 0; 
    struct list_head*   ptr_temp;
    struct ifreq        ifr;
    struct net_device*  ptr_src_device;

    /****************************** BASIC VALIDATIONS ***************************/

    /* Validate the number of arguments. */
    if (argc != 2)
    {
        wb_show_help(); 
        return -EINVAL;
    }

    /* ARGUMENT 1: BRIDGE NAME VALIDATION.
     * Cycle through the list of Bridges, to see if there is one with the existing name */
    list_for_each(ptr_temp, &wan_bridge_list)
    {
        /* Get the WAN Bridge Information and check for the name. */
        ptr_wan_bridge = (WAN_BRIDGE *)ptr_temp;
        if (strcmp(ptr_wan_bridge->name, argv[1]) == 0)
        {
            found = 1; 
            break;
        }
    }

    /* Did we get a hit ? */
    if ((found == 0) || (ptr_wan_bridge == NULL))
    {
        /* No. There was no match. */
        wblogMsg(WB_LOG_FATAL,"ERROR: WAN Bridge %s does not exist.\n",(int)argv[1],0,0,0,0);
        return -EINVAL;
    }

    /************************ End of BASIC VALIDATIONS ******************************/

    /* Deattach the WAN Bridge from the source interface. */
    ptr_src_device = ptr_wan_bridge->ptr_src_device;
    memset ((void *)&ifr, 0, sizeof (ifr));
    if (ptr_src_device->do_ioctl (ptr_src_device, (void *)&ifr, TIWAN_DEL_WAN_BRIDGE_IOCTL) < 0)
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: Src Interface %s does NOT support WAN Bridge removal\n",
                 (int)ptr_src_device->name,0,0,0,0);
        return -EINVAL;
    }

    /* First, cycle thorugh and clean up all attached WAN Device interfaces. */
    list_for_each(ptr_temp, &ptr_wan_bridge->device_list)
    {
        /* Get the WAN device information. Compare the device name for a HIT ? */
        ptr_wan_device = (WAN_BRIDGE_DEVICE *)ptr_temp;

        /* Uninitialize the WAN bridge device. */ 
        wblogMsg (WB_LOG_DEBUG,"DEBUG: Deattaching %s\n",(int)ptr_wan_device->ptr_net_device->name,0,0,0,0);
        wb_device_uninitialize(ptr_wan_device->ptr_net_device);

        /* Remove the device from the WAN Bridge. */
        list_del ((struct list_head *)&ptr_wan_device->links);

        /* Clean the memory for the WAN Bridge Device. */
        wb_free (ptr_wan_device);
    }

    /* Remove the bridge from the global list. */
    list_del((struct list_head *)&ptr_wan_bridge->bridge_list);

    /* Release the source interface. */
    dev_put (ptr_src_device);

    /* Clean the memory of the bridge. */
    wb_free(ptr_wan_bridge);
    wblogMsg (WB_LOG_DEBUG, "DEBUG: WAN Bridge succesfully deleted.\n",0,0,0,0,0);
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_add_interface
 **************************************************************************
 * DESCRIPTION   :
 * 	Command Handler for creating a WAN bridge device and attaching it to 
 * 	the appropriate WAN Bridge. 
 *
 * RETURNS       :
 *      < 0     - Error.
 *       0      - Success.
 ***************************************************************************/
static int wb_add_interface (char *argv[], int argc)
{
    int                     found = 0;
    struct list_head*       ptr_temp;
    WAN_BRIDGE*             ptr_wan_bridge = NULL;
    struct net_device*      ptr_net_device;
    WAN_BRIDGE_DEVICE*      ptr_wan_device;
    WAN_BRIDGE_DEVICE_PRIV* ptr_private;
    int                     vlan_key;
    int                     user_priority;

    /****************************** BASIC VALIDATIONS ***************************/

    /* Validate the number of arguments. */
    if ((argc != 4) && (argc != 3) && (argc != 5))
    {
        wb_show_help();
        return -1;
    }

    /* ARGUMENT 1: BRIDGE NAME VALIDATION.
     * Cycle through the list of Bridges, to see if there is one with the existing name. */
    list_for_each(ptr_temp, &wan_bridge_list)
    {
        /* Get the WAN Bridge Information and check for the name. */
        ptr_wan_bridge = (WAN_BRIDGE *)ptr_temp;
        if (strcmp(ptr_wan_bridge->name, argv[1]) == 0)
        {
            found = 1;
            break;
        }
    }

    /* Did we get a hit on the bridge name or not ? */
    if ((found == 0) || (ptr_wan_bridge == NULL))
    {
        /* The WAN Bridge name does not exist. */
        wblogMsg (WB_LOG_FATAL,"ERROR: The WAN Bridge %s does NOT exist.\n",(int)argv[1],0,0,0,0);
        return -EINVAL;
    }

    /* ARGUMENT 2: INTERFACE NAME VALIDATION.
     * Check if the interface is not already present. */
    ptr_net_device = dev_get_by_name (argv[2]);
    if (ptr_net_device != NULL)
    {
        /* The device already exists. The function has incremented the refcnt, which needs to be 
         * decremented hence the dev_put needs to be called, if you do not call this the 
         * unregistration will hang as ref_cnt is non zero. */
        wblogMsg (WB_LOG_FATAL,"ERROR: WAN Bridge Device %s already exists.\n",(int)argv[2],0,0,0,0);
        dev_put(ptr_net_device);
        return -EINVAL;
    }

    /* ARGUMENT 3: VLAN Tag range validation.
     * VLAN Tags are a must for VLAN Mode bridges. */
    if (ptr_wan_bridge->wb_mode == WAN_BRIDGE_VLAN_MODE)
    {
        /* VLAN Mode Bridges. VLAN Key should be present. */
        if (argv[3] == NULL)
        {
            wblogMsg (WB_LOG_FATAL,"ERROR: VLAN Mode Bridge should have a VLAN key\n",0,0,0,0,0);
            return -EINVAL;
        }
        
        /* Validate the range of the VLAN key. */
        vlan_key = (int)simple_strtoul(argv[3], NULL, 0);
        if ((vlan_key <= 0) || (vlan_key > MAX_VLAN_SUPPORTED))
        {
            wblogMsg (WB_LOG_FATAL,"ERROR: VLAN should be between 1-%d\n",MAX_VLAN_SUPPORTED,0,0,0,0);
            return -EINVAL;
        }

        /* Validate the user priority bits if entered. */
        if (argv[4] != NULL)
        {
            user_priority = (int)simple_strtoul(argv[4], NULL, 0);
            if ((user_priority < 0) || (user_priority > 7))
            {
                wblogMsg (WB_LOG_FATAL,"ERROR: User Priority should be between 1-7\n",0,0,0,0,0);
                return -1;
            }
        }
        else
        {
            /* Assume no user priority was requested. */
            user_priority = 0;
        }
        
        /* Debug Message. */
        wblogMsg (WB_LOG_DEBUG,"DEBUG: VLAN Id:%d and User Priority:%d.\n",vlan_key,user_priority,0,0,0);
    }
    else
    {
        /* For NON VLAN Bridge we use the special VLAN tag of 0. Actually this does not have any
         * meaning. */
        vlan_key = 0;
        user_priority = 0;
    }

    /************************ End of BASIC VALIDATIONS ******************************/

    /* The arguments have been validated. Now lets do the actual work. Create the network device. */
    ptr_net_device = wb_device_initialize(argv[2], ptr_wan_bridge->wb_mode);
    if (ptr_net_device == NULL)
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: Unable to create WAN device %s.\n",(int)argv[2],0,0,0,0);
        return -ENOMEM;
    }

    /* DEBUG Message. */
    wblogMsg (WB_LOG_DEBUG,"DEBUG: Network Device %s Initialized.\n",(int)argv[2],0,0,0,0);

    /* The WAN Network Device has been created. Create the WAN bridge device and attach 
     * the device to the WAN Bridge. */
    ptr_wan_device = (WAN_BRIDGE_DEVICE *)wb_malloc(sizeof(WAN_BRIDGE_DEVICE));
    if (ptr_wan_device == NULL)
    {
        wblogMsg (WB_LOG_FATAL,"ERROR: Unable to allocate memory for the WAN Bridge device.\n",0,0,0,0,0);
        return -ENOMEM;
    }
    memset ((void *)ptr_wan_device, 0, sizeof(WAN_BRIDGE_DEVICE));

    /* Fill up the fields for the WAN BRIDGE Device.  */
    ptr_wan_device->ptr_net_device = ptr_net_device;
    ptr_wan_device->ptr_wan_bridge = ptr_wan_bridge;
    ptr_wan_device->vlan_key       = vlan_key;
    ptr_wan_device->user_priority  = user_priority;

    /* Fill up the fields for the WAN Network device. These are filled here and not in the 
     * 'wb_device_initialize' only because the WAN Bridge device is created after the network
     * device. */
    ptr_private = (WAN_BRIDGE_DEVICE_PRIV *)ptr_net_device->priv;
    ptr_private->ptr_wan_device = ptr_wan_device;

    /* Add the WAN Device to the list of devices attached to the bridge. */
    list_add_tail((struct list_head *)&ptr_wan_device->links,&ptr_wan_bridge->device_list);

    /* The WAN Bridge now moves to the ready state, as there is at least 1 WAN Bridge Device. */
    ptr_wan_bridge->wb_state = WAN_BRIDGE_READY;

    /* Add the VLAN key to the hash bucket only for a VLAN mode bridge. */
    if (ptr_wan_bridge->wb_mode == WAN_BRIDGE_VLAN_MODE)
    {
        if (wb_add (ptr_wan_bridge, ptr_wan_device, ptr_wan_device->vlan_key) < 0)
        {
            /* Error: Unable to add the WAN bridge device to the WAN bridge. This could
             * happen on duplicate VLAN Tags. In this case we need to completely unintialize
             * the device and clean up all the memory. */
            wb_device_uninitialize(ptr_wan_device->ptr_net_device);
            wblogMsg (WB_LOG_DEBUG,"DEBUG: Device %s Uninitialized\n",
                      (int)ptr_wan_device->ptr_net_device->name,0,0,0,0);

            /* Remove the device from the WAN Bridge. */
            list_del ((struct list_head *)&ptr_wan_device->links);

            /* Clean the memory for the WAN Bridge Device. */
            wb_free (ptr_wan_device);

            /* Determine if the WAN Bridge is ready or not. If we deleted the last interface
             * move the bridge to the NOT ready state. */
            if (list_empty(&ptr_wan_bridge->device_list) == 1)
                ptr_wan_bridge->wb_state = WAN_BRIDGE_NOT_READY;

            return -EINVAL;
        }
    }

    /* DEBUG Message. */
    wblogMsg (WB_LOG_DEBUG,"DEBUG: Created WAN Bridge Device 0x%p.\n",(int)ptr_wan_device,0,0,0,0);
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_del_interface
 **************************************************************************
 * DESCRIPTION   :
 * 	Command Handler for deleting a WAN bridge device and deattaching it from
 * 	the appropriate WAN Bridge.
 *
 * RETURNS       :
 *      < 0     - Error.
 *       0      - Success.
 ***************************************************************************/
static int wb_del_interface (char *argv[], int argc)
{
    int                 found = 0;
    WAN_BRIDGE*         ptr_wan_bridge = NULL;
    WAN_BRIDGE_DEVICE*  ptr_wan_device = NULL;
    struct list_head*   ptr_temp;    
 
    /****************************** BASIC VALIDATIONS ***************************/

    /* Validate the number of arguments. */
    if (argc != 3)
    {
        wb_show_help(); 
        return -EINVAL;
    }

    /* ARGUMENT 1: BRIDGE NAME VALIDATION.
     * Cycle through the list of Bridges, to see if there is one with the existing name. */
    list_for_each(ptr_temp, &wan_bridge_list)
    {
        /* Get the WAN Bridge Information and check for the name. */
        ptr_wan_bridge = (WAN_BRIDGE *)ptr_temp;
        if (strcmp(ptr_wan_bridge->name, argv[1]) == 0)
        {
            found = 1;
            break;
        }
    }

    /* Did we get a hit on the bridge name or not ? */
    if ((found == 0) || (ptr_wan_bridge == NULL))
    {
        /* The WAN Bridge name does not exist. */
        wblogMsg (WB_LOG_FATAL,"ERROR: The WAN Bridge %s does NOT exist.\n",(int)argv[1],0,0,0,0);
        return -EINVAL;
    }

    /* ARGUMENT 2: INTERFACE NAME VALIDATION. 
     * Check if the interface is attached to the bridge or not. */
    found = 0;
    list_for_each(ptr_temp, &ptr_wan_bridge->device_list)
    {
        /* Get the WAN device information. Compare the device name for a HIT ? */
        ptr_wan_device = (WAN_BRIDGE_DEVICE *)ptr_temp;
        if (strcmp(ptr_wan_device->ptr_net_device->name, argv[2]) == 0)
        {
            /* Got a match. */
            found = 1;
            break;
        }
    }
        
    /* Was there a hit or not ? */
    if ((found == 0) || (ptr_wan_device == NULL))
    {
        /* The interface name either does not exist or is NOT connected to the bridge. Abort
         * in either case. */
        wblogMsg (WB_LOG_FATAL,"ERROR: Interface %s is not attached to the bridge.\n",(int)argv[2],0,0,0,0);
        return -EINVAL;
    }
    
    /************************ End of BASIC VALIDATIONS ******************************/

    /* Uninitialize the WAN bridge device. */ 
    wb_device_uninitialize(ptr_wan_device->ptr_net_device);
    wblogMsg (WB_LOG_DEBUG,"DEBUG: Device %s Uninitialized\n",(int)argv[2],0,0,0,0);

    /* Delete the VLAN key from the hash bucket. only for a VLAN mode bridge. */
    if (ptr_wan_bridge->wb_mode == WAN_BRIDGE_VLAN_MODE)
    {
        if (wb_delete (ptr_wan_bridge, ptr_wan_device, ptr_wan_device->vlan_key) < 0)
            return -EINVAL;
    }

    /* Remove the device from the WAN Bridge. */
    list_del ((struct list_head *)&ptr_wan_device->links);

    /* Clean the memory for the WAN Bridge Device. */
    wb_free (ptr_wan_device);

    /* Determine if the WAN Bridge is ready or not. If we deleted the last interface
     * move the bridge to the NOT ready state. */
    if (list_empty(&ptr_wan_bridge->device_list) == 1)
        ptr_wan_bridge->wb_state = WAN_BRIDGE_NOT_READY;

    /* DEBUG Message. */
    wblogMsg (WB_LOG_DEBUG,"DEBUG: WAN Bridge Device %s has been deleted.\n",(int)argv[2],0,0,0,0);
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_show_help
 **************************************************************************
 * DESCRIPTION   :
 *  Generic Utility function which shows the help and various commands that
 *  are supported by the WAN Bridge.
 *
 * RETURNS       :
 *  Always returns 0.
 ***************************************************************************/
static void wb_show_help(void)
{
    printk ("Commands.\n");
    printk ("Syntax: echo \"<wan_br_cmd> [args]\" > /proc/avalanche/wan_bridge.\n");
    printk ("Supported wan_br_cmds are as follows :-\n");
    printk (" show                                 - Shows Information about all WAN bridges.\n");
    printk (" addbr <bridge> <iface> <0/1>         - Creates WAN bridge over 'iface_name'.\n");
    printk ("      0- Non VLAN Mode 1-VLAN Mode.\n");
    printk (" delbr <bridge>                       - Deletes the WAN bridge.\n");
    printk (" addif <bridge> <devName> [vlan] [UP] - Creates and adds interface to bridge.\n");
    printk ("     VLAN Tag is required for VLAN mode bridges\n");
    printk ("     and is ommited for Non-VLAN mode bridges\n");
    printk ("     UP - User Priority between 0-7\n");
    printk (" delif <bridge> <dev_name>            - Deletes interface from the WAN bridge.\n");
    printk (" debug <on/off>                       - Enable / Disable Debugging.\n");
    printk (" help                                 - Shows this screen.\n");
    return;
}

/**************************************************************************
 * FUNCTION NAME : wb_get_mode_string
 **************************************************************************
 * DESCRIPTION   :
 *  The function returns the string to be displayed depending on the mode of
 *  the WAN Bridge.
 *
 * RETURNS       :
 *  Pointer to the string.
 ***************************************************************************/
static char *wb_get_mode_string(WAN_BRIDGE_MODE wb_mode)
{
    switch (wb_mode)
    {
       case WAN_BRIDGE_NON_VLAN_MODE:
            return "Non VLAN Mode";
       case WAN_BRIDGE_VLAN_MODE:
            return "VLAN Mode";
       case WAN_BRIDGE_HALF_BRIDGE_MODE:
            return "Half Bridge";
       default:
            return "Error: WAN Bridge Mode is incorrect";
    }
}

/**************************************************************************
 * FUNCTION NAME : wb_debug
 **************************************************************************
 * DESCRIPTION   :
 *  Utility command handler for dynamically changing the debug level for the
 *  WAN Bridge module. 
 *
 * RETURNS       :
 *  Always returns 0.
 ***************************************************************************/
static int wb_debug (char *argv[], int argc)
{
    /* Validate the number of arguments. */
    if (argc != 2)
    {
        wb_show_help(); 
        return -1;
    }

    /* Determine the log level. */
    if ( (strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "ON") == 0) )
    {
        /* Set the log level to DEBUG. */
        wbDbgLevel = WB_LOG_DEBUG;    
    }
    else
    {
        /* Set the log level to FATAL. */
        wbDbgLevel = WB_LOG_FATAL;    
    }
    return 0;
}

/**************************************************************************
 * FUNCTION NAME : wb_malloc
 **************************************************************************
 * DESCRIPTION   :
 * 	Wrapper routine that allocates memory for the WAN Bridge Module.
 *
 * NOTES         :
 *  The flag 'GFP_KERNEL' needs to be verified, because of the problem faced in  
 *  the normal local bridge, where we were unable to grow the heap on memory 
 *  backpressure.
 ***************************************************************************/
void *wb_malloc (int num_bytes)
{
    wb_malloc_counter++; 
    return kmalloc(num_bytes, GFP_KERNEL);
}

/**************************************************************************
 * FUNCTION NAME : wb_free
 **************************************************************************
 * DESCRIPTION   :
 * 	Wrapper routine that frees memory allocated for the WAN Bridge Module.
 ***************************************************************************/
void wb_free (void *ptr)
{
    wb_free_counter++; 
    kfree(ptr);
    return;
}

/**************************************************************************
 * FUNCTION NAME : wblogMsg
 **************************************************************************
 * DESCRIPTION   :
 * 	Logs messages from the WAN Bridge module. 
 ***************************************************************************/
int	wblogMsg 
(
	int 	level, 	/* Debug Level of the message. 	*/
	char* 	fmt, 	/* Format String 				*/
	int 	arg1, 	/* Argument 1 					*/
	int 	arg2, 	/* Argument 2 					*/
	int 	arg3, 	/* Argument 3 					*/
	int 	arg4, 	/* Argument 4 					*/
	int 	arg5	/* Argument 5 					*/
)
{
	/* Check if the message needs to be logged ? */	
	if (level > wbDbgLevel)
		return 0;

	/* YES. The Message DEBUG level is greater than the module debug level. */
	printk (fmt, arg1, arg2, arg3, arg4, arg5, 0);

	return 0;
}

module_init(wb_init)
module_exit(wb_deinit)
