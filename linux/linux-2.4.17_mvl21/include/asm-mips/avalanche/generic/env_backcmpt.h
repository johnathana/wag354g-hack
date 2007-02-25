
/*
 * env_backcmpt.h
 *   This file supports the backward compatibility with Adam2 environment
 *   variables support. This is for OS co-working on boards that run Adam2 as
 *   bootloader.
 */

#if 0
#include <sbl/env.h>
#include <sbl/stddef.h>
#endif

/* 
 * This array provides the mapping of the new env variables with the Adam2
 * counter parts.
 */
typedef struct ADAM2ENVDESC {
	char        *new_nm;
	char        *nm;
} ADAM2ENVDESC;
ADAM2ENVDESC env_adam2_alias[] = {
	{"CPUFREQ",      "cpufrequency",        },
	{"MEMSZ",        "memsize",             },
	{"FLASHSZ",      "flashsize",           },
	{"MODETTY0",     "modetty0",            },
	{"MODETTY1",     "modetty1",            },
	{"PROMPT",       "prompt",              },
	{"BOOTCFG",      "bootcfg",              },
	{"HWA_0",        "maca",                },
	{"HWA_1",        "macb",                },
	{"HWA_RNDIS",    "usb_board_mac",       },
	{"HWA_3",        "macc",                },
	{"IPA",          "my_ipaddress",        },
	{"IPA_SVR",      "remote_ipaddress",    },
	{"IPA_GATEWAY",  "ipa_gateway",         },
	{"SUBNET_MASK",  "subnet_mask",         },
	{"BLINE_MAC0",   "bootline1",           },
	{"BLINE_MAC1",   "bootline2",           },
	{"BLINE_RNDIS",  "rndisbootline",       },
	{"BLINE_ATM",    "atmbootline",         },
	{"USB_PID",      "usb_prod_id",         },
	{"USB_VID",      "usb_vend_id",         },
	{"USB_EPPOLLI",  "usb_ep_poll",         },
    {"USB_SERIAL",   "usb_serial",          },
   	{"HWA_HRNDIS",   "usb_rndis_mac",      }
};
