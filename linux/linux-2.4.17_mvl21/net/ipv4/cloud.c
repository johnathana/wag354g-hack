#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>

volatile unsigned int cloud_enabled = 0; /* cloud enabled ? */

int proc_read_enable_cloud(char *page, char **start, off_t off, int count, int *eof, void *context)
{
	char *p = page;
	p += sprintf(page, "%s (%s)\n", cloud_enabled?"Enabled":"Disabled", "use echo \"1(0)\" to enable or disbable");
	return end_proc_read(p, page, off, count, start, eof);

}

int proc_write_enable_cloud(struct file *file, const char *buffer, unsigned long count, void *data)
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
				cloud_enabled = 0;
				break;
			case '1':
				cloud_enabled = 1;
				break;
			default:
				printk("<1>invalid args\n");
		}
		return count;
	}
	return 0;
}


