/*****************************************************************************
**      TNETD53xx Software Support
**      Copyright(c) 2002, Texas Instruments Incorporated. All Rights Reserved.
**
**      FILE: cpmdio.h   User Include for MDIO API Access
**
**      DESCRIPTION:
**              This include file contains definitions for the the MDIO API
**
**      HISTORY:
**              27Mar02 Michael Hanrahan Original (modified from emacmdio.h)
**              04Apr02 Michael Hanrahan Added Interrupt Support
*****************************************************************************/
#ifndef _INC_CPMDIO
#define _INC_CPMDIO


#ifndef __CPHAL_CPMDIO
typedef void PHY_DEVICE;
#endif


/*Version Information */

void cpMacMdioGetVer(bit32u miiBase, bit32u *ModID,  bit32u *RevMaj,  bit32u *RevMin);

/*Called once at the begining of time                                        */

int  cpMacMdioInit(PHY_DEVICE *PhyDev, bit32u miibase, bit32u inst, bit32u PhyMask, bit32u MLinkMask, bit32u MdixMask, bit32u ResetBase, bit32u ResetBit, bit32u MdioBusFreq, bit32u MdioClockFreq, int verbose, void *Info);
int  cpMacMdioGetPhyDevSize(void);


/*Called every 10 mili Seconds, returns TRUE if there has been a mode change */

int cpMacMdioTic(PHY_DEVICE *PhyDev);

/*Called to set Phy mode                                                     */

void cpMacMdioSetPhyMode(PHY_DEVICE *PhyDev,bit32u PhyMode);

/*Calls to retreive info after a mode change!                                */

int  cpMacMdioGetDuplex(PHY_DEVICE *PhyDev);
int  cpMacMdioGetSpeed(PHY_DEVICE *PhyDev);
int  cpMacMdioGetPhyNum(PHY_DEVICE *PhyDev);
int  cpMacMdioGetLinked(PHY_DEVICE *PhyDev);
void cpMacMdioLinkChange(PHY_DEVICE *PhyDev);

/*  Shot Down  */

void cpMacMdioClose(PHY_DEVICE *PhyDev, int Full);


/* Phy Mode Values  */
#define NWAY_AUTOMDIX       (1<<16)
#define NWAY_FD100          (1<<8)
#define NWAY_HD100          (1<<7)
#define NWAY_FD10           (1<<6)
#define NWAY_HD10           (1<<5)
#define NWAY_AUTO           (1<<0)

/*
 *
 *    Tic() return values
 *
 */

#define _MIIMDIO_MDIXFLIP (1<<28)

#define _AUTOMDIX_DELAY_MIN  80  /* milli-seconds*/
#define _AUTOMDIX_DELAY_MAX 200  /* milli-seconds*/

#endif  /*  _INC_CPMDIO */
