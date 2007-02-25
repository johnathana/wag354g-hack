/******************************************************************************
 * FILE PURPOSE:    PSP Config Manager Source
 ******************************************************************************
 * FILE NAME:       psp_config_mgr.c
 *
 * DESCRIPTION:
 *
 * Manages configuration information. The repository is managed on the basis of 
 * <key, info> pair. It is possible to have multiple occurrence of the same key. 
 * Multiple occurences of the same keys are referred to as 'instances'. 
 * 'instances' are assigned in the order of configuration arrival. The first 
 * config for a 'key' added to the repository would be treated as instance 0 and
 * next config to arrive for the same key would be treated as instance '1' and 
 * so on.
 * 
 * Info is retrieved from the repository based on the 'key' and 'instance' value.
 *
 * No assumption is made about the format of the information that is put in the 
 * repository. The only requirement is that 'key' should be NULL terminated 
 * string.
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

//#include <stdio.h>
//#include <stdlib.h>
#include "psp_config_mgr.h"
#include "psp_config_util.h"

#include <linux/slab.h>

/*-----------------------------------------------------------
  Implemented elsewhere
 -----------------------------------------------------------*/
extern int sys_read_options_conf(void);
extern int sys_write_options_conf(char *cfg_info);
extern int sys_load_default_static_cfg(void);
extern int sys_run_enumerator(void);

#define os_malloc(size) kmalloc(size, GFP_KERNEL)

/*---------------------------------------------------------
 * Data structures.
 *--------------------------------------------------------*/
struct device_cfg_data;

typedef struct device_instance_cfg_data
{
    struct device_instance_cfg_data       *next;
    char                                  locale[100];
    unsigned int                          data_size;
    char                                  *data;

} DEV_INSTANCE_CFG_DATA_T;

struct device_cfg_collection;

typedef struct device_cfg_collection
{
    struct device_cfg_collection *next;
    char                         *device_name;
    CFG_TYPE_T                   cfg_type;
    int                          count;
    DEV_INSTANCE_CFG_DATA_T      *dev_inst_list_begin;
    DEV_INSTANCE_CFG_DATA_T      *dev_inst_list_end;
} DEVICE_CFG_T;


typedef struct device_cfg_list
{
    DEVICE_CFG_T   *device_cfg_begin;
    int            count;
} DEVICE_CFG_LIST_T;

/*-----------------------------------------------------------------------------
 * Functions used locally with in the file.
 *---------------------------------------------------------------------------*/
static void p_init_device_cfg_list(void);
static int  p_add_instance_cfg_data(DEVICE_CFG_T            *p_dev_cfg,
                                    DEV_INSTANCE_CFG_DATA_T *p_dev_inst_data);
static DEVICE_CFG_T* p_create_dev_cfg(char *device_name);
static DEVICE_CFG_T* p_get_dev_cfg(char *device_name);
static int p_set_device_cfg_type(DEVICE_CFG_T     *p_dev_cfg,
                                 CFG_TYPE_T       cfg_type);

/* PSP Config manager debug */                                 
#define PSP_CFG_MGR_DEBUG 0

#define dbgPrint if (PSP_CFG_MGR_DEBUG) printk

/*-----------------------------------------------------------------------------
 * The repository.
 *---------------------------------------------------------------------------*/
static DEVICE_CFG_LIST_T g_device_cfg_list;

/*---------------------------------------------
 * Initialize the device collection pool.
 *--------------------------------------------*/ 
void p_init_device_cfg_list(void)
{
    g_device_cfg_list.count = 0;
    g_device_cfg_list.device_cfg_begin = NULL;
}

/*----------------------------------------------------------------------
 * Add the device cfg into the device linked list.
 *---------------------------------------------------------------------*/
int p_add_dev_cfg_to_list(DEVICE_CFG_LIST_T *p_dev_list,
                          DEVICE_CFG_T      *p_dev_cfg)
{
    if(p_dev_list->count != 0)
        p_dev_cfg->next            = p_dev_list->device_cfg_begin;

    p_dev_list->device_cfg_begin   = p_dev_cfg;

    p_dev_list->count++;

    return (0);
}

