#include <linux/kernel.h>
#include <linux/module.h>
#include "emacmdio.h"

void _EmacMdioWaitForAccessComplete(u32 macbase);
void _EmacMdioDumpState(u32 macbase, u32 state);
void _EmacMdioDumpPhy(u32 macbase,u32 p);
void _EmacMdioResetPhy(u32 macbase,u32 PhyNum);
void _EmacMdioDisablePhy(u32 macbase,u32 PhyNum);
void _EmacMdioPhyTimeOut(u32 macbase,u32 *PhyState);

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==2)
#endif

#define MDIO_GO    0x80000000
#define MDIO_WRITE 0x40000000
#define MDIO_READ  0x00000000

#define PHY_CONTROL_REG       0
  #define PHY_RESET           (1<<15)
  #define PHY_LOOP            (1<<14)
  #define PHY_100             (1<<13)
  #define AUTO_NEGOTIATE_EN   (1<<12)
  #define PHY_PDOWN           (1<<11)
  #define PHY_ISOLATE         (1<<10)
  #define RENEGOTIATE         (1<<9)
  #define PHY_FD              (1<<8)

#define PHY_STATUS_REG        1
  #define NWAY_COMPLETE       (1<<5)
  #define NWAY_CAPABLE        (1<<3)
  #define PHY_LINKED          (1<<2)

#define NWAY_ADVERTIZE_REG    4
#define NWAY_REMADVERTISE_REG 5
  #define NWAY_FD100          (1<<8)
  #define NWAY_HD100          (1<<7)
  #define NWAY_FD10           (1<<6)
  #define NWAY_HD10           (1<<5)
  #define NWAY_SEL            (1<<0)
  #define NWAY_AUTO           (1<<0)

#ifdef CONFIG_MIPS_ACPEP
#define LXT971_LED_REG	0x14
#define LXT_LED_STRETCH 0x02
  enum LXT971_LED_CFG 
 {
  LXT971_LED_SPEED,
  LXT971_LED_TX,
  LXT971_LED_RX,
  LXT971_LED_COLL,
  LXT971_LED_LINK,
  LXT971_LED_DUPLEX,
  LXT971_LED_UNUSED1,
  LXT971_LED_RXTX,
  LXT971_LED_TST_CONT_ON,
  LXT971_LED_TST_CONT_OFF,
  LXT971_LED_TST_CONT_FAST,
  LXT971_LED_TST_CONT_SLOW,
  LXT971_LED_LINK_RX,
  LXT971_LED_LINK_ACT,
  LXT971_LED_DUPLEX_COLL,
  LX9171_LED_UNUSED2
  };
#endif 

#define MDIOCONTROLi    (*(volatile unsigned *)(macbase+0x380))
#define MDIOUSERACCESSi (*(volatile unsigned *)(macbase+0x384))
#define MDIOACKi        (*(volatile unsigned *)(macbase+0x388))
#define MDIOLINKi       (*(volatile unsigned *)(macbase+0x38C))
#define MDIOMACPHYi     (*(volatile unsigned *)(macbase+0x390))


/*PhyState breakout                                                          */

#define PHY_DEV_OFFSET      (0)
#define PHY_DEV_SIZE        (5)
#define PHY_DEV_MASK        (0x1f<<PHY_DEV_OFFSET)

#define PHY_STATE_OFFSET    (PHY_DEV_SIZE+PHY_DEV_OFFSET)
#define PHY_STATE_SIZE      (5)
#define PHY_STATE_MASK      (0x1f<<PHY_STATE_OFFSET)
  #define INIT       (1<<PHY_STATE_OFFSET)
  #define FINDING    (2<<PHY_STATE_OFFSET)
  #define FOUND      (3<<PHY_STATE_OFFSET)
  #define NWAY_START (4<<PHY_STATE_OFFSET)
  #define NWAY_WAIT  (5<<PHY_STATE_OFFSET)
  #define LINK_WAIT  (6<<PHY_STATE_OFFSET)
  #define LINKED     (7<<PHY_STATE_OFFSET)

#define PHY_SPEED_OFFSET    (PHY_STATE_OFFSET+PHY_STATE_SIZE)
#define PHY_SPEED_SIZE      (1)
#define PHY_SPEED_MASK      (1<<PHY_SPEED_OFFSET)

