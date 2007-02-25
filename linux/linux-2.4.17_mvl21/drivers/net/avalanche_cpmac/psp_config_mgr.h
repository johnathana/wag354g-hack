/******************************************************************************
 * FILE PURPOSE:    PSP Config Manager Header
 ******************************************************************************
 * FILE NAME:       psp_config_mgr.h
 *
 * DESCRIPTION:     Storing and retrieving the configuration based on key
 * A set of APIs to be used by one and sundry (including drivers and enumerator) to build
 * and read cfg information of the devices for an avalanche SOC.
 *
 * This set of APIs isolates the configuration management from the world and provides simple
 * access convinience.
 *
 * Device in this set refers to the peripherals that can be found on the SOC or on VLYNQ.
 * The configuration is stored in the form of string and drivers can use these APIs to get
 * a particular parameter value.
 *
 * The memory allocation for the pass back parameters is done by the caller.
 *
 * 0 is returned for SUCCESS or TRUE.
 *  -1 is returned for FAILURE or FALSE.
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/
 
#ifndef __PSP_CONFIG_MGR_H__
#define __PSP_CONFIG_MGR_H__

typedef enum cfg_type
{
    en_raw     = 0,
    en_compile,
    en_opt_conf,
    en_vlynq
} CFG_TYPE_T;

/* Build psp configuration */
void build_psp_config(void);
 
/********************************************************
 * Access Operations.
 ********************************************************/

/*------------------------------------------------------------------------- 
   initializes the configuration repository. 
 -------------------------------------------------------------------------*/
void  psp_config_init(void);

/*-------------------------------------------------------------------------- 
   Adds the configuration information into the repository. 'key' is required 
   to be NULL terminated string. 'cfg_ptr' points to the configuration data.
   'cfg_len' is the length of the data pointed to by 'cfg_ptr' in bytes. 
   'cfg_type' indicates the type of config information.

   psp_config_mgr copies the 'cfg_len' bytes of data pointed to by 'cfg_ptr' 
   into its internal repository. 

   Returns: 0 on success, -1 on failure.
  -------------------------------------------------------------------------*/
int   psp_config_add(char *key, void *cfg_ptr, 
                     unsigned int cfg_len, CFG_TYPE_T cfg_type);


/* --------------------------------------------------------------------------
   Passes back, in "*cfg_out_val" a pointer to the config data in the repository
   for the specified 'key' and 'instance'. It returns the size of the config
   info

   psp_config_mgr passes back a pointer in '*cfg_out_val' which refers to 
   some location in its internal repository. It is strongly recommended that 
   if the user intends to modify the contents of the config info for reasons 
   whatsoever, then, user should allocate memory of size returned by this 
   routine and copy the contents from '*cfg_out_val'.
  
   Any, modification carried out on the repository would lead to un-expected
   results.

   Returns: 0 or more for the size of config info, -1 on error.
  --------------------------------------------------------------------------*/ 
int   psp_config_get(char *key, int instance, char **cfg_out_val);


/*--------------------------------------------------------------------------
   Get the number of keys that have been added in the repository so far.

   Returns: 0 or more for the num of keys, -1 on error.
  -------------------------------------------------------------------------*/
int   psp_config_get_num_keys(void);


/*--------------------------------------------------------------------------
   Get the number of instances that are present in the repository for the 
   given 'key'.

   Returns: 0 or more for the num of instances, -1 on error.
  -------------------------------------------------------------------------*/  
int   psp_config_get_num_instances(char *key);


/*--------------------------------------------------------------------------
   Prints the config data for all instances associated with the specified 
   'key'.
  -------------------------------------------------------------------------*/
void  psp_config_print(char *key);

void  dump_device_cfg_pool(void);

#endif /* __PSP_CONFIG_MGR_H__ */
