/******************************************************************************
 * FILE PURPOSE:    PSP Config Manager - Configuration Build Source
 ******************************************************************************
 * FILE NAME:       psp_config_build.c
 *
 * DESCRIPTION:     Configuration Build API Implementation
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifdef INCLUDE_FFS
#include "ffs.h"
#endif /* INCLUDE_FFS */

#include "psp_config_mgr.h"
#include "psp_config_build.h"
#include "psp_config_util.h"

#define MAX_DEVICE_NAME_LEN     16
#define MAX_DEVICE_STR_LEN      512

#ifndef NULL
#define NULL (char *)0
#endif

#include <asm/avalanche/avalanche_map.h>
#include <linux/slab.h>
#include <linux/config.h>



#define os_malloc(size) kmalloc(size, GFP_KERNEL)

int psp_run_enumerator(void)
{
    return(0);
}

#if defined (CONFIG_AVALANCHE_CPMAC_AUTO)

static int auto_detect_cpmac_phy(void)
{

#define SELECT_INT_PHY_MAC 0
#define SELECT_EXT_PHY_MAC 1

    volatile unsigned long *reset_cntl = AVALANCHE_RESET_CONTROL_BASE, *mdio_cntl  = ((int)AVALANCHE_MDIO_BASE + 0x4);
    unsigned int j= 0, detected_phy_map = 0, auto_select = SELECT_INT_PHY_MAC;
                                                                                                                   
    *reset_cntl |= (1 << AVALANCHE_MDIO_RESET_BIT) | (1 << AVALANCHE_LOW_CPMAC_RESET_BIT) | (1 << AVALANCHE_HIGH_CPMAC_RESET_BIT) | (1 << AVALANCHE_LOW_EPHY_RESET_BIT);                                 
    *mdio_cntl   = (1 << 30) | (avalanche_get_vbus_freq()/2200000);                                              
                                                                                                                   
    for(j=0;j < 300000; j++) 
    { 
        if(j%100000) continue;

        detected_phy_map = *(mdio_cntl + 1);
        if(detected_phy_map) 
        {
            detected_phy_map &= ~AVALANCHE_LOW_CPMAC_PHY_MASK;
                
            if(detected_phy_map && !(detected_phy_map & (detected_phy_map - 1)))
            {
                auto_select = SELECT_EXT_PHY_MAC;
                break;
            }
        } 
    }

    return(auto_select);

}

#endif


#ifndef AVALANCHE_LOW_CPMAC_MDIX_MASK
#define AVALANCHE_LOW_CPMAC_MDIX_MASK 0
#endif

