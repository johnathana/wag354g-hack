#define __KERNEL_SYSCALLS__

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

#if defined(CONFIG_ARCH_S390)
#include <asm/s390mach.h>
#include <asm/ccwcache.h>
#endif

#ifdef CONFIG_PCI
#include <linux/pci.h>
#endif

#ifdef CONFIG_DIO
#include <linux/dio.h>
#endif

#ifdef CONFIG_ZORRO
#include <linux/zorro.h>
#endif

#ifdef CONFIG_MTRR
#  include <asm/mtrr.h>
#endif

#ifdef CONFIG_NUBUS
#include <linux/nubus.h>
#endif

#ifdef CONFIG_ISAPNP
#include <linux/isapnp.h>
#endif

#ifdef CONFIG_X86_LOCAL_APIC
#include <asm/smp.h>
#endif

#if defined(CONFIG_MIPS_AVALANCHE_ADAM2) || defined (CONFIG_MIPS_AVALANCHE_PSPBOOT)
#include <asm/avalanche/generic/adam2_env.h> 
#endif

#include <asm/uaccess.h>
#include <sysexits.h>
#include <linux/in.h>

#define SMTP_BUFF_SIZE 1024

__u32 SMTP_SERVER_IP;
//volatile static unsigned char mail_buf[SMTP_BUFF_SIZE];
static DECLARE_WAIT_QUEUE_HEAD(smtp_wq);
volatile char *email_for_log;
volatile char *email_for_return;

#define TRACE_BUFF_SIZE 1024
/*static*/volatile unsigned char trace_buf[TRACE_BUFF_SIZE];
volatile int e_w_pos = 0;
volatile int g_email_thresholds = 20;
void email_send(int fd, char *subject, char *content);

void k_mail_d(void);
static char *g_subject, *g_content;
static unsigned int run_pos = 0;
extern unsigned char email2log;
extern unsigned char email2ret;
extern volatile int email_alert_enable = 0; /* 0:disable; 1:enable */

static int new_atoi(char *str)
{
	int val = 0;
	for(;;str ++)
	{
		switch(*str)
		{
			case '0'...'9':
				val = 10 * val + (*str - '0');
				break;
			default:
				return val;
		}
	}
}

static int net_putline(int fd, char *line)
{
	ssize_t error_code;
	size_t count;

	count = strlen(line);
	error_code = sys_write(fd, line, count);
	if(error_code < 0)
	{
	//	printk("write error [write() ]\n");
		return 1;
	}
	else if((size_t)error_code == count)
	{
		return 0;
	}
	else
	{
		printk("write error [write(): unknown error]\n");
		return 1;
	}
}

static int net_getchar(int fd, char *c)
{
	ssize_t error_code;
	if((error_code = read(fd, c, (size_t)1)) == 1)
		return 0;
	else if(error_code == 0)
		return -1;
	else
	{
		printk("read error [read()]\n");
		return 1;
	}
}

static int net_getline(int fd, char *line, int size)
{
	char c;
	int i;
	int error_code;

	i = 0;
	while((error_code = net_getchar(fd, &c)) == 0)
	{
		if(i == size - 1)
		{
			line[i] = '\0';
			return 2;
		}
		else if(c == '\n')
			break;
		else
			line[i ++] = c;
	}
	if(c == '\n' && line[i - 1] == '\r')
	{
		line[i - 1] = '\0';
	}
	line[i] = '\0';

	if(error_code  == 0 || error_code == -1)
		return 0;
	else
		return 1;
}

static int smtp_get_full_response(int fd, char *buffer)
{
	int error_code;
	error_code = net_getline(fd, buffer, 1024);

	if(error_code != 0)
	{
		if(error_code == 1)
			printk("cannot get a response from SMTP server\n");
		else
			printk("smtp server sent a line longer than %d"
				"characters(this violates RFC 2821)", 1024 - 3);
		return -1;
	}
	return new_atoi(buffer);
}

static int smtp_putline(int fd, char *format, ...)
{
	va_list args;
	char tmp[1024];
	int error_code;
	int i = 0;
	
	va_start(args, format);
	vsprintf(tmp, format, args);
	va_end(args);
	i = strlen(tmp);
	if(i == 1024)
		i = 1021;
	tmp[i] = '\r';
	tmp[i+1] = '\n';
	tmp[i+2] = '\0';
	error_code = net_putline(fd, tmp);
	if(error_code != 0)
	{
		printk("can not write to smtp server\n");
		error_code = 1;
	}
	return error_code;

}

