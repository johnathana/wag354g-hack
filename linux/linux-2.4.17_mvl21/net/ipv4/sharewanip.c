#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>

volatile unsigned int half_bridge_enabled = 0; /* half_bridge enabled ? */
volatile unsigned int wan_ip_addr = 0; /* wan_ip_addr  */
volatile unsigned int lan_ip_addr = 0; /* lan_ip_addr  */
volatile unsigned int lan_netmask = 0xffffff00; /* lan_netmask  */
volatile unsigned char lan_mac_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
volatile unsigned char local_lan_mac_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
volatile unsigned int icmp_block = 0; /* block icmp packet when needed */

extern unsigned int my_strtohex(char *str);

int proc_read_enable_half_bridge(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%s (%s)\n", half_bridge_enabled?"Enabled":"Disabled", "use echo \"1(0)\" to enable or disbable");
	return end_proc_read(p, page, off, count, start, eof);

}

int proc_write_enable_half_bridge(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmp[2];
	if(buffer)
	{
		memset(tmp, 0, sizeof(tmp));
		copy_from_user(tmp, buffer, count);
		tmp[1] = 0x00;
		switch(*tmp)
		{
			case '0':
				half_bridge_enabled = 0;
				break;
			case '1':
				half_bridge_enabled = 1;
				break;
			default:
				printk("<1>invalid args\n");
		}
		return count;
	}
	return 0;
}

int proc_read_wan_ip_addr(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%08x\n", wan_ip_addr);
	return end_proc_read(p, page, off, count, start, eof);

}

int proc_write_wan_ip_addr(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmp[9];
	if(buffer)
	{
		memset(tmp, 0, sizeof(tmp));
		copy_from_user(tmp, buffer, count);
		tmp[count] = 0x00;
		printk("tmp:%s\n", tmp);
		wan_ip_addr = my_strtohex(tmp);
		return count;
	}
	return 0;
}

int proc_read_lan_ip_addr(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%08x\n", lan_ip_addr);
	return end_proc_read(p, page, off, count, start, eof);

}

int proc_write_lan_ip_addr(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmp[9];
	if(buffer)
	{
		memset(tmp, 0, sizeof(tmp));
		copy_from_user(tmp, buffer, count);
		tmp[count] = 0x00;
		printk("tmp:%s\n", tmp);
		lan_ip_addr = my_strtohex(tmp);
		return count;
	}
	return 0;
}

int proc_read_lan_ip_netmask(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%08x\n", lan_netmask);
	return end_proc_read(p, page, off, count, start, eof);

}

int proc_write_lan_ip_netmask(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmp[9];
	if(buffer)
	{
		memset(tmp, 0, sizeof(tmp));
		copy_from_user(tmp, buffer, count);
		tmp[count] = 0x00;
		printk("tmp:%s\n", tmp);
		lan_netmask = my_strtohex(tmp);
		return count;
	}
	return 0;
}

int proc_read_lan_mac_addr(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%02x:%02x:%02x:%02x:%02x:%02x\n",
		 lan_mac_addr[0],
		 lan_mac_addr[1],
		 lan_mac_addr[2],
		 lan_mac_addr[3],
		 lan_mac_addr[4],
		 lan_mac_addr[5]
		);
	return end_proc_read(p, page, off, count, start, eof);

}

int proc_write_lan_mac_addr(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmp[18];
	int i = 0, j = 0;
	if(buffer)
	{
		memset(tmp, 0, sizeof(tmp));
		copy_from_user((unsigned char *)tmp, buffer, count);
		tmp[count] = 0x00;
		printk("tmp:%s\n", tmp);
		for(; (i < 6) && (j < 18); i ++, j += 3){
			lan_mac_addr[i] = my_strtohex((unsigned char *)(&tmp[j]));
		}
		return count;
	}
	return 0;
}

int proc_read_local_lan_mac_addr(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%02x:%02x:%02x:%02x:%02x:%02x\n",
		 local_lan_mac_addr[0],
		 local_lan_mac_addr[1],
		 local_lan_mac_addr[2],
		 local_lan_mac_addr[3],
		 local_lan_mac_addr[4],
		 local_lan_mac_addr[5]
		);
	return end_proc_read(p, page, off, count, start, eof);

}

int proc_write_local_lan_mac_addr(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmp[18];
	int i = 0, j = 0;
	if(buffer)
	{
		memset(tmp, 0, sizeof(tmp));
		copy_from_user((unsigned char *)tmp, buffer, count);
		tmp[count] = 0x00;
		printk("tmp:%s\n", tmp);
		for(; (i < 6) && (j < 18); i ++, j += 3){
			local_lan_mac_addr[i] = my_strtohex((unsigned char *)(&tmp[j]));
		}
		return count;
	}
	return 0;
}