#define PHY_DUPLEX_OFFSET   (PHY_SPEED_OFFSET+PHY_SPEED_SIZE)
#define PHY_DUPLEX_SIZE     (1)
#define PHY_DUPLEX_MASK     (1<<PHY_DUPLEX_OFFSET)

#define PHY_TIM_OFFSET      (PHY_DUPLEX_OFFSET+PHY_DUPLEX_SIZE)
#define PHY_TIM_SIZE        (10)
#define PHY_TIM_MASK        (0x3ff<<PHY_TIM_OFFSET)
  #define PHY_FIND_TO (  2<<PHY_TIM_OFFSET)
  #define PHY_RECK_TO (200<<PHY_TIM_OFFSET)
  #define PHY_LINK_TO (500<<PHY_TIM_OFFSET)
  #define PHY_NWST_TO (500<<PHY_TIM_OFFSET)
  #define PHY_NWDN_TO (800<<PHY_TIM_OFFSET)

#define PHY_SMODE_OFFSET    (PHY_TIM_OFFSET+PHY_TIM_SIZE)
#define PHY_SMODE_SIZE      (5)
#define PHY_SMODE_MASK      (0x1f<<PHY_SMODE_OFFSET)
  #define SMODE_AUTO   (0x10<<PHY_SMODE_OFFSET)
  #define SMODE_FD100  (0x08<<PHY_SMODE_OFFSET)
  #define SMODE_HD100  (0x04<<PHY_SMODE_OFFSET)
  #define SMODE_FD10   (0x02<<PHY_SMODE_OFFSET)
  #define SMODE_HD10   (0x01<<PHY_SMODE_OFFSET)
  #define SMODE_ALL    (0x1f<<PHY_SMODE_OFFSET)

#define PHY_CHNG_OFFSET    (PHY_SMODE_OFFSET+PHY_SMODE_SIZE)
#define PHY_CHNG_SIZE      (1)
#define PHY_CHNG_MASK      (1<<PHY_CHNG_OFFSET)
  #define PHY_CHANGE (1<<PHY_CHNG_OFFSET)

static char *lstate[]={"NULL","INIT","FINDING","FOUND","NWAY_START","NWAY_WAIT","LINK_WAIT","LINKED"};

static int EmacDebug;

void _EmacMdioDumpState(u32 macbase, u32 state)
  {

  if (!EmacDebug) return;
  printk("Phy: %d, ",(state&PHY_DEV_MASK)>>PHY_DEV_OFFSET);
  printk("State: %d/%s, ",(state&PHY_STATE_MASK)>>PHY_STATE_OFFSET,lstate[(state&PHY_STATE_MASK)>>PHY_STATE_OFFSET]);
  printk("Speed: %d, ",(state&PHY_SPEED_MASK)>>PHY_SPEED_OFFSET);
  printk("Dup: %d, ",(state&PHY_DUPLEX_MASK)>>PHY_DUPLEX_OFFSET);
  printk("Tim: %d, ",(state&PHY_TIM_MASK)>>PHY_TIM_OFFSET);
  printk("SMode: %d, ",(state&PHY_SMODE_MASK)>>PHY_SMODE_OFFSET);
  printk("Chng: %d",(state&PHY_CHNG_MASK)>>PHY_CHNG_OFFSET);
  printk("\n");
  if (((state&PHY_STATE_MASK)!=FINDING)&&((state&PHY_STATE_MASK)!=INIT))
    _EmacMdioDumpPhy(macbase,(state&PHY_DEV_MASK)>>PHY_DEV_OFFSET);
  }

void _EmacMdioDumpPhy(u32 macbase,u32 p)
  {
  u32 i,j,m,n,PhyAcks;

  PhyAcks=MDIOACKi;
  for(i=0,j=1;i<32;i++,j<<=1)
    {
    if (PhyAcks&j)
      {
      printk("%2d%s:",i,(i==p)?">":" ");
      for(m=0;m<6;m++)
        {
        MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(m<<21)|(i<<16);
        _EmacMdioWaitForAccessComplete(macbase);
        n=MDIOUSERACCESSi;
        printk(" %04x",n&0x0ffff);
        }
      printk("\n");
      }
    }
  }

