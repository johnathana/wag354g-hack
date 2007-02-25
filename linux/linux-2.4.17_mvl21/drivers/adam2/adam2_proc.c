#include <linux/init.h>
#include <linux/module.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ctype.h>
#include <asm/avalanche/generic/adam2_env.h>

#ifdef CONFIG_SYSCTL

#define DEV_ADAM2	    6
#define DEV_ADAM2_ENV	    1

#define ADAM2_ENV_STR_SIZE  80

static char info[ADAM2_ENV_STR_SIZE];

static int adam2_setenv_sysctl(ctl_table *ctl, int write, struct file * filp,
			void *buffer, size_t *lenp)
{
	char *var, *val, *ptr;
        int ret, i;
        size_t total, len;
	

	if (!*lenp || (filp->f_pos && !write))
	{
		*lenp = 0;
		return 0;
	}
        
	if(ctl->ctl_name != DEV_ADAM2_ENV)
        {
                return -ENODEV;
        }

	if(write)
	{
		ret = proc_dostring(ctl, write, filp, buffer, lenp);

                ptr = strpbrk(info, " \t");
                if(!ptr)	
		{
			/* We have no way to distinguish between unsetting a 
			 * variable and setting a non-value variable.
			 * 
			 * Consequently, we will treat any variable
			 * without a value as an unset request.
			 */
			adam2_env_unset_variable(info);
		}
		else
		{
			/* Set the variable-value pair in flash */
			*ptr++ = 0;
			if(adam2_env_set_variable(info, ptr) != 0)
			{
				printk(KERN_NOTICE "Defragging the environment variable region.\n");
				adam2_env_defrag();
				if( adam2_env_set_variable(info, ptr) != 0 )
					printk(KERN_WARNING "Failed to write %s to environment variable region.\n", info);
			}
		}

	}
	else
	{
		total=0;
		len=0;
		/* Scan thru the flash, looking for each possible variable index */
		for(i = 0; i < MAX_ENV_ENTRY; i++)
		{
    			if( (var=adam2_env_get_variable(i))!=NULL )
    			{
				if( (val=adam2_env_get_value(var)) != NULL) {
					len = sprintf(info, "%s\t%s\n", var, val);
					kfree(val);
				}

				kfree(var);

				if(len > *lenp - total)
					len = *lenp - total;
			        
				if(len)
				{
					if(copy_to_user(buffer+total, info, len))
						return -EFAULT;
				}
				else
				{
					break;
				}
				total += len;
			} 
		}
		*lenp = total;
		filp->f_pos += total;
	}
	
        return 0;
}

/* Place files in /proc/sys/dev/adam2 */
ctl_table adam2_env_table[] = {
	{DEV_ADAM2_ENV, "environment", info,
		ADAM2_ENV_STR_SIZE, 0644, NULL, &adam2_setenv_sysctl },
	{0}
	};

ctl_table adam2_table[] = {
	{DEV_ADAM2, "adam2", NULL, 0, 0555, adam2_env_table},
	{0}
	};

/* Make sure that /proc/sys/dev is there */
ctl_table adam2_root_table[] = {
#ifdef CONFIG_PROC_FS
	{CTL_DEV, "dev", NULL, 0, 0555, adam2_table},
#endif /* CONFIG_PROC_FS */
	{0}
	};

static struct ctl_table_header *adam2_sysctl_header;

static void adam2_env_sysctl_register(void)
{
	static int initialized;

	if (initialized == 1)
		return;

	adam2_sysctl_header = register_sysctl_table(adam2_root_table, 1);
	adam2_root_table->child->de->owner = THIS_MODULE;

	/* set the defaults */
	info[0] = 0;

	initialized = 1;
}

static void adam2_env_sysctl_unregister(void)
{
	unregister_sysctl_table(adam2_sysctl_header);
}

static int __init adam2_env_init(void)
{
	adam2_env_sysctl_register();
	printk(KERN_INFO "Adam2 environment variables API installed.\n");
	return 0;
}

static void __exit adam2_env_exit(void)
{
	adam2_env_sysctl_unregister();
}


module_init(adam2_env_init);
module_exit(adam2_env_exit);

#endif /* CONFIG_SYSCTL */