/*------------------------------------------------------------------
 * Add the cfg data into the cfg data linked list of the collection.
 *------------------------------------------------------------------*/
int p_add_instance_cfg_data(DEVICE_CFG_T                 *p_dev_cfg,
                            DEV_INSTANCE_CFG_DATA_T      *p_dev_inst_data)
{
    if(p_dev_cfg->count == 0)
        p_dev_cfg->dev_inst_list_begin     = p_dev_inst_data;
    else
        p_dev_cfg->dev_inst_list_end->next = p_dev_inst_data;
        
    p_dev_cfg->dev_inst_list_end           = p_dev_inst_data;

    p_dev_cfg->count++;
    
    return (0);
}

/*-----------------------------------------------------------------------------
 * Create the device cfg.
 *---------------------------------------------------------------------------*/
DEVICE_CFG_T *p_create_dev_cfg(char *device_name)
{
    DEVICE_CFG_T *p_dev_cfg = NULL;

    if((p_dev_cfg = os_malloc(sizeof(DEVICE_CFG_T))) == NULL)
    {
        dbgPrint("Failed to allocate memory for DEVICE_CFG_T.\n");
    }
    else if((p_dev_cfg->device_name = os_malloc(psp_config_strlen(device_name) + 1))==NULL)
    {
        dbgPrint("Failed to allocate memory for device name.\n");
    }
    else
    {
        psp_config_strcpy(p_dev_cfg->device_name, device_name);
        p_dev_cfg->cfg_type = en_raw;
        p_dev_cfg->count = 0;
        p_dev_cfg->dev_inst_list_begin = NULL;
        p_dev_cfg->dev_inst_list_end   = NULL;
        p_dev_cfg->next                = NULL;
    }

    return(p_dev_cfg);
}

/*------------------------------------------------------------------------------
 * Get the device cfg collection.
 *-----------------------------------------------------------------------------*/
DEVICE_CFG_T *p_get_dev_cfg(char *device_name)
{
    int count               = 0;
    DEVICE_CFG_T *p_dev_cfg = g_device_cfg_list.device_cfg_begin;

    for(count=0; count < g_device_cfg_list.count; count++)
    {
        if(psp_config_strcmp(device_name, p_dev_cfg->device_name) == 0)
        {
            break;
        }

        p_dev_cfg = p_dev_cfg->next;
    }
    
    return(p_dev_cfg); 
}

/*-------------------------------------------------------------------------
 * Gets the name for the static cfg type. Utility function. Debug purposes.
 *-------------------------------------------------------------------------*/
char *p_get_cfg_type_name_for_en(CFG_TYPE_T  cfg_type)
{
    static char raw_str      [] = "still raw";
    static char compile_str  [] = "configured at compile time";
    static char optconf_str  [] = "configured by options.conf";
    static char vlynq_str    [] = "configured by VLYNQ";
    static char no_static_str[] = "no static configuration";

    if(cfg_type == en_raw)
        return (raw_str);
    else if(cfg_type == en_compile)
        return (compile_str);
    else if(cfg_type == en_opt_conf)
        return (optconf_str);
    else if(cfg_type == en_vlynq)
        return (vlynq_str);
    else
        return (no_static_str);

}

/*-----------------------------------------------------------------------------
 * Sets the static cfg status of the device collection.
 *
 * If the collection is en_virgin then, the collection is assigned to cfg_type.
 * If the cfg_type is en_vlynq then, the old cfg_type is retained. 
 * en_compile and en_opt_conf are mutually exclusive. One of these can be 
 * accomodated.
 *
 *---------------------------------------------------------------------------*/
