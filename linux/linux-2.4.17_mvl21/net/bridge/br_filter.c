/*
 *	Handle incoming frames
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Qui Le		<qle@telogy.com>
 *
 *	$Id: br_filter.c,v 1.1.1.2 2005/03/28 06:57:03 sparq Exp $
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/netfilter_bridge.h>
#include <linux/byteorder/little_endian.h>
#include "br_private.h"

#include <asm/uaccess.h>


int br_add_filter_entry(struct net_bridge *br, unsigned char *_entry)
{
	struct __br_filter_entry *filter_input;
	struct bridge_fw_db_entry *new_filter_entry;
	struct bridge_fw_db_entry *search_entry;
	struct bridge_fw_db_entry *temp_entry;
	unsigned char record_match = 0;
        int err, i;


	write_lock(&br->filter_lock);

	/* Allocate Memory to Copy Structure Entry */
	filter_input = kmalloc(sizeof(*filter_input), GFP_KERNEL);
	if (filter_input == NULL) {
		write_unlock(&br->filter_lock);
		return -1;
	}

     	memset(filter_input, 0, sizeof(*filter_input));
	err = copy_from_user(filter_input, (struct __br_filter_entry *)_entry, 
				sizeof(struct __br_filter_entry));
	write_unlock(&br->filter_lock);

	if (err) {
		printk(KERN_INFO "copy_from_user failed\n");
		return -1;
	}

#if 0
	printk(KERN_INFO "To Kernel Destination Port - %s\n", filter_input->dport);
	printk(KERN_INFO "To Kernel Source Port - %s\n", filter_input->sport);
	printk(KERN_INFO "To Kernel MAC Destination Addr - %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", filter_input->dst_mac_addr[0], filter_input->dst_mac_addr[1], filter_input->dst_mac_addr[2], filter_input->dst_mac_addr[3], filter_input->dst_mac_addr[4], filter_input->dst_mac_addr[5]);
	printk(KERN_INFO "To Kernel MAC Source Addr - %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", filter_input->src_mac_addr[0], filter_input->src_mac_addr[1], filter_input->src_mac_addr[2], filter_input->src_mac_addr[3], filter_input->src_mac_addr[4], filter_input->src_mac_addr[5]);
	printk(KERN_INFO "To Kernel Protocol Type - %x\n", filter_input->proto);
#endif


	write_lock(&br->filter_lock);

	/* Copy Over to Bridge Filter Entry Structure */
	new_filter_entry = kmalloc(sizeof(*new_filter_entry), GFP_KERNEL);
	if (new_filter_entry == NULL) {
		write_unlock(&br->filter_lock);
		return -1;
	}
	memset(new_filter_entry, 0, sizeof(*new_filter_entry));

	memcpy(new_filter_entry->dport, filter_input->dport, sizeof(filter_input->dport));
	memcpy(new_filter_entry->sport, filter_input->sport, sizeof(filter_input->sport));

	memcpy(new_filter_entry->dst_mac_addr, filter_input->dst_mac_addr, MAC_ADDR_LEN);
	memcpy(new_filter_entry->src_mac_addr, filter_input->src_mac_addr, MAC_ADDR_LEN);
	new_filter_entry->eth_proto = htons(filter_input->proto);
	new_filter_entry->access_type = filter_input->access_type;

	write_unlock(&br->filter_lock);

	/* Determine the Filter Matching for Each Entry */
	for (i = 0; i < MAC_ADDR_LEN; i++) {
	  if (new_filter_entry->dst_mac_addr[i] == 0)
	    continue;

	  record_match |= MATCH_DST_MAC;
	  break;
	}

	for (i = 0; i < MAC_ADDR_LEN; i++) {
	  if (new_filter_entry->src_mac_addr[i] == 0)
	    continue;

	  record_match |= MATCH_SRC_MAC;
	  break;
	}

	if (new_filter_entry->eth_proto != 0)
	  record_match |= MATCH_PROTO;

	if (strcmp(new_filter_entry->sport, "0") != 0)
	    record_match |= MATCH_SPORT;

	if (strcmp(new_filter_entry->dport, "0") != 0)
	    record_match |= MATCH_DPORT;

	new_filter_entry->match_check = record_match;

