#include <linux/init.h>
#include <linux/module.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/mtd/mtd.h>
#include <linux/ctype.h>
#include <asm/avalanche/generic/ticfg.h>
#include <asm/avalanche/generic/adam2_env.h>

extern int avalanche_mtd_ready;

extern int env_selection;

/*
 * Get the variable referenced by index
 *
 * NOTE: Caller is responsibe for freeing the memory. 
 */
char *adam2_env_get_variable(int idx)
{
   if(env_selection)
   {
	struct mtd_info *mtd;
	char entry[FLASH_ENV_ENTRY_SIZE];
	char last_entry[FLASH_ENV_ENTRY_SIZE];
	int retval;
	int bytes_to_read;
	int bytes_read;
	char *p_variable=NULL;
	int i,j;

	char *cfe;
	char esum, checksum, control;
	int index, size;
	int s1len, s2len;
	char *s1ptr, *s2ptr;

	int found;

	mtd=get_mtd_device(NULL,ENV_VAR_MTD);
	if(!mtd) {
		printk(KERN_ERR "Failed to access the environment variable region:  mtd%d\n", ENV_VAR_MTD);
		return NULL;
	}

	/* find the last occurrence of the variable-index pair */
	found=FALSE;
	for(i=0; i<MAX_ENV_ENTRY; i++)
	{
		ticfg_lock_region();

		bytes_to_read=sizeof(entry);
		retval=mtd->read(mtd, (i*FLASH_ENV_ENTRY_SIZE), bytes_to_read, &bytes_read, entry);
		if( bytes_read!=bytes_to_read) 
		{
			printk(KERN_ERR "Error while reading mtd%d, read %d out of %d\n", 
			       ENV_VAR_MTD, bytes_read, bytes_to_read);
			break;
		} 

		ticfg_unlock_region();

		/* extrace the header */
		cfe=entry;
		control=*cfe++;
		checksum=*cfe++;
		index=((int)*cfe++)&0x0ff;
		size=((int)*cfe++)&0x0ff;

		if( control==(char)0xff )
			break;		

		if( index==idx && control==0x42 ) {
			/* use the last entry for this variable */
			memcpy(last_entry, entry, sizeof(last_entry));
			found=TRUE;
		}
	}

	/* at this point, we no longer need the MTD */
	put_mtd_device(mtd);

	if(found==FALSE)
		return NULL;
	
	/* extract the header */
	cfe=last_entry;
	control=*cfe++;
	checksum=*cfe++;
	index=((int)*cfe++)&0x0ff;
	size=((int)*cfe++)&0x0ff;


	if(size)
	{
		/* decode and validate the entry */
		if (size>(FLASH_ENV_ENTRY_SIZE-4))
			return NULL;
		
		s1ptr=cfe;
		s1len=strlen(s1ptr);
		if (s1len>(FLASH_ENV_ENTRY_SIZE-5))
			return NULL;
		
		s2ptr=s1ptr+s1len+1;
		s2len=strlen(s2ptr);
		if (s2len>(FLASH_ENV_ENTRY_SIZE-5))
			return NULL;
		
		if ((s2len+s1len)>(FLASH_ENV_ENTRY_SIZE-6)) 
			return NULL;
		
		esum=0;
		for(j=0;j<size;j++) esum+=s1ptr[j];
		if (checksum!=esum) 
			return NULL;
		
		if ((size)&&((size<(s1len+s2len+2))||(size>(s1len+s2len+2+3)))) 
			return NULL;
		
		p_variable=kmalloc(s1len+1, GFP_KERNEL);
		if( p_variable==NULL )
			return p_variable;
		strcpy(p_variable, s1ptr);
	}

	return p_variable;
   }
   else
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
}

/*
 * Get the value associated with an environment variable
 *
 * NOTE: Caller is responsibe for freeing the memory. 
 */
