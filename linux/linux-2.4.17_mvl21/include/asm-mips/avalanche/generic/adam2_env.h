/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*   Copyright (C) 1996-2003 by Texas Instruments, Inc.  All rights reserved.  */
/*   Copyright (C) 2001-2003 Telogy Networks.	    						   */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
#ifndef _INCLUDE_ADAM2_ENV_H_
#define	_INCLUDE_ADAM2_ENV_H_

/******************************************************************************* 
 *The structure for each entry in the environment variable region is 
 * as follows:
 *
 * control    1:   Can be either 0x42 (entry) or 0xff (empty)
 * checksum   1: 
 * index      1:   Unique number associated with a variable-value pair
 * size       1:   Size of the payload (x+1+y+1)
 * variable   x+1: ASCII string 
 * value      y+1: ASCII string
 *
 * The last entry for a given value is the most current value.
 *
 * A set variable will contain both a variable and a value, even if the value is ""
 *
 * An unset variable will contain only the header (e.g. For a variable that is 
 * in the region with an index=0x11, [0x42 0x00 0x11 0x00] )
 ******************************************************************************/

/* Note that these definitions must match with the definitions in ADAM2. */
/* These definitions allow for a 10kB area for saving the environment variables */
#define FLASH_ENV_ENTRY_SIZE	        128 
#define MAX_ENV_ENTRY	                80  /* max # of Adam2 environment variables */

#define MAX_ENV_ERRORS                  16   /* max error msgs before we abort the action */

typedef struct
{
	char *name;
	char *val;
}t_env_var;

extern char *adam2_env_get_value(char *var);
extern char *adam2_env_get_variable(int idx);
extern int adam2_env_set_variable(char *var, char *val);
extern int adam2_env_unset_variable(char *var);
extern int adam2_env_defrag(void);

extern char *adam2_env_get_value_direct(char *var);

#endif /* _INCLUDE_ADAM2_ENV_H_ */

