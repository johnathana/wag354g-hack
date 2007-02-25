/* Shared library add-on to iptables to add packet info matching support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <iptables.h>
#include <linux/netfilter_ipv4/ipt_packet.h>

/* Function which prints out usage message. */
static void
help(void)
{
	printf(
"packet match v%s options:\n"
"[!] --len value         Match skb->len value\n"
"[!] --proto value       Match skb->proto value\n"
"[!] --vid value       Match skb->vid value\n"
"[!] --cos value       Match skb->cos value\n"
"\n",
IPTABLES_VERSION);
}

static struct option opts[] = {
	{ "len", 1, 0, '1' },
	{ "proto", 1, 0, '2' },
	{ "vid", 1, 0, '3' },
	{ "cos", 1, 0, '4' },
	{0}
};

/* Initialize the match. */
static void
init(struct ipt_entry_match *m, unsigned int *nfcache)
{
	/* Can't cache this. */
	*nfcache |= NFC_UNKNOWN;
}

/* Function which parses command options; returns true if it
   ate an option */
static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      unsigned int *nfcache,
      struct ipt_entry_match **match)
{
	struct ipt_packet_info *packetinfo = (struct ipt_packet_info *)(*match)->data;

	switch (c) {
		char *end;
	case '1':
		check_inverse(optarg, &invert, &optind, 0);
		packetinfo->len = strtoul(optarg, &end, 0);
		if (*end != '\0' || end == optarg)
			exit_error(PARAMETER_PROBLEM, "Bad len value `%s'", optarg);
		if (invert)
			packetinfo->invert |= IPT_PACKET_INV_LEN;
		*flags = 1;
		break;
	case '2':
		check_inverse(optarg, &invert, &optind, 0);
		packetinfo->proto = strtoul(optarg, &end, 0);
		if (*end != '\0' || end == optarg)
			exit_error(PARAMETER_PROBLEM, "Bad proto value `%s'", optarg);
		if (invert)
			packetinfo->invert |= IPT_PACKET_INV_PROTO;
		*flags = 1;
		break;
	case '3':
		check_inverse(optarg, &invert, &optind, 0);
		packetinfo->vid = strtoul(optarg, &end, 0);
		if (*end != '\0' || end == optarg)
			exit_error(PARAMETER_PROBLEM, "Bad proto vid `%s'", optarg);
		if (invert)
			packetinfo->invert |= IPT_PACKET_INV_VID;
		*flags = 1;
		break;
	case '4':
		check_inverse(optarg, &invert, &optind, 0);
		packetinfo->cos = strtoul(optarg, &end, 0);
		if (*end != '\0' || end == optarg)
			exit_error(PARAMETER_PROBLEM, "Bad cos value `%s'", optarg);
		if (invert)
			packetinfo->invert |= IPT_PACKET_INV_COS;
		*flags = 1;
		break;
		
	default:
		return 0;
	}
	return 1;
}

void print_packet(unsigned long len, unsigned short proto, unsigned short vid, unsigned short cos, unsigned short invert)
{
	printf("len ");
	if (invert & IPT_PACKET_INV_LEN)
		printf("!");
	printf("0x%lx ", len);
	printf("proto ");
	if (invert & IPT_PACKET_INV_PROTO)
		printf("!");
	printf("0x%x ", proto);
	printf("vid ");
	if (invert & IPT_PACKET_INV_VID)
		printf("!");
	printf("0x%x ", vid);
	printf("cos ");
	if (invert & IPT_PACKET_INV_COS)
		printf("!");
	printf("0x%x ", cos);
	return;
}

/* Final check; must have specified --mark. */
static void
final_check(unsigned int flags)
{
	if (!flags)
		exit_error(PARAMETER_PROBLEM,
			   "packet match: You must specify `--len' or '--proto' or '--vid' or '--cos'");
}

/* Prints out the matchinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_match *match,
      int numeric)
{
	printf("packet match ");
	print_packet(((struct ipt_packet_info *)match->data)->len,
		  ((struct ipt_packet_info *)match->data)->proto,
		  ((struct ipt_packet_info *)match->data)->vid,
		  ((struct ipt_packet_info *)match->data)->cos,
		  ((struct ipt_packet_info *)match->data)->invert);
}

/* Saves the union ipt_matchinfo in parsable form to stdout. */
static void
save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
{
	printf("--packet ");
	print_packet(((struct ipt_packet_info *)match->data)->len,
		  ((struct ipt_packet_info *)match->data)->proto,
		  ((struct ipt_packet_info *)match->data)->vid,
		  ((struct ipt_packet_info *)match->data)->cos,
		  ((struct ipt_packet_info *)match->data)->invert);
}

static
struct iptables_match packet
= { NULL,
    "packet",
    IPTABLES_VERSION,
    IPT_ALIGN(sizeof(struct ipt_packet_info)),
    IPT_ALIGN(sizeof(struct ipt_packet_info)),
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
	register_match(&packet);
}