char *adam2_env_get_value(char *var)
{
    if(env_selection)
    {
	struct mtd_info *mtd;
	char entry[FLASH_ENV_ENTRY_SIZE];
	int retval;
	int bytes_to_read;
	int bytes_read;
	char *p_value=NULL;
	int i,j;

	char *cfe;
	char esum, checksum, control;
	int index, size;
	int s1len, s2len;
	char *s1ptr, *s2ptr;
	int variable_index;

	mtd=get_mtd_device(NULL,ENV_VAR_MTD);
	if(!mtd) {
		printk(KERN_ERR "Failed to access the environment variable region:  mtd%d\n", 
		       ENV_VAR_MTD);
		return NULL;
	}

	variable_index=-1;
	for(i=0; i<MAX_ENV_ENTRY;i++)
	{

		ticfg_lock_region();

		bytes_to_read=sizeof(entry);
		retval=mtd->read(mtd, (i*FLASH_ENV_ENTRY_SIZE), bytes_to_read, &bytes_read, entry);
		if( bytes_read!=bytes_to_read) 
		{
			printk(KERN_ERR "Error while reading mtd%d, read %d out of %d\n", 
			       ENV_VAR_MTD, bytes_read, bytes_to_read);
			break;
		}
		
		ticfg_unlock_region();

		/* */
		cfe=entry;

		/* extract the header */
		control=*cfe++;
		checksum=*cfe++;
		index=((int)*cfe++)&0x0ff;
		size=((int)*cfe++)&0x0ff;

		/* validate and decode the entry */
		if (control==(char)0xff) 
			break;
		
		if( control!=0x42 )
			continue;

		if(size)
		{
			if (size>(FLASH_ENV_ENTRY_SIZE-4)) 
				continue;

			s1ptr=cfe;
			s1len=(size)?strlen(s1ptr):0;
			if (s1len>(FLASH_ENV_ENTRY_SIZE-5)) 
				continue;
			
			s2ptr=cfe+s1len+1;
			s2len=(size)?strlen(s2ptr):0;
			if (s2len>(FLASH_ENV_ENTRY_SIZE-5)) 
				continue;
			
			if ((s2len+s1len)>(FLASH_ENV_ENTRY_SIZE-6)) 
				continue;
			
			esum=0;
			for(j=0;j<size;j++) esum+=s1ptr[j];
			if (checksum!=esum) 
				break;
			
			if((size)&&((size<(s1len+s2len+2))||(size>(s1len+s2len+2+3)))) 
				break;
			

			if (strcmp(s1ptr,var)==0)   
			{
				/* variable is set */
				p_value=kmalloc(s2len+1, GFP_KERNEL);
				if( p_value==NULL )
					break;
				strcpy( p_value, s2ptr );
				variable_index=index;
			}
		}
		else
		{
			if( index==variable_index )
			{
				/* variable is unset */
				if( p_value!=NULL )
					kfree(p_value);
				p_value=NULL;
				variable_index=-1;
			}
		}

	}

	put_mtd_device(mtd);

	return p_value;
    }
    else
    {
 	char *p_variable=NULL;
	p_variable=kmalloc(strlen(sys_getenv(var)) + 1, GFP_KERNEL);
	if( p_variable==NULL )
		return p_variable;
	strcpy(p_variable, sys_getenv(var)); 

    	return p_variable;
     }
}

/*
 * This converts an ASCII address entry from the environment variable 
 * region.  It is based on the function with a similar name in the
 * Adam2 code.
 */
