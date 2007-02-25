#ifndef _code_pattern_h_
#define _code_pattern_h_

#define CODE_ID		"U2ND"
#define PSPB_ID		"PBOT"
#define BOOT_PATTERN	"EST"
#define UP_PMON		1
#define UP_MAC		2
#define UP_CODE		3
#define UP_PIGGY	4
#define UP_LANG		5
#define UP_SN		6

#define LINKSYS		7
#define PCI		9	// <remove the line>
#define ELSA		10	// <remove the line>
#define ALLNET		11	// <remove the line>
#define REENET		12	// 2003/06/21	<remove the line>
#define CISCO		90
#define	OTHER_OEM	99	// <remove the line>

#define ENGLISH 	1
#define JAPANESE	2
#define KOREAN		3	// <remove the line>

#define	USA		1
#define	JAPAN		2
#define	EUROPE		3
#define WW		4	// <remove the line>
#define GERMANY		5	// <remove the line>
#define	KOREA		6	// <remove the line>
#define	AUSTRALIA	7	// <remove the line>
#define	FRANCE		8	// <remove the line>
#define	SWEDEN		9	// <remove the line>
#define ICELAND		11	/*for ICELAND*/

#define AnnexA		1
#define AnnexB		2
//#define ADSL_TYPE	AnnexA
//#define ADSL_TYPE	AnnexB

#define AMD 		3
#define INTELC3 	1
#define INTELJ3 	2

#define X5J   		1
#define COMMON   	2
//#define CODE_TYPE 	X5J
//#define CODE_TYPE 	COMMON

/***************************************	// <remove the line>
 * define oem                          *	// <remove the line>
 * OEM =                               *	// <remove the line>
 ***************************************/	// <remove the line>
/* define OEM */				// <remove the line>
#define OEM             LINKSYS			// <remove the line>
//#define OEM             PCI			// <remove the line>
//#define OEM             ELSA			// <remove the line>
//#define	OEM		ALLNET		// <remove the line>
//#define	OEM		REENET		// <remove the line>
//#define OEM             OTHER_OEM	// for Non-brand	<remove the line>

/***************************************	// <remove the line>
 * define project                      *	// <remove the line>
 * PROJECT =                               *	// <remove the line>
 ***************************************/	// <remove the line>
//By Charles Addition for Project check
#define	DWG814D		1
#define	DWG914D		2
#define	DWG814E_HM	3
#define	WDR814G		4
#define	WDR834G		5
#define	WAG54GV2	6
#define	AG241	        7
#define	WAG354G	        8
#define	HG200	        9
#define	DWG811A1	10
#define	DWG814H		11
#define AG241V2		12
#define WAG354GV2	13
#define	NONE		100

//#define	PROJECT		WAG54GV2
//#define	PROJECT		AG241

//By junzhao Add for Customer
#if OEM==LINKSYS
#define VZ	        1
#define SPRINT          2
/***************************************	// <remove the line>
 * define  customer 	               *	// <remove the line>
 * CUSTOMER =                          *	// <remove the line>
 ***************************************/	// <remove the line>
//#define CUSTOMER       VZ
//#define CUSTOMER      SPRINT                  // <remove the line>
//#define CUSTOMER      HUAWEI                  // <remove the line>

/***************************************
 * define country                      *
 * LOCALE=COUNTRY =                    *
 ***************************************/
//#define COUNTRY		LOCALE
//#define LOCALE		USA
//#define	LOCALE		JAPAN
//#define	LOCALE		EUROPE
//#define	LOCALE		WW		// <remove the line>
//#define	LOCALE		GERMANY		// <remove the line>
//#define	LOCALE		KOREA		// <remove the line>

#include "cy_codepattern.h"

/***************************************	// <remove the line>
 * define  spec modules                *	// <remove the line>
 ***************************************/	// <remove the line>
#if CUSTOMER==VZ
#define VZ7  
#define VZ16
//...add your spec modules here
#endif
#endif

/***************************************
 * define model name and code pattern  *
 * MODEL_NAME =                        *
 * CODE_PATTERN =                      *
 ***************************************/
#if OEM == LINKSYS
 #define UI_STYLE	CISCO
 //#define CODE_PATTERN   "W54G"
 #if	(PROJECT == DWG814D)		//By Charles addition for DWG-814D Project Define
 	#define CODE_PATTERN   "DWGA"
 	#define MODEL_NAME	"DWG-814D"
	#define	VENDOR		"CYBERTAN"
// #elif defined (DWG914D_PROJECT)
 #elif (PROJECT == DWG914D)	//By Charles addition for DWG-914D Project Define
 	#define	CODE_PATTERN   "DWGB"
	#define	MODEL_NAME	"DWG-914D"
	#define	VENDOR		"CYBERTAN"
 #elif (PROJECT == DWG814E_HM)	//By Charles addition for DWG-814E-HM Project Define
	#define	CODE_PATTERN   "DWGE"
	#define	MODEL_NAME	"DWG-814E-HM"
	#define	VENDOR		"CYBERTAN"
 #elif (PROJECT == WDR814G)	//By Charles addition for WDR814g Project Define
	#define	CODE_PATTERN   "DWG1"
	//#define	CODE_PATTERN   "DWGE"
	#define	MODEL_NAME	"WDR814g"
	#define	VENDOR		"HUAWEI-3COM"
