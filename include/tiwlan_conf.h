#ifndef TIWLAN_CONF_H
#define TIWLAN_CONF_H

#include "cy_codepattern.h"

#if (FLASH_SIZE == 8)
	#define APDK_VERSION	554  //note: 554(5.5.4), 5520(5.5.20)
#elif (FLASH_SIZE == 4)
	#define APDK_VERSION	5520  //note: 554(5.5.4), 5520(5.5.20)
#endif

#define TIWLAN_VERSION	  APDK_VERSION 

#if (TIWLAN_VERSION == 554)
	#define TIWLAN_CONF_SIZE	3427 //3426
#elif (TIWLAN_VERSION == 5520)
	#define TIWLAN_CONF_SIZE	3426 //3427
#endif
#define TIWLAN_NVRAM_SIZE	1024

#endif
