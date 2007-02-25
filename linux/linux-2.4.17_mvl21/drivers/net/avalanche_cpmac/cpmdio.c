/***************************************************************************
**  TNETD53xx Software Support
**  Copyright(c) 2002, Texas Instruments Incorporated. All Rights Reserved.
**
**  FILE: cpmdio.c
**
**  DESCRIPTION:
**   MDIO Polling State Machine API. Functions will enable mii-Phy
**   negotiation.
**
**  HISTORY:
**    01Jan01 Denis, Bill      Original
**      27Mar02 Michael Hanrahan (modified from emacmdio.c)
**      07May02 Michael Hanrahan replaced clockwait for code delay
**      10Jul02 Michael Hanrahan more debug, if fallback link is selected
*****************************************************************************/
#define __CPHAL_CPMDIO

#include "mdio_reg.h"

#ifdef _CPHAL_CPMAC
#define mdioPrintf PhyDev->HalDev->OsFunc->Printf
#else
#define mdioPrintf printf
#endif

typedef struct _phy_device
{
   bit32u miibase;
   bit32u inst;
   bit32u PhyState;
   bit32u MdixMask;
   bit32u PhyMask;
   bit32u MLinkMask;
   bit32u PhyMode;
#ifdef _CPHAL_CPMAC
   HAL_DEVICE *HalDev;
#endif
} _PHY_DEVICE;

static void   _mdioDelayEmulate(PHY_DEVICE *PhyDev, int ClockWait);
static void   _mdioWaitForAccessComplete(PHY_DEVICE *PhyDev);
static void   _mdioUserAccess(PHY_DEVICE *PhyDev, bit32u method, bit32u regadr, bit32u phyadr, bit32u data);
static bit32u _mdioUserAccessRead(PHY_DEVICE *PhyDev, bit32u regadr, bit32u phyadr);
static void   _mdioUserAccessWrite(PHY_DEVICE *PhyDev, bit32u regadr, bit32u phyadr, bit32u data);

static void _mdioDisablePhy(PHY_DEVICE *PhyDev,bit32u PhyNum);
static void _mdioPhyTimeOut(PHY_DEVICE *PhyDev);
static void _mdioResetPhy(PHY_DEVICE *PhyDev,bit32u PhyNum);

static void _mdioDumpPhy(PHY_DEVICE *PhyDev, bit32u p);
static void _mdioDumpState(PHY_DEVICE *PhyDev);

/* Auto Mdix */
static void _mdioMdixDelay(PHY_DEVICE *PhyDev);
static int  _mdioMdixSupported(PHY_DEVICE *PhyDev);

static void _MdioDefaultState  (PHY_DEVICE *PhyDev);
static void _MdioFindingState  (PHY_DEVICE *PhyDev);
static void _MdioFoundState    (PHY_DEVICE *PhyDev);
static void _MdioInitState     (PHY_DEVICE *PhyDev);
static void _MdioLinkedState   (PHY_DEVICE *PhyDev);
static void _MdioLinkWaitState (PHY_DEVICE *PhyDev);
static void _MdioLoopbackState (PHY_DEVICE *PhyDev);
static void _MdioNwayStartState(PHY_DEVICE *PhyDev);
static void _MdioNwayWaitState (PHY_DEVICE *PhyDev);



#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==2)
#endif

#define PHY_NOT_FOUND  0xFFFF    /*  Used in Phy Detection */

/*PhyState breakout                                                          */

#define PHY_DEV_OFFSET      (0)
#define PHY_DEV_SIZE        (5)        /*  5 Bits used */
#define PHY_DEV_MASK        (0x1f<<PHY_DEV_OFFSET)

#define PHY_STATE_OFFSET    (PHY_DEV_SIZE+PHY_DEV_OFFSET)
#define PHY_STATE_SIZE      (5)        /* 10 Bits used */
#define PHY_STATE_MASK      (0x1f<<PHY_STATE_OFFSET)
  #define INIT       (1<<PHY_STATE_OFFSET)
  #define FINDING    (2<<PHY_STATE_OFFSET)
  #define FOUND      (3<<PHY_STATE_OFFSET)
  #define NWAY_START (4<<PHY_STATE_OFFSET)
  #define NWAY_WAIT  (5<<PHY_STATE_OFFSET)
  #define LINK_WAIT  (6<<PHY_STATE_OFFSET)
  #define LINKED     (7<<PHY_STATE_OFFSET)
  #define LOOPBACK   (8<<PHY_STATE_OFFSET)

#define PHY_SPEED_OFFSET    (PHY_STATE_OFFSET+PHY_STATE_SIZE)
#define PHY_SPEED_SIZE      (1)     /* 11 Bits used */
#define PHY_SPEED_MASK      (1<<PHY_SPEED_OFFSET)

#define PHY_DUPLEX_OFFSET   (PHY_SPEED_OFFSET+PHY_SPEED_SIZE)
#define PHY_DUPLEX_SIZE     (1)     /* 12 Bits used */
#define PHY_DUPLEX_MASK     (1<<PHY_DUPLEX_OFFSET)