#elif (PROJECT == WDR834G)	//By Charles addition for WDR834g Project Define
	#define	CODE_PATTERN   "DWGE"
	#define	MODEL_NAME	"WDR834g"
	#define	VENDOR		"HUAWEI-3COM"
#elif (PROJECT == WAG54GV2)	//By Charles addition for WAG5G4V2 Project Define
	#if (FLASH_SIZE == 8)
	#if (ADSL_TYPE == AnnexA)
		#define	CODE_PATTERN   "WAG2"
	#elif (ADSL_TYPE == AnnexB)
		#define	CODE_PATTERN   "WA2B"
	#endif
	#endif
	
	#if (FLASH_SIZE == 4)
	#if (ADSL_TYPE == AnnexA)
		#define	CODE_PATTERN   "WA21"
	#elif (ADSL_TYPE == AnnexB)
		#define	CODE_PATTERN   "WA22"
	#endif
	#endif

 	#define	MODEL_NAME	"WAG54G-V2"
	#define	VENDOR		"LINKSYS"
#elif (PROJECT == AG241)
	#if (FLASH_SIZE == 8)
	#if (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "AG2A"
	#elif (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "AG2B"
	#endif
	#endif
	
	#if (FLASH_SIZE == 4)
	#if (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "AG1A"
	#elif (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "AG1B"
	#endif
	#endif

	#define MODEL_NAME	"AG241"
	#define	VENDOR		"LINKSYS"
#elif (PROJECT == AG241V2)
	#if (FLASH_SIZE == 8)
	#if (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "AG4A"
	#elif (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "AG4B"
	#endif
	#endif
	
	#if (FLASH_SIZE == 4)
	#if (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "AG3A"
	#elif (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "AG3B"
	#endif
	#endif

	#define MODEL_NAME	"AG241V2"
	#define	VENDOR		"LINKSYS"
#elif (PROJECT == WAG354G)
	#if (FLASH_SIZE == 8)
	#if (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "WA3A"
	#elif (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "WA3B"
	#endif
	#endif
	
	#if (FLASH_SIZE == 4)
	#if (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "WA31"
	#elif (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "WA32"
	#endif
	#endif

	#define MODEL_NAME	"WAG354G"
	#define	VENDOR		"LINKSYS"
#elif (PROJECT == WAG354GV2)
	#if   (FLASH_SIZE == 8) && (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "WA8A"
	#elif (FLASH_SIZE == 8) && (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "WA8B"
	#elif (FLASH_SIZE == 4) && (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "WA7A"
	#elif (FLASH_SIZE == 4) && (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "WA7B"
	#endif

	#define MODEL_NAME	"WAG354GV2"
	#define	VENDOR		"LINKSYS"
#elif (PROJECT == HG200)
	#if   (FLASH_SIZE == 8) && (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "HG2A"
	#elif (FLASH_SIZE == 8) && (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "HG2B"
	#elif (FLASH_SIZE == 4) && (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "HG1A"
	#elif (FLASH_SIZE == 4) && (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "HG1B"
	#endif

	#define MODEL_NAME	"HG200"
	#define	VENDOR		"LINKSYS"
#elif (PROJECT == DWG811A1)
	#if   (FLASH_SIZE == 8) && (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "HG6A"
	#elif (FLASH_SIZE == 8) && (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "HG6B"
	#elif (FLASH_SIZE == 4) && (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "HG5A"
	#elif (FLASH_SIZE == 4) && (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "HG5B"
	#endif

	#define MODEL_NAME	"DWG811A1"
	#define	VENDOR		"NONE"
#elif (PROJECT == DWG814H)
	#if   (FLASH_SIZE == 8) && (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "WA6A"
	#elif (FLASH_SIZE == 8) && (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "WA6B"
	#elif (FLASH_SIZE == 4) && (ADSL_TYPE == AnnexA)
		#define CODE_PATTERN   "WA5A"
	#elif (FLASH_SIZE == 4) && (ADSL_TYPE == AnnexB)
		#define CODE_PATTERN   "WA5B"
	#endif

	#define MODEL_NAME	"DWG814H"
	#define	VENDOR		"NONE"
 #elif (PROJECT == NONE)	//By Charles addition for None Project Define
	#define	CODE_PATTERN   "NONE"
	#define	MODEL_NAME	"GENERAL"
	#define	VENDOR		"NONE"
 #endif

 /*Barry add for Control Intel Flash Downgrade code*/
 #define INTEL_FLASH_SUPPORT_VERSION_FROM "v1.41.8"
 #define BCM4712_CHIP_SUPPORT_VERSION_FROM "v1.50.0"
 #define INTEL_FLASH_SUPPORT_BOOT_VERSION_FROM "v1.3"
 #define BCM4712_CHIP_SUPPORT_BOOT_VERSION_FROM "v2.0"
