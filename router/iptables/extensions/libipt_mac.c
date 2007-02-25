/* Shared library add-on to iptables to add MAC address support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#if defined(__GLIBC__) && __GLIBC__ == 2
#include <net/ethernet.h>
#else
#include <linux/if_ether.h>
#endif
#include <iptables.h>
#include <linux/netfilter_ipv4/ipt_mac.h>

/* Function which prints out usage message. */

static void
help(void)
{
	
	printf(
"MAC v%s options:\n"
" --mac-source [!] XX:XX:XX:XX:XX:XX\n"
"				Match source MAC address\n"
" --mac-destination [!] XX:XX:XX:XX:XX:XX\n"
"				Match destination MAC address\n"
"\n", IPTABLES_VERSION);

}

static struct option opts[] = {
	{ "mac-source", 1, 0, '1' },
	{ "mac-destination", 1, 0, '2' },
	{0}
};

/* Initialize the match. */
static void
init(struct ipt_entry_match *m, unsigned int *nfcache)
{
	/* Can't cache this */
	*nfcache |= NFC_UNKNOWN;
}

static void
parse_mac(const char *mac, struct ipt_mac_info *info, int type)
{
	unsigned int i = 0;

	if (strlen(mac) != ETH_ALEN*3-1)
		exit_error(PARAMETER_PROBLEM, "Bad mac address `%s'", mac);

	for (i = 0; i < ETH_ALEN; i++) {
		long number;
		char *end;

		number = strtol(mac + i*3, &end, 16);

		if (end == mac + i*3 + 2
		    && number >= 0
		    && number <= 255)
		{
			if(type == 1)
				info->srcaddr[i] = number;
			else
				info->dstaddr[i] = number;
		}		
		else
			exit_error(PARAMETER_PROBLEM,
				   "Bad mac address `%s'", mac);
	}
}

/* Function which parses command options; returns true if it
   ate an option */
static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      unsigned int *nfcache,
      struct ipt_entry_match **match)
{
	struct ipt_mac_info *macinfo = (struct ipt_mac_info *)(*match)->data;

	switch (c) {
	case '1':
		check_inverse(optarg, &invert, &optind, 0);
		parse_mac(argv[optind-1], macinfo, 1);
		if (invert)
			macinfo->invert &= 1<<SRCMAC_MASK;
		macinfo->flags |= IPT_MAC_SOURCE;
		*flags = 1;
		break;
	case '2':
		check_inverse(optarg, &invert, &optind, 0);
		parse_mac(argv[optind-1], macinfo, 2);
		if (invert)
			//macinfo->invert = 1;
			macinfo->invert &= 1<<DSTMAC_MASK;
		macinfo->flags |= IPT_MAC_DESTINATION;
		*flags = 1;
		break;

	default:
		return 0;
	}

	return 1;
}

static void print_mac(unsigned char macaddress[ETH_ALEN], int invert)
{
	unsigned int i;

	printf("%s%02X", invert ? "!" : "", macaddress[0]);
	for (i = 1; i < ETH_ALEN; i++)
		printf(":%02X", macaddress[i]);
	printf(" ");
}

/* Final check; must have specified --mac. */
static void final_check(unsigned int flags)
{
	if (!flags)
		exit_error(PARAMETER_PROBLEM,
			   "You must specify `--mac-source' or '--mac-destination'");
}

/* Prints out the matchinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_match *match,
      int numeric)
{
	struct ipt_mac_info *macinfo = (struct ipt_mac_info *)match->data;
	
	if(macinfo->flags & IPT_MAC_SOURCE)
	{
	printf("SRCMAC ");
//	print_mac(((struct ipt_mac_info *)match->data)->srcaddr,
//		  ((struct ipt_mac_info *)match->data)->invert);
	print_mac(((struct ipt_mac_info *)match->data)->srcaddr,
		  (((struct ipt_mac_info *)match->data)->invert) & (1<<SRCMAC_MASK)?1:0);
	}
	if(macinfo->flags & IPT_MAC_DESTINATION)
	{
	printf("DSTMAC ");
//	print_mac(((struct ipt_mac_info *)match->data)->dstaddr,
//		  ((struct ipt_mac_info *)match->data)->invert);
	print_mac(((struct ipt_mac_info *)match->data)->dstaddr,
		  (((struct ipt_mac_info *)match->data)->invert) & (1<<DSTMAC_MASK)?1:0);
	}

}

/* Saves the union ipt_matchinfo in parsable form to stdout. */
static void save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
{
	struct ipt_mac_info *macinfo = (struct ipt_mac_info *)match->data;
	
	if(macinfo->flags & IPT_MAC_SOURCE)
	{
		printf("--srcmac ");
		print_mac(((struct ipt_mac_info *)match->data)->srcaddr,
		  (((struct ipt_mac_info *)match->data)->invert) & (1<<SRCMAC_MASK)?1:0);
		//print_mac(((struct ipt_mac_info *)match->data)->srcaddr,
		//	  ((struct ipt_mac_info *)match->data)->invert);
	}
	if(macinfo->flags & IPT_MAC_DESTINATION)
	{
		printf("--dstmac ");
		print_mac(((struct ipt_mac_info *)match->data)->dstaddr,
		  (((struct ipt_mac_info *)match->data)->invert) & (1<<DSTMAC_MASK)?1:0);
		//print_mac(((struct ipt_mac_info *)match->data)->dstaddr,
		//		  ((struct ipt_mac_info *)match->data)->invert);
	}
}

static
struct iptables_match mac
= { NULL,
    "mac",
    IPTABLES_VERSION,
    IPT_ALIGN(sizeof(struct ipt_mac_info)),
    IPT_ALIGN(sizeof(struct ipt_mac_info)),
    &help,
    &init,
    &parse,
    &final_check,
    &print,
    &save,
    opts
};

void _init(void)
{
	register_match(&mac);
}
