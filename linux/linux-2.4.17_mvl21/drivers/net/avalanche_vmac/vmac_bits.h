/*************************************************
 *     vmac_bits.h
 *     Host Register Contents
 *
 *************************************************/

#ifndef __VMAC_BITS_H
#define __VMAC_BITS_H

/*------------------------------------------*/                                
/* Ethernet MAC register offset definitions */                                
/*------------------------------------------*/                                
#define VMAC_DMACONFIG(X)      (*(volatile unsigned int *)(X + 0x00000000))   
#define VMAC_INTSTS(X)         (*(volatile unsigned int *)(X + 0x00000004))   
#define VMAC_INTMASK(X)        (*(volatile unsigned int *)(X + 0x00000008))   
                                                                              
#define VMAC_WRAPCLK(X)        (*(volatile unsigned int *)(X + 0x00000340))   
#define VMAC_STATSBASE(X)      (*(volatile unsigned int *)(X + 0x00000400))   
                                                                              
#define VMAC_TCRPTR(X)         (*(volatile unsigned int *)(X + 0x00000100))   
#define VMAC_TCRSIZE(X)        (*(volatile unsigned int *)(X + 0x00000104))   
#define VMAC_TCRINTTHRESH(X)   (*(volatile unsigned int *)(X + 0x00000108))   
#define VMAC_TCRTOTENT(X)      (*(volatile unsigned int *)(X + 0x0000010C))   
#define VMAC_TCRFREEENT(X)     (*(volatile unsigned int *)(X + 0x00000110))   
#define VMAC_TCRPENDENT(X)     (*(volatile unsigned int *)(X + 0x00000114))   
#define VMAC_TCRENTINC(X)      (*(volatile unsigned int *)(X + 0x00000118))   
#define VMAC_TXISRPACE(X)      (*(volatile unsigned int *)(X + 0x0000011c))   
                                                                              
                                                                              
#define VMAC_TDMASTATE0(X)     (*(volatile unsigned int *)(X + 0x00000120))   
#define VMAC_TDMASTATE1(X)     (*(volatile unsigned int *)(X + 0x00000124))   
#define VMAC_TDMASTATE2(X)     (*(volatile unsigned int *)(X + 0x00000128))   
#define VMAC_TDMASTATE3(X)     (*(volatile unsigned int *)(X + 0x0000012C))   
#define VMAC_TDMASTATE4(X)     (*(volatile unsigned int *)(X + 0x00000130))   
#define VMAC_TDMASTATE5(X)     (*(volatile unsigned int *)(X + 0x00000134))   
#define VMAC_TDMASTATE6(X)     (*(volatile unsigned int *)(X + 0x00000138))   
#define VMAC_TDMASTATE7(X)     (*(volatile unsigned int *)(X + 0x0000013C))   
#define VMAC_TXPADDCNT(X)      (*(volatile unsigned int *)(X + 0x00000140))   
#define VMAC_TXPADDSTART(X)    (*(volatile unsigned int *)(X + 0x00000144))   
#define VMAC_TXPADDEND(X)      (*(volatile unsigned int *)(X + 0x00000148))   
#define VMAC_TXQFLUSH(X)       (*(volatile unsigned int *)(X + 0x0000014C))   
                                                                              
#define VMAC_RCRPTR(X)         (*(volatile unsigned int *)(X + 0x00000200))   
#define VMAC_RCRSIZE(X)        (*(volatile unsigned int *)(X + 0x00000204))   
#define VMAC_RCRINTTHRESH(X)   (*(volatile unsigned int *)(X + 0x00000208))   
#define VMAC_RCRTOTENT(X)      (*(volatile unsigned int *)(X + 0x0000020C))   
#define VMAC_RCRFREEENT(X)     (*(volatile unsigned int *)(X + 0x00000210))   
#define VMAC_RCRPENDENT(X)     (*(volatile unsigned int *)(X + 0x00000214))   
#define VMAC_RCRENTINC(X)      (*(volatile unsigned int *)(X + 0x00000218))   
#define VMAC_RXISRPACE(X)      (*(volatile unsigned int *)(X + 0x0000021c))   