#elif OEM == PCI
 #define CODE_PATTERN    "W54P"
 #if LOCALE == JAPAN
	#define MODEL_NAME	"Planex BLW-04G Wireless Router"
 #else
	#define MODEL_NAME	"Advanced Wireless 11g Router"
 #endif
 #define	VENDOR		"PCI"
#elif OEM == ELSA
 #define CODE_PATTERN    "W54E"
 #define MODEL_NAME	"ELSA WLAN DSL-Router 4P G"
 #define	VENDOR		"ELSA"
#elif OEM == ALLNET
 #define CODE_PATTERN    "W54A"
 #define MODEL_NAME	"Advanced Wireless 11g Router"
 #define	VENDOR		"ALLNET"
#elif OEM == REENET
 #define CODE_PATTERN    "W54R"
 #define MODEL_NAME	"RW-IPG500"
 #define	VENDOR		"REENET"
#elif OEM == OTHER_OEM
 #define CODE_PATTERN    "W54O"
 #define MODEL_NAME	"Advanced Wireless 11g Router"
 #define	VENDOR		"OTHER_OEM"
#else
 #error "Your must select a OEM name!!"
#endif

/***************************************
 * define language                     *
 * LANGUAGE =                          *
 ***************************************/
#if LOCALE == JAPAN
	#define	LANGUAGE	JAPANESE
#elif LOCALE == KOREA			// <remove the line>
	#define LANGUAGE 	KOREAN	// <remove the line>
#else
	#define LANGUAGE 	ENGLISH
#endif

/***************************************
 * define wireless max channel         *
 * WL_MAX_CHANNEL =                    *
 ***************************************/
#if LOCALE == JAPAN || LOCALE == EUROPE
	#define	WL_MAX_CHANNEL	"13"
#elif OEM == KOREA			// <remove the line>
	#define	WL_MAX_CHANNEL	"13"	// <remove the line>
#else
	#define WL_MAX_CHANNEL	"11"
#endif

/***************************************
 * define web file path                *
 * WEB_PAGE =                          *
 ***************************************/

#if OEM == LINKSYS
#if CUSTOMER == VZ
	#define WEB_PAGE                "wag54gv2"
#elif CUSTOMER == SPRINT
	#define WEB_PAGE                "wag54gv2"
#endif
#if UI_STYLE ==  CISCO
	#if LOCALE == JAPAN
		#define	WEB_PAGE	"cisco_wrt54g_jp"
	#else
		#define WEB_PAGE	"cisco_wrt54g_en"
	#endif
#else
	#if LOCALE == JAPAN
		#define	WEB_PAGE	"linksys_wrt54g_jp"
	#else
		#define WEB_PAGE	"linksys_wrt54g_en"
	#endif
#endif
#elif OEM == PCI
	#if LOCALE == JAPAN
		#define	WEB_PAGE	"pci_wrt54g_jp"
	#else
		#define WEB_PAGE	"pci_wrt54g_en"
	#endif
#elif OEM == ELSA
	#define WEB_PAGE	"elsa_wrt54g_en"
#elif OEM == ALLNET
	#define WEB_PAGE	"nonbrand"
#elif OEM == REENET
	#define WEB_PAGE	"reenet_wrt54g_kr"
#elif	OEM == OTHER_OEM
	#define WEB_PAGE	"nonbrand"
#endif

/***************************************
 * check LOCALE
 ***************************************/
#if OEM == LINKSYS
 #if LOCALE != JAPAN && LOCALE != USA && LOCALE != EUROPE
	#error	"The LOCALE for LINKSYS is error, must be USA, EUROPE or JAPAN"
 #endif
#elif OEM == PCI
 #if LOCALE != JAPAN && LOCALE != USA && LOCALE != EUROPE
	#error	"The LOCALE for PCI is error, must be USA or JAPAN"
 #endif
#elif OEM == ELSA
 #if LOCALE != EUROPE
	#error	"The LOCALE for ELSA is error, must be EUROPE"
 #endif
#elif OEM == ALLNET
 #if LOCALE != EUROPE
	#error	"The LOCALE for ALLNET is error, must be EUROPE"
 #endif
#elif OEM == REENET
 #if LOCALE != KOREA
	#error	"The LOCALE for REENET is error, must be KOREA"
 #endif
#elif OEM == OTHER_OEM
 #if LOCALE != USA && LOCALE != EUROPE
	#error	"The LOCALE for NONBRAND is error, must be USA or EUROPE"
 #endif
#endif

/***************************************
 * define other                        *
 ***************************************/
#if OEM == LINKSYS
#define	DIAG_SUPPORT	// Ping and traceroute test
#endif
//#define	SECOND_BOOT	// Put boot.bin in code.bin
#endif	/* _code_pattern_h_ */