int p_set_device_cfg_type(DEVICE_CFG_T    *p_dev_cfg,
                          CFG_TYPE_T      cfg_type)
{
    int ret_val = 0;

    if(p_dev_cfg->cfg_type == en_raw)
        p_dev_cfg->cfg_type = cfg_type;
    else if((cfg_type == en_vlynq) || (p_dev_cfg->cfg_type == cfg_type))
        ;
    else
    {
        dbgPrint("Device %s has been %s which overrides %s.\n",
                  p_dev_cfg->device_name,
                  p_get_cfg_type_name_for_en(p_dev_cfg->cfg_type),
                  p_get_cfg_type_name_for_en(cfg_type));
        ret_val = -1;
    }

    return(ret_val);
}

/*------------------------------------------------------------------------
 * Add the config str into the repository. The cfg type indicates
 * whether the device has been configured statically, from options.conf or
 * by vlynq enumeration.
 *------------------------------------------------------------------------*/
int psp_config_add(char *key, void *p_cfg_str, unsigned int cfg_len, 
                   CFG_TYPE_T cfg_type)
{
    int                     ret_val           = -1;
    DEVICE_CFG_T            *p_dev_cfg        = NULL;
    DEV_INSTANCE_CFG_DATA_T *p_dev_inst_data  = NULL;  

    if(p_cfg_str == NULL || key == NULL)
    {
        dbgPrint("Null input pointer(s).\n");
    }
    /* check if there exist a dev_cfg for the given key, if not,
       then create one and add it to the device list. */
    else if(((p_dev_cfg = p_get_dev_cfg(key)) == NULL) &&
            (((p_dev_cfg = p_create_dev_cfg(key)) == NULL) ||
               p_add_dev_cfg_to_list(&g_device_cfg_list, p_dev_cfg) != 0))
    {
        dbgPrint("Failed to allocate mem or add dev cfg for %s.\n", key);
    }
    /* make sure that we can add this cfg type to the repository */
    else if(p_set_device_cfg_type(p_dev_cfg, cfg_type) == -1)
    {
        dbgPrint("Ignoring \"%s\" for device \"%s\".\n",
                  p_get_cfg_type_name_for_en(cfg_type),  
                  p_dev_cfg->device_name); 
    }
    else if((p_dev_inst_data = os_malloc(sizeof(DEV_INSTANCE_CFG_DATA_T)))== NULL)
    {
        dbgPrint("Failed to allocate memory for DEV_INSTANCE_CFG_DATA_T.\n");
    }
    else if((p_dev_inst_data->data = os_malloc(cfg_len) + 1) == NULL)
    {
        dbgPrint("Failed to allocate memory for the config data.\n");
    }
    else
    {
        p_dev_inst_data->next = NULL;

        if(cfg_type == en_opt_conf || cfg_type == en_compile)
            psp_config_strcpy(p_dev_inst_data->locale, "dev on chip ");
        else if(cfg_type == en_vlynq)
            psp_config_strcpy(p_dev_inst_data->locale, "dev on vlynq");
        else 
            psp_config_strcpy(p_dev_inst_data->locale, "dev locale ?");

        psp_config_memcpy(p_dev_inst_data->data, p_cfg_str, cfg_len);
        p_dev_inst_data->data_size = cfg_len;
        *(p_dev_inst_data->data + cfg_len) = '\0';

        ret_val = p_add_instance_cfg_data(p_dev_cfg, p_dev_inst_data);
    } 

    return(ret_val);
}

/*-------------------------------------------------------------
 * Get the total number of device instances in the repository
 *------------------------------------------------------------*/
int psp_config_get_num_keys(void)
{
    return(g_device_cfg_list.count);
}


/*--------------------------------------------------------------------
 * Get the device configuration info from the repository.
 *-------------------------------------------------------------------*/
