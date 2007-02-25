#ifndef _SANGAM_BOARDS_H
#define _SANGAM_BOARDS_H

// Let us define board specific information here. 


#if defined(CONFIG_MIPS_AR7DB)

#define AFECLK_FREQ                                 35328000
#define REFCLK_FREQ                                 25000000
#define OSC3_FREQ                                   24000000
#define AVALANCHE_LOW_CPMAC_PHY_MASK                0x80000000
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0x55555555  
#define AVALANCHE_LOW_CPMAC_MDIX_MASK               0x80000000

#endif


#if defined(CONFIG_MIPS_AR7RD)
#define AFECLK_FREQ                                 35328000
#define REFCLK_FREQ                                 25000000
#define OSC3_FREQ                                   24000000
#define AVALANCHE_LOW_CPMAC_PHY_MASK                0x80000000
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0x2
#define AVALANCHE_LOW_CPMAC_MDIX_MASK               0x80000000
#endif


#if (defined(CONFIG_MIPS_AR7WI) || defined(CONFIG_MIPS_AR7V))
#define AFECLK_FREQ                                 35328000
#define REFCLK_FREQ                                 25000000
#define OSC3_FREQ                                   24000000
#define AVALANCHE_LOW_CPMAC_PHY_MASK                0x80000000
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0x2
#define AVALANCHE_LOW_CPMAC_MDIX_MASK               0x80000000
#endif


#if defined(CONFIG_MIPS_AR7WRD) 
#define AFECLK_FREQ                                 35328000
#define REFCLK_FREQ                                 25000000
#define OSC3_FREQ                                   24000000
#define AVALANCHE_LOW_CPMAC_PHY_MASK                0x80000000
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0x00010000
#define AVALANCHE_LOW_CPMAC_MDIX_MASK               0x80000000
#endif


#if (defined(CONFIG_MIPS_AR7VWI) || defined(CONFIG_MIPS_AR7VW))
#define AFECLK_FREQ                                 35328000
#define REFCLK_FREQ                                 25000000
#define OSC3_FREQ                                   24000000
#define AVALANCHE_LOW_CPMAC_PHY_MASK                0x80000000
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0x00010000
#define AVALANCHE_LOW_CPMAC_MDIX_MASK               0x80000000
#endif


#if defined CONFIG_MIPS_SEAD2
#define AVALANCHE_LOW_CPMAC_PHY_MASK                0xAAAAAAAA
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0x55555555
#define AVALANCHE_LOW_CPMAC_MDIX_MASK               0
#include <asm/mips-boards/sead.h>
#endif


#endif
