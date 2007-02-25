/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 * Jeff Harrell, jharrell@ti.com
 * Copyright (C) 2000,2001 Texas Instruments, Inc.  All rights reserved.
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 * 
 * Display routines for display messages in MIPS boards ascii display.
 *
 */

#include <linux/config.h>
#include <asm/mips-boards/generic.h>


void mips_display_message(const char *str)
{
#if defined(CONFIG_MIPS_SEAD2)
        volatile unsigned int *display = (void *)SEAD_ASCII_DISPLAY_POS_BASE;
	int i;

        for (i = 0; i <= 14; i+=2)
          {
            if (*str)
              display[i] = *str++;
            else
              display[i] = ' ';
          }
#endif /* CONFIG_MIPS_SEAD2 */
}

void mips_display_word(unsigned int num)
{
#if (CONFIG_MIPS_SEAD2)
        volatile char *display = (void *)SEAD_ASCII_DISPLAY_WORD_BASE; 
	*display = num;
#endif /* CONFIG_MIPS_SEAD2 */
}
