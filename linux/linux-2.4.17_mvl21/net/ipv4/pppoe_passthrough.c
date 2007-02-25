#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
//cjg:2004-5-10:for pppoe_passthrough function
volatile unsigned int bridge_only_enabled = 0; /* bridge only enabled ? */
volatile unsigned int pppoe_pass_enabled = 1;  /* pppoe passthrough enabled ? */

int proc_read_enable_bridge(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%s (%s)\n", bridge_only_enabled?"Enabled":"Disabled", "use echo \"1(0)\" to enable or disbable");
	return end_proc_read(p, page, off, count, start, eof);
}

int proc_write_enable_bridge(struct file *file, const char *buffer, unsigned long count, void *data)
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
				bridge_only_enabled = 0;
				break;
			case '1':
				bridge_only_enabled = 1;
				break;
			default:
				printk("<1>invalid arg\n");
		}
		return count;
	}
	return 0;
}

int proc_read_enable_pppoe_pass(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%s (%s)\n", pppoe_pass_enabled?"Enabled":"Disabled", "use echo \"1(0)\" to enable or disbable");
	return end_proc_read(p, page, off, count, start, eof);

}

int proc_write_enable_pppoe_pass(struct file *file, const char *buffer, unsigned long count, void *data)
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
				pppoe_pass_enabled = 0;
				break;
			case '1':
				pppoe_pass_enabled = 1;
				break;
			default:
				printk("<1>invalid args\n");
		}
		return count;
	}
	return 0;
}