void _EmacMdioResetPhy(u32 macbase,u32 PhyNum)
  {

  MDIOUSERACCESSi = MDIO_GO|MDIO_WRITE|(PHY_CONTROL_REG<<21)|(PhyNum<<16)|PHY_RESET;
  if (EmacDebug)
    printk("EmacMdioPhYReset(%d)\n",PhyNum);
  _EmacMdioWaitForAccessComplete(macbase);
  }

void _EmacMdioDisablePhy(u32 macbase,u32 PhyNum)
  {

  MDIOUSERACCESSi = MDIO_GO|MDIO_WRITE|(PHY_CONTROL_REG<<21)|(PhyNum<<16)|PHY_ISOLATE|PHY_PDOWN;
  if (EmacDebug)
    printk("EmacMdioDisablePhy(%d)\n",PhyNum);
  _EmacMdioWaitForAccessComplete(macbase);
  }

void _EmacInitState(u32 macbase,u32 *PhyState)
  {
  u32 CurrentState;

  CurrentState=*PhyState;
  CurrentState=(CurrentState&~PHY_TIM_MASK)|(PHY_FIND_TO);
  CurrentState=(CurrentState&~PHY_STATE_MASK)|(FINDING);
  CurrentState=(CurrentState&~PHY_SPEED_MASK);
  CurrentState=(CurrentState&~PHY_DUPLEX_MASK);
  CurrentState|=PHY_CHANGE;
  *PhyState=CurrentState;
  }

void _EmacFindingState(u32 macbase,u32 *PhyState)
  {
  u32 PhyNum,i,j,PhyAcks;


  PhyNum=-1;
  if (*PhyState&PHY_TIM_MASK)
    {
    *PhyState=(*PhyState&~PHY_TIM_MASK)|((*PhyState&PHY_TIM_MASK)-(1<<PHY_TIM_OFFSET));
    }
   else
    {
    PhyAcks=MDIOACKi;
    for(i=0,j=1;(i<32)&&((j&PhyAcks)==0);i++,j<<=1);
    if ((PhyAcks)&&(i<32)) PhyNum=i;
    if (PhyNum==-1)
      {
      if (EmacDebug)
        printk("EmacMdioFindAPhy: Timed Out looking for a Phy!\n");
      *PhyState|=PHY_RECK_TO;
      }
     else
      {
      if (EmacDebug)
        printk("EmacMdioFindAPhy: PhyNum: %d\n",PhyNum);
      *PhyState=(*PhyState&~PHY_DEV_MASK)|((PhyNum&PHY_DEV_MASK)<<PHY_DEV_OFFSET);
      *PhyState=(*PhyState&~PHY_STATE_MASK)|(FOUND);
      *PhyState|=PHY_CHANGE;
      }
    }
  }