static int smtp_server_msg_continues(char *buffer)
{
	return (buffer[0] && buffer[1] && buffer[2] && buffer[3] == '-');
}

static int smtp_get_response(int fd, int expected_status)
{
	int smtp_status;
	char buf[1024];
	int error = 0;

	if((smtp_status = smtp_get_full_response(fd, buf)) == -1)
	{
		return -1;
	}
	while(smtp_server_msg_continues(buf))
	{
		if((smtp_status = smtp_get_full_response(fd, buf)) == -1)
			return -1;
	}
	return smtp_status;
}

static int smtp_ehlo(int fd)
{
	char buffer[1024];
	if(smtp_putline(fd, "EHLO localhost") != 0)
	{
		return -1;
	}
	for(;;)
	{
		if(smtp_get_full_response(fd, buffer) == -1)
			return -1;
		if(strncmp(buffer, "250", 3) != 0 || !buffer[3])
			printk("stmp error: %s\n", buffer);
		else if(!smtp_server_msg_continues(buffer))
		{
			break;
		}
	}
	return 0;
}

static void send_to_server(int fd, char *subject, char *content)
{
//	unsigned char tmp[100];
//	unsigned int ret = 0;
//	unsigned char c;
	int error_code ;
	
	if((error_code = smtp_ehlo(fd)) != 0)
	{
		goto close_exit;
	}
	if(smtp_putline(fd, "MAIL FROM:<%s>", email_for_return) != 0)
	{
		error_code = -1;
		printk("MAIL FROM send error\n");
		goto close_exit;
	}
	if(smtp_get_response(fd, 250) != 250)
	{
		printk("MAIL FROM:<%s> not accepted by SMTP server\n", email_for_return);
		error_code = -2;
		goto close_exit;
	}
	if(smtp_putline(fd, "RCPT TO:<%s>", email_for_log) != 0)
	{
		error_code = -1;
		printk("RCPT TO send error\n");
		goto close_exit;
	}
	if(smtp_get_response(fd, 250) != 250)
	{
		printk("RCPT TO:<%s> not accepted by SMTP server\n", email_for_log);
		error_code = -2;
		goto close_exit;
	}
	if(smtp_putline(fd, "DATA") != 0)
	{
		error_code = -1;
		printk("DATA error\n");
		goto close_exit;
	}

	if(smtp_get_response(fd, 354) != 354)
	{
		error_code = -2;
		printk("SMTP server does not accept the mail");
		goto close_exit;
	}
	smtp_putline(fd, "From:		\"%s\"<%s>", "test", email_for_return);
	smtp_putline(fd, "To:		%s", email_for_log);

	smtp_putline(fd, "Subject: %s", subject);
	/*--message body--*/
//	smtp_putline(fd,"Flood IN=eth1 OUT= MAC=00:90:4C:21:00:2B:00:A0:C9:A4:F8:BA:08:00 SRC=192.168.66.21 LEN=60 TOS=0x10 PREC=0x00 TTL=64 ID=38958 DF PROTO=TCP SPT=39264 SEQ=883597505 ACK=0 WINDOW=5840 RES=0x00 SYN URGP=0 OPT");
	smtp_putline(fd, trace_buf);
#if 0
	{
		int size = strlen(content);
		int i = 0;
		for(; i < size; i += 1021)
		{
			smtp_putline(fd, "%s\\0", content + i);
		}
	}
#endif
	if(smtp_putline(fd, ".") != 0)
	{
		error_code = -2;
		goto close_exit;
	}
	if(smtp_get_response(fd, 250) != 250)
	{
		error_code = -2;
		printk("end of data error\n");
		goto close_exit;
	}
	smtp_putline(fd, "QUIT");
	smtp_get_response(fd, 221);
close_exit:	
	return;
}