static int adam2_env_atox(char *str, unsigned long *num)
{
        unsigned long tmp;

        if (*str++ != '0')
                return -1;

        if (*str++ != 'x')
                return -1;

        tmp=0;
        while(*str) {
		
                if (isxdigit(*str)) {
                        tmp<<=4;
                        if (*str<='9')
                                tmp+=*str-'0';
                        else
                                tmp+=(*str&0x5f)-'A'+10;
                } 
		else if (*str == ',') {
                        break;
                }
                else {
                        return -1;
                }
                str++;
	}
	
        *num=tmp;
        return 0;
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
    if(env_selection)
    {
	struct mtd_info *mtd;
	int retval;
	int bytes_to_read;
	int bytes_read;
	char *p_value=NULL;
	int i,j;

	char *cfe;
	char *entry;
	char esum, checksum, control;
	int index, size;
	int s1len, s2len;
	char *s1ptr, *s2ptr;
	int variable_index;
	
	char env_var_mtdname[FLASH_ENV_ENTRY_SIZE];
	char *pval;
	unsigned long base;

	/* Check to make sure the MTD is up and running so we
	 * can get the base address of the environment variable region */
	if( !avalanche_mtd_ready )
		return NULL;

	sprintf(env_var_mtdname,"mtd%d", ENV_VAR_MTD);
	pval=adam2_env_get_value(env_var_mtdname);
	if(pval==NULL) {
		printk(KERN_ERR "Failed to get base address to %s\n", env_var_mtdname);
		return NULL;
	}

	if( adam2_env_atox(pval,&base) )
		return NULL;

	ticfg_lock_region();
	
	variable_index=-1;
	for(i=0; i<MAX_ENV_ENTRY;i++)
	{
		/* calculate the starting address of the current flash entry */
		cfe=(char *)(base+(i*FLASH_ENV_ENTRY_SIZE));

		/* extract the header */
		control=*cfe++;
		checksum=*cfe++;
		index=((int)*cfe++)&0x0ff;
		size=((int)*cfe++)&0x0ff;

		/* validate and decode the entry */
		if (control==(char)0xff) 
			break;
		
		if( control!=0x42 )
			continue;

		if(size)
		{
			if (size>(FLASH_ENV_ENTRY_SIZE-4)) 
				continue;

			s1ptr=cfe;
			s1len=(size)?strlen(s1ptr):0;
			if (s1len>(FLASH_ENV_ENTRY_SIZE-5)) 
				continue;
			
			s2ptr=cfe+s1len+1;
			s2len=(size)?strlen(s2ptr):0;
			if (s2len>(FLASH_ENV_ENTRY_SIZE-5)) 
				continue;
			
			if ((s2len+s1len)>(FLASH_ENV_ENTRY_SIZE-6)) 
				continue;
			
			esum=0;
			for(j=0;j<size;j++) esum+=s1ptr[j];
			if (checksum!=esum) 
				break;
			
			if((size)&&((size<(s1len+s2len+2))||(size>(s1len+s2len+2+3)))) 
				break;
			

			if (strcmp(s1ptr,var)==0)  {
				/* variable is set */
				p_value=s2ptr;
				variable_index=index;
			}
		}
		else
		{
			if( index==variable_index ) {
				/* variable is unset */
				p_value=NULL;
				variable_index=-1;
			}
		}

	}

	ticfg_unlock_region();

	return p_value;
     }
     else
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
//	int csh;
	//for(csh=1;csh<1000;csh++)printk("---<<<<<<<---------\n");
     if(env_selection)
     {
	struct mtd_info *mtd;
	char entry[FLASH_ENV_ENTRY_SIZE];
	char last_entry[FLASH_ENV_ENTRY_SIZE];
	int bytes_to_read, bytes_read;
	int bytes_to_write, bytes_written;
	int i,j;

	char *cfe;
	char esum, checksum, control;
	int index, size;
	int highest_index;
	int s1len, s2len;
	char *s1ptr, *s2ptr;
	int found;
	int variable_index;
	int empty_entry_idx;

	int ret;
	//for(csh=1;csh<1000;csh++)printk("|||||||||||||||||||\n");

	mtd=get_mtd_device(NULL,ENV_VAR_MTD);
	if(!mtd) {
		printk(KERN_ERR "Failed to access the environment variable region: mtd%d\n", 
		       ENV_VAR_MTD);
		return 1;
	}

	found=0;
	highest_index=0;
	variable_index=-1;
	empty_entry_idx=-1;
	s1ptr=NULL; /* for silencing compiler warnings */
	for(i=0; i<MAX_ENV_ENTRY;i++)
	{

		ticfg_lock_region();

		bytes_to_read=sizeof(entry);
		ret=mtd->read(mtd, (i*FLASH_ENV_ENTRY_SIZE), bytes_to_read, &bytes_read, entry);
		if( bytes_read!=bytes_to_read) 
		{
			printk(KERN_ERR "Error while reading mtd%d, read %d out of %d\n", 
			       ENV_VAR_MTD, bytes_read, bytes_to_read);
			put_mtd_device(mtd);
			return 1;
		}
		
		ticfg_unlock_region();

		/* */
		cfe=entry;

		/* decode the header */
		control=*cfe++;
		checksum=*cfe++;
		index=((int)*cfe++)&0x0ff;
		size=((int)*cfe++)&0x0ff;

		/* validate and decode the entry */
		if( control==(char)0xff ) {
			empty_entry_idx=i;
			break;
		}
		
		/* In general, stop processing at the first sign of a problem
		 * with the environment variable entry.
		 */

		if( control!=0x42 ) {
			put_mtd_device(mtd);
			return 1;
		}

		if( size )
		{
			if ( size>(FLASH_ENV_ENTRY_SIZE-4)) {
				put_mtd_device(mtd);
				return 1;
			}
			
			s1ptr=cfe;
			s1len=strlen(s1ptr);
			if (s1len>(FLASH_ENV_ENTRY_SIZE-5)) {
				put_mtd_device(mtd);
				return 1;
			}
			
			s2ptr=s1ptr+s1len+1;
			s2len=strlen(s2ptr);
			if (s2len>(FLASH_ENV_ENTRY_SIZE-5)) {
				put_mtd_device(mtd);
				return 1;
			}
			
			if ((s2len+s1len)>(FLASH_ENV_ENTRY_SIZE-6)) {
				put_mtd_device(mtd);
				return 1;
			}
			
			esum=0;
			for(j=0;j<size;j++) esum+=s1ptr[j];
			if (checksum!=esum) {
				put_mtd_device(mtd);
				return 1;
			}
			
			if((size)&&((size<(s1len+s2len+2))||(size>(s1len+s2len+2+3))) ) {
				put_mtd_device(mtd);
				return 1;
			}
				
			/* if we have reached here, we have a valid entry on our hands */
		
			if( index>highest_index ) highest_index=index;

			if (strcmp(s1ptr,var)==0) {
				/* variable is set */
				memcpy(last_entry, entry, sizeof(last_entry));
				found=TRUE;
				variable_index=index;
			}
		}
		else
		{
			/* variable is unset */
			if( index==highest_index ) highest_index=index-1;
			if( index==variable_index ) {
				variable_index=-1;
				found=FALSE;
			}
		}

	}

	/* check to see if the value is different */
	if( found==TRUE && val!=NULL )
	{
		/* since we know the entry is valid, we do not need to perform any
		 * checking, we just need to extract the relevant data */
		cfe=last_entry;
		control=*cfe++;
		checksum=*cfe++;
		index=((int)*cfe++)&0x0ff;
		size=((int)*cfe++)&0x0ff;
		s1len=strlen(s1ptr);
		s2ptr=s1ptr+s1len+1;

		variable_index=index;

		if( strcmp(s2ptr,val)==0 )
		{
			/* value is the same, no need to write it */
			put_mtd_device(mtd);
			return 0;
		}
	}


	/* check to see if there is space left on the device */
	if( empty_entry_idx == -1 ) {
		if( val ) {
			put_mtd_device(mtd);
			return 4;
		} else {
			put_mtd_device(mtd);
			return 5;
		}
	}
	
	/* populate the new entry  */
	if( val==NULL )
	{
		/* unset the variable */
		s1len=0;
		s2len=0;
		size=0;
		checksum=0;
		index=(found==TRUE)?variable_index:(highest_index+1);
	}
	else
	{
		/* set the variable */
		s1len=strlen(var);
		s2len=strlen(val);
		size=s1len+s2len+2;
		checksum=0;
		if(val) {
			for(i=0;i<s1len;i++) checksum+=var[i];
			for(i=0;i<s2len;i++) checksum+=val[i];
		} 
		index=(found==TRUE)?variable_index:(highest_index+1);
	}
	
	if( size>(FLASH_ENV_ENTRY_SIZE-4) ) {
		put_mtd_device(mtd);
		return 6;
	}

	/* create the entry */
	cfe=entry;
	*cfe++=0x42;
	*cfe++=checksum;
	*cfe++=index;
	*cfe++=size;
	for(i=0;i<s1len;i++) *cfe++=var[i];
	*cfe++=0;
	for(i=0;i<s2len;i++) *cfe++=val[i];
	*cfe++=0;

	/* update the flash */

	ticfg_lock_region();

	bytes_to_write=sizeof(entry);
	ret=mtd->write(mtd, (empty_entry_idx*FLASH_ENV_ENTRY_SIZE), 
		       bytes_to_write, &bytes_written, entry);

	if( bytes_written!=bytes_to_write )
		printk(KERN_ERR "Error while writing mtd%d, wrote %d out of %d\n", 
		       ENV_VAR_MTD, bytes_written, bytes_to_write);

	ticfg_unlock_region();

	put_mtd_device(mtd);
	//for(csh=1;csh<1000;csh++)printk("UUUUUUUUUUUUUUUUUUU\n");
	return 0;
    }
    else
    {
	//for(csh=1;csh<1000;csh++)printk("TTTTTTTTTTTTTTTTTTTTTTTTTTT\n");
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
}

/*
 * Unset the variable to a specific value.
 */
int adam2_env_unset_variable(char *var)
{
	return adam2_env_set_variable(var,NULL);
}

struct env_s
{
	char var[124];
	char val[124];
	int  valid;
} adam2_env[MAX_ENV_ENTRY];

/*
 * Defrag the block associated with the Adam2 environment variables.
 */
int adam2_env_defrag(void)
{
   if(env_selection)
   {
	int i,j;
	char *var;
	char *val;
	int ret;
	struct mtd_info *mtd;

	
	/* reset the table */
	for(i=0; i<MAX_ENV_ENTRY; i++ )
		adam2_env[i].valid=FALSE;
	
	/* scan for all of possible variable indices */
	for(i=0,j=0; i<MAX_ENV_ENTRY; i++ )
	{
		var=adam2_env_get_variable(i);
		if(var==NULL) {
			continue;
		}

		/* printk(KERN_INFO "Read var %s\n", var); */

		/* 
		 * Make provisions for the special variables that have no value 
		 *
		 * TODO: I thought these had values of "" which are valid...
		 * So why was this check in the Adam2 bootloader?
		 *
		 * NOTE: We have no way of knowing which features are turned on in
		 * the bootloader (i.e. DHCP) so we cannot filter out variables
		 * that are not applicable as does Adam2.
		 */
		val=adam2_env_get_value(var);
		if( (val==NULL) && ( (strcmp(var,"autoload")!=0) && 
				     (strcmp(var,"dhcp")!=0) &&
				     (strcmp(var,"crash")!=0) ) )
		{
			kfree(var);
			continue;
		}

		/* save the information */
		strcpy(adam2_env[j].var,var);
		strcpy(adam2_env[j].val,val);
		adam2_env[j].valid=TRUE;
		j++;

		kfree(var);
		kfree(val);

	}			

	mtd=get_mtd_device(NULL,ENV_VAR_MTD);
	if(!mtd) {
		printk(KERN_ERR "Failed to access the environment variable region:  mtd%d\n", 
		       ENV_VAR_MTD);
		return -EIO;
	}
	
	/* erase the environment varaible region in the mtd */
	ret=ticfg_erase_env_vars(mtd);
	if( ret ) {
		put_mtd_device(mtd);
		return ret;
	}

	put_mtd_device(mtd);

	/* restore the table */
       	for(i=0; i<MAX_ENV_ENTRY; i++ )
	{
		if( adam2_env[i].valid==TRUE )
		{
			adam2_env_set_variable(adam2_env[i].var, adam2_env[i].val);
		}
	}

	return 0;
    }
   else
    	return sys_defragenv(); 
}