#define PHY_TIM_OFFSET      (PHY_DUPLEX_OFFSET+PHY_DUPLEX_SIZE)
#define PHY_TIM_SIZE        (10)    /* 22 Bits used */
#define PHY_TIM_MASK        (0x3ff<<PHY_TIM_OFFSET)
  #define PHY_FIND_TO (  2<<PHY_TIM_OFFSET)
  #define PHY_RECK_TO (200<<PHY_TIM_OFFSET)
  #define PHY_LINK_TO (500<<PHY_TIM_OFFSET)
  #define PHY_NWST_TO (500<<PHY_TIM_OFFSET)
  #define PHY_NWDN_TO (800<<PHY_TIM_OFFSET)
  #define PHY_MDIX_TO (274<<PHY_TIM_OFFSET) /* 2.74 Seconds <--Spec and empirical */

#define PHY_SMODE_OFFSET    (PHY_TIM_OFFSET+PHY_TIM_SIZE)
#define PHY_SMODE_SIZE      (5)     /* 27 Bits used */
#define PHY_SMODE_MASK      (0x1f<<PHY_SMODE_OFFSET)
  #define SMODE_AUTO   (0x10<<PHY_SMODE_OFFSET)
  #define SMODE_FD100  (0x08<<PHY_SMODE_OFFSET)
  #define SMODE_HD100  (0x04<<PHY_SMODE_OFFSET)
  #define SMODE_FD10   (0x02<<PHY_SMODE_OFFSET)
  #define SMODE_HD10   (0x01<<PHY_SMODE_OFFSET)
  #define SMODE_ALL    (0x1f<<PHY_SMODE_OFFSET)

#define PHY_CHNG_OFFSET    (PHY_SMODE_OFFSET+PHY_SMODE_SIZE)
#define PHY_CHNG_SIZE      (1)     /* 28 Bits used */
#define PHY_CHNG_MASK      (1<<PHY_CHNG_OFFSET)
  #define PHY_CHANGE (1<<PHY_CHNG_OFFSET)

#define PHY_TIMEDOUT_OFFSET    (PHY_CHNG_OFFSET+PHY_CHNG_SIZE)
#define PHY_TIMEDOUT_SIZE      (1)     /*  29 Bits used */
#define PHY_TIMEDOUT_MASK      (1<<PHY_TIMEDOUT_OFFSET)
  #define PHY_MDIX_SWITCH  (1<<PHY_TIMEDOUT_OFFSET)

#define PHY_MDIX_OFFSET    (PHY_TIMEDOUT_OFFSET+PHY_TIMEDOUT_SIZE)
#define PHY_MDIX_SIZE      (1)     /*  30 Bits used */
#define PHY_MDIX_MASK      (1<<PHY_MDIX_OFFSET)
  #define PHY_MDIX     (1<<PHY_MDIX_OFFSET)

static char *lstate[]={"NULL","INIT","FINDING","FOUND","NWAY_START","NWAY_WAIT","LINK_WAIT","LINKED", "LOOPBACK"};
static int cpMacDebug;

/*  Local MDIO Register Macros    */

#define myMDIO_ALIVE           MDIO_ALIVE     (PhyDev->miibase)
#define myMDIO_CONTROL         MDIO_CONTROL   (PhyDev->miibase)
#define myMDIO_LINK            MDIO_LINK      (PhyDev->miibase)
#define myMDIO_LINKINT         MDIO_LINKINT   (PhyDev->miibase)
#define myMDIO_USERACCESS      MDIO_USERACCESS(PhyDev->miibase, PhyDev->inst)
#define myMDIO_USERPHYSEL      MDIO_USERPHYSEL(PhyDev->miibase, PhyDev->inst)
#define myMDIO_VER             MDIO_VER       (PhyDev->miibase)

#ifndef VOLATILE32
#define VOLATILE32(addr) (*((volatile bit32u *)(addr)))
#endif

/************************************
***
*** Delays at least ClockWait cylces
*** before returning
***
**************************************/
void _mdioDelayEmulate(PHY_DEVICE *PhyDev, int ClockWait)
  {
#ifdef _CPHAL_CPMAC                                                  /*+RC3.02*/
  HAL_DEVICE *HalDev = PhyDev->HalDev;                               /*+RC3.02*/
  osfuncSleep((int*)&ClockWait);                                     /*+RC3.02*/
#else                                                                /*+RC3.02*/
  volatile bit32u i=0;
  while(ClockWait--)
    {
    i |= myMDIO_LINK; /*  MDIO register access to burn cycles */
    }
#endif
  }

void _mdioWaitForAccessComplete(PHY_DEVICE *PhyDev)
  {
  while((myMDIO_USERACCESS & MDIO_USERACCESS_GO)!=0)
    {
    }
  }

void _mdioUserAccess(PHY_DEVICE *PhyDev, bit32u method, bit32u regadr, bit32u phyadr, bit32u data)
  {
  bit32u  control;

  control =  MDIO_USERACCESS_GO |
             (method) |
             (((regadr) << 21) & MDIO_USERACCESS_REGADR) |
             (((phyadr) << 16) & MDIO_USERACCESS_PHYADR) |
             ((data) & MDIO_USERACCESS_DATA);

  myMDIO_USERACCESS = control;
  }



