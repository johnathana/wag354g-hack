#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <stb.h>

struct stb_dev_info stb_devinfo[MAX_STB_NUM];
volatile unsigned int stb_enabled = 0;  /* stb support enabled ? */
volatile unsigned int stb_devnum = 0;  /* stb support num ? */
volatile unsigned int stb_network = 0;  /* stb server net */
volatile unsigned int stb_netmask = 0;  /* stb server netmask */

extern unsigned int my_strtohex(char *str);

void stbinfo_init()
{
	int i;
	stb_enabled = 0;
	stb_devnum = 0;
	stb_network = 0;
	stb_netmask = 0;
	for(i=0; i<MAX_STB_NUM; i++)
		memset(&stb_devinfo[i], 0, sizeof(stb_devinfo[i]));
	return;
}

struct stb_dev_info *find_stbinfo(unsigned int type, unsigned int ipaddr, unsigned char *macaddr, struct stb_dev_info **freestb)
{
	struct stb_dev_info *tmp;
	unsigned char macvalue[6];
	int i, firstfree = 0;
	
	my_printk("enter find_stbinfo: type %x ipaddr %x\n", type, ipaddr);
	if(macaddr)
	{
		for(i=0; i<6; i++)
			memcpy(&macvalue[i], macaddr+i, sizeof(unsigned char));
	
		my_printk("enter find_stbinfo: type %x ipaddr %x mac %02x:%02x:%02x:%02x:%02x:%02x\n", type, ipaddr, macvalue[0], macvalue[1], macvalue[2], macvalue[3], macvalue[4], macvalue[5]);
	}

	if((type & 2) && (!macaddr))
		return NULL;
		
	for(i=0; i<MAX_STB_NUM; i++)
	{
		tmp = &stb_devinfo[i];
		if(!tmp->used)
		{
			if(!firstfree && (freestb))
			{
				firstfree = 1;
				(*freestb) = tmp;
			}
			continue;
		}
		if((type & 1) && (ipaddr != tmp->stb_ipaddr)) //find ip
			continue;
		if(type & 2)
		{
			if((macvalue[0] != tmp->stb_macaddr[0]) || 
			   (macvalue[1] != tmp->stb_macaddr[1]) || 
			   (macvalue[2] != tmp->stb_macaddr[2]) || 
			   (macvalue[3] != tmp->stb_macaddr[3]) || 
			   (macvalue[4] != tmp->stb_macaddr[4]) || 
			   (macvalue[5] != tmp->stb_macaddr[5]))
				continue;	
		}			
		//my_printk("enter find_stbinfo2: %x\n", i);
		return tmp;
	}	
	return NULL;
}

int proc_read_enable_stb(char *page, char **start, off_t off, int count, int *eof, void *context)
{
        char *p = page;
        p += sprintf(page, "%s (%s)\n", stb_enabled?"Enabled":"Disabled", "use echo \"1(0)\" to enable or disbable");
        return end_proc_read(p, page, off, count, start, eof);
}
int proc_write_enable_stb(struct file *file, const char *buffer, unsigned long count, void *data)
{
	        unsigned char tmp[2];
	        if(buffer)
	        {
	          memset(tmp, 0, sizeof(tmp));
	          copy_from_user(tmp, buffer, count);
	          tmp[1] = 0x00;
	          switch(*buffer)
	           {
	            case '0':
	              stb_enabled = 0;
	              break;
	            case '1':
	              stb_enabled = 1;
	              break;
	            default:
	              printk("<1>invalid arg\n");
	          }
	          return count;
	       }
	      return 0;
}				

int proc_read_stb_network(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	        char *p = page;
                p += sprintf(page, "%08x\n", stb_network);
	       return end_proc_read(p, page, off, count, start, eof);
}

int proc_write_stb_network(struct file *file, const char *buffer, unsigned long count, void *data)
{
	        unsigned char tmp[9];
	        if(buffer)
	         {
	           memset(tmp, 0, sizeof(tmp));
	           copy_from_user(tmp, buffer, count);
	           tmp[count] = 0x00;
	           my_printk("tmp:%s\n", tmp);
	           stb_network = my_strtohex(tmp);
	           return count;
	        }
	       return 0;
}

int proc_read_stb_netmask(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	        char *p = page;
 	        p += sprintf(page, "%08x\n", stb_netmask);
	        return end_proc_read(p, page, off, count, start, eof);
}

