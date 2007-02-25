#include "code_pattern.h"

#ifndef _cyutils_h_
#define _cyutils_h_
//#define CYBERTAN_VERSION	"v2.02.0"

//for wireless
#define CYBERTAN_VERSION	"1.01.11"
#define LANG_VERSION		"1.01.11"    //for lang.bin version control(setupwizard)
#define HARDWARE_TYPE            1             //0:AnnexA  1:AnnexB
//for nowireless
//#define CYBERTAN_VERSION	"0.01.1v"

//#define RELEASE_DATE            "Feb 16 2006" //2006/03/06 Joanne remove 

#define RELEASE_DATE            __DATE__ //2006/03/06 Joanne add

#define MINOR_VERSION		""
#define SERIAL_NUMBER		"00"
#define PMON_BOOT_VERSION	"v1.6"
#define CFE_BOOT_VERSION	"v2.3"

#define SERIAL_NUMBER_DEFAULT   "000000"

/* convert 2.0 to 20 to be an integer, and add 0x30 to skip special ASCII */
#define HW_Version		((HW_REV * 10) + 0x30)

#define BOOT_IPADDR "192.168.1.1"
#define BOOT_NETMASK "255.255.255.0"

struct code_header{
	char magic[4];
	char res1[4];
	char fwdate[3];
	char fwvern[3];
	char id[4];
	char hwver[1];
	unsigned char res2[13];
} ;

//#ifdef MULTILANG_SUPPORT
struct lang_header {
	char magic[4];
	char res1[4];   // for extra magic
	char fwdate[3];
	char fwvern[3];
	char id[4];     // U2ND
	char hw_ver[1];    // 0: for 4702, 1: for 4712
	char res2;
	unsigned long len;
	unsigned char res3[8];
} ;
//#endif

struct boot_header {
	char magic[3];
	char res[29];
};

/***************************************
 * define upnp misc                    *
 ***************************************/
#if OEM == LINKSYS
  #if LANGUAGE == ENGLISH
    #define URL			"http://www.linksys.com/"
  #else
    #define URL			"http://www.linksys.co.jp/"
  #endif
  #define DEV_FRIENDLY_NAME	MODEL_NAME
  #define DEV_MFR		"Linksys Inc."
  #define DEV_MFR_URL		URL
  #define DEV_MODEL_DESCRIPTION	"Internet Access Server"
  #define DEV_MODEL		MODEL_NAME
  #define DEV_MODEL_NO		CYBERTAN_VERSION
  #define DEV_MODEL_URL		URL
#elif OEM == PCI
  #if LANGUAGE == ENGLISH
    #define URL			"http://www.planex.com/"
  #else
    #define URL			"http://www.planex.co.jp/"
  #endif
  #define DEV_FRIENDLY_NAME	"BLW-04G Wireless Broadband Router"
  #define DEV_MFR		"Planex Communciations Inc."
  #define DEV_MFR_URL		URL
  #define DEV_MODEL_DESCRIPTION	"Internet Gateway Device with UPnP support"
  #define DEV_MODEL		"BLW-04G"
  #define DEV_MODEL_NO 		CYBERTAN_VERSION
  #define DEV_MODEL_URL		URL
#else
  #define URL			""
  #define DEV_FRIENDLY_NAME	MODEL_NAME
  #define DEV_MFR		""
  #define DEV_MFR_URL		URL
  #define DEV_MODEL_DESCRIPTION	"Internet Access Server"
  #define DEV_MODEL		MODEL_NAME
  #define DEV_MODEL_NO		CYBERTAN_VERSION
  #define DEV_MODEL_URL		URL
#endif
#endif	/* _cyutils_h_ */
