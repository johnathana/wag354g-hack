#ifndef CY_FLASH_H
#define CY_FLASH_H

#define INTELC3  1
#define INTELJ3  2
#include <cy_codepattern.h>

#undef CONFIG_MTD_CFI_AMDSTD
#undef CONFIG_MTD_CFI_INTELEXT
#undef CONFIG_MTD_AVALANCHE_LEN

#if (FLASHTYPE == INTELJ3)
#define CONFIG_MTD_CFI_INTELEXT 1
#else
#define CONFIG_MTD_CFI_AMDSTD 1
#endif

#if (FLASH_SIZE == 8)
#define CONFIG_MTD_AVALANCHE_LEN 0x800000
#elif (FLASH_SIZE == 4)
#define CONFIG_MTD_AVALANCHE_LEN 0x400000
#endif

#endif		
