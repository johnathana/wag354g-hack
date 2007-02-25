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
 * Texas Instruments generic avalanche setup.
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mc146818rtc.h>
#include <linux/ioport.h>

#include <asm/cpu.h>
#include <asm/mipsregs.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>

#include <linux/proc_fs.h>

#include <asm/uaccess.h>
#include <asm/avalanche/avalanche_map.h>
#include <asm/avalanche/generic/adam2_env.h>

#if defined(CONFIG_MIPS_AVALANCHE_TIMER16)
#include <asm/avalanche/generic/hal_modules/timer16.h>
#endif 


#if defined(CONFIG_MIPS_AVALANCHE_WDTIMER)
#include <asm/avalanche/generic/hal_modules/wdtimer.h>
#endif 

extern void avalanche_soc_platform_init(void);
struct proc_dir_entry *avalanche_proc_root;

#if defined(CONFIG_SERIAL_CONSOLE) || defined(CONFIG_PROM_CONSOLE)
//extern void console_setup(char *, int *);
extern int console_setup(char *);
char serial_console[20];
#endif

#ifdef CONFIG_REMOTE_DEBUG
extern void rs_kgdb_hook(int);
//extern void saa9730_kgdb_hook(void);
extern void evm3_kgdb_hook(void);
extern void breakpoint(void);
int remote_debug = 0;
#endif

#if defined(CONFIG_MIPS_EVM3)
extern struct rtc_ops evm3_rtc_ops;
#else
extern struct rtc_ops no_rtc_ops;
#endif /* CONFIG_MIPS_EVM3 */

extern void mips_reboot_setup(void);
extern unsigned int cpu_freq;
static struct proc_dir_entry *avalanche_proc_env_dir = NULL;
extern void sangam_reset_init(void);

#if defined (CONFIG_MIPS_SANGAM)
#define PSP_NAME    "DSL-PSP"
#define PSP_VERSION "4.3.2.7 on BasePSP Version 5.7.6.12 "
#else
#define PSP_NAME    "BasePSP"
#define PSP_VERSION "5.7.6.12"
#endif

static int avalanche_p_read_base_psp_version(char* buf, char **start, off_t offset, 
                                             int count, int *eof, void *data)
{
    int len   = 0;
    int limit = count - 80;
    char *cache_mode[4] = {"cached, write through, no write allocate", \
                           "cached, write through, write allocate", \
                           "uncached", \
                           "cached, write back, write allocate"};

    int cache_index = read_32bit_cp0_register(CP0_CONFIG) & 0x3;

    if(len<=limit)
        len+= sprintf(buf+len, "\nLinux OS %s version %s %s %s \nAvalanche SOC Version: 0x%x operating in %s mode\nCpu Frequency:%u MHZ\nSystem Bus frequency: %u MHZ\n\n", PSP_NAME, PSP_VERSION, __DATE__, __TIME__, avalanche_get_chip_version_info(), cache_mode[cache_index], cpu_freq/1000000, 2*avalanche_get_vbus_freq()/1000000);
    
    return (len);
}

#if !defined (CONFIG_MIPS_AVALANCHE_ADAM2_JMP_TBL) && !defined (CONFIG_MIPS_AVALANCHE_TICFG)
static int avalanche_p_proc_env_read(char *page, char **start, off_t pos, int count,
                                     int *eof, void *data)
{
    int len = 0, i, limit=count-80;
    char *pvar, *pval;

    if( pos > 0 ) 
    {
        len=0;
        return 0;
    }

    /* Scan thru the flash, looking for each possible variable index */
    for(i=0; i< MAX_ENV_ENTRY; i++) 
    {
        if(len > limit) 
            break;

        if( (pvar=adam2_env_get_variable(i))!=NULL ) 
        {
            if( (pval=adam2_env_get_value(pvar)) != NULL) 
            {
                len += sprintf(page+len, "%s\t%s\n", pvar, pval);
                kfree(pval);
            }
            kfree(pvar);
        }
    }

    *eof=1;
    return len;
}

#define MAX_ENV_DATA_LEN (FLASH_ENV_ENTRY_SIZE)
static int avalanche_p_proc_env_write(struct file *file, const char *buffer, 
                                      unsigned long count, void *data)
{
    char envdata[MAX_ENV_DATA_LEN];
    char *pvar, *pval;

    /* TODO: Check for count > len */
    if( copy_from_user(envdata, buffer, count) ) 
    {
        return -EFAULT;
    }

    /* extract the variable/value pair */
    if( count )
        envdata[count-1]='\0';
    else
        envdata[count]='\0';
        
    pvar=envdata;
    pval=strpbrk(envdata," \t");
    if(pval) *pval++='\0';

    if(pval) 
    {
        /* write the variable/value pair to flash */
        if(adam2_env_set_variable(pvar, pval) != 0)
        {
            printk(KERN_WARNING "Defragginig environment variable region.\n");
            adam2_env_defrag();
            if( adam2_env_set_variable(pvar, pval) != 0 )
                printk(KERN_ERR "Failed to write %s to environment variable region.\n", pvar);
        }
    } 
    else 
    {
        /* We have no way to distinguish between unsetting a
         * variable and setting a non-value variable.
         *
         * Consequently, we will treat any variable
         * without a value as an unset request.
         */
        adam2_env_unset_variable(pvar);
    }   

    return count;
}
#endif

