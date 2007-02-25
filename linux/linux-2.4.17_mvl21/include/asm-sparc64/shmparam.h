/* $Id: shmparam.h,v 1.1.1.2 2005/03/28 06:56:51 sparq Exp $ */
#ifndef _ASMSPARC64_SHMPARAM_H
#define _ASMSPARC64_SHMPARAM_H

#include <asm/spitfire.h>

/* attach addr a multiple of this */
#define	SHMLBA	((PAGE_SIZE > L1DCACHE_SIZE) ? PAGE_SIZE : L1DCACHE_SIZE)

#endif /* _ASMSPARC64_SHMPARAM_H */