void _EmacFoundState(u32 macbase,u32 *PhyState)
  {
  u32 PhyNum,PhyStatus,NWAYadvertise,m,i,j,PhyAcks;


  if ((*PhyState&PHY_SMODE_MASK)==0) return;
  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  PhyAcks=MDIOACKi;
  for(i=0,j=1;i<32;i++,j<<=1)
    {
    if (PhyAcks&j)
      {
      if (i==PhyNum)
        _EmacMdioResetPhy(macbase,i);
       else
        _EmacMdioDisablePhy(macbase,i);
      }
    }

  MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(PHY_STATUS_REG<<21)|(PhyNum<<16);
  _EmacMdioWaitForAccessComplete(macbase);
  PhyStatus=MDIOUSERACCESSi;

  if (EmacDebug)
    printk("Enable Phy to negotiate external connection\n");

  NWAYadvertise=NWAY_SEL;
  if (*PhyState&SMODE_FD100) NWAYadvertise|=NWAY_FD100;
  if (*PhyState&SMODE_HD100) NWAYadvertise|=NWAY_HD100;
  if (*PhyState&SMODE_FD10)  NWAYadvertise|=NWAY_FD10;
  if (*PhyState&SMODE_HD10)  NWAYadvertise|=NWAY_HD10;

  *PhyState&=~(PHY_TIM_MASK|PHY_STATE_MASK);
  if ((PhyStatus&NWAY_CAPABLE)&&(*PhyState&SMODE_AUTO))   /*NWAY Phy Detected*/
    {

    /*For NWAY compliant Phys                                                */

    MDIOUSERACCESSi=MDIO_GO|MDIO_WRITE|(NWAY_ADVERTIZE_REG<<21)|(PhyNum<<16)|NWAYadvertise;

    if (EmacDebug)
      {
      printk("NWAY Advertising: ");
      if (NWAYadvertise&NWAY_FD100) printk("FullDuplex-100 ");
      if (NWAYadvertise&NWAY_HD100) printk("HalfDuplex-100 ");
      if (NWAYadvertise&NWAY_FD10) printk("FullDuplex-10 ");
      if (NWAYadvertise&NWAY_HD10) printk("HalfDuplex-10 ");
      printk("\n");
      }

    _EmacMdioWaitForAccessComplete(macbase);
    MDIOUSERACCESSi=MDIO_GO|MDIO_WRITE|(PHY_CONTROL_REG<<21)|(PhyNum<<16)|AUTO_NEGOTIATE_EN;
    _EmacMdioWaitForAccessComplete(macbase);
    MDIOUSERACCESSi=MDIO_GO|MDIO_WRITE|(PHY_CONTROL_REG<<21)|(PhyNum<<16)|AUTO_NEGOTIATE_EN|RENEGOTIATE;
    _EmacMdioWaitForAccessComplete(macbase);

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
      j|=PHY_FD;
    if (EmacDebug)
      printk("Requested PHY mode %s Duplex %s Mbps\n",(j&PHY_FD)?"Full":"Half",(j&PHY_100)?"100":"10");
    MDIOUSERACCESSi=MDIO_GO|MDIO_WRITE|(PHY_CONTROL_REG<<21)|(PhyNum<<16)|j; /*Phy Speed and Duplex*/
    *PhyState&=~PHY_SPEED_MASK;
    if (j&PHY_100)
      *PhyState|=(1<<PHY_SPEED_OFFSET);
    *PhyState&=~PHY_DUPLEX_MASK;
    if (j&PHY_FD)
      *PhyState|=(1<<PHY_DUPLEX_OFFSET);
    *PhyState|=PHY_CHANGE|PHY_LINK_TO|LINK_WAIT;
    _EmacMdioWaitForAccessComplete(macbase);
    }
  }

void _EmacNwayStartState(u32 macbase,u32 *PhyState)
  {
  u32 PhyNum,PhyMode;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  /*Wait for Negotiation to start                                            */

  MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(PHY_CONTROL_REG<<21)|(PhyNum<<16);
  _EmacMdioWaitForAccessComplete(macbase);
  PhyMode=MDIOUSERACCESSi;
  if((PhyMode&RENEGOTIATE)==0)
    {
    MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(PHY_STATUS_REG<<21)|(PhyNum<<16); /*Flush pending latch bits*/
    *PhyState&=~(PHY_STATE_MASK|PHY_TIM_MASK);
    *PhyState|=PHY_CHANGE|NWAY_WAIT|PHY_NWDN_TO;
    _EmacMdioWaitForAccessComplete(macbase);
    }
   else
    {
    if (*PhyState&PHY_TIM_MASK)
      *PhyState=(*PhyState&~PHY_TIM_MASK)|((*PhyState&PHY_TIM_MASK)-(1<<PHY_TIM_OFFSET));
     else
      _EmacMdioPhyTimeOut(macbase, PhyState);
    }
  }

