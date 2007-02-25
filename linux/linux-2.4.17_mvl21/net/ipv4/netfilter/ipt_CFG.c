/*------------------------------------------------------------------------------

 Copyright (c) 2003 Texas Instruments Incorporated

 ALL RIGHTS RESERVED



"This computer program is subject to a separate license agreement

 signed by Texas Instruments Incorporated and the licensee, and is

 subject to the restrictions therein.  No other rights to reproduce,

 use, or disseminate this computer program, whether in part or in

 whole, are granted."



--------------------------------------------------------------------------------

 Module Name:   Kernel extension in netfilter.New target CFG



 Module Purpose:   To Control the execution of udhcp client



--------------------------------------------------------------------------------

 Revision History:

 Date and Type of Change.
 04/08/04 -> Creation date

------------------------------------------------------------------------------*/
#include <linux/config.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/tcp.h>
struct in_device;
#include <net/route.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/proc_fs.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif


static int device = 0;
static char indevname[25];
struct net_device *inIntf=NULL;

static ssize_t proc_read_ctrap_fops(struct file *filp,
                                 char *buf,size_t count , loff_t *offp);
struct file_operations ctrap_fops = {
                                     read: proc_read_ctrap_fops,
                                   };                                 
                                 
static struct proc_dir_entry *ctrap_file;

static ssize_t proc_read_ctrap_fops(struct file *filp,
                                 char *buf,size_t count , loff_t *offp)
{
  char *pdata = NULL;
  int len = 0;
  char line[35];

  if( *offp != 0 )
    return 0;
  pdata = buf;
  len += sprintf(line,"DEV_ID=%d IF_NAME=%s\n",device,indevname);
  copy_to_user(pdata,line,len);
  *offp = len;
  return len;
}


static unsigned int cfg(struct sk_buff **pskb,
			   unsigned int hooknum,
			   const struct net_device *in,
			   const struct net_device *out,
			   const void *targinfo,
			   void *userinfo)
{
  if( inIntf != in)
  {
    if(in->name)
    {
      memcpy( indevname,in->name,strlen(in->name));   
    }
  }
  inIntf = in;
  device = (*pskb)->__unused;
  return IPT_CONTINUE;
}

static int check(const char *tablename,
		 const struct ipt_entry *e,
		 void *targinfo,
		 unsigned int targinfosize,
		 unsigned int hook_mask)
{
	return 1;
}

static struct ipt_target ipt_cfg_reg
= { { NULL, NULL }, "CFG", cfg, check, NULL, THIS_MODULE };

static int __init init(void)
{
	if (ipt_register_target(&ipt_cfg_reg))
		return -EINVAL;

  /* Create proc file */
  ctrap_file = create_proc_entry("net/port_trap", 0444, NULL);
  if( ctrap_file == NULL)
  {
    printk("ERROR:COULD NOT CREATE FILE\n");
    return -1;
  }
  ctrap_file->owner = THIS_MODULE;
  ctrap_file->proc_fops = &ctrap_fops;
  memset(indevname,0x00,sizeof(indevname));
	return 0;
}

static void __exit fini(void)
{
	ipt_unregister_target(&ipt_cfg_reg);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
