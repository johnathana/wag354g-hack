/*
 * linux/include/asm-arm/arch-ixp1200/uncompress.h
 *
 * Copyright (C) 1996,1997,1998 Russell King
 *
 * March 26 2000  Uday Naik     Created
 */

/* At this point, the MMU is not on, so use physical addresses */

#ifndef MMU_ON

#define PHYS(x)         ((volatile unsigned long*)(0x90000000 + x))
#define LED            ((volatile unsigned long*)0x38508000)

#else

#define PHYS(x)         ((volatile unsigned long*)(0xf0000000 + x))
#define LED            ((volatile unsigned long*)0xf0300000)

#endif

#define UARTDR		PHYS(0x3c00)
#define UARTSR		PHYS(0x3400)
#define UARTCR		PHYS(0x3800)

/* Set the LED to a */
#define SETLED(a)          *LED = a

/* Set the LED to a and loop forever */
#define SETLEDFOREVER(a)   {*LED = a; while(1);}

/*
 * The following code assumes the serial port has already been
 * initialized by the bootloader or such...
 */

#define TXFULL  (1 << 7)

/* write a character to the serial port */
#define PUTC(c)            {while (*UARTSR & TXFULL); *UARTDR = c;} 

/* write a string to the serial port */
#define PUTS(str)            {                     \
 char* s = str;                                    \
 while (*s) {                                      \
   PUTC(*s)                                        \
   if (*s == 10)                                   \
     PUTC(13)                                      \
   s++;                                            \
 }                                                 \
} 

   
/* write a long word to the serial port */
#define PUTLONG(x)          { \
  int i; \
  int j; \
  for (i = 28; i >=0; i-=4) { \
    j = (x >> i) & 0xf; \
    if (j < 10) \
     j += '0'; \
    else \
     j += ('a' - 10); \
    PUTC(j) \
  } \
} 

/* write a long word to the serial port */
#define PUTB(x)          { \
  int i; \
  int j; \
  for (i = 4; i >=0; i-=4) { \
    j = (x >> i) & 0xf; \
    if (j < 10) \
     j += '0'; \
    else \
     j += ('a' - 10); \
    PUTC(j) \
  } \
} 


/*
 * nothing to do
 */

#define arch_decomp_setup()
#define arch_decomp_wdog()

static void puts( const char *s )
{
    int i;
    
    for (i = 0; *s; i++, s++) {

	/* wait for space in the UART's transmiter */
	while (*UARTSR & TXFULL);

	/* send the character out. */
	*UARTDR = *s;

	/* if a LF, also do CR... */
	if (*s == 10) {

	  /* wait for space in the UART's transmiter */
	  while (*UARTSR & TXFULL);

	  /* send the CR character out. */
	  *UARTDR = 13;
	  
	}

    }

}





