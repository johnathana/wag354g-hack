//#include <linux/init.h>
//#include <linux/module.h>
//#include <linux/sysctl.h>
//#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
//#include <linux/mtd/mtd.h>
#include <linux/ctype.h>
#include "env.h"
//#include <asm/avalanche/ticfg.h>
//#include <asm/avalanche/adam2_env.h>

/*------------------------------------------------------------------------------
 * Set of API to support the ticfg from the NSP on top of PSP Boot library. 
 * ticfg assumes the presence of the ADAM2 boot loader. Since, PSP Boot is 
 * replacing ADAM2, we are required to support the APIs anctipated by the 
 * ticfg, hence this file.
 * 
 *----------------------------------------------------------------------------*/
/*
 * Get the variable referenced by index
 *
 * NOTE: Caller is responsibe for freeing the memory. 
 */
char *adam2_env_get_variable(int idx)
{
    	char *p_variable=NULL;
        if(sys_getivar(idx) == NULL) {
            return NULL;
        }
		p_variable=kmalloc(strlen(sys_getivar(idx)) + 1, GFP_KERNEL);
		if( p_variable==NULL )
			return p_variable;
		strcpy(p_variable, sys_getivar(idx)); 

    	return p_variable;
}

/*
 * Get the value associated with an environment variable
 *
 * NOTE: Caller is responsibe for freeing the memory. 
 */
char *adam2_env_get_value(char *var)
{
        char *p_variable=NULL;
		p_variable=kmalloc(strlen(sys_getenv(var)) + 1, GFP_KERNEL);
		if( p_variable==NULL )
			return p_variable;
		strcpy(p_variable, sys_getenv(var)); 

    	return p_variable;
}

/*
 * Get the value associated with an environment variable
 *
 * This function is a special case for the Adam2 environment
 * variable API as it bypasses the MTD driver layer.  It returns
 * a pointer to the location of the value in flash itself.  This
 * was done to support the prom_getenv() interface.
 */
char *adam2_env_get_value_direct(char *var)
{
   return sys_getenv(var) ;
}


/*
 * Set the variable to a specific value.
 *
 * NOTE: If the value is NULL, the variable will be unset. Otherwise, the
 * variable-value pair will be written to flash.
 */
int adam2_env_set_variable(char *var, char *val)
{
    if (strcmp("CPUFREQ", var) == 0) {
	    printk("Env: CPUFREQ is read-only.\n");
		return 0;       
	}
	if (strcmp("SYSFREQ", var) == 0) {
        printk("Env: SYSFREQ is read-only.\n");
	    return 0;       
	}
    
    if(val == NULL) {
        return sys_unsetenv(var);
    } else {
        return sys_setenv(var, val);
    }
}

/*
 * Unset the variable to a specific value.
 */
int adam2_env_unset_variable(char *var)
{
	return adam2_env_set_variable(var,NULL);
}

/*
 * Defrag the block associated with the Adam2 environment variables.
 */
int adam2_env_defrag(void)
{
   return sys_defragenv(); 
}


