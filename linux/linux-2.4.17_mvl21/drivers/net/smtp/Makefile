#  Copyright 2001, Cybertan Corporation
#  All Rights Reserved.
#  
#  This is UNPUBLISHED PROPRIETARY SOURCE CODE of Cybertan Corporation;
#  the contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of Cybertan Corporation.
#
#
# $Id: Makefile,v 1.1.1.2 2005/03/28 06:56:15 sparq Exp $
#

O_TARGET	:= smtp.o

SMTP_OBJS	:= smtp_server_dev.o smtp_ntp.o smtp_utils.o

export-objs	:= $(SMTP_OBJS)
obj-y		:= $(SMTP_OBJS)
obj-m		:= $(O_TARGET)

#SRCBASE		:= /Work/bcm4712/src
#EXTRA_CFLAGS	+= -I$(SRCBASE)/include

#vpath %.c $(SRCBASE)/shared

include $(TOPDIR)/Rules.make