void _EmacNwayWaitState(u32 macbase,u32 *PhyState)
  {
  u32 PhyNum,PhyStatus,NWAYadvertise,NegMode,i,j;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(PHY_STATUS_REG<<21)|(PhyNum<<16);
  _EmacMdioWaitForAccessComplete(macbase);
  PhyStatus=MDIOUSERACCESSi;
  if (PhyStatus&NWAY_COMPLETE)
    {
    *PhyState|=PHY_CHANGE;
    *PhyState&=~PHY_SPEED_MASK;
    *PhyState&=~PHY_DUPLEX_MASK;
    MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(NWAY_ADVERTIZE_REG<<21)|(PhyNum<<16);
    _EmacMdioWaitForAccessComplete(macbase);
    NWAYadvertise=MDIOUSERACCESSi;
    MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(NWAY_REMADVERTISE_REG<<21)|(PhyNum<<16);
    _EmacMdioWaitForAccessComplete(macbase);
    NegMode=MDIOUSERACCESSi&NWAYadvertise;
    NegMode&=(NWAY_FD100|NWAY_HD100|NWAY_FD10|NWAY_HD10);
    if (NegMode==0)
      {
      NegMode=(NWAY_HD100|NWAY_HD10)&NWAYadvertise; /*or 10 ?? who knows, Phy is not MII compliant*/
      }
    for(j=0x8000,i=0;(i<16)&&((j&NegMode)==0);i++,j>>=1);
    NegMode=j;
    if (EmacDebug)
      {
      printk("Negotiated connection: ");
      if (NegMode&NWAY_FD100) printk("FullDuplex 100 Mbs\n");
      if (NegMode&NWAY_HD100) printk("HalfDuplex 100 Mbs\n");
      if (NegMode&NWAY_FD10) printk("FullDuplex 10 Mbs\n");
      if (NegMode&NWAY_HD10) printk("HalfDuplex 10 Mbs\n");
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
      _EmacMdioPhyTimeOut(macbase, PhyState);
    }
  }

void _EmacLinkWaitState(u32 macbase,u32 *PhyState)
  {
  u32 PhyStatus;
  u32 PhyNum;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(PHY_STATUS_REG<<21)|(PhyNum<<16);
  _EmacMdioWaitForAccessComplete(macbase);
  PhyStatus=MDIOUSERACCESSi;
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
      _EmacMdioPhyTimeOut(macbase, PhyState);
    }
  }

void _EmacMdioPhyTimeOut(u32 macbase,u32 *PhyState)
  {
  /*Things you may want to do if we cannot establish link, like look for another Phy*/
  /* and try it.                                                             */
  }

void _EmacLinkedState(u32 macbase,u32 *PhyState)
  {
  u32 PhyNum;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;
  if (MDIOLINKi&(1<<PhyNum)) return;
  *PhyState&=~(PHY_STATE_MASK|PHY_TIM_MASK);
  *PhyState|=PHY_CHANGE|NWAY_WAIT|PHY_NWDN_TO;

  }

void _EmacDefaultState(u32 macbase,u32 *PhyState)
  {
  /*Awaiting a EmacMdioInit call                                             */
  *PhyState|=PHY_CHANGE;
  }

void _EmacMdioWaitForAccessComplete(u32 macbase)
  {
  while((MDIOUSERACCESSi&MDIO_GO)!=0)
    {
    }
  }

/*User Calles*********************************************************       */

void EmacMdioInit(u32 macbase, u32 *PhyState, u32 cpufreq,int verbose)
  {

  /*Setup MII MDIO access regs                                               */

  EmacDebug=verbose;
  MDIOCONTROLi    = 0x40008000|(cpufreq/1000000);  /* Enable MDIO            */
  *PhyState=INIT;
  if (EmacDebug)
    printk("EmacMdioInit\n");
  _EmacMdioDumpState(macbase,*PhyState);
  }

void EmacMdioSetPhyMode(u32 macbase,u32 *PhyState,u32 PhyMode)
  {
  u32 CurrentState;

  *PhyState&=~PHY_SMODE_MASK;
  if (PhyMode&NWAY_AUTO)  *PhyState|=SMODE_AUTO;
  if (PhyMode&NWAY_FD100) *PhyState|=SMODE_FD100;
  if (PhyMode&NWAY_HD100) *PhyState|=SMODE_HD100;
  if (PhyMode&NWAY_FD10) *PhyState|=SMODE_FD10;
  if (PhyMode&NWAY_HD10) *PhyState|=SMODE_HD10;
  CurrentState=*PhyState&PHY_STATE_MASK;
  if ((CurrentState==NWAY_START)||
      (CurrentState==NWAY_WAIT) ||
      (CurrentState==LINK_WAIT) ||
      (CurrentState==LINKED)      )
    *PhyState=(*PhyState&~PHY_STATE_MASK)|FOUND|PHY_CHANGE;
  if (EmacDebug)
    printk("EmacMdioSetPhyMode\n");
  _EmacMdioDumpState(macbase,*PhyState);
  }