int psp_config_get(char *key, int instance, char **cfg_data_out)
{
    int ret_val             = -1;
    DEVICE_CFG_T *p_dev_cfg = NULL;
    *cfg_data_out           = NULL;

    if(key == NULL && cfg_data_out == NULL)
    {
        dbgPrint("Key has a NULL value.\n");
    }
    else if((p_dev_cfg = p_get_dev_cfg(key)) == NULL)
    {
        dbgPrint("cfg information for %s could not be found.\n", key);
    }
    else if(p_dev_cfg->count)
    {
         DEV_INSTANCE_CFG_DATA_T *p_dev_inst_data = 
                                  p_dev_cfg->dev_inst_list_begin;
         int index = 0;
         for(index = 0; 
             index != instance && index < p_dev_cfg->count; 
             index++)
         {
             p_dev_inst_data = p_dev_inst_data->next;
         }

         if(p_dev_inst_data != NULL && p_dev_inst_data->data != NULL)
         {
             *cfg_data_out = p_dev_inst_data->data;
             ret_val = p_dev_inst_data->data_size;
         }
    }

    return (ret_val);
}

/*----------------------------------------------------------------
 * Returns the number of instances found in the repository for the
 * specified key.
 *---------------------------------------------------------------*/
int psp_config_get_num_instances(char *key)
{
    int ret_val             = 0;
    DEVICE_CFG_T *p_dev_cfg = NULL;

    if(key == NULL)
    {
        dbgPrint("Key has a NULL value.\n");
    }
    else if((p_dev_cfg = p_get_dev_cfg(key)) == NULL)
    {
        dbgPrint("cfg information for %s could not be found.\n", key);
    }
    else
    {
        ret_val = p_dev_cfg->count;
    }
    
    return (ret_val);
}

/*------------------------------------------------------------------
 * Dump the configuration repository. 
 * Caution: DO NOT USE THIS FOR ANY NON NBU specified config format.
 *-----------------------------------------------------------------*/
void psp_config_print(char *key)
{
    DEVICE_CFG_T *p_dev_cfg = NULL;

    if(key == NULL)
    {
        dbgPrint("Key has a NULL value.\n");
    }
    else if((p_dev_cfg = p_get_dev_cfg(key)) == NULL)
    {
        dbgPrint("cfg information for %s could not be found.\n", key);
    }
    else if(p_dev_cfg && p_dev_cfg->count)
    {
        DEV_INSTANCE_CFG_DATA_T   *p_dev_inst_data;

        p_dev_inst_data = p_dev_cfg->dev_inst_list_begin;

        do
        {
            dbgPrint("%s : %s\n", p_dev_inst_data->locale, 
                      p_dev_inst_data->data);
            p_dev_inst_data = p_dev_inst_data->next;

        } while(p_dev_inst_data);
    }
    else
    {
        dbgPrint("Nothing was found for %s.\n", key);
    }
}

void dump_device_cfg_pool(void)
{
    DEVICE_CFG_T *p_dev_cfg = g_device_cfg_list.device_cfg_begin;
    
    if(p_dev_cfg != NULL && g_device_cfg_list.count)
    {
        int index=0;

        for(index=0; index < g_device_cfg_list.count; index++)
        {
            psp_config_print(p_dev_cfg->device_name);
            p_dev_cfg = p_dev_cfg->next;
        }
    }
    else
    {
        dbgPrint("repository is empty.\n");
    }
}

void psp_config_init(void)
{
    p_init_device_cfg_list();
}

void psp_config_cleanup()                                                       
{                                                                               
    int dev_count              = 0;                                             
    int inst_count             = 0;                                             
    DEVICE_CFG_T            *p = g_device_cfg_list.device_cfg_begin;            
    DEV_INSTANCE_CFG_DATA_T *q = NULL;                                          
                                                                                
    for(dev_count = 0; dev_count < g_device_cfg_list.count; dev_count++)        
    {                                                                           
        DEVICE_CFG_T            *p_temp = NULL;                                 
        if(p) q = p->dev_inst_list_begin;                                       
                                                                                
        for(inst_count = 0; inst_count < p->count && q != NULL; inst_count++)   
        {                                                                       
            DEV_INSTANCE_CFG_DATA_T *q_temp = q;                                
            q_temp = q->next;                                                   
            kfree(q->data);                                                     
            kfree(q);                                                           
            q = q_temp;                                                         
        }                                                                       
                                                                                
        p_temp = p->next;                                                       
        kfree(p);                                                               
        p = p_temp;                                                             
    }                                                                           
}                                                                               