#define VMAC_RDMASTATE0(X)     (*(volatile unsigned int *)(X + 0x00000220))     
#define VMAC_RDMASTATE1(X)     (*(volatile unsigned int *)(X + 0x00000224))     
#define VMAC_RDMASTATE2(X)     (*(volatile unsigned int *)(X + 0x00000228))     
#define VMAC_RDMASTATE3(X)     (*(volatile unsigned int *)(X + 0x0000022C))     
#define VMAC_RDMASTATE4(X)     (*(volatile unsigned int *)(X + 0x00000230))     
#define VMAC_RDMASTATE5(X)     (*(volatile unsigned int *)(X + 0x00000234))     
#define VMAC_RDMASTATE6(X)     (*(volatile unsigned int *)(X + 0x00000238))     
#define VMAC_RDMASTATE7(X)     (*(volatile unsigned int *)(X + 0x0000023C))     
#define VMAC_FBLADDCNT(X)      (*(volatile unsigned int *)(X + 0x00000240))     
#define VMAC_FBLADDSTART(X)    (*(volatile unsigned int *)(X + 0x00000244))     
#define VMAC_FBLADDEND(X)      (*(volatile unsigned int *)(X + 0x00000248))     
#define VMAC_RXONOFF(X)        (*(volatile unsigned int *)(X + 0x0000024C))     
                                                                                
#define VMAC_FBL0NEXTD(X)      (*(volatile unsigned int *)(X + 0x00000280))     
#define VMAC_FBL0LASTD(X)      (*(volatile unsigned int *)(X + 0x00000284))     
#define VMAC_FBL0COUNTD(X)     (*(volatile unsigned int *)(X + 0x00000288))     
#define VMAC_FBL0BUFSIZE(X)    (*(volatile unsigned int *)(X + 0x0000028C))     
                                                                                
#define VMAC_MACCONTROL(X)     (*(volatile unsigned int *)(X + 0x00000300))     
#define VMAC_MACSTATUS(X)      (*(volatile unsigned int *)(X + 0x00000304))     
#define VMAC_MACADDRHI(X)      (*(volatile unsigned int *)(X + 0x00000308))     
#define VMAC_MACADDRLO(X)      (*(volatile unsigned int *)(X + 0x0000030C))     
#define VMAC_MACHASH1(X)       (*(volatile unsigned int *)(X + 0x00000310))     
#define VMAC_MACHASH2(X)       (*(volatile unsigned int *)(X + 0x00000314))     
                                                                                
#define VMAC_WRAPCLK(X)        (*(volatile unsigned int *)(X + 0x00000340))     
#define VMAC_BOFTEST(X)        (*(volatile unsigned int *)(X + 0x00000344))     
#define VMAC_PACTEST(X)        (*(volatile unsigned int *)(X + 0x00000348))     
#define VMAC_PAUSEOP(X)        (*(volatile unsigned int *)(X + 0x0000034C))     
                                                                                
#define VMAC_MDIOCONTROL(X)    (*(volatile unsigned int *)(X + 0x00000380))     
#define VMAC_MDIOUSERACCESS(X) (*(volatile unsigned int *)(X +0x00000384))      
#define VMAC_MDIOACK(X)        (*(volatile unsigned int *)(X + 0x00000388))     
#define VMAC_MDIOLINK(X)       (*(volatile unsigned int *)(X + 0x0000038C))     
#define VMAC_MDIOMACPHY(X)     (*(volatile unsigned int *)(X + 0x00000390))     
                                                                                
#define VMAC_STATS_BASE(X)     (X + 0x00000400)                                 




/*----------------------------------------------------------*/
/* DMACONFIG register bits (ETH_MACA_BASE + 0x00) */
/*----------------------------------------------------------*/
#define SRESET             (1<<31)    // 1 = Soft Reset DMA State Machine & MAC
#define RX_ENABLE          (1<<1)     // 1 = Rx DMA State Machine Enable
#define TX_ENABLE          (1<<0)     // 1 = Tx DMA State Machine Enable


/* INTSTS @ 0x04 */
#define LINK_CHANGE (1<<9)
#define LNKEVT      (1<<8)
#define CPLEVT      (1<<7)
#define ACKEVT      (1<<6)
#define RCR_FREEZE  (1<<5)
#define RCRA_THRESH (1<<4)
#define POSTED_RCRA (1<<3)
#define TCR_FREEZE  (1<<2)
#define TCRA_THRESH (1<<1)
#define POSTED_TCRA (1<<0)

/* TCRENTINC @ 0x118 */
#define TCR_RDY     (1<<31)


/* TXPADDCNT @ 0x140 */
#define TPK_RDY     (1<<31)

/* TXQFLUSH @ 0x14c */
#define TFL_RDY     (1<<31)

/* RCRENTINC @ 0X218 */
#define RCR_RDY     (1<<31)

/* FBLADDCNT @ 0x240 */
#define FBL_RDY     (1<<31)

/*  RXONOFF 0x24c */
#define RX_RDY      (1<<31)
#define ON_OFF      (1<<30)

