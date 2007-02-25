/* $Id: agent.h,v 1.1.1.2 2005/03/28 06:56:44 sparq Exp $
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * This file has definitions for the hub and snac interfaces.
 *
 * Copyright (C) 1992 - 1997, 1999 Silcon Graphics, Inc.
 * Copyright (C) 1999 Ralf Baechle (ralf@gnu.org)
 */
#ifndef _ASM_SGI_SN_AGENT_H
#define _ASM_SGI_SN_AGENT_H

#include <linux/config.h>

#include <asm/sn/addrs.h>
#include <asm/sn/arch.h>
//#include <asm/sn/io.h>

#if defined(CONFIG_SGI_IP35) || defined(CONFIG_IA64_SGI_SN1) || defined(CONFIG_IA64_GENERIC)
#include <asm/sn/sn1/bedrock.h>
#endif	/* CONFIG_SGI_IP35 */

/*
 * NIC register macros
 */

#if defined(CONFIG_SGI_IP35) || defined(CONFIG_IA64_SGI_SN1) || defined(CONFIG_IA64_GENERIC)
#define HUB_NIC_ADDR(_cpuid) 						   \
	REMOTE_HUB_ADDR(COMPACT_TO_NASID_NODEID(cputocnode(_cpuid)),       \
		LB_MICROLAN_CTL)
#endif

#define SET_HUB_NIC(_my_cpuid, _val) 				  	   \
	(HUB_S(HUB_NIC_ADDR(_my_cpuid), (_val)))

#define SET_MY_HUB_NIC(_v) 					           \
	SET_HUB_NIC(cpuid(), (_v))

#define GET_HUB_NIC(_my_cpuid) 						   \
	(HUB_L(HUB_NIC_ADDR(_my_cpuid)))

#define GET_MY_HUB_NIC() 						   \
	GET_HUB_NIC(cpuid())

#endif /* _ASM_SGI_SN_AGENT_H */