int proc_write_stb_netmask(struct file *file, const char *buffer, unsigned long count, void *data)
{
	        unsigned char tmp[9];
	        if(buffer)
  	        {
                   memset(tmp, 0, sizeof(tmp));
                   copy_from_user(tmp, buffer, count);
                   tmp[count] = 0x00;
                   my_printk("tmp:%s\n", tmp);
                   stb_netmask = my_strtohex(tmp);
                   return count;
               }
              return 0;
}

static int clear_stb_info()
{
	int i;
	stb_devnum = 0;
	for(i=0; i<MAX_STB_NUM;i++)
		memset(&stb_devinfo[i],0,sizeof(stb_devinfo[i]));
	return 0;
}

static int modify_stb_info(char *op, char *ipaddr, char *netmask, char *macaddr)
{
        unsigned int iptmp = my_strtohex(ipaddr);
        unsigned int netmasktmp = my_strtohex(netmask);
        unsigned char mactmp[6];
        int i, j;

        if(!strcmp(macaddr, "00:00:00:00:00:00"))
	                return -1;

        for(i=0, j=0; (i<6) && (j<18); i++, j+=3)
              mactmp[i] = my_strtohex((unsigned char *)(&macaddr[j]));
        if(*op == '1') //for add
	   {
                struct stb_dev_info *tmp, *freetmp;
                if(stb_devnum >= MAX_STB_NUM)
	           {
		        printk("stb dev full!\n");
		        return -1;
		   }
                if(tmp = find_stbinfo(MACONLY, 0, &mactmp[0], &freetmp))
	           {
		        printk("stb dev has exist!\n");
		        return -1;
		   }
                else
	           {
				
                       freetmp->used = 1;
		       freetmp->stb_ipaddr = iptmp;
		       freetmp->stb_netmask = netmasktmp;
		       for(j=0; j<6; j++){
		         freetmp->stb_macaddr[j] = mactmp[j];}
		       stb_devnum++;
		       my_printk("ok, added!\n");
		   }
        }
        else if(*op == '0') //for del
        {
			
                struct stb_dev_info *tmp;
                if(stb_devnum == 0)
                {
                   printk("stb table empty!\n");
                   return -1;
                }
                if(tmp = find_stbinfo(MACONLY, 0, &mactmp[0], NULL))
                {
                   printk("stb dev has exist, del it!\n");
                   memset(tmp, 0, sizeof(*tmp));
		   stb_devnum--;
                }
        }
        return 0;
}

int proc_read_stb_info(char *page, char **start, off_t off, int count, int *eof, void *context)
{
        char *p = page;
        if(!stb_devnum)
	      p += sprintf(page, "There is not any stb dev\n");
        else
        {
                int i;
                for(i=0; i<MAX_STB_NUM; i++)
                {
                        if(stb_devinfo[i].used)
	                                p += sprintf(p, "%08x\t%08x\t%02x:%02x:%02x:%02x:%02x:%02x\n", stb_devinfo[i].stb_ipaddr, stb_devinfo[i].stb_netmask, stb_devinfo[i].stb_macaddr[0], stb_devinfo[i].stb_macaddr[1], stb_devinfo[i].stb_macaddr[2], stb_devinfo[i].stb_macaddr[3], stb_devinfo[i].stb_macaddr[4], stb_devinfo[i].stb_macaddr[5]);
                }
        }
        my_printk("buffer %s\n", page);
        return end_proc_read(p, page, off, count, start, eof);
}

int proc_write_stb_info(struct file *file, const char *buffer, unsigned long count, void *data)
{
	 unsigned char tmp[38];
	 if(buffer)
	   {
                char op[2], ipaddr[9], netmask[9], macaddr[18];
                memset(tmp, 0, sizeof(tmp));
                memset(op, 0, sizeof(op));
                memset(ipaddr, 0, sizeof(ipaddr));
                memset(netmask, 0, sizeof(netmask));
                memset(macaddr, 0, sizeof(macaddr));
                copy_from_user(tmp, buffer, count);
                tmp[count] = 0x00;
                my_printk("tmp:%s\n", tmp);
		//1015 by junzhao
		if(!strcmp(tmp,"clear"))
			clear_stb_info();
		else
		{
                         sscanf(tmp, "%s %s %s %s", op, ipaddr, netmask, macaddr);
                         modify_stb_info(op, ipaddr, netmask, macaddr);
		}
	         return count;
        }
        return 0;
}

