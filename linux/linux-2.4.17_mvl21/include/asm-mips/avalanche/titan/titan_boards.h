#ifndef _TITAN_BOARDS_H
#define _TITAN_BOARDS_H

// Let us define board specific information here. 

#if defined(CONFIG_MIPS_TNETV1050SDB)

#define AUDCLK_FREQ                                  8192000
#define REFCLK_FREQ                                 25000000
#define ALTCLK_FREQ                                 12000000

#define AVALANCHE_LOW_CPMAC_PHY_MASK                0x40000000 /* CPMAC with low base address */
#define AVALANCHE_HIGH_CPMAC_PHY_MASK               0x80000000 /* CPMAC with high base address*/ 

#endif



#endif
