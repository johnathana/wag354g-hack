/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
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
 * PROM library initialisation code.
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>

#include <asm/io.h>
#include <asm/mips-boards/prom.h>
#include <asm/mips-boards/generic.h>
#include <asm/gt64120.h>
#include <asm/mips-boards/malta.h>
#include <asm/avalanche/generic/adam2_env.h>

#ifdef CONFIG_MIPS_AVALANCHE_PSPBOOT
#include <asm/avalanche/generic/env_backcmpt.h>
#endif


int prom_argc;
char **prom_argv, **prom_envp;

int init_debug = 0;

//junzhao 2005.1.4
int env_selection = 1;

//#define JUNZHAO_DEBUG

/* Local copy of the environment variables from Adam2 that are passed to
 * the kernel at startup.
 *
 * TODO: Can we get away with defining this as init data? */
static t_env_var local_envp[MAX_ENV_ENTRY];

/* */
extern int avalanche_mtd_ready;


//junzhao 2005.1.4
/* get an environment variable from the flash region */
char *prom_getenv(char *envname)
{
    if(env_selection)
    {
	t_env_var *env = (t_env_var *)local_envp;
	int i;

	/* check to see if the MTD layer is available */
	if( !avalanche_mtd_ready ) {
		/* use the environment parameters passed into the
		 * kernel at startup */
	i = strlen(envname);
	while(env->name) {
		if(strncmp(envname, env->name, i) == 0) {
			return(env->val);
		}
		env++;
	}
	return(NULL);
	}
	else
	{
		/* use the latest and greatest values
		 * from the flash region */
		return( adam2_env_get_value_direct(envname) );
	}
    }
    else
    {	
	int index;
	for (index = 0; index < sizeof(env_adam2_alias)/sizeof(env_adam2_alias[0]); index++) {
		if (strcmp(env_adam2_alias[index].nm, envname) == 0) {
			return sys_getenv(env_adam2_alias[index].new_nm);		
		}	
	}
	return sys_getenv(envname);
    }
}

/* get an environment variable from the flash region */
#if 0
char *prom_getenv(char *envname)
{
#if defined (CONFIG_MIPS_AVALANCHE_ADAM2_JMP_TBL)
	/*
         * Return a pointer to the given environment variable.
         */

        t_env_var *env = (t_env_var *)prom_envp;
        int i;

        i = strlen(envname);

        while(env->name) {
                if(strncmp(envname, env->name, i) == 0) {
                        return(env->val);
                }
                env++;
        }
        return(NULL);

#elif defined (CONFIG_MIPS_AVALANCHE_ADAM2)
	t_env_var *env = (t_env_var *)local_envp;
	int i;

	/* check to see if the MTD layer is available */
	if( !avalanche_mtd_ready ) {
		/* use the environment parameters passed into the
		 * kernel at startup */
	i = strlen(envname);
	while(env->name) {
		if(strncmp(envname, env->name, i) == 0) {
			return(env->val);
		}
		env++;
	}
	return(NULL);
}
	else
	{
		/* use the latest and greatest values
		 * from the flash region */
		return( adam2_env_get_value_direct(envname) );
	}
#else /* PSP Boot related code. */
	/* 
	 * code for finding out the env var belongs to pre 
	 * defined list or NOT
	 */
	 
	int index;
	for (index = 0; index < sizeof(env_adam2_alias)/sizeof(env_adam2_alias[0]); index++) {
		if (strcmp(env_adam2_alias[index].nm, envname) == 0) {
			return sys_getenv(env_adam2_alias[index].new_nm);		
		}	
	}
	return sys_getenv(envname);
#endif
}
#endif


static inline unsigned char str2hexnum(unsigned char c)
{
	if(c >= '0' && c <= '9')
		return c - '0';
	if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return 0; /* foo */
}

static inline void str2eaddr(unsigned char *ea, unsigned char *str)
{
	int i;

	for(i = 0; i < 6; i++) {
		unsigned char num;

		if((*str == '.') || (*str == ':'))
			str++;
		num = str2hexnum(*str++) << 4;
		num |= (str2hexnum(*str++));
		ea[i] = num;
	}
}

int get_ethernet_addr(char *ethernet_addr)
{
        char *ethaddr_str;

        ethaddr_str = prom_getenv("ethaddr");
	if (!ethaddr_str) {
	        printk("ethaddr not set in boot prom\n");
		return -1;
	}
	str2eaddr(ethernet_addr, ethaddr_str);

	if (init_debug > 1) {
	        int i;
		printk("get_ethernet_addr: ");
	        for (i=0; i<5; i++)
		        printk("%02x:", (unsigned char)*(ethernet_addr+i));
		printk("%02x\n", *(ethernet_addr+i));
	}

	return 0;
}

//junzhao 2005.1.4
#ifdef JUNZHAO_DEBUG
extern unsigned char buff[10];
extern unsigned int myaddress;
extern unsigned int mysize;
extern unsigned int myptr;
#endif