void psp_load_default_static_cfg(void)
{
    char s2[100],  s3[100];
    char s4[2000], s6[2000];
    int threshold = 20;
    char *tx_threshold_ptr = prom_getenv("threshold");
    
    if(tx_threshold_ptr)
      threshold = simple_strtol(tx_threshold_ptr, (char **)NULL, 10);

    /* Static configuration if options.conf not present */
    sprintf(s3,"cpmdio(id=mii, base=%u, reset_bit=%d)", AVALANCHE_MDIO_BASE, 22);
    sprintf(s2, "reset( id=[ResetRegister], base=%u)", AVALANCHE_RESET_CONTROL_BASE);

    sprintf(s4, "cpmac(id=[cpmac], unit=0, base=%u, size=0x800, reset_bit=%d, PhyMask=%u, MdixMask=%u, MLink=0, int_line=%d, memory_offset=0, RX_CAF=1, RX_PASSCRC=0, RX_CEF=1, RX_BCAST=0, RX_BCASTCH=0, Ch0=[TxNumBuffers=256, TxNumQueues=1, TxServiceMax=%d, RxNumBuffers=256, RxBufferOffset=0, RxBufSize=1000, RxServiceMax=128], Ch1=[TxNumBuffers=256, TxNumQueues=1, TxServiceMax=%d, RxNumBuffers=256, RxBufferOffset=0, RxBufSize=1000, RxServiceMax=128], Ch2=[TxNumBuffers=256, TxNumQueues=1, TxServiceMax=%d, RxNumBuffers=256, RxBufferOffset=0, RxBufSize=1000, RxServiceMax=128])", AVALANCHE_LOW_CPMAC_BASE, AVALANCHE_LOW_CPMAC_RESET_BIT, AVALANCHE_LOW_CPMAC_PHY_MASK, AVALANCHE_LOW_CPMAC_MDIX_MASK, AVALANCHE_LOW_CPMAC_INT,threshold,threshold,threshold);

    sprintf(s6, "cpmac(id=[cpmac], unit=1, base=%u, size=0x800, reset_bit=%d, PhyMask=%u, MLink=0, int_line=%d, memory_offset=0, RX_CAF=1, RX_PASSCRC=0, RX_CEF=1, RX_BCAST=0, RX_BCASTCH=0, Ch0=[TxNumBuffers=256, TxNumQueues=1, TxServiceMax=%d, RxNumBuffers=256, RxBufferOffset=0, RxBufSize=1000, RxServiceMax=128], Ch1=[TxNumBuffers=256, TxNumQueues=1, TxServiceMax=%d, RxNumBuffers=256, RxBufferOffset=0, RxBufSize=1000, RxServiceMax=128], Ch2=[TxNumBuffers=256, TxNumQueues=1, TxServiceMax=%d, RxNumBuffers=256, RxBufferOffset=0, RxBufSize=1000, RxServiceMax=128])", AVALANCHE_HIGH_CPMAC_BASE, AVALANCHE_HIGH_CPMAC_RESET_BIT, AVALANCHE_HIGH_CPMAC_PHY_MASK, AVALANCHE_HIGH_CPMAC_INT,threshold,threshold,threshold);

    psp_config_add("reset", s2, psp_config_strlen(s2), en_compile);


#if defined (CONFIG_AVALANCHE_LOW_CPMAC)

    psp_config_add("cpmdio", s3, psp_config_strlen(s3), en_compile);
    psp_config_add("cpmac", s4, psp_config_strlen(s4), en_compile);

#endif


#if defined (CONFIG_AVALANCHE_HIGH_CPMAC)

    psp_config_add("cpmdio", s3, psp_config_strlen(s3), en_compile);
    psp_config_add("cpmac", s6, psp_config_strlen(s6), en_compile);

#endif

#if defined (CONFIG_AVALANCHE_CPMAC_AUTO)
    { 
        char *phy_sel_ptr = prom_getenv("mac_phy_sel"); 
        int   phy_sel = SELECT_EXT_PHY_MAC;
        char *mac_port = prom_getenv("MAC_PORT"); /* Internal: 0, External: 1 */

        if(phy_sel_ptr && (0 == strcmp(phy_sel_ptr, "int")))
        {
            phy_sel = SELECT_INT_PHY_MAC;
        }

        //if(phy_sel == auto_detect_cpmac_phy())
        if(!mac_port || (0 != strcmp(mac_port, "0")))
        {
            printk("Using the MAC with external PHY\n");
            psp_config_add("cpmdio", s3, psp_config_strlen(s3), en_compile);
            psp_config_add("cpmac", s6, psp_config_strlen(s6), en_compile);
        }
        else
        {
            printk("Using the MAC with internal PHY\n");
            psp_config_add("cpmdio", s3, psp_config_strlen(s3), en_compile);
            psp_config_add("cpmac", s4, psp_config_strlen(s4), en_compile);
        }
    }
                      
#endif    

}

