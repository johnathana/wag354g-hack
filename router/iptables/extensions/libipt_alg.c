/* Shared library add-on to iptables to add TOS matching support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <iptables.h>
#include <linux/netfilter_ipv4/ipt_alg.h>

/* TOS names and values. */
static
struct ALG_name
{
	const char *name;
} ALG_names[] = {
	{ "ftp" },
	{ "tftp" },
	{ "h323" },
	{ "irc" },
	{ "mms" },
	{ "gre" },
	{ "pptp" },
	{ "sip" },
};

/* Function which prints out usage message. */
static void
help(void)
{
	unsigned int i;

	printf(
"ALG match v%s options:\n"
"[!] --alg type             Match ALG type from one of the\n"
"                                following values:\n",
IPTABLES_VERSION);

	for (i = 0; i < sizeof(ALG_names)/sizeof(struct ALG_name);i++)
		printf("                                     %s\n",
		       ALG_names[i].name);
	fputc('\n', stdout);
}

static struct option opts[] = {
	{ "alg", 1, 0, '1' },
	{0}
};

/* Initialize the match. */
static void
init(struct ipt_entry_match *m, unsigned int *nfcache)
{
	*nfcache |= NFC_UNKNOWN;
}

static void
parse_alg(const unsigned char *s, struct ipt_alg_info *info)
{
	unsigned int i;

	for (i=0; i<sizeof(ALG_names)/sizeof(struct ALG_name); i++)
	{
		if (!strcasecmp(s, ALG_names[i].name)) 
		{
			strncpy(info->name, s, sizeof(info->name));
			return;
		}
	}
	exit_error(PARAMETER_PROBLEM, "Bad ALG Type `%s'", s);
}

/* Function which parses command options; returns true if it
   ate an option */
static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      unsigned int *nfcache,
      struct ipt_entry_match **match)
{
	struct ipt_alg_info *alg = (struct ipt_alg_info *)(*match)->data;

	switch (c) {
	case '1':
		check_inverse(optarg, &invert, &optind, 0);
		parse_alg(argv[optind-1], alg);
		if (invert)
			alg->invert = 1;
		*flags = 1;
		break;

	default:
		return 0;
	}
	return 1;
}

static void
print_alg(char *name, int invert, int numeric)
{
	unsigned int i;

	if (invert)
		fputc('!', stdout);

	for (i = 0; i<sizeof(ALG_names)/sizeof(struct ALG_name); i++)
	{
		if (!strcmp(ALG_names[i].name, name)) 
		{
			printf("%s ", ALG_names[i].name);
			return;
		}
	}
}

/* Final check; must have specified --alg. */
static void
final_check(unsigned int flags)
{
	if (!flags)
		exit_error(PARAMETER_PROBLEM,
			   "alg match: You must specify `--alg'");
}

/* Prints out the matchinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_match *match,
      int numeric)
{
	printf("alg match ");
	print_alg(((struct ipt_alg_info *)match->data)->name,
		  ((struct ipt_alg_info *)match->data)->invert, numeric);
}

/* Saves the union ipt_matchinfo in parsable form to stdout. */
static void
save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
{
	printf("--alg ");
	print_alg(((struct ipt_alg_info *)match->data)->name,
		  ((struct ipt_alg_info *)match->data)->invert, 0);
}

static
struct iptables_match alg
= { NULL,
    "alg",
    IPTABLES_VERSION,
    IPT_ALIGN(sizeof(struct ipt_alg_info)),
    IPT_ALIGN(sizeof(struct ipt_alg_info)),
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
	register_match(&alg);
}