int __init prom_init(int argc, char **argv, char **envp)
{
    env_selection = EnvInit();

    if(env_selection)
    {
	int i;
	t_env_var *env = (t_env_var *)envp;

	/* TODO: Do we need to save the args locally also? */
	prom_argc = argc;
	prom_argv = argv;

	/* Copy what we need locally so we are not dependent on 
	 * bootloader RAM.  In Adam2, the environment paramters
	 * are in flash but the table that references them is in
	 * RAM.   */
	for(i=0; i<MAX_ENV_ENTRY; i++, env++) {
		if( env->name ) {
			local_envp[i].name=env->name;
			local_envp[i].val=env->val;
		} else {
			local_envp[i].name=NULL;
			local_envp[i].val=NULL;
		}
	}
     }
     else
     {
	prom_argc = argc;
	prom_argv = argv;
	prom_envp = envp;
        sys_initenv();
     }
	
     	mips_display_message("LINUX");
	
	/*
	 * Setup the North bridge to do Master byte-lane swapping when
	 * running in bigendian.
	 */
#if defined(CONFIG_MIPS_AVALANCHE_SOC)
        /*nothing*/
#else
#if defined(__MIPSEL__)
	GT_WRITE(GT_PCI0_CMD_OFS, GT_PCI0_CMD_MBYTESWAP_BIT |
		 GT_PCI0_CMD_SBYTESWAP_BIT);
#else
	GT_WRITE(GT_PCI0_CMD_OFS, 0);
#endif /* __MIPSEL__ */

#if defined(CONFIG_MIPS_MALTA)
	set_io_port_base(MALTA_PORT_BASE);
#else
	set_io_port_base(KSEG1);
#endif /* CONFIG_MIPS_MALTA     */
#endif /* CONFIG_MIPS_AVALANCHE_SOC */

#if defined(CONFIG_MIPS_AVALANCHE_SOC)
        set_io_port_base(0);
#endif /* CONFIG_MIPS_AVALANCHE_SOC */

	setup_prom_printf(0);
	prom_printf("\nLINUX started...\n");
	prom_init_cmdline();
	prom_meminit();

#ifdef JUNZHAO_DEBUG
{
	int i;
	prom_printf(".........boot_code %s...........\n", env_selection?"Adam2":"Pspboot");
	for(i=0; i<10; i++)
		prom_printf("..%02x..", buff[i]);
	prom_printf("\n");
	prom_printf("base address: %08x\n", myaddress);
	prom_printf("env size: %d\n", mysize);
	prom_printf("ptr: %08x\n", myptr);
}
#endif	
	
	return 0;
}

#if 0
int __init prom_init(int argc, char **argv, char **envp)
{
#if defined (CONFIG_MIPS_AVALANCHE_ADAM2_JMP_TBL)
	prom_argc = argc;
	prom_argv = argv;
	prom_envp = envp;	
#elif defined (CONFIG_MIPS_AVALANCHE_ADAM2)
	int i;
	t_env_var *env = (t_env_var *)envp;

	/* TODO: Do we need to save the args locally also? */
	prom_argc = argc;
	prom_argv = argv;

	/* Copy what we need locally so we are not dependent on 
	 * bootloader RAM.  In Adam2, the environment paramters
	 * are in flash but the table that references them is in
	 * RAM.   */
	for(i=0; i<MAX_ENV_ENTRY; i++, env++) {
		if( env->name ) {
			local_envp[i].name=env->name;
			local_envp[i].val=env->val;
		} else {
			local_envp[i].name=NULL;
			local_envp[i].val=NULL;
		}
	}
#else
	prom_argc = argc;
	prom_argv = argv;
	prom_envp = envp;
        sys_initenv();
#endif
	mips_display_message("LINUX");

	/*
	 * Setup the North bridge to do Master byte-lane swapping when
	 * running in bigendian.
	 */
#if defined(CONFIG_MIPS_AVALANCHE_SOC)
        /*nothing*/
#else
#if defined(__MIPSEL__)
	GT_WRITE(GT_PCI0_CMD_OFS, GT_PCI0_CMD_MBYTESWAP_BIT |
		 GT_PCI0_CMD_SBYTESWAP_BIT);
#else
	GT_WRITE(GT_PCI0_CMD_OFS, 0);
#endif /* __MIPSEL__ */

#if defined(CONFIG_MIPS_MALTA)
	set_io_port_base(MALTA_PORT_BASE);
#else
	set_io_port_base(KSEG1);
#endif /* CONFIG_MIPS_MALTA     */
#endif /* CONFIG_MIPS_AVALANCHE_SOC */

#if defined(CONFIG_MIPS_AVALANCHE_SOC)
        set_io_port_base(0);
#endif /* CONFIG_MIPS_AVALANCHE_SOC */


	setup_prom_printf(0);
	prom_printf("\nLINUX started...\n");
	prom_init_cmdline();
	prom_meminit();

	return 0;
}
#endif