/************************************
***
*** Waits for MDIO_USERACCESS to be ready and reads data
*** If 'WaitForData' set, waits for read to complete and returns Data,
*** otherwise returns 0
*** Note: 'data' is 16 bits but we use 32 bits
***        to be consistent with rest of the code.
***
**************************************/
bit32u _mdioUserAccessRead(PHY_DEVICE *PhyDev, bit32u regadr, bit32u phyadr)
  {

  _mdioWaitForAccessComplete(PhyDev);  /* Wait until UserAccess ready */
  _mdioUserAccess(PhyDev, MDIO_USERACCESS_READ, regadr, phyadr, 0);
  _mdioWaitForAccessComplete(PhyDev);  /* Wait for Read to complete */

  return(myMDIO_USERACCESS & MDIO_USERACCESS_DATA);
  }


/************************************
***
*** Waits for MDIO_USERACCESS to be ready and writes data
***
**************************************/
void _mdioUserAccessWrite(PHY_DEVICE *PhyDev, bit32u regadr, bit32u phyadr, bit32u data)
  {
  _mdioWaitForAccessComplete(PhyDev);  /* Wait until UserAccess ready */
  _mdioUserAccess(PhyDev, MDIO_USERACCESS_WRITE, regadr, phyadr, data);
  }

void _mdioDumpPhyDetailed(PHY_DEVICE *PhyDev)
{
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u  PhyNum;
  int     RegData;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  RegData = _mdioUserAccessRead(PhyDev, 0, PhyNum);
  mdioPrintf("PhyControl: %04X, Lookback=%s, Speed=%s, Duplex=%s\n",
    RegData,
    RegData&PHY_LOOP?"On":"Off",
    RegData&PHY_100?"100":"10",
    RegData&PHY_FD?"Full":"Half");
  RegData = _mdioUserAccessRead(PhyDev, 1, PhyNum);
  mdioPrintf("PhyStatus: %04X, AutoNeg=%s, Link=%s\n",
    RegData,
    RegData&NWAY_COMPLETE?"Complete":"NotComplete",
    RegData&PHY_LINKED?"Up":"Down");
  RegData = _mdioUserAccessRead(PhyDev, 4, PhyNum);
  mdioPrintf("PhyMyCapability: %04X, 100FD=%s, 100HD=%s, 10FD=%s, 10HD=%s\n",
    RegData,
    RegData&NWAY_FD100?"Yes":"No",
    RegData&NWAY_HD100?"Yes":"No",
    RegData&NWAY_FD10?"Yes":"No",
    RegData&NWAY_HD10?"Yes":"No");

  RegData = _mdioUserAccessRead(PhyDev, 5, PhyNum);
  mdioPrintf("PhyPartnerCapability: %04X, 100FD=%s, 100HD=%s, 10FD=%s, 10HD=%s\n",
    RegData,
    RegData&NWAY_FD100?"Yes":"No",
    RegData&NWAY_HD100?"Yes":"No",
    RegData&NWAY_FD10?"Yes":"No",
    RegData&NWAY_HD10?"Yes":"No");
}
void _mdioDumpPhy(PHY_DEVICE *PhyDev, bit32u p)
  {
  bit32u j,n,PhyAcks;
  bit32u PhyRegAddr;
  bit32u phy_num;
  bit32u PhyMask  = PhyDev->PhyMask;

  PhyAcks=myMDIO_ALIVE;
  PhyAcks&=PhyMask;   /* Only interested in 'our' Phys */

  for(phy_num=0,j=1;phy_num<32;phy_num++,j<<=1)
    {
    if (PhyAcks&j)
      {
      mdioPrintf("%2d%s:",phy_num,(phy_num==p)?">":" ");
      for(PhyRegAddr=0;PhyRegAddr<6;PhyRegAddr++)
        {
        n = _mdioUserAccessRead(PhyDev, PhyRegAddr, phy_num);
        mdioPrintf(" %04x",n&0x0ffff);
        }
      mdioPrintf("\n");
      }
    }
  _mdioDumpPhyDetailed(PhyDev);
  }

void _mdioDumpState(PHY_DEVICE *PhyDev)
  {
  bit32u state    = PhyDev->PhyState;

  if (!cpMacDebug) return;

  mdioPrintf("Phy: %d, ",(state&PHY_DEV_MASK)>>PHY_DEV_OFFSET);
  mdioPrintf("State: %d/%s, ",(state&PHY_STATE_MASK)>>PHY_STATE_OFFSET,lstate[(state&PHY_STATE_MASK)>>PHY_STATE_OFFSET]);
  mdioPrintf("Speed: %d, ",(state&PHY_SPEED_MASK)>>PHY_SPEED_OFFSET);
  mdioPrintf("Dup: %d, ",(state&PHY_DUPLEX_MASK)>>PHY_DUPLEX_OFFSET);
  mdioPrintf("Tim: %d, ",(state&PHY_TIM_MASK)>>PHY_TIM_OFFSET);
  mdioPrintf("SMode: %d, ",(state&PHY_SMODE_MASK)>>PHY_SMODE_OFFSET);
  mdioPrintf("Chng: %d",(state&PHY_CHNG_MASK)>>PHY_CHNG_OFFSET);
  mdioPrintf("\n");

  if (((state&PHY_STATE_MASK)!=FINDING)&&((state&PHY_STATE_MASK)!=INIT))
    _mdioDumpPhy(PhyDev, (state&PHY_DEV_MASK)>>PHY_DEV_OFFSET);
  }