/* MACCONTROL 0x300 */
#define DISABLE     (1<<31)
#define TXHOLD      (1<<30)
#define CLRP        (1<<29)
#define FFC         (1<<28)
#define NO_LOOP     (1<<27)
#define MTEST       (1<<26)
/*                  Bits 20-25      RESERVED            */
#define PEFCE       (1<<19)
#define NO_BRX      (1<<18)
#define FLOW_EN     (1<<17)
#define MAXLEN      (1<<16)
#define LONG        (1<<15)
#define PASS_CRC    (1<<14)
#define CMF         (1<<13)
#define CSF         (1<<12)
#define CEF         (1<<11)
#define CAF         (1<<10)
#define NO_CHAIN    (1<<9)
#define TXPACE      (1<<8)
/*                  Bits 3-7        RESERVED            */
#define PAUSE       (1<<2)
#define SPEED_100   (1<<1)
#define FULL_DUPLEX (1<<0)


/*----------------------------------------------------------*/
/* MDIOCONTROL register bits (ETH_MACA_BASE + 0x380) */
/*----------------------------------------------------------*/
#define IDLE             (1<<31)    // 1 = MDIO SM is IDLE
#define MDIO_ENABLE      (1<<30)    // 1 = Enable control
#define SHARED           (1<<29)    // 1 = Share control
/*                       Bits 27-28    MDIOOID             */
/*                       Bits 21-26    RESERVED            */
#define PREAMBLE         (1<<20)    // 1 = Preamble disable
#define FAULT            (1<<19)    // 1 = Fault indicator
/*                       Bits 16-18    RESERVED            */
#define RESUME             (1<<15)  // 1 = Resume control
/*                       Bits 8-14     RESERVED            */
/*                       Bits 0-7      CLKDIV              */

/*----------------------------------------------------------*/
/* MDIOUSERACCESS register bits (ETH_MACA_BASE + 0x384) */
/*----------------------------------------------------------*/
#define GO               (1<<31)    // 1 = GO, MDIO access attempt
#define MDIOWRITE        (1<<30)    // 1 = causes MDIO register write
#define ACK              (1<<29)    // (RO) 1 = Phy acknowleged transfer
/*                       Bits 26-28    RESERVED             */
/*                       Bits 21-25    PHY register address */
/*                       Bits 16-20    PHY to be accessed   */
/*                       Bits 0-15     PHY data             */

/*----------------------------------------------------------*/
/* MDIOMACPHY register bits (ETH_MACA_BASE + 0x390) */
/*----------------------------------------------------------*/
#define LN               (1<<7)     // 1 = Enable MDIO Link Determination
/*                       Bits 6-5    RESERVED             */
/*                       Bits 0-4    PHY register address */


struct vmac_stat_regs /* Avalanche VMAC statistics Registers */
{
  volatile unsigned long  rx_octets;
  volatile unsigned long  good_rx_frames;
  volatile unsigned long  broadcast_rx_frames;
  volatile unsigned long  multicast_rx_frames;
  volatile unsigned long  rx_crc_errors;
  volatile unsigned long  rx_align_code_errors;
  volatile unsigned long  oversized_rx_frames;
  volatile unsigned long  rx_jabbers;
  volatile unsigned long  undersized_rx_frames;
  volatile unsigned long  rx_fragments;
  volatile unsigned long  frames_64octet;
  volatile unsigned long  frames_65_127octet;
  volatile unsigned long  frames_128_255octet;
  volatile unsigned long  frames_256_511octet;
  volatile unsigned long  frames_512_1023octet;
  volatile unsigned long  frames_1024_1518octet;
  volatile unsigned long  net_octets;
  volatile unsigned long  sqe_test_errors;
  volatile unsigned long  tx_octets;
  volatile unsigned long  good_tx_frames;
  volatile unsigned long  single_col_tx_frames;
  volatile unsigned long  mul_col_tx_frames;
  volatile unsigned long  carrier_sense_errors;
  volatile unsigned long  deferred_tx_frames;
  volatile unsigned long  late_collisions;
  volatile unsigned long  excessive_collisions;
  volatile unsigned long  broadcast_tx_frames;
  volatile unsigned long  multicast_tx_frames;
  volatile unsigned long  filtered_rx_frames;
  volatile unsigned long  tx_error_frames;
  volatile unsigned long  collisions;
  volatile unsigned long  rx_overruns;
  volatile unsigned long  pause_tx_frames;
  volatile unsigned long  pause_rx_frames; 
};    

#endif __VMAC_BITS_H