#if 0
	printk(KERN_INFO "The Match ID = %x\n", new_filter_entry->match_check);
#endif

	/* Link the New Entry to Filter Table Link List */
	if(br->blacklist == NULL) {
		br->blacklist = new_filter_entry;
		br->blacklist->next = NULL;
	}


	search_entry = (struct bridge_fw_db_entry *)br->blacklist;
	while (search_entry != NULL) {

		temp_entry = search_entry;
		search_entry = search_entry->next;
	}

	search_entry = temp_entry;
	search_entry->next = (struct bridge_fw_db_entry *)new_filter_entry;
	search_entry = search_entry->next;
	search_entry->next = NULL;


	/* Free the Temporary Entry Structure */
	kfree(filter_input);

	return 0;
}


int br_del_filter_entry(struct net_bridge *br, unsigned char *_entry)
{
  struct __br_filter_entry *filter_input;
  struct bridge_fw_db_entry *previous_entry;
  struct bridge_fw_db_entry *current_entry;
  unsigned short ent_proto = 0;
  int err;
  unsigned char filter_status = 0;


  /* First Disable Bridge Filtering */
  filter_status = br->br_filter_active;
  br->br_filter_active = 0;

  write_lock(&br->filter_lock);

  /* Allocate Memory to Copy Structure Entry */
  filter_input = kmalloc(sizeof(*filter_input), GFP_KERNEL);
  if (filter_input == NULL) {
    write_unlock(&br->filter_lock);
    return -1;
  }

  memset(filter_input, 0, sizeof(*filter_input));
  err = copy_from_user(filter_input, (struct __br_filter_entry *)_entry, 
		       sizeof(struct __br_filter_entry));
  write_unlock(&br->filter_lock);


  if (err) {
    printk(KERN_INFO "copy_from_user failed\n");
    return -1;
  }

#if 0
  printk(KERN_INFO "To Kernel Destination Port - %s\n", filter_input->dport);
  printk(KERN_INFO "To Kernel Source Port - %s\n", filter_input->sport);
  printk(KERN_INFO "To Kernel MAC Destination Addr - %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", filter_input->dst_mac_addr[0], filter_input->dst_mac_addr[1], filter_input->dst_mac_addr[2], filter_input->dst_mac_addr[3], filter_input->dst_mac_addr[4], filter_input->dst_mac_addr[5]);
  printk(KERN_INFO "To Kernel MAC Source Addr - %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n", filter_input->src_mac_addr[0], filter_input->src_mac_addr[1], filter_input->src_mac_addr[2], filter_input->src_mac_addr[3], filter_input->src_mac_addr[4], filter_input->src_mac_addr[5]);
  printk(KERN_INFO "To Kernel Protocol Type - %x\n", filter_input->proto);
#endif

  
  /* Searching for the Matching Entry to Delete */
  previous_entry = (struct bridge_fw_db_entry *)br->blacklist;
  current_entry = (struct bridge_fw_db_entry *)br->blacklist;
  while (current_entry != NULL) {

    ent_proto = ntohs(current_entry->eth_proto);
    
    if (ent_proto == (filter_input->proto)) {

	if (strcmp(current_entry->dport, filter_input->dport) == 0) {

	    if (strcmp(current_entry->sport, filter_input->sport) == 0) {

		if((*(current_entry->src_mac_addr) == *(filter_input->src_mac_addr)) && ((*(current_entry->src_mac_addr + 4) & 0x0000ffff) == (*(filter_input->src_mac_addr + 4) & 0x0000ffff))) {

		    if((*(current_entry->dst_mac_addr) == *(filter_input->dst_mac_addr)) && ((*(current_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(filter_input->dst_mac_addr + 4) & 0x0000ffff))) {

			printk(KERN_INFO "Deleting a Bridge Filter Entry\n");

			write_lock(&br->filter_lock);
			if ((current_entry == br->blacklist) && (current_entry->next == NULL)) {
			    br->blacklist = NULL;
			    kfree(current_entry);
			    break;
			}
			else if (current_entry == br->blacklist) {
			    br->blacklist = current_entry->next;
			    kfree(current_entry);
			    break;
			}		 
			else { 
			    previous_entry->next = current_entry->next;
			    kfree(current_entry);
			    break;
			}

			write_unlock(&br->filter_lock);
		    }
		}
	    }
	}
    }
    previous_entry = current_entry;
    current_entry = current_entry->next;
    printk(KERN_INFO "delete miss ...\n");
  }

  /* Free the Temporary Entry Structure */
  kfree(filter_input);

  /* Restore the Bridge Filter Original Status */
  br->br_filter_active = filter_status;

  return 0;
}


int br_flush_filter(struct net_bridge *br)
{
  struct bridge_fw_db_entry *list_entry;
  struct bridge_fw_db_entry *remove_entry;
  unsigned char filter_status = 0;

  /* First Disable Bridge Filtering */
  filter_status = br->br_filter_active;
  br->br_filter_active = 0;

  list_entry = br->blacklist;
  remove_entry = list_entry;

  /* Check for Empty Table Entries */
  if(br->blacklist == NULL)
    return 0;

  write_lock(&br->filter_lock);
  while(list_entry->next != NULL)
    {
      remove_entry = list_entry->next;
      kfree(list_entry);
      list_entry = remove_entry;
    }

  kfree(list_entry);
  br->blacklist = NULL;
  write_unlock(&br->filter_lock);

  /* Restore the Bridge Filter Original Status */
  br->br_filter_active = filter_status;

  return 0;
}


int br_show_filter_entries(struct net_bridge *br, unsigned char *_buf, int maxnum)
{
  struct __br_filter_entry *walk;
  struct bridge_fw_db_entry *list_entry;
  int i, err;
  int num;


  num = 0;
  walk = (struct __br_filter_entry *)_buf;
  list_entry = br->blacklist;

  /* Check for Empty Table Entries */
  if(br->blacklist == NULL)
    return 0;


  while (list_entry != NULL && num < maxnum) {
    struct __br_filter_entry *ent;

    write_lock(&br->filter_lock);
    /* Allocate Memory to Copy Structure Entry */
    ent = kmalloc(sizeof(*ent), GFP_KERNEL);
    if (ent == NULL) {
      write_unlock(&br->filter_lock);
      return -1;
    }

    memset(ent, 0, sizeof(struct __br_filter_entry));
    memcpy(ent->sport, list_entry->sport, sizeof(list_entry->sport));
    memcpy(ent->dport, list_entry->dport, sizeof(list_entry->dport));

    memcpy(ent->src_mac_addr, list_entry->src_mac_addr, ETH_ALEN);
    memcpy(ent->dst_mac_addr, list_entry->dst_mac_addr, ETH_ALEN);
    ent->proto = list_entry->eth_proto;
    ent->access_type = list_entry->access_type;
    ent->frames_matched = list_entry->frames_matched;


    err = copy_to_user(&walk[num], ent, sizeof(struct __br_filter_entry));
    write_unlock(&br->filter_lock);

    kfree(ent);

    num++;
    list_entry = list_entry->next;
  }

  return num;

}

int br_filter_frame (struct sk_buff *skb)
{
#if 1
        struct net_bridge *br;
	unsigned char *src;
        struct net_bridge_fdb_entry *dst;
        struct net_bridge_port *p;
 
	struct bridge_fw_db_entry *search_entry;
	unsigned short ent_proto = 0;
	unsigned char policy = 0;

    /* PANKAJ -- Added this code to initialize the ethernet header. */
    skb->mac.raw = skb->data;

	src = skb->mac.ethernet->h_source;

#if 0
        printk ("DEBUG: Bridge Filtering 2.\n");
        printk ("DEBUG: Received packet from source MAC Address 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n",
                    (int)src[0], (int)src[1], (int)src[2], (int)src[3], (int)src[4],(int)src[5]);
#endif
        
        p = skb->dev->br_port;
        br = p->br;                     
                                                                                
        dst = br_fdb_get(br, src);

        if (dst == NULL)
        {
            printk ("DEBUG: No match for source MAC Address 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n",
                    (int)src[0], (int)src[1], (int)src[2], (int)src[3], (int)src[4],(int)src[5]);
            goto passthru;
        }
                                                                                
        br_fdb_put(dst);                                                                                

#if 0
        /* PANKAJ -- Debugging. */
	    printk("The destination port is %s\n", p->dev->name);
#endif

	if (br->br_filter_active) {

		search_entry = (struct bridge_fw_db_entry *)br->blacklist;
		while (search_entry != NULL) {

		  ent_proto = search_entry->eth_proto;
		  policy = search_entry->access_type;

		  switch(search_entry->match_check) {
		  case 8:
		      /* Filter Source Port Only */
		      //printk(KERN_INFO "Filter Source Port Only\n");
		      if(strcmp(search_entry->sport, dst->dst->dev->name) == 0) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 9:
		      /* Filter Source Port and Protocol */
		      //printk(KERN_INFO "Filter Source Port and Protocol\n");
		      if((strcmp(search_entry->sport, dst->dst->dev->name) == 0) && 
			 (ent_proto == (skb->mac.ethernet->h_proto))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 10:
		      /* Filter Source Port and Source MAC Address */
		      //printk(KERN_INFO "Filter Source Port and Source MAC Address\n");
		      if ((strcmp(search_entry->sport, dst->dst->dev->name) == 0) && 
			  ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff)))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 11:
		      /* Filter Source Port, Protocol & Source MAC Address */
		      //printk(KERN_INFO "Filter Source Port, Protocol & Source MAC Address\n");
		      if ((strcmp(search_entry->sport, dst->dst->dev->name) == 0) && 
			  ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) &&
			  (ent_proto == (skb->mac.ethernet->h_proto))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 12:
		      /* Filter Source Port and Destination MAC Address */
		      //printk(KERN_INFO "Filter Source Port and Destination MAC Address\n");
		      if ((strcmp(search_entry->sport, dst->dst->dev->name) == 0) && 
			  (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 13:
		      /* Filter Source Port, Protocol & Destination MAC Address */
		      //printk(KERN_INFO "Filter Source Port, Protocol & Destination MAC Address\n");
		      if ((strcmp(search_entry->sport, dst->dst->dev->name) == 0) && 
			  (ent_proto == (skb->mac.ethernet->h_proto)) &&
			  (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 14:
		      /* Filter Source Port, Source MAC Address and Destination MAC Address */
		      //printk(KERN_INFO "Filter Source Port, Source MAC Address and Destination MAC Address\n");
		      if ((strcmp(search_entry->sport, dst->dst->dev->name) == 0) && 
			  ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) &&
			  (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 15:
		      /* Filter Source Port, Protocol, Source MAC Address and Destination MAC Address */
		      //printk(KERN_INFO "Filter Source Port, Protocol, Source MAC Address and Destination MAC Address\n");
		      if ((strcmp(search_entry->sport, dst->dst->dev->name) == 0) && 
			  (ent_proto == (skb->mac.ethernet->h_proto)) && 
			  ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) &&
			  (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 16:
		      /* Filter Destination Port Only */
		      //printk(KERN_INFO "Filter Destination Port Only\n");
		      if(strcmp(search_entry->dport, p->dev->name) == 0) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 17:
		      /* Filter Destination Port and Protocol */
		      //printk(KERN_INFO "Filter Destination Port and Protocol\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && (ent_proto == (skb->mac.ethernet->h_proto))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 18:
		      /* Filter Destination Port and Source MAC Address */
		      //printk(KERN_INFO "Filter Destination Port and Source MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && 
			 ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff)))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 19:
		      /* Filter Destination Port, Protocol & Source MAC Address */
		      //printk(KERN_INFO "Filter Destination Port, Protocol & Source MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && 
			  ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) &&
			  (ent_proto == (skb->mac.ethernet->h_proto))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 20:
		      /* Filter Destination Port and Destination MAC Address */
		      //printk(KERN_INFO "Filter Destination Port and Destination MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && 
			  (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 21:
		      /* Filter Destination Port, Protocol & Destination MAC Address */
		      //printk(KERN_INFO "Filter Destination Port, Protocol & Destination MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && 
			  (ent_proto == (skb->mac.ethernet->h_proto)) &&
			  (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 22:
		      /* Filter Destination Port, Source MAC Address and Destination MAC Address */
		      //printk(KERN_INFO "Filter Destination Port, Source MAC Address and Destination MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && 
			  ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) &&
			  (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 23:
		      /* Filter Destination Port, Protocol, Source MAC Address and Destination MAC Address */
		      //printk(KERN_INFO "Filter Destination Port, Protocol, Source MAC Address and Destination MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && 
			 (ent_proto == (skb->mac.ethernet->h_proto)) &&
			 ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) &&
			 (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 24:
		      /* Filter Destination & Source Port */
		      //printk(KERN_INFO "Filter Destination & Source Port\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && 
			 (strcmp(search_entry->dport, p->dev->name) == 0)) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 25:
		      /* Filter Destination & Source Port and Protocol*/
		      //printk(KERN_INFO "Filter Destination & Source Port and Protocol\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && (strcmp(search_entry->dport, p->dev->name) == 0) &&
			 (ent_proto == (skb->mac.ethernet->h_proto))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 26:
		      /* Filter Destination & Source Port and Source MAC Address */
		      //printk(KERN_INFO "Filter Destination & Source Port and Source MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && (strcmp(search_entry->dport, p->dev->name) == 0) && 
			 ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff)))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 27:
		      /* Filter Destination & Source Port, Source MAC Address and Protocol*/
		      //printk(KERN_INFO "Filter Destination & Source Port, Source MAC Address and Protocol\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && (strcmp(search_entry->dport, p->dev->name) == 0) && 
			 ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) && 
			 (ent_proto == (skb->mac.ethernet->h_proto))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 28:
		      /* Filter Destination & Source Port and Destination MAC Address */
		      //printk(KERN_INFO "Filter Destination & Source Port and Destination MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && (strcmp(search_entry->dport, p->dev->name) == 0) && 
			 (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 29:
		      /* Filter Destination & Source Port, Protocol and Destination MAC Address */
		      //printk(KERN_INFO "Filter Destination & Source Port and Destination MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && (strcmp(search_entry->dport, p->dev->name) == 0) && 
			 (ent_proto == (skb->mac.ethernet->h_proto)) && 
			 (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 30:
		      /* Filter Destination & Source Port and Source & Destination MAC Address */
		      //printk(KERN_INFO "Filter Destination & Source Port and Source & Destination MAC Address\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && (strcmp(search_entry->dport, p->dev->name) == 0) && 
			 ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) && 
			 (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 31:
		      /* Filter Destination & Source Port and Source & Destination MAC Address and Protocol */
		      //printk(KERN_INFO "Filter Destination & Source Port and Source & Destination MAC Address and Protocol\n");
		      if((strcmp(search_entry->dport, p->dev->name) == 0) && (strcmp(search_entry->dport, p->dev->name) == 0) && 
			 (ent_proto == (skb->mac.ethernet->h_proto)) && 
			 ((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) &&
			 (*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			  search_entry->frames_matched++;

			  if (policy == 1)
			      goto passthru;
			  else
			      goto free_out;

		      }
		      break;

		  case 0:
		    /* Filter Matching on Access Type */

		    if (policy == 0) {
		      search_entry->frames_matched++;
		      goto free_out;
		    }
		    
		    if (policy == 1) {
		      search_entry->frames_matched++;
		      goto passthru;
		    }
		    break;

		  case 1:
		    /* Filter Matching only Protocol */
		    //		    printk(KERN_INFO "Filter Matching only Protocol\n");
		    if(ent_proto == (skb->mac.ethernet->h_proto)) {

		      search_entry->frames_matched++;

		      if (policy == 1)
			goto passthru;
		      else
			goto free_out;
		    }
		    break;

		  case 2:
		    /* Filter Matching only Source MAC Address */
		    //		    printk(KERN_INFO "Filter Matching only Src MAC Address\n");

		    if((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) {

		      search_entry->frames_matched++;

		      if (policy == 1)
			goto passthru;
		      else
			goto free_out;
		    }
		    break;

		  case 3:
		    /* Filter Matching on Protocol and Source MAC Address */
		    //		    printk(KERN_INFO "Filter Matching on Src MAC Address and Protocol\n");
		    if(ent_proto == (skb->mac.ethernet->h_proto)) {

		      if((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) {

			search_entry->frames_matched++;

			if (policy == 1)
			  goto passthru;
			else
			  goto free_out;
		      }
		    }
		    break;

		  case 4:
		    /* Filter Matching only Destination MAC Address */
		    //		    printk(KERN_INFO "Filter Matching only Dst MAC Address\n");

		    if((*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

		      search_entry->frames_matched++;

		      if (policy == 1)
			goto passthru;
		      else
			goto free_out;
		    }
		    break;

		  case 5:
		    /* Filter Matching on Protocol and Destination MAC Address */
		    //		    printk(KERN_INFO "Filter Matching on Dst MAC Address and Protocol\n");
		    if(ent_proto == (skb->mac.ethernet->h_proto)) {

		      if((*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			search_entry->frames_matched++;

			if (policy == 1)
			  goto passthru;
			else
			  goto free_out;
		      }
		    }
		    break;

		  case 6:
		    /* Filter Matching on Source and Destination MAC Address Filtering */
		    //		    printk(KERN_INFO "Filter Matching on Source and Destination MAC Address Filtering\n");

		    if((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) {

		      if((*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {

			search_entry->frames_matched++;

			if (policy == 1)
			  goto passthru;
			else
			  goto free_out;
		      }
		    }
		    break;

		  case 7:
		    /* Filter Matching on Source-Destination MAC Address and Protocol */
		    //		    printk(KERN_INFO "Filter Matching on Source-Destination MAC Address and Protocol\n");
		    if(ent_proto == (skb->mac.ethernet->h_proto)) {

		      if((*(search_entry->src_mac_addr) == *(skb->mac.ethernet->h_source)) && ((*(search_entry->src_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_source + 4) & 0x0000ffff))) {

			if((*(search_entry->dst_mac_addr) == *(skb->mac.ethernet->h_dest)) && ((*(search_entry->dst_mac_addr + 4) & 0x0000ffff) == (*(skb->mac.ethernet->h_dest + 4) & 0x0000ffff))) {


			  search_entry->frames_matched++;

			  if (policy == 1)
			    goto passthru;
			  else
			    goto free_out;
			}
		      }
		    }
		    break;

		  default:
		    printk(KERN_INFO "Free Pass\n");
		    break;
		  }

		  search_entry = search_entry->next;
		}
	}
#endif

 passthru:
	return 0;


 free_out:
	return 1;
}