void _mdioResetPhy(PHY_DEVICE *PhyDev,bit32u PhyNum)
  {
  bit16u PhyControlReg;

  _mdioUserAccessWrite(PhyDev, PHY_CONTROL_REG, PhyNum, PHY_RESET);
  if (cpMacDebug)
    mdioPrintf("cpMacMdioPhYReset(%d)\n",PhyNum);

  /* Read control register until Phy Reset is complete */
  do
   {
    PhyControlReg = _mdioUserAccessRead(PhyDev, PHY_CONTROL_REG, PhyNum);
   }
   while (PhyControlReg & PHY_RESET); /* Wait for Reset to clear */
  }

void _mdioDisablePhy(PHY_DEVICE *PhyDev,bit32u PhyNum)
  {
  _mdioUserAccessWrite(PhyDev, PHY_CONTROL_REG, PhyNum, PHY_ISOLATE|PHY_PDOWN);

  if (cpMacDebug)
    mdioPrintf("cpMacMdioDisablePhy(%d)\n",PhyNum);

  }

void _MdioInitState(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u CurrentState;

  CurrentState=*PhyState;
  CurrentState=(CurrentState&~PHY_TIM_MASK)|(PHY_FIND_TO);
  CurrentState=(CurrentState&~PHY_STATE_MASK)|(FINDING);
  CurrentState=(CurrentState&~PHY_SPEED_MASK);
  CurrentState=(CurrentState&~PHY_DUPLEX_MASK);
  CurrentState|=PHY_CHANGE;

  *PhyState=CurrentState;

  }

void _MdioFindingState(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u  PhyMask  = PhyDev->PhyMask;
  bit32u  PhyNum,i,j,PhyAcks;


  PhyNum=PHY_NOT_FOUND;

  if (*PhyState&PHY_TIM_MASK)
    {
    *PhyState=(*PhyState&~PHY_TIM_MASK)|((*PhyState&PHY_TIM_MASK)-(1<<PHY_TIM_OFFSET));
    }
   else
    {
    PhyAcks=myMDIO_ALIVE;
    PhyAcks&=PhyMask;   /* Only interested in 'our' Phys */

    for(i=0,j=1;(i<32)&&((j&PhyAcks)==0);i++,j<<=1);

    if ((PhyAcks)&&(i<32)) PhyNum=i;
    if (PhyNum!=PHY_NOT_FOUND)
      {
      /*  Phy Found! */
      *PhyState=(*PhyState&~PHY_DEV_MASK)|((PhyNum&PHY_DEV_MASK)<<PHY_DEV_OFFSET);
      *PhyState=(*PhyState&~PHY_STATE_MASK)|(FOUND);
      *PhyState|=PHY_CHANGE;
      if (cpMacDebug)
        mdioPrintf("cpMacMdioFindingState: PhyNum: %d\n",PhyNum);
      }
     else
      {
      if (cpMacDebug)
        mdioPrintf("cpMacMdioFindingState: Timed Out looking for a Phy!\n");
      *PhyState|=PHY_RECK_TO;  /* This state currently has no support?*/
      }
    }
  }

