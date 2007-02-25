/*
 * linux/include/asm-arm/arch-ebsa285/serial.h
 *
 * Copyright (c) 1996,1997,1998 Russell King.
 *
 * Changelog:
 *  15-10-1996	RMK	   Created
 *  25-05-1998	PJB	   CATS support
 *  26-Mar-2000 Uday Naik  Created for IXP1200 EVB
 */

#ifndef __ASM_ARCH_SERIAL_H
#define __ASM_ARCH_SERIAL_H

#include "irqs.h"

#define BASE_BAUD (3686400 / 16)

/* Base port for UART */
#define UART_PORT CSR_UARTSR

/* serial port flags */
#define STD_COM_FLAGS (ASYNC_BOOT_AUTOCONF | ASYNC_SKIP_TEST)

/* UART CLK        PORT  IRQ     FLAGS        */
#define SERIAL_PORT_DFNS \
	{ 0, BASE_BAUD, UART_PORT, IRQ_UART, STD_COM_FLAGS } /* ttyS0 */	

#endif

