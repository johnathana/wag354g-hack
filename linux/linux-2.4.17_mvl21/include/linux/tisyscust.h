/*###########################################################################
# Copyright (C) 2003 Texas Instruments Incorporated                         #
#                                                                           #
# This program is free software; you can redistribute it and/or modify it   #
# under the terms of the GNU General Public License as published by the Free#
# Software Foundation; either version 2 of the License, or (at your option) #
# any later version.                                                        #
#                                                                           #
# This program is distributed in the hope that it will be useful, but       #
# WITHOUT  ANY WARRANTY; without even the                                   #
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  #
# See the GNU General Public License for more details.                      #
#                                                                           #
# You should have received a copy of the GNU General Public License along   #
# with this program; if not, write to the Free Software Foundation, Inc.,   #
# 59 Temple  Place - Suite 330, Boston, MA  02111-1307, USA.                #
###########################################################################*/
/*
  ti_syscall custom implementation
  FILE: tisyscust.h
*/
#ifndef _TI_SYSCUST_H_
#define _TI_SYSCUST_H_

#ifdef __KERNEL__

#include <linux/config.h>
#ifdef CONFIG_TI_CUSTOM_SYS_CALLS

//#define NR_TI_SYSCALLS   16
#define NR_TI_SYSCALLS   CONFIG_NR_TI_CUSTOM_SYSCALLS

typedef int (*ti_syscall_t)(void *param);
extern ti_syscall_t ti_syscalls[NR_TI_SYSCALLS];

asmlinkage int sys_ti_custom(int nfunc, void* param, int *ret_val);

#endif

#endif

#endif /* _TI_SYSCUST_H_  */