static int jisu = 0;
void email_send(int fd, char *subject, char *content)
{
	int error_code;
	char buf[30];
	jisu ++;
	if((error_code = smtp_ehlo(fd)) != EX_OK)
	{
		goto close_exit;
	}	
	memset(buf, 0, 30);
	run_pos = 5;
#if 0
	sprintf(buf,"MAIL FROM:<%s>\r\n", email_for_return);
	if(smtp_putline(fd, buf) != 0)
	{
		error_code = EX_IOERR;
		goto close_exit;
	}
	if(smtp_get_response(fd, 250) != 250) /*250*/
	{
		error_code = EX_DATAERR;
		goto close_exit;
	}
	memset(buf, 0, 30);	
	run_pos = 6;
	sprintf(buf, "RCPT TO:<%s>\r\n", email_for_log);
	if(smtp_putline(fd, buf) != 0)
	{
		error_code = EX_IOERR;
		goto close_exit;
	}
	
	if(smtp_get_response(fd, 250) != 250)
	{
		error_code = EX_DATAERR;
		goto close_exit;
	}

	if(smtp_putline(fd, "DATA\r\n") != 0)
	{
		error_code = EX_IOERR;
		goto close_exit;
	}
	if(smtp_get_response(fd, 354) != 354) /*354*/
	{
		error_code = EX_DATAERR;
		goto close_exit;
	}

/*
 data body
 * */
	if(smtp_putline(fd, mail_buf) != 0)
	{
		error_code = EX_IOERR;
		goto close_exit;
	}

	if(smtp_putline(fd, "\r\n.\r\n\0") != 0)
	{
		error_code = EX_IOERR;
		goto close_exit;
	}
	
	if(smtp_get_response(fd, 250) != 250)
	{
		error_code = EX_DATAERR;
		goto close_exit;
	}
#endif
	error_code = EX_OK;
	smtp_putline(fd, "QUIT\r\n");
	smtp_get_response(fd, 221);
	run_pos = 7;
close_exit:
	return;
}

void k_mail_d()
{
	int fd;
	struct sockaddr_in svr_addr;
//	
	struct task_struct *tsk = current;
	DECLARE_WAITQUEUE(wait, tsk);
	daemonize();
	strcpy(current->comm, "mail");
	spin_lock_irq(&tsk->sigmask_lock);
	sigfillset(&tsk->blocked);
	recalc_sigpending(tsk);
	spin_unlock_irq(&tsk->sigmask_lock);
//	set_fs(KERNEL_DS);
	tsk->flags |= PF_MEMALLOC;
	tsk->tty = NULL;
//	exit_mm(tsk);
//	exit_files(tsk);
//	exit_sighand(tsk);
//	exit_fs(tsk);
resock:	
	add_wait_queue(&smtp_wq, &wait);
	__set_current_state(TASK_INTERRUPTIBLE);
	schedule();
	__set_current_state(TASK_RUNNING);
	remove_wait_queue(&smtp_wq, &wait);
	fd = sys_socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		goto resock;
	}
	
	memset(&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(25);
	svr_addr.sin_addr.s_addr = SMTP_SERVER_IP;
	if(sys_connect(fd, (struct sockaddr *)(&svr_addr), sizeof(struct sockaddr))  < 0)
	{
		sys_close(fd);
		goto resock;
	}
	send_to_server(fd, g_subject, g_content);
	sys_close(fd);
	memset(&trace_buf, 0, TRACE_BUFF_SIZE);
	goto resock;

}

int send_mail(char *subject, char *content)
{
	int i = 0;
	e_w_pos = 0;
	if(email_alert_enable != 1)/*disable email_alert_enable*/
		return 0;
	g_subject = subject;
	g_content = content;
	email_for_log = &email2log;
	email_for_return = &email2ret;
	if(waitqueue_active(&smtp_wq))
		wake_up_interruptible(&smtp_wq);
	return 0;
}

int trace(const char *fmt, ...)
{
	va_list args;
	int i, len;
	static char buf[512];
	int cur_pos;

	va_start(args, fmt);
	len = vsprintf(buf, fmt, args);
	va_end(args);

	i = 0;
	while(i < len)
	{
		cur_pos = e_w_pos;
		cur_pos ++;
		cur_pos %= TRACE_BUFF_SIZE;
//		if(cur_pos == e_r_pos)
//		{
//			e_r_pos ++;
//			e_r_pos %= TRACE_BUFF_SIZE;
//		}
		trace_buf[e_w_pos] = buf[i++];
		e_w_pos = cur_pos;
	}
	trace_buf[e_w_pos + 1] = '\0';
	return len;
}

static int emac_get_info(char *buffer, char **start, off_t offset, int length)
{
	int len = 0;
	off_t begin = 0;
	off_t pos = 0;
	int size = 0;

	size = sprintf(buffer, "thresholds = %d\n w_pos = %d\n email_alert_enable =%d\n %s\n", g_email_thresholds, e_w_pos, email_alert_enable, trace_buf);
	pos += size;
	len += size;

	*start = buffer + offset;
	len -= offset;
	if(len > length)
		len = length;
	if(len < 0)
		len = 0;
	return len;	
}

void smtp_buf_proc_init()
{
	proc_net_create("emac", 0, emac_get_info);
}