char* psp_conf_read_file(char *p_file_name)
{
#ifdef INCLUDE_FFS
    
    char        *p_file_data = NULL;
    unsigned int file_size;
    FFS_FILE    *p_file = NULL;

    if(p_file_name == NULL)
    {
        return (NULL);
    }

    if(!(p_file = ffs_fopen(p_file_name, "r"))) 
    {
        return(NULL);
    }

    file_size = p_file->_AvailableBytes;

    p_file_data = os_malloc(file_size + 1);
    
    if(ffs_fread(p_file_data, file_size, 1, p_file) == 0)
    {
        kfree(p_file_data);
        return(NULL);
    }

    ffs_fclose(p_file);
   
    p_file_data[file_size] = '\0';

    return(p_file_data);
    
#else /* NO FFS */
    return(NULL);
#endif /* INCLUDE_FFS */
}

int psp_conf_get_line(char *p_in_data, char **next_line)
{
    char *p       = p_in_data;

    while(*p && *p++ != '\n')
    {

    } 
   
    *next_line = p;

    return(p - 1 - p_in_data);
}


int psp_conf_is_data_line(char *line)
{
    int ret_val = 1;

    if(*line == '\0' || *line == '\n' || *line == '#')
        ret_val = 0;

    return(ret_val);
}

int psp_conf_get_key_size(char *data)
{
    char *p = data;

    while(*p && *p != '\n' && *p != '(' && *p != ' ')
        p++;

    return(p - data);
}

char* psp_conf_eat_white_spaces(char *p)
{
    while(*p && *p != '\n' && *p == ' ')
        p++;

    return (p);
}

int psp_build_from_opt_conf(void)
{
    char *data       = NULL;
    char *data_hold  = NULL;
    char *next_line  = NULL; 
    int  line_size   = 0;

    if((data = psp_conf_read_file("/etc/options.conf")) == NULL)
        return(-1);

    data_hold = data;

    while((line_size=psp_conf_get_line(data, &next_line)) != -1)
    {

        char *name = NULL;
        int  name_size; 
 
    data = psp_conf_eat_white_spaces(data);

        if(psp_conf_is_data_line(data))
        {
            data[line_size] = '\0';
 
            name_size = psp_conf_get_key_size(data);
           
            if(name_size > 0) 
            {
                name = (char *) os_malloc(name_size + 1);
                if(name == NULL) break;

                psp_config_memcpy(name, data, name_size);
                name[name_size] = '\0';

                psp_config_add(name, data, line_size, en_opt_conf);
            
                kfree(name);
            }

            data[line_size] = '\n';
        }

        data = next_line;
    }

    kfree(data_hold);
    return (0);
}


int psp_write_conf_file(char *p_write_file, char * dev_cfg_string)
{
#ifdef INCLUDE_FFS
    int bytes_written=0;
        FFS_FILE *file_ptr=NULL;    
 
    /*
     * NOTE: In current implementation of FFS in ADAM2 if the file exists beforehand, it
     * can't be opened for write.
     */
        if(!(file_ptr=ffs_fopen(p_write_file, "w"))) {
        return(-1);
        }
    
    /* Write into the file "output.con" the character string */
    /* write a \n before a writing a line */
    if(!(bytes_written = ffs_fwrite("\n", 1, sizeof(char), file_ptr))) {
        return (-1);
    }
    
    if(!(bytes_written = ffs_fwrite(dev_cfg_string, psp_config_strlen(dev_cfg_string), sizeof(char), file_ptr))) {
        return (-1);        
    }
    ffs_fclose(file_ptr);
    return (bytes_written+1);
#else /* NO FFS */
    return(-1);
#endif /* INCLUDE_FFS */
}

void build_psp_config(void)
{

    /* initialize the repository. */
    psp_config_init();
    
#ifdef INCLUDE_FFS
    ffs_init();
#endif /* INCLUDE_FFS */

    /* read the configuration from the options.conf to override default ones */
    psp_build_from_opt_conf();

    /* read the configuration which were not over ridden in options.conf */
    psp_load_default_static_cfg();

    /* let the vlynq be enumerated. Enumerator will add cfg info
       of the discovered device instances to the repository.*/
    psp_run_enumerator();

    /* dump the repository*/
    dump_device_cfg_pool();

}

