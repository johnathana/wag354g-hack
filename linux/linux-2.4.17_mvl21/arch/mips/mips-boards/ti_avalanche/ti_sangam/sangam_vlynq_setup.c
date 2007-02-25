/*
 * Jeff Harrell, jharrell@ti.com
 * Copyright (C) 2001 Texas Instruments, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Texas Instruments Sangam specific setup.
 */
#include <linux/config.h>
#include <asm/avalanche/generic/pal.h>

#include <asm/avalanche/generic/pal_vlynq.h>
#include <asm/avalanche/generic/pal_vlynqIoctl.h>  

/* WLAN 1130 definition */   
#define VLYNQ_TNETW1X30_MEM_OFFSET 0xC0000000  /* Physical address of ACX111 memory */
#define VLYNQ_TNETW1X30_MEM_SIZE   0x00040000  /* Total size of the ACX111 memory   */
#define VLYNQ_TNETW1X30_REG_OFFSET 0xF0000000  /* PHYS_ADDR of ACX111 control registers   */
#define VLYNQ_TNETW1X30_REG_SIZE   0x00022000  /* Size of ACX111 registers area, MAC+PHY  */
#define TNETW1X30_VL1_REMOTE_SIZE  0x1000000
#define TNETW1X30_TX_MAP           0x00000000   /* Window start address into APEX */
/* WLAN 1350 definition */
#define VLYNQ_TNETW1X50_MEM_OFFSET 0x00000000  /* Physical address of TNETW1150 memory */
#define VLYNQ_TNETW1X50_MEM_SIZE   0x00080000  /* Total size of the TNETW1150 memory   */
#define VLYNQ_TNETW1X50_REG_OFFSET 0x00300000  /* PHYS_ADDR of TNETW1150 control registers   */
#define VLYNQ_TNETW1X50_REG_SIZE   0x00100000  /* Size of TNETW1150 registers area, MAC+PHY  */
#define TNETW1X50_VL1_REMOTE_SIZE  0x01000000
#define TNETW1X50_TX_MAP           0x60000000   /* Window start address into APEX */

void prom_printf(char *fmt, ...);
void vlynq_delay_wait(Uint32 count);
char *prom_getenv(char *name);

#define DEFAULT_VLYNQ_POLARITY 0
#define VLYNQ_POLARITY_LOW     1
#define VLYNQ_POLARITY_HIGH    2

typedef struct
{
    int           id;
    unsigned int  offset;
    unsigned int  size;
    unsigned char remote;

} REGION_CONFIG_T;

typedef struct
{
    char hw_intr_line;
    int  irq;

} IRQ_CONFIG_T;

static REGION_CONFIG_T region_config_1130[] = {

{ 0, VLYNQ_TNETW1X30_MEM_OFFSET, VLYNQ_TNETW1X30_MEM_SIZE, 1},
{ 1, VLYNQ_TNETW1X30_REG_OFFSET, VLYNQ_TNETW1X30_REG_SIZE, 1},
{ 0, AVALANCHE_SDRAM_BASE,    TNETW1X30_VL1_REMOTE_SIZE,0},
{-1, 0, 0, 0}
	                                 };
static REGION_CONFIG_T region_config_1350[] = {

{ 0, VLYNQ_TNETW1X50_MEM_OFFSET, VLYNQ_TNETW1X50_MEM_SIZE, 1},
{ 1, VLYNQ_TNETW1X50_REG_OFFSET, VLYNQ_TNETW1X50_REG_SIZE, 1},
{ 0, AVALANCHE_SDRAM_BASE,       TNETW1X50_VL1_REMOTE_SIZE,0},
{-1, 0, 0, 0}
	                                 };

static IRQ_CONFIG_T irq_config[] = {
                                       { 0, 0},
                                       {-1, 0}
                                   };