void _MdioFoundState(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState  = &PhyDev->PhyState;
  bit32u  PhyMask   = PhyDev->PhyMask;
  bit32u  MLinkMask = PhyDev->MLinkMask;
  bit32u  PhyNum,PhyStatus,NWAYadvertise,m,phynum,i,j,PhyAcks;
  bit32u  PhySel;

  if ((*PhyState&PHY_SMODE_MASK)==0) return;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  PhyAcks=myMDIO_ALIVE;
  PhyAcks&=PhyMask;   /* Only interested in 'our' Phys */

  /* Will now isolate all our Phys, except the one we have decided to use */
  for(phynum=0,j=1;phynum<32;phynum++,j<<=1)
    {
    if (PhyAcks&j)
      {
        if (phynum!=PhyNum)  /* Do not disabled Found Phy */
          _mdioDisablePhy(PhyDev,phynum);
      }
    }

  /*  Reset the Phy and proceed with auto-negotiation */
  _mdioResetPhy(PhyDev,PhyNum);

  /* Now setup the MDIOUserPhySel register */

  PhySel=PhyNum;  /* Set the phy address */

  /*  Set the way Link will be Monitored */
  /* Check the Link Selection Method */
  if ((1 << PhyNum) & MLinkMask)
    PhySel |= MDIO_USERPHYSEL_LINKSEL;

  myMDIO_USERPHYSEL = PhySel;  /* update PHYSEL */

  /* Get the Phy Status */
  PhyStatus = _mdioUserAccessRead(PhyDev, PHY_STATUS_REG, PhyNum);


#ifdef _CPHAL_CPMAC
  /*  For Phy Internal loopback test, need to wait until Phy
      found, then set Loopback */
  if (PhyDev->HalDev->MdioConnect & _CPMDIO_LOOPBK)
    {
    /* Set Phy in Loopback */
    _mdioUserAccessWrite(PhyDev, PHY_CONTROL_REG, PhyNum, PHY_LOOP|PHY_FD);
    /* Do a read to ensure PHY_LOOP has completed */
    _mdioUserAccessRead(PhyDev, PHY_STATUS_REG, PhyNum);
    *PhyState=(*PhyState&~PHY_STATE_MASK)|(LOOPBACK);
    *PhyState|=PHY_CHANGE;
    return;
    }
#endif


  if (cpMacDebug)
    mdioPrintf("Enable Phy to negotiate external connection\n");

  NWAYadvertise=NWAY_SEL;
  if (*PhyState&SMODE_FD100) NWAYadvertise|=NWAY_FD100;
  if (*PhyState&SMODE_HD100) NWAYadvertise|=NWAY_HD100;
  if (*PhyState&SMODE_FD10)  NWAYadvertise|=NWAY_FD10;
  if (*PhyState&SMODE_HD10)  NWAYadvertise|=NWAY_HD10;

  *PhyState&=~(PHY_TIM_MASK|PHY_STATE_MASK);
  if ((PhyStatus&NWAY_CAPABLE)&&(*PhyState&SMODE_AUTO))   /*NWAY Phy Detected*/
    {
    /*For NWAY compliant Phys                                                */

    _mdioUserAccessWrite(PhyDev, NWAY_ADVERTIZE_REG, PhyNum, NWAYadvertise);

    if (cpMacDebug)
      {
      mdioPrintf("NWAY Advertising: ");
      if (NWAYadvertise&NWAY_FD100) mdioPrintf("FullDuplex-100 ");
      if (NWAYadvertise&NWAY_HD100) mdioPrintf("HalfDuplex-100 ");
      if (NWAYadvertise&NWAY_FD10)  mdioPrintf("FullDuplex-10 ");
      if (NWAYadvertise&NWAY_HD10)  mdioPrintf("HalfDuplex-10 ");
      mdioPrintf("\n");
      }

    _mdioUserAccessWrite(PhyDev, PHY_CONTROL_REG, PhyNum, AUTO_NEGOTIATE_EN);

    _mdioUserAccessWrite(PhyDev, PHY_CONTROL_REG, PhyNum, AUTO_NEGOTIATE_EN|RENEGOTIATE);

    *PhyState|=PHY_CHANGE|PHY_NWST_TO|NWAY_START;
    }
   else
    {
    *PhyState&=~SMODE_AUTO;   /*The Phy is not capable of auto negotiation!  */
    m=NWAYadvertise;
    for(j=0x8000,i=0;(i<16)&&((j&m)==0);i++,j>>=1);
    m=j;
    j=0;
    if (m&(NWAY_FD100|NWAY_HD100))
      {
      j=PHY_100;
      m&=(NWAY_FD100|NWAY_HD100);
      }
    if (m&(NWAY_FD100|NWAY_FD10))
      j |= PHY_FD;
    if (cpMacDebug)
      mdioPrintf("Requested PHY mode %s Duplex %s Mbps\n",(j&PHY_FD)?"Full":"Half",(j&PHY_100)?"100":"10");
    _mdioUserAccessWrite(PhyDev, PHY_CONTROL_REG, PhyNum, j);
    *PhyState&=~PHY_SPEED_MASK;
    if (j&PHY_100)
      *PhyState|=(1<<PHY_SPEED_OFFSET);
    *PhyState&=~PHY_DUPLEX_MASK;
    if (j&PHY_FD)
      *PhyState|=(1<<PHY_DUPLEX_OFFSET);
    *PhyState|=PHY_CHANGE|PHY_LINK_TO|LINK_WAIT;
    }
     _mdioMdixDelay(PhyDev);  /* If AutoMdix add delay */
  }

void _MdioNwayStartState(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u PhyNum,PhyMode;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  /*Wait for Negotiation to start                                            */

  PhyMode=_mdioUserAccessRead(PhyDev, PHY_CONTROL_REG, PhyNum);

  if((PhyMode&RENEGOTIATE)==0)
    {
    _mdioUserAccessRead(PhyDev, PHY_STATUS_REG, PhyNum); /*Flush pending latch bits*/
    *PhyState&=~(PHY_STATE_MASK|PHY_TIM_MASK);
    *PhyState|=PHY_CHANGE|NWAY_WAIT|PHY_NWDN_TO;
    _mdioMdixDelay(PhyDev);  /* If AutoMdix add delay */
    }
   else
    {
    if (*PhyState&PHY_TIM_MASK)
      *PhyState=(*PhyState&~PHY_TIM_MASK)|((*PhyState&PHY_TIM_MASK)-(1<<PHY_TIM_OFFSET));
     else
      _mdioPhyTimeOut(PhyDev);
    }
  }