int avalanche_proc_init(void)
{
    avalanche_proc_root = proc_mkdir("avalanche",NULL);
    if(!avalanche_proc_root)
        return -ENOMEM;

#if defined (CONFIG_MIPS_SANGAM)
    create_proc_read_entry("avalanche/psp_version", 0, NULL, avalanche_p_read_base_psp_version, NULL);
#else
    create_proc_read_entry("avalanche/base_psp_version", 0, NULL, avalanche_p_read_base_psp_version, NULL);
#endif

#if !defined (CONFIG_MIPS_AVALANCHE_ADAM2_JMP_TBL) && !defined (CONFIG_MIPS_AVALANCHE_TICFG)
    avalanche_proc_env_dir=create_proc_entry("avalanche/env", 0644, NULL);
        
    if(avalanche_proc_env_dir==NULL )
    {
        printk(KERN_ERR "Unable to create /proc/%s/%s entry\n", "avalanche", "env");
        return -ENOMEM;
    }
        
    avalanche_proc_env_dir->read_proc =avalanche_p_proc_env_read;
    avalanche_proc_env_dir->write_proc=avalanche_p_proc_env_write;
#endif

#if defined(CONFIG_MIPS_SANGAM)
   /* create proc entry for reset to  factory defaults */  
   sangam_reset_init();                                    

#endif

    return (0);
}

void __init ti_avalanche_setup(void)
{
#ifdef CONFIG_REMOTE_DEBUG
        int rs_putDebugChar(char);
        char rs_getDebugChar(void);
        //      int saa9730_putDebugChar(char);
        //      char saa9730_getDebugChar(void);
        int evm3_putDebugChar(char);
        int evm3_getDebugChar(void);
        extern int (*putDebugChar)(char);
        extern char (*getDebugChar)(void);
#endif
        char *argptr;
        static char *nfs_ptr;
        static char *ip_ptr;

        // JAH_TBD
        //      ioport_resource.end = 0x7fffffff;

#ifdef CONFIG_SERIAL_CONSOLE
        argptr = prom_getcmdline();
        if ((argptr = strstr(argptr, "console=ttyS0")) == NULL) {
                int i = 0;
                char *s = prom_getenv("modetty0");
                while(s[i] >= '0' && s[i] <= '9')
                        i++;
                strcpy(serial_console, "ttyS0,");
                strncpy(serial_console + 6, s, i);
                prom_printf("Config serial console: %s\n", serial_console);
                console_setup(serial_console);
        }
#endif  /* CONFIG_SERIAL_CONSOLE */

#ifdef CONFIG_REMOTE_DEBUG
        argptr = prom_getcmdline();
        if ((argptr = strstr(argptr, "kgdb=ttyS")) != NULL) {
                int line;
                argptr += strlen("kgdb=ttyS");
                if (*argptr != '0' && *argptr != '1')
                        printk("KGDB: Uknown serial line /dev/ttyS%c, "
                               "falling back to /dev/ttyS1\n", *argptr);
                line = *argptr == '0' ? 0 : 1;
                printk("KGDB: Using serial line /dev/ttyS%d for session\n",
                       line ? 1 : 0);

                rs_kgdb_hook(line);
                putDebugChar = rs_putDebugChar;
                getDebugChar = rs_getDebugChar;

                prom_printf("KGDB: Using serial line /dev/ttyS%d for session, "
                            "please connect your debugger\n", line ? 1 : 0);

                remote_debug = 1;
                /* Breakpoints and stuff are in atlas_irq_setup() */
        }
#endif
        argptr = prom_getcmdline();

        if ((argptr = strstr(argptr, "nofpu")) != NULL)
                mips_cpu.options &= ~MIPS_CPU_FPU;


        /*
         * Add nfsroot and IP enviroment variables to the cmdline.
         * these are stored in the ADAM2 enviroment space in the
         * following manner:
         *
         *  for nfsroot:
         *    nfsroot=[<server-ip>:]<root-dir>[,<nfs-options>]
         *
         *
         *  for ipaddress
         *    ip=<client-ip>:<server-ip>:<gw-ip>:<netmask>:<hostname>:<device>:<autoconf>
         *
         *  Note that if the Adam2 enviroment contains nfs set to "yes" the
         *  following code will also add the root=/dev/nfs automatically.
         *
         *    (see Documentation/nfsroot.txt for more information)
         */

        argptr = prom_getcmdline();

        nfs_ptr = prom_getenv("nfs");
        if(nfs_ptr)
          {
            if(!strncmp(nfs_ptr,"yes",3))
              {
                strcat(argptr,"root=/dev/nfs ");
                argptr = prom_getcmdline();
                nfs_ptr = prom_getenv("nfsroot");
                strcat(argptr,nfs_ptr);
                strcat(argptr," ");
              }
          }

        ip_ptr = prom_getenv("linuxip");
        argptr = prom_getcmdline();

        if(ip_ptr)
          {
          strcat(argptr,ip_ptr);
          }

        avalanche_soc_platform_init();

#if defined(CONFIG_MIPS_EVM3)
        rtc_ops = &evm3_rtc_ops;
#else
        rtc_ops = &no_rtc_ops;
#endif /* CONFIG_MIPS_EVM3 */

        mips_reboot_setup();
}


#if defined(CONFIG_MIPS_AVALANCHE_TIMER16)
EXPORT_SYMBOL(timer16_set_params);    
EXPORT_SYMBOL(timer16_get_freq_range);
EXPORT_SYMBOL(timer16_ctrl);
#endif 


#if defined(CONFIG_MIPS_AVALANCHE_WDTIMER)
EXPORT_SYMBOL(wdtimer_init);      
EXPORT_SYMBOL(wdtimer_set_period);
EXPORT_SYMBOL(wdtimer_ctrl);      
EXPORT_SYMBOL(wdtimer_kick);      
#endif 
