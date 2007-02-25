/*
 * time.c: Extracting time information from ARCS prom.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 *
 * $Id: time.c,v 1.1.1.2 2005/03/28 06:55:53 sparq Exp $
 */
#include <linux/init.h>
#include <asm/sgialib.h>

struct linux_tinfo * __init prom_gettinfo(void)
{
	return romvec->get_tinfo();
}

unsigned long __init prom_getrtime(void)
{
	return romvec->get_rtime();
}