// To maintain the reference of vlynq configuration across wlan driver load and 
// unload. Not an ideal solution but still cleanup for this release.
static PAL_VLYNQ_CONFIG_T  vlynq_config;      
static PAL_VLYNQ_DEV_HND   *p_vlynq_dev = NULL;
static PAL_VLYNQ_HND       *p_vlynq     = NULL;
static int is_1130 = TRUE;
static int vlynqPolarityEnv = DEFAULT_VLYNQ_POLARITY;

int avalanche_1130_or_1230_vlynq_teardown(void)
{
    REGION_CONFIG_T *p_region;
    IRQ_CONFIG_T    *p_irq    = &irq_config[0];
    int             ret_val   = 0;
        
	if(is_1130)
	{
		p_region = &region_config_1130[0];
	}
	else
	{
		p_region = &region_config_1350[0];
	}

    while(p_irq->hw_intr_line > -1)
    {
        if(PAL_vlynqUnMapIrq(p_vlynq, p_irq->irq, p_vlynq_dev))
        {
             printk("Failed to clean unmap the irq.\n");
             ret_val = -1;
             break;
        }
    
        p_irq++;
    }

    while(p_region->id > -1)
    {
        if(PAL_vlynqUnMapRegion(p_vlynq, p_region->remote, p_region->id,
			        p_vlynq_dev))
        {
             printk("Failed to unmap the regions.\n");
             ret_val = -1;
             break;
        }
  
        p_region++;
    }

    printk("Preparing for vlynq tearing down....\n");
    if(PAL_vlynqIoctl(p_vlynq, 
                      PAL_VLYNQ_IOCTL_MAJOR_VAL(PAL_VLYNQ_IOCTL_PREP_LINK_DOWN), 0))
    {
        printk("Failed to back up the VLYNQ data.\n");
        ret_val = -1;
    }

    return(ret_val);
}

int avalanche_1130_or_1230_vlynq_irq_cfg(void)
{
    IRQ_CONFIG_T    *p_irq    = &irq_config[0];

    if(PAL_vlynqSetIrqPol(p_vlynq, p_irq->irq, pal_vlynq_high_irq_pol))
        return(-1);

    if(PAL_vlynqSetIrqType(p_vlynq, p_irq->irq, pal_vlynq_level_irq_type))
        return(-1);

    return(0);
}

int avalanche_1130_or_1230_vlynq_re_setup(void)
{
    REGION_CONFIG_T    *p_region;
    IRQ_CONFIG_T       *p_irq    = &irq_config[0];

    vlynq_delay_wait(0xfffff);
	
	if(is_1130)
	{
		p_region = &region_config_1130[0];
	}
	else
	{
		p_region = &region_config_1350[0];
	}
   
    if(!PAL_vlynqGetLinkStatus(p_vlynq))
    {
        printk("The Link to peer is down.\n");
        return (-1);
    }

    printk("Preparing the VLYNQ, now the Link is up. \n");

    if(PAL_vlynqIoctl(p_vlynq, 
                      PAL_VLYNQ_IOCTL_MAJOR_VAL(PAL_VLYNQ_IOCTL_CLEAR_INTERN_ERR), 0))
    {
        printk("Failed to clear the internal VLYNQ errors.\n");
        return (-1);
    }

    if(PAL_vlynqIoctl(p_vlynq,  
                      PAL_VLYNQ_IOCTL_MAJOR_VAL(PAL_VLYNQ_IOCTL_PREP_LINK_UP), 0))
    {
        printk("Failed to restore the VLYNQ. \n");
        return (-1);
    }

    while(p_region->id > -1)
    {
        if(PAL_vlynqMapRegion(p_vlynq, p_region->remote, p_region->id,
				p_region->offset, p_region->size,
				p_vlynq_dev))
	{
            p_region->id = -1;
            printk("Failed to map the regions.\n");
            return (-1);	
	}

	p_region++;
    }

    while(p_irq->hw_intr_line > -1)
    {
        if(PAL_vlynqMapIrq(p_vlynq, p_irq->hw_intr_line, 
			     p_irq->irq, p_vlynq_dev))
	{
	    p_irq->hw_intr_line = -1;
            printk("Failed to map the irq(s).\n");
	    return(-1);
	}

        p_irq++;
    }

    if(avalanche_1130_or_1230_vlynq_irq_cfg())
    {
        printk("Failed to set irq.\n");
        return (-1);
    }

    return(0);
}

