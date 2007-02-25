/**@file*********************************************************************
 *  TNETDxxxx Software Support
 *  Copyright (c) 2003 Texas Instruments Incorporated. All Rights Reserved.
 *
 *  FILE: CPSWHALCOMMON_STDDEF.H
 *
 *  DESCRIPTION:
 *      Standard definitions needed for CPSWHAL
 *
 *  HISTORY:
 *      Date      Modifier              Ver    Notes
 *      01Oct03   Michael Hanrahan      1.00   
 *****************************************************************************/
#ifndef _CPSWHALCOMMON_STDDEF_H
#define _CPSWHALCOMMON_STDDEF_H

#ifndef _INC_ADAM2

#ifndef size_t
#define size_t unsigned int
#endif

typedef char           bit8;
typedef short          bit16;
typedef int            bit32;

typedef unsigned char  bit8u;
typedef unsigned short bit16u;
typedef unsigned int   bit32u;
#endif

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE !(TRUE)
#endif

#ifndef NULL
#define NULL 0
#endif

typedef const char HAL_CONTROL_KEY;
#endif
