/* $Id: fixenv.S,v 1.1.1.2 2005/03/28 06:55:59 sparq Exp $
 *  
 *   Copyright(C) Texas Instruments, 2000-2001
*/               

#include <asm/asm.h>
#include <asm/regdef.h>

#include "quick.h"
        
        .text

        .set noreorder
        .set noat
        
	quick_link (adam2_fixenv, 0, 29)
        
        .set at