static int reset_flag = 1; /* 1: in reset, 0: out of reset */

void avalanche_1130_or_1230_vlynq_init(void)
{
    REGION_CONFIG_T *p_region;
    IRQ_CONFIG_T *p_irq       = &irq_config[0];

    char dev_name[] = "wlan";
    int instance    = 0;
	
	if(is_1130)
	{
		p_region = &region_config_1130[0];
	}
	else
	{
		p_region = &region_config_1350[0];
	}

    vlynq_config.on_soc    = TRUE;
    vlynq_config.base_addr = AVALANCHE_LOW_VLYNQ_CONTROL_BASE;

#if defined(CONFIG_AVALANCHE_VLYNQ_CLK_LOCAL)
    vlynq_config.local_clock_dir    = pal_vlynq_clk_out;
    vlynq_config.local_clock_div    = 0x2;
    vlynq_config.peer_clock_dir     = pal_vlynq_clk_in;
    vlynq_config.peer_clock_div     = 0x1;
#else
    vlynq_config.local_clock_dir    = pal_vlynq_clk_in;
    vlynq_config.peer_clock_dir     = pal_vlynq_clk_out;
	vlynq_config.local_clock_div    = 0x1;
	vlynq_config.peer_clock_div     = 0x1;   
#endif
    vlynq_config.local_intr_local   = 0x1;
    vlynq_config.local_intr_vector  = 31;
    vlynq_config.local_intr_enable  = 1;
    vlynq_config.local_int2cfg      = 1;
    vlynq_config.local_intr_pointer = 0x08612400;
    vlynq_config.local_endianness   = pal_vlynq_little_en;
    vlynq_config.local_tx_addr      = PHYSADDR(AVALANCHE_LOW_VLYNQ_MEM_MAP_BASE); 

    vlynq_config.peer_intr_local    = 0x0;
    vlynq_config.peer_intr_vector   = 30;
    vlynq_config.peer_intr_enable   = 1;
    vlynq_config.peer_int2cfg       = 0;
    vlynq_config.peer_intr_pointer  = 0x14;    
    vlynq_config.peer_endianness    = pal_vlynq_little_en;
	if(is_1130)
		vlynq_config.peer_tx_addr       = TNETW1X30_TX_MAP;
	else
		vlynq_config.peer_tx_addr       = TNETW1X50_TX_MAP;

    p_vlynq = PAL_vlynqInit(&vlynq_config);
    if(!p_vlynq)
    {
        prom_printf("Failed to initialize the vlynq @ 0x%08x.\n", 
                     vlynq_config.base_addr);
        prom_printf("The error msg: %s.\n", vlynq_config.error_msg);
        goto av_vlynq_init_fail;
    }

    p_vlynq_dev = PAL_vlynqDevCreate(p_vlynq, dev_name, instance, 64, 1);
    if(!p_vlynq_dev)
    {
        prom_printf("Failed to create the %s%d reference for vlynq.\n", 
                     dev_name, instance);
	goto av_vlynq_dev_fail;
    }

    if(PAL_vlynqAddDevice(p_vlynq, p_vlynq_dev, 1))
    {
        prom_printf("Failed to add %s%d reference into vlynq.\n", 
                     dev_name, instance);
        goto av_vlynq_add_device_fail;
    }

    while(p_region->id > -1)
    {
        if(PAL_vlynqMapRegion(p_vlynq, p_region->remote, p_region->id,
				p_region->offset, p_region->size,
				p_vlynq_dev))
	{
            p_region->id = -1;
            prom_printf("Failed to map regions for %s%d in vlynq.\n", 
                         dev_name, instance);
            goto av_vlynq_map_region_fail;	
	}

	p_region++;
    }

    while(p_irq->hw_intr_line > -1)
    {
        if(PAL_vlynqMapIrq(p_vlynq, p_irq->hw_intr_line, 
			     p_irq->irq, p_vlynq_dev))
	{
	    p_irq->hw_intr_line = -1;
            prom_printf("Failed to map interrupts for %s%d in vlynq.\n",
                         dev_name, instance);
	    goto av_vlynq_map_irq_fail;
	}

        p_irq++;
    }

    if(avalanche_1130_or_1230_vlynq_irq_cfg())
    {
        prom_printf("Failed to set the irq on peer vlynq.\n");
        goto av_vlynq_set_irq_fail;
    }

    prom_printf("Success in setting up the VLYNQ.\n");

    reset_flag = 0;

    return;

av_vlynq_set_irq_fail:
av_vlynq_map_irq_fail:

    prom_printf("Unmapping the VLYNQ IRQ(s).\n");
    p_irq = &irq_config[0];
    while(p_irq->hw_intr_line > -1)
    {
        PAL_vlynqUnMapIrq(p_vlynq, p_irq->irq, p_vlynq_dev);
        p_irq++;
    }


av_vlynq_map_region_fail:

    prom_printf("Un mapping the VLYNQ regions.\n");
	
	if(is_1130)
	{
		p_region = &region_config_1130[0];
	}
	else
	{
		p_region = &region_config_1350[0];
	}

    while(p_region->id > -1)
    {
        PAL_vlynqUnMapRegion(p_vlynq, p_region->remote, p_region->id,
			     p_vlynq_dev);
 
        p_region++;
    }

    PAL_vlynqRemoveDevice(p_vlynq, p_vlynq_dev);

av_vlynq_add_device_fail:

    prom_printf("Destroying the VLYNQ device.\n");
    PAL_vlynqDevDestroy(p_vlynq_dev);

av_vlynq_dev_fail:

    prom_printf("Cleaning up.......\n");
    if(PAL_vlynqCleanUp(p_vlynq))
        prom_printf("It is not a Clean shut down...\n");

av_vlynq_init_fail:

}