/* EmacMdioTic is called every 10 mili seconds to process Phy states         */

int EmacMdioTic(u32 macbase,u32 *PhyState)
  {
  u32 CurrentState;
  u32 PhyNum;
  /*Act on current state of the Phy                                          */

  CurrentState=*PhyState;
  switch(CurrentState&PHY_STATE_MASK)
    {
    case INIT:       _EmacInitState(macbase,PhyState);      break;
    case FINDING:    _EmacFindingState(macbase,PhyState);   break;
    case FOUND:      _EmacFoundState(macbase,PhyState);     break;
    case NWAY_START: _EmacNwayStartState(macbase,PhyState); break;
    case NWAY_WAIT:  _EmacNwayWaitState(macbase,PhyState);  break;
    case LINK_WAIT:  _EmacLinkWaitState(macbase,PhyState);  break;
    case LINKED:     _EmacLinkedState(macbase,PhyState);    break;
    default:         _EmacDefaultState(macbase,PhyState);   break;
    }

  /*Dump state info if a change has been detected                            */

  if ((CurrentState&~PHY_TIM_MASK)!=(*PhyState&~PHY_TIM_MASK))
    _EmacMdioDumpState(macbase,*PhyState);

  /*Return state change to user                                              */

  if (*PhyState&PHY_CHNG_MASK)
    {
    *PhyState&=~PHY_CHNG_MASK;

    PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;
#ifdef CONFIG_MIPS_ACPEP
    /*Once the link is established we enable the LEDS */
    MDIOUSERACCESSi = MDIO_GO|MDIO_WRITE|(LXT971_LED_REG<<21)|(PhyNum<<16)|
	               (LXT971_LED_RX<<12) | (LXT971_LED_TX << 8) |
		       (LXT971_LED_COLL << 4)| (LXT_LED_STRETCH);
    _EmacMdioWaitForAccessComplete(macbase);
#endif
    return(TRUE);
    }
   else
    return(FALSE);
  }

/* EmacMdioGetDuplex is called to retreive the Duplex info                   */

int EmacMdioGetDuplex(u32 macbase,u32 *PhyState)
  {
  return(*PhyState&PHY_DUPLEX_MASK);
  }

/* EmacMdioGetSpeed is called to retreive the Speed info                     */

int EmacMdioGetSpeed(u32 macbase,u32 *PhyState)
  {
  return(*PhyState&PHY_SPEED_MASK);
  }

/* EmacMdioGetPhyNum is called to retreive the Phy Device Adr info           */

int EmacMdioGetPhyNum(u32 macbase,u32 *PhyState)
  {
  return((*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET);
  }

/* EmacMdioGetLinked is called to Determine if the LINKED state has been reached*/

int EmacMdioGetLinked(u32 macbase,u32 *PhyState)
  {
  return((*PhyState&PHY_STATE_MASK)==LINKED);
  }

void EmacMdioLinkChange(u32 macbase,u32 *PhyState)
  {
  u32 PhyNum,PhyStatus;

  PhyNum=(*PhyState&PHY_DEV_MASK)>>PHY_DEV_OFFSET;

  if (EmacMdioGetLinked(macbase,PhyState))
    {
    MDIOUSERACCESSi=MDIO_GO|MDIO_READ|(PHY_STATUS_REG<<21)|(PhyNum<<16);
    _EmacMdioWaitForAccessComplete(macbase);
    PhyStatus=MDIOUSERACCESSi;
    if ((PhyStatus&PHY_LINKED)==0)
      {
      *PhyState&=~(PHY_TIM_MASK|PHY_STATE_MASK);
      if (*PhyState&SMODE_AUTO)
        {
        MDIOUSERACCESSi=MDIO_GO|MDIO_WRITE|(PHY_CONTROL_REG<<21)|(PhyNum<<16)|AUTO_NEGOTIATE_EN|RENEGOTIATE;
        _EmacMdioWaitForAccessComplete(macbase);

        *PhyState|=PHY_CHANGE|PHY_NWST_TO|NWAY_START;
        }
       else
        {
        *PhyState|=PHY_CHANGE|PHY_LINK_TO|LINK_WAIT;
        }
      }
    }
  }
