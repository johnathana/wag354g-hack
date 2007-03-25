
#include <linux/config.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>

#include <linux/config.h>
#include <linux/proc_fs.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/unistd.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/delay.h>
#include <linux/utsname.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <linux/blk.h>
#include <linux/hdreg.h>
#include <linux/iobuf.h>
#include <linux/bootmem.h>
#include <linux/tty.h>

#include <asm/io.h>
#include <asm/bugs.h>

#include <asm/uaccess.h>


extern int proc_read_enable_bridge(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_enable_bridge(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_enable_pppoe_pass(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_enable_pppoe_pass(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_enable_half_bridge(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_enable_half_bridge(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_wan_ip_addr(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_wan_ip_addr(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_lan_ip_addr(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_lan_ip_addr(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_lan_ip_netmask(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_lan_ip_netmask(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_lan_mac_addr(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_lan_mac_addr(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_local_lan_mac_addr(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_local_lan_mac_addr(struct file *file, const char *buffer, unsigned long count, void *data);

/* zhangbin 2005.5.11*/
extern int proc_read_enable_cloud(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_enable_cloud(struct file *file, const char *buffer, unsigned long count, void *data);

#ifdef CONFIG_STB_SUPPORT
extern void stbinfo_init();
extern int proc_read_enable_stb(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_enable_stb(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_stb_network(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_stb_network(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_stb_netmask(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_stb_netmask(struct file *file, const char *buffer, unsigned long count, void *data);
extern int proc_read_stb_info(char *page, char **start, off_t off, int count, int *eof, void *context);
extern int proc_write_stb_info(struct file *file, const char *buffer, unsigned long count, void *data);
#endif

static struct proc_dir_entry *cyber_dir = NULL;

unsigned int my_strtohex(char *str)
{
        unsigned int val = 0;

        while((*str) && (*str != ':'))
        {
                if((*str >= '0') && (*str <= '9'))
                {
                        val = (val << 4) | (*str - '0');
                }
                if((*str >= 'a') && (*str <= 'z'))
                {
                        val = (val << 4) | (*str - 'a' + 10) ;
                }
                if((*str >= 'A') && (*str <= 'Z'))
                {
                        val = (val << 4) | (*str - 'A' + 10);
                }
                str ++;
        }
        return val;
}

int end_proc_read(const char *p, char *page, off_t off, int count, char **start, int *eof)
{
	int len = p - page;
	if(len < off + count)
		*eof = 1;
	*start = page + off;
	len -= off;
	if(len > count)
		len = count;
	if(len < 0)
		len = 0;
	return len;
}

extern unsigned int wl_lan_ipaddr;
extern unsigned int wl_lan_enable;
static int proc_read_wl_management(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	
	p += sprintf(page, "%08x:%x\n", wl_lan_ipaddr, wl_lan_enable);
	return end_proc_read(p, page, off, count, start, eof);
}
static int proc_write_wl_management(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmp[32];
	char ipaddr[20];
	unsigned int ip1, ip2, ip3, ip4;
	
	if (buffer)
	{
		memset((void *)tmp, 0, sizeof(tmp));
		copy_from_user(tmp, buffer, count);
		tmp[count] = 0x00;
		
		sscanf(tmp, "%d.%d.%d.%d:%d", &ip1, &ip2, &ip3, &ip4, &wl_lan_enable);
		wl_lan_ipaddr = ((ip4 & 0xff) << 24) | ((ip3 & 0xff) << 16) | ((ip2 & 0xff) << 8) | (ip1 &0xff);
		return count;
	}
	return 0;
}

extern int env_selection;
void init_cybertan_config()
{
	struct proc_dir_entry *proc;
	struct proc_dir_entry *wl;


	cyber_dir = proc_mkdir("Cybertan", NULL);
	if(cyber_dir)
	{
		wl = create_proc_read_entry("wl_management", S_IFREG | S_IRUGO |S_IWUSR, cyber_dir, proc_read_wl_management, 0);
		if (wl)
		{
			wl->write_proc = proc_write_wl_management;
		}
		//for pppoe_passthrough
		proc = create_proc_read_entry("bridge_only_enable", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_enable_bridge, 0);
		if(proc)
		{
			proc->write_proc = proc_write_enable_bridge;
		}
		proc = create_proc_read_entry("pppoe_pass", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_enable_pppoe_pass, 0);
		if(proc)
		{
			proc->write_proc = proc_write_enable_pppoe_pass;
		}
		//for share wan ip
		proc = create_proc_read_entry("half_bridge_enable", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_enable_half_bridge, 0);
		if(proc)
		{
			proc->write_proc = proc_write_enable_half_bridge;
		}
		proc = create_proc_read_entry("wan_ip_addr", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_wan_ip_addr, 0);
		if(proc)
		{
			proc->write_proc = proc_write_wan_ip_addr;
		}
		/*virtual lan ip address for half bridge function*/
		proc = create_proc_read_entry("lan_ip_addr", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_lan_ip_addr, 0);
		if(proc)
		{
			proc->write_proc = proc_write_lan_ip_addr;
		}
		proc = create_proc_read_entry("lan_mac_addr", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_lan_mac_addr, 0);
		if(proc)
		{
			proc->write_proc = proc_write_lan_mac_addr;
		}
		proc = create_proc_read_entry("local_lan_mac_addr", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_local_lan_mac_addr, 0);
		if(proc)
		{
			proc->write_proc = proc_write_local_lan_mac_addr;
		}
		proc = create_proc_read_entry("lan_netmask", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_lan_ip_netmask, 0);
		if(proc)
		{
			proc->write_proc = proc_write_lan_ip_netmask;
		}
		/*
		proc8 = create_proc_read_entry("icmp_block", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_enable_icmp_block, 0);
		if(proc8)
		{
			proc8->write_proc = proc_write_enable_icmp_block;
		}
		*/

		/* zhangbin 2005.5.11 for cloud */
		proc = create_proc_read_entry("cloud_enable", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_enable_cloud, 0);
		if(proc)
		{
			proc->write_proc = proc_write_enable_cloud;
		}

   
	//for stb support			
	#ifdef CONFIG_STB_SUPPORT
		stbinfo_init();
                proc = create_proc_read_entry("stb_enabled", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_enable_stb, 0);
	        if(proc)
	         {
	           proc->write_proc = proc_write_enable_stb;
	         }
	         proc = create_proc_read_entry("stb_network", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_stb_network, 0);
	       if(proc)
	        {
	           proc->write_proc = proc_write_stb_network;
	        }
	        proc = create_proc_read_entry("stb_netmask", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_stb_netmask, 0);
	        if(proc)
	        {
	           proc->write_proc = proc_write_stb_netmask;
	        }
	        proc = create_proc_read_entry("stb_info", S_IFREG | S_IRUGO | S_IWUSR, cyber_dir, proc_read_stb_info, 0);
                if(proc)
                {
                        proc->write_proc = proc_write_stb_info;
                }
	#endif
	}

	//junzhao 2005.1.13 for boot_type
	if(env_selection)
		_nvram_set("boot_type", "adam2");
	else
		_nvram_set("boot_type", "pspboot");
}