void _MdioNwayWaitState(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u  PhyNum,PhyStatus,NWAYadvertise,NWAYREadvertise,NegMode,i,j;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  PhyStatus=_mdioUserAccessRead(PhyDev, PHY_STATUS_REG, PhyNum);

  if (PhyStatus&NWAY_COMPLETE)
    {
    *PhyState|=PHY_CHANGE;
    *PhyState&=~PHY_SPEED_MASK;
    *PhyState&=~PHY_DUPLEX_MASK;

    NWAYadvertise   =_mdioUserAccessRead(PhyDev, NWAY_ADVERTIZE_REG, PhyNum);
    NWAYREadvertise =_mdioUserAccessRead(PhyDev, NWAY_REMADVERTISE_REG, PhyNum);

    /* Negotiated mode is we and the remote have in common */
    NegMode = NWAYadvertise & NWAYREadvertise;

    if (cpMacDebug)
      {
       mdioPrintf("Phy: %d, ",(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET);
       mdioPrintf("NegMode %04X, NWAYadvertise %04X, NWAYREadvertise %04X\n",
         NegMode,  NWAYadvertise, NWAYREadvertise);
      }

    /* Limit negotiation to fields below */
    NegMode &= (NWAY_FD100|NWAY_HD100|NWAY_FD10|NWAY_HD10);

    if (NegMode==0)
      {
      NegMode=(NWAY_HD100|NWAY_HD10)&NWAYadvertise; /*or 10 ?? who knows, Phy is not MII compliant*/
      if(cpMacDebug)
        {
         mdioPrintf("Mdio:WARNING: Negotiation complete but NO agreement, default is HD\n");
         _mdioDumpPhyDetailed(PhyDev);
        } 
      }
    for(j=0x8000,i=0;(i<16)&&((j&NegMode)==0);i++,j>>=1);


    NegMode=j;
    if (cpMacDebug)
      {
      mdioPrintf("Negotiated connection: ");
      if (NegMode&NWAY_FD100) mdioPrintf("FullDuplex 100 Mbs\n");
      if (NegMode&NWAY_HD100) mdioPrintf("HalfDuplex 100 Mbs\n");
      if (NegMode&NWAY_FD10) mdioPrintf("FullDuplex 10 Mbs\n");
      if (NegMode&NWAY_HD10) mdioPrintf("HalfDuplex 10 Mbs\n");
      }
    if (NegMode!=0)
      {
      if (PhyStatus&PHY_LINKED)
        *PhyState=(*PhyState&~PHY_STATE_MASK)|LINKED;
       else
        *PhyState=(*PhyState&~PHY_STATE_MASK)|LINK_WAIT;
      if (NegMode&(NWAY_FD100|NWAY_HD100))
        *PhyState=(*PhyState&~PHY_SPEED_MASK)|(1<<PHY_SPEED_OFFSET);
      if (NegMode&(NWAY_FD100|NWAY_FD10))
        *PhyState=(*PhyState&~PHY_DUPLEX_MASK)|(1<<PHY_DUPLEX_OFFSET);
      }
    }
   else
    {
    if (*PhyState&PHY_TIM_MASK)
      *PhyState=(*PhyState&~PHY_TIM_MASK)|((*PhyState&PHY_TIM_MASK)-(1<<PHY_TIM_OFFSET));
     else
      _mdioPhyTimeOut(PhyDev);
    }
  }

void _MdioLinkWaitState(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u  PhyStatus;
  bit32u  PhyNum;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  PhyStatus=_mdioUserAccessRead(PhyDev, PHY_STATUS_REG, PhyNum);

  if (PhyStatus&PHY_LINKED)
    {
    *PhyState=(*PhyState&~PHY_STATE_MASK)|LINKED;
    *PhyState|=PHY_CHANGE;
    }
   else
    {
    if (*PhyState&PHY_TIM_MASK)
      *PhyState=(*PhyState&~PHY_TIM_MASK)|((*PhyState&PHY_TIM_MASK)-(1<<PHY_TIM_OFFSET));
     else
      _mdioPhyTimeOut(PhyDev);
    }
  }

void _mdioPhyTimeOut(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState;

  if(_mdioMdixSupported(PhyDev) == 0)
    return;  /* AutoMdix not supported */

  PhyState = &PhyDev->PhyState;

  /*  Indicate MDI/MDIX mode switch is needed */
  *PhyState|=PHY_MDIX_SWITCH;

  /* Toggle the MDIX mode indicatir */
  if(*PhyState & PHY_MDIX)
    *PhyState &= ~PHY_MDIX_MASK;       /* Current State is MDIX, set to MDI */
  else
    *PhyState |=  PHY_MDIX_MASK;      /* Current State is MDI, set to MDIX */

  /* Reset state machine to FOUND */
  *PhyState=(*PhyState&~PHY_STATE_MASK)|(FOUND);
  }

void _MdioLoopbackState(PHY_DEVICE *PhyDev)
  {
  return;
  }

void _MdioLinkedState(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u  PhyNum   = (*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  if (myMDIO_LINK&(1<<PhyNum)) return;  /* if still Linked, exit*/

  /* Not Linked */
  *PhyState&=~(PHY_STATE_MASK|PHY_TIM_MASK);
  if (*PhyState&SMODE_AUTO)
    *PhyState|=PHY_CHANGE|NWAY_WAIT|PHY_NWDN_TO;
   else
    *PhyState|=PHY_CHANGE|PHY_LINK_TO|LINK_WAIT;
    
  _mdioMdixDelay(PhyDev);  /* If AutoMdix add delay */
  }

void _MdioDefaultState(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  /*Awaiting a cpMacMdioInit call                                             */
  *PhyState|=PHY_CHANGE;
  }


/*User Calls*********************************************************       */

void cpMacMdioClose(PHY_DEVICE *PhyDev, int Full)
  {
  }


int cpMacMdioInit(PHY_DEVICE *PhyDev, bit32u miibase, bit32u inst, bit32u PhyMask, bit32u MLinkMask, bit32u MdixMask, bit32u ResetReg, bit32u ResetBit, bit32u MdioBusFreq, bit32u MdioClockFreq, int verbose, void *Info)
  {
  bit32u HighestChannel;
  bit32u ControlState;
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u clkdiv;                                                  /*MJH+030328*/

  cpMacDebug=verbose;  
 
  PhyDev->miibase   = miibase;
  PhyDev->inst      = inst;
  PhyDev->PhyMask   = PhyMask;
  PhyDev->MLinkMask = MLinkMask;
  PhyDev->MdixMask  = MdixMask;
#ifdef _CPHAL_CPMAC
  PhyDev->HalDev    = (HAL_DEVICE*) Info;
#endif

  *PhyState &= ~PHY_MDIX_MASK;   /* Set initial State to MDI */
   
  /* Check that the channel supplied is within range */
  HighestChannel = (myMDIO_CONTROL & MDIO_CONTROL_HIGHEST_USER_CHANNEL) > 8;
  if(inst > HighestChannel)
    return(HighestChannel);

  /*Setup MII MDIO access regs                                              */

  /* Calculate the correct value for the mclkdiv */
  /* See PITS #14 */
  if (MdioClockFreq)                                              /*MJH+030402*/
     clkdiv = (MdioBusFreq / MdioClockFreq) - 1;                  /*MJH+030402*/
  else                                                            /*MJH+030402*/
     clkdiv = 0xFF;                                               /*MJH+030402*/

  ControlState  = MDIO_CONTROL_ENABLE;
  ControlState |= (clkdiv & MDIO_CONTROL_CLKDIV);                 /*MJH+030328*/

  /*
      If mii is not out of reset or if the Control Register is not set correctly
      then initalize
  */
  if( !(VOLATILE32(ResetReg) & (1 << ResetBit)) ||
     ((myMDIO_CONTROL & (MDIO_CONTROL_CLKDIV | MDIO_CONTROL_ENABLE)) != ControlState) )/*GSG~030404*/
    {
    /*  MII not setup, Setup initial condition  */
    VOLATILE32(ResetReg) &= ~(1 << ResetBit);
    _mdioDelayEmulate(PhyDev, 64);
    VOLATILE32(ResetReg) |= (1 << ResetBit);  /* take mii out of reset  */
    _mdioDelayEmulate(PhyDev, 64);
    myMDIO_CONTROL = ControlState;  /* Enable MDIO   */
    }

  *PhyState=INIT;

  if (cpMacDebug)
    mdioPrintf("cpMacMdioInit\n");
  _mdioDumpState(PhyDev);
  return(0);
  }

void cpMacMdioSetPhyMode(PHY_DEVICE *PhyDev,bit32u PhyMode)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u CurrentState;

  PhyDev->PhyMode = PhyMode;   /* used for AUTOMIDX, planned to replace PhyState fields */

  *PhyState&=~PHY_SMODE_MASK;

  if (PhyMode&NWAY_AUTO)  *PhyState|=SMODE_AUTO;
  if (PhyMode&NWAY_FD100) *PhyState|=SMODE_FD100;
  if (PhyMode&NWAY_HD100) *PhyState|=SMODE_HD100;
  if (PhyMode&NWAY_FD10)  *PhyState|=SMODE_FD10;
  if (PhyMode&NWAY_HD10)  *PhyState|=SMODE_HD10;

  CurrentState=*PhyState&PHY_STATE_MASK;
  if ((CurrentState==NWAY_START)||
      (CurrentState==NWAY_WAIT) ||
      (CurrentState==LINK_WAIT) ||
      (CurrentState==LINKED)      )
    *PhyState=(*PhyState&~PHY_STATE_MASK)|FOUND|PHY_CHANGE;
  if (cpMacDebug)
    mdioPrintf("cpMacMdioSetPhyMode:%08X Auto:%d, FD10:%d, HD10:%d, FD100:%d, HD100:%d\n", PhyMode,
     PhyMode&NWAY_AUTO, PhyMode&NWAY_FD10, PhyMode&NWAY_HD10, PhyMode&NWAY_FD100,
     PhyMode&NWAY_HD100);
  _mdioDumpState(PhyDev);
  }

/* cpMacMdioTic is called every 10 mili seconds to process Phy states         */

int cpMacMdioTic(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u  CurrentState;

  /*Act on current state of the Phy                                          */

  CurrentState=*PhyState;
  switch(CurrentState&PHY_STATE_MASK)
    {
    case INIT:       _MdioInitState(PhyDev);      break;
    case FINDING:    _MdioFindingState(PhyDev);   break;
    case FOUND:      _MdioFoundState(PhyDev);     break;
    case NWAY_START: _MdioNwayStartState(PhyDev); break;
    case NWAY_WAIT:  _MdioNwayWaitState(PhyDev);  break;
    case LINK_WAIT:  _MdioLinkWaitState(PhyDev);  break;
    case LINKED:     _MdioLinkedState(PhyDev);    break;
    case LOOPBACK:   _MdioLoopbackState(PhyDev);  break;
    default:         _MdioDefaultState(PhyDev);   break;
    }

  /*Dump state info if a change has been detected                            */

  if ((CurrentState&~PHY_TIM_MASK)!=(*PhyState&~PHY_TIM_MASK))
    _mdioDumpState(PhyDev);

  /*  Check is MDI/MDIX mode switch is needed */
  if(*PhyState & PHY_MDIX_SWITCH)
    {
    bit32u Mdix;

    *PhyState &= ~PHY_MDIX_SWITCH;  /* Clear Mdix Flip indicator */

    if(*PhyState & PHY_MDIX)
      Mdix = 1;
    else
      Mdix = 0;
    return(_MIIMDIO_MDIXFLIP|Mdix);
    }

  /*Return state change to user                                              */

  if (*PhyState&PHY_CHNG_MASK)
    {
    *PhyState&=~PHY_CHNG_MASK;
    return(1);
    }
   else
    return(0);
  }

/* cpMacMdioGetDuplex is called to retrieve the Duplex info                   */

int cpMacMdioGetDuplex(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  return((*PhyState&PHY_DUPLEX_MASK)?1:0);  /* return 0 or a 1  */
  }

/* cpMacMdioGetSpeed is called to retreive the Speed info                     */

int cpMacMdioGetSpeed(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  return(*PhyState&PHY_SPEED_MASK);
  }

/* cpMacMdioGetPhyNum is called to retreive the Phy Device Adr info           */

int cpMacMdioGetPhyNum(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  return((*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET);
  }

/* cpMacMdioGetLoopback is called to Determine if the LOOPBACK state has been reached*/

int cpMacMdioGetLoopback(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  return((*PhyState&PHY_STATE_MASK)==LOOPBACK);
  }
/* cpMacMdioGetLinked is called to Determine if the LINKED state has been reached*/

int cpMacMdioGetLinked(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  return((*PhyState&PHY_STATE_MASK)==LINKED);
  }

void cpMacMdioLinkChange(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u  PhyNum,PhyStatus;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  if (cpMacMdioGetLinked(PhyDev))
    {
    PhyStatus=_mdioUserAccessRead(PhyDev, PHY_STATUS_REG, PhyNum);

    if ((PhyStatus&PHY_LINKED)==0)
      {
      *PhyState&=~(PHY_TIM_MASK|PHY_STATE_MASK);
      if (*PhyState&SMODE_AUTO)
        {
        _mdioUserAccessWrite(PhyDev, PHY_CONTROL_REG, PhyNum, AUTO_NEGOTIATE_EN|RENEGOTIATE);
        *PhyState|=PHY_CHANGE|PHY_NWST_TO|NWAY_START;
        }
       else
        {
        *PhyState|=PHY_CHANGE|PHY_LINK_TO|LINK_WAIT;
        }
      }
    }
  }

void cpMacMdioGetVer(bit32u miibase, bit32u *ModID,  bit32u *RevMaj,  bit32u *RevMin)
  {
  bit32u  Ver;

  Ver = MDIO_VER(miibase);

  *ModID  = (Ver & MDIO_VER_MODID) >> 16;
  *RevMaj = (Ver & MDIO_VER_REVMAJ) >> 8;
  *RevMin = (Ver & MDIO_VER_REVMIN);
  }

int cpMacMdioGetPhyDevSize(void)
  {
  return(sizeof(PHY_DEVICE));
  }

  /* returns 0 if current Phy has AutoMdix support, otherwise 0 */
int _mdioMdixSupported(PHY_DEVICE *PhyDev)
  {
  bit32u *PhyState = &PhyDev->PhyState;
  bit32u PhyNum;

  if((PhyDev->PhyMode & NWAY_AUTOMDIX) == 0)
    return(0);  /* AutoMdix not turned on */

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;
  if( ((1<<PhyNum) & PhyDev->MdixMask) == 0)
    return(0);  /*  Phy does not support AutoMdix*/

  return(1);
  }

/* If current Phy has AutoMdix support add Mdix Delay to the Timer State Value */
void _mdioMdixDelay(PHY_DEVICE *PhyDev)
  {
  int Delay;
  bit32u *PhyState = &PhyDev->PhyState;  
#ifdef _CPHAL_CPMAC
  HAL_DEVICE *HalDev = PhyDev->HalDev;
#endif

  if(_mdioMdixSupported(PhyDev) == 0)
    return;  /* AutoMdix not supported */
/* Currently only supported when used with the CPMAC */
#ifdef _CPHAL_CPMAC
  /* Get the Delay value in milli-seconds and convert to ten-milli second value */
  Delay = cpmacRandomRange(HalDev, _AUTOMDIX_DELAY_MIN, _AUTOMDIX_DELAY_MAX);
  Delay /= 10;

  /*  Add AutoMidx Random Switch Delay to AutoMdix Link Delay */

  Delay += (PHY_MDIX_TO>>PHY_TIM_OFFSET);

  /* Change Timeout value to AutoMdix standard */
  *PhyState &= ~(PHY_TIM_MASK);  /* Clear current Time out value */
  *PhyState |=  (Delay<<PHY_TIM_OFFSET);     /* Set new value */
#endif
  }