extern REMOTE_VLYNQ_DEV_RESET_CTRL_FN p_remote_vlynq_dev_reset_ctrl;

/* This function is board specific and should be ported for each board. */
void remote_vlynq_dev_reset_ctrl(unsigned int module_reset_bit,
                                 PAL_SYS_RESET_CTRL_T reset_ctrl)
{

    if(module_reset_bit >= 32)
        return;

    switch(module_reset_bit)
    {
        case 0:
        {
            if(OUT_OF_RESET == reset_ctrl)
            {
                /* We do not want to bring 1130 out of reset, if already done so. */ 
                if(reset_flag == 0)
                    return;

				if(vlynqPolarityEnv != DEFAULT_VLYNQ_POLARITY)
				{
					if(vlynqPolarityEnv == VLYNQ_POLARITY_LOW)
						PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);
					else if(vlynqPolarityEnv == VLYNQ_POLARITY_HIGH)
						PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 0);

				}else
				{
				
#if defined(CONFIG_AVALANCHE_VLYNQ_POLARITY_LOW)
                PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);
#else
                PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 0);
#endif
				}
                vlynq_delay_wait(20000);
                printk("Un-resetting the remote device.\n");
                if(avalanche_1130_or_1230_vlynq_re_setup( ))
                    printk("Failed to get the peer vlynq up. Please try cold boot.\n");
                else
                    reset_flag = 0;
            }
            else if(IN_RESET == reset_ctrl)
            {
                /* We do not want to put 1130 in reset, if it is already in reset. */
                if(reset_flag == 1)
                    return;

                if(avalanche_1130_or_1230_vlynq_teardown( ))
                     printk("Failed to bring down the peer vlynq. Please try cold boot.\n");
                else
                     reset_flag = 1;

				if(vlynqPolarityEnv != DEFAULT_VLYNQ_POLARITY)
				{
					if(vlynqPolarityEnv == VLYNQ_POLARITY_LOW)
						PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM,0);
					else if(vlynqPolarityEnv == VLYNQ_POLARITY_HIGH)
						PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);

				}else
				{
#if defined(CONFIG_AVALANCHE_VLYNQ_POLARITY_LOW)
                PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM,0);
