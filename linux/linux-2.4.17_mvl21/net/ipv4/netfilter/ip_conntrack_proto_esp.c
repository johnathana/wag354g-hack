/*------------------------------------------------------------------------------

 Copyright (c) 2003 Texas Instruments Incorporated

 ALL RIGHTS RESERVED



"This computer program is subject to a separate license agreement

 signed by Texas Instruments Incorporated and the licensee, and is

 subject to the restrictions therein.  No other rights to reproduce,

 use, or disseminate this computer program, whether in part or in

 whole, are granted."

 DESCRIPTION: NAT/Conntrack extension to support ESP

------------------------------------------------------------------------------*/

#include <linux/config.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/netfilter.h>

#include <linux/ip.h>
#include <linux/in.h>
#include <linux/list.h>

#include <linux/netfilter_ipv4/lockhelp.h>
#include <linux/netfilter_ipv4/listhelp.h>
#include <linux/netfilter_ipv4/ip_conntrack_protocol.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mukesh Kumar <mkumar@telogy.com>");
MODULE_DESCRIPTION("netfilter connection tracking protocol helper for ESP");

#define DEBUGP(format, args...) printk(KERN_DEBUG __FILE__ ":" __FUNCTION__ \
                                      ": " format, ## args)


/*
#define DUMP_TUPLE_ESP(x) printk("SOURCE::%u.%u.%u.%u:%x -> DEST::%u.%u.%u.%u:%x\n", \
                       NIPQUAD((x)->src.ip), ntohl((x)->src.u.esp.spi), \
                       NIPQUAD((x)->dst.ip),ntohl((x)->dst.u.esp.spi))
*/
#define DUMP_TUPLE_ESP(x)


DECLARE_RWLOCK(ip_ct_esp_lock);

#define TEMP_TIME 0
#define PERM_TIME 60*3

#define FREE 0
#define OUT  1
#define IN   2

struct ip_conntrack_tuple out_tuple,in_tuple;
int state = FREE;

/* invert esp part of tuple */
static int esp_invert_tuple(struct ip_conntrack_tuple *tuple,
                           const struct ip_conntrack_tuple *orig)
{
  tuple->dst.u.esp.spi = orig->dst.u.esp.spi;
  return 1;
}

/* esp hdr info to tuple */
static int esp_pkt_to_tuple(const void *datah, size_t datalen,
                           struct ip_conntrack_tuple *tuple)
{
   unsigned int temp;

   temp = *(unsigned int*)(datah);
   tuple->dst.u.esp.spi = temp;

   return 1;

}

/* print esp part of tuple */
static unsigned int esp_print_tuple(char *buffer,
                                   const struct ip_conntrack_tuple *tuple)
{
	return sprintf(buffer, "SPI=%x ",tuple->dst.u.esp.spi);
}

/* print private data for conntrack */
static unsigned int esp_print_conntrack(char *buffer,
                                       const struct ip_conntrack *ct)
{
       return sprintf(buffer, "OUT SPI=%x ,IN SPI = %x \n",
	   	               ct->tuplehash[0].tuple.dst.u.esp.spi,ct->tuplehash[1].tuple.dst.u.esp.spi);
}

/* Returns verdict for packet, and may modify conntrack */
static int esp_packet(struct ip_conntrack *ct,
                     struct iphdr *iph, size_t len,
                     enum ip_conntrack_info conntrackinfo)
{

      /* If we've seen traffic both ways, this is a ESP connection.
        * Extend timeout. */
       if (ct->status & IPS_SEEN_REPLY) {
               ip_ct_refresh(ct, PERM_TIME*HZ);
               /* Also, more likely to be important, and not a probe. */
               set_bit(IPS_ASSURED_BIT, &ct->status);
       }
       else if( state == FREE)
       {
          ip_ct_refresh(ct, PERM_TIME*HZ);

          DUMP_TUPLE_ESP(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
          DUMP_TUPLE_ESP(&ct->tuplehash[IP_CT_DIR_REPLY].tuple);
       }
       else
       {
          ip_ct_refresh(ct, TEMP_TIME*HZ);
          DUMP_TUPLE_ESP(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
          DUMP_TUPLE_ESP(&ct->tuplehash[IP_CT_DIR_REPLY].tuple);
       }

       return NF_ACCEPT;
}

/* Called when a new connection for this protocol found. */
static int esp_new(struct ip_conntrack *ct,
                  struct iphdr *iph, size_t len)
{

      /*
        if free -- out tuple
                   temp entry
        if out  -- in tuple
                   temp entry
        if in   -- new out tuple,erase in tuple
      */
      switch(state)
      {
        case FREE:
          WRITE_LOCK(&ip_ct_esp_lock);
          out_tuple = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple;
          state = OUT;
       	  WRITE_UNLOCK(&ip_ct_esp_lock);
          break;
        case OUT:
          WRITE_LOCK(&ip_ct_esp_lock);
          if(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip == out_tuple.dst.ip)
          {
            in_tuple = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple;
            state = IN;
          }
          else if(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip != out_tuple.src.ip)
            out_tuple = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple;
       	  WRITE_UNLOCK(&ip_ct_esp_lock);
          break;
        case IN:
          WRITE_LOCK(&ip_ct_esp_lock);
          if(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.esp.spi == out_tuple.dst.u.esp.spi)
            ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.esp.spi = in_tuple.dst.u.esp.spi;
          out_tuple.dst.u.esp.spi = in_tuple.dst.u.esp.spi = 0;
          out_tuple.dst.ip = in_tuple.dst.ip = 0;
          state = FREE;
       	  WRITE_UNLOCK(&ip_ct_esp_lock);
          break;
        default:
          break;
      }
       return 1;

}

/* Called when a conntrack entry has already been removed from the hashes
 * and is about to be deleted from memory */
static void esp_destroy(struct ip_conntrack *ct)
{
  return 1;
}

/* protocol helper struct */
static struct ip_conntrack_protocol esp = { { NULL, NULL }, IPPROTO_ESP,
                                           "esp",
                                           esp_pkt_to_tuple,
                                           esp_invert_tuple,
                                           esp_print_tuple,
                                           esp_print_conntrack,
                                           esp_packet,
                                           esp_new,
                                           esp_destroy,
                                           NULL,
                                           THIS_MODULE };

/* ip_conntrack_proto_gre initialization */
static int __init init(void)
{
       int retcode;
       if ((retcode = ip_conntrack_protocol_register(&esp))) {
                printk(KERN_ERR "Unable to register conntrack protocol "
                               "helper for esp: %d\n",        retcode);
               return -EIO;
       }

       return 0;
}

static void __exit fini(void)
{
       ip_conntrack_protocol_unregister(&esp);
}

module_init(init);
module_exit(fini);