#else
                PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);
#endif
				}
                vlynq_delay_wait(20000);
                printk("Resetting the remote device.\n");
            }
            else
                ;
        }
        break;

        default:
        break;

    }
}

void  vlynq_dev_init(void)
{
    volatile unsigned int *reset_base = (unsigned int *) AVALANCHE_RESET_CONTROL_BASE;
    char *env_wlanId = prom_getenv("wlan_id");
    char *env_vlynqPolarity = prom_getenv("vlynq_polarity");
        
	if(env_wlanId)
	{ 
		if(strcmp(env_wlanId,"1x30")==0)
			is_1130 = 1;
		else
		if(strcmp(env_wlanId,"1x50")==0)
			is_1130 = 0;
		else /* Take the default values */
		{
			if(IS_OHIO_CHIP()) 
				is_1130 = 0;  /* The default of Ohio is to work with 1350 */
			else
				is_1130 = 1;  /* The default of Ohio is to work with 1130 */

		}
	}else
	{
		if(IS_OHIO_CHIP()) 
			is_1130 = 0;  /* The default of Ohio is to work with 1350 */
		else
			is_1130 = 1;  /* The default of Ohio is to work with 1130 */
	}

	prom_printf("WLAN %s chip configured.\n", is_1130 ? "1130" : "1350");
    p_remote_vlynq_dev_reset_ctrl = remote_vlynq_dev_reset_ctrl;

    *reset_base &= ~((1 << AVALANCHE_LOW_VLYNQ_RESET_BIT));

    vlynq_delay_wait(20000);

    /* Bring vlynq out of reset if not already done */
    *reset_base |= (1 << AVALANCHE_LOW_VLYNQ_RESET_BIT);
    vlynq_delay_wait(20000); /* Allowing sufficient time to VLYNQ to settle down.*/

    PAL_sysGpioCtrl(VLYNQ0_RESET_GPIO_NUM,GPIO_PIN,GPIO_OUTPUT_PIN);
    vlynq_delay_wait(20000);
	
	/* Check if there is Boot enviornment that indicate the Vlynq polarity */
	if(env_vlynqPolarity)
	{
		if(strcmp(env_vlynqPolarity,"low")==0)
		{
			prom_printf("vlynq_polarity = low\n");
			vlynqPolarityEnv = VLYNQ_POLARITY_LOW;
			PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 0);
			vlynq_delay_wait(50000);
			PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);

		}
		else if(strcmp(env_vlynqPolarity,"high")==0)
		{
			prom_printf("vlynq_polarity = high\n");
			vlynqPolarityEnv = VLYNQ_POLARITY_HIGH;
			PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);
			vlynq_delay_wait(50000);
			PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 0);
		}
		else
		{
			prom_printf("Wrong value of vlynq_polarity setting to Polarity High as default\n");
			vlynqPolarityEnv = DEFAULT_VLYNQ_POLARITY;
			PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);
			vlynq_delay_wait(50000);
			PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 0);
		}

	}else /* Take the default as the image was compiled */
	{
		vlynqPolarityEnv = DEFAULT_VLYNQ_POLARITY;
#if defined(CONFIG_AVALANCHE_VLYNQ_POLARITY_LOW)
	prom_printf("CONFIG_AVALANCHE_VLYNQ_POLARITY_LOW\n");
    PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 0);
    vlynq_delay_wait(50000);
    PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);
#else
	prom_printf("Do not defined CONFIG_AVALANCHE_VLYNQ_POLARITY_LOW\n");
    PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 1);
    vlynq_delay_wait(50000);
    PAL_sysGpioOutBit(VLYNQ0_RESET_GPIO_NUM, 0);
#endif
	}
    vlynq_delay_wait(50000);

    PAL_sysVlynqInit();

    avalanche_1130_or_1230_vlynq_init( );
}

