/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2003 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

/*******************************************************************************
 * FILE PURPOSE:    Environment variables routines.
 *******************************************************************************
 * FILE NAME:       env.c
 *
 * DESCRIPTION:     Environment variables routines.
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 ******************************************************************************/

#ifdef _STANDALONE
#include <psbl/sysconf.h>
#include <psbl/stdio.h>
#include <psbl/env.h>
#include <psbl/flashop.h>
#include <psbl/debug.h>
#include <psbl/errno.h>
#include <psbl/heapmgr.h>
#include "shell.h"
#else
#include "platform.h"    
#include "env.h"         
#include "flashop.h"     
#include "debug.h"       
#include "errno.h"       
#include "shell.h" 
#include "sysconf.h"
#endif

#include "hw.h"

#define JUNZHAO_DEBUG
#define MAX_ENV_ENTRY 			(block_size/FLASH_ENV_ENTRY_SIZE)


unsigned int strlen(const char *str);
int strcmp(const char *A, const char *B);
char *strcpy(char *DST, const char *SRC);

typedef enum ENV_VARS {
	env_vars_start = 0,
	CPUFREQ,
	MEMSZ,
	FLASHSZ,
	MODETTY0,
	MODETTY1,
	PROMPT,
	BOOTCFG,
	HWA_0,
#if !defined (AVALANCHE) || defined(TNETC401B)     
	HWA_1,
#endif        
#if !defined(TNETV1020_BOARD)        
	HWA_RNDIS,
#endif    
#if defined (TNETD73XX_BOARD)    
	HWA_3,
#endif    
	IPA,
	IPA_SVR,
	BLINE_MAC0,
#if !defined (AVALANCHE) || defined(TNETC401B)         
	BLINE_MAC1,
#endif    
#if !defined(TNETV1020_BOARD)        
	BLINE_RNDIS,
#endif    
#if defined (TNETD73XX_BOARD)    
	BLINE_ATM,
#endif
#if !defined(TNETV1020_BOARD)            
	USB_PID,
	USB_VID,
	USB_EPPOLLI,
#endif    
	IPA_GATEWAY,
	SUBNET_MASK,
#if defined (TNETV1050_BOARD)    
    BLINE_ESWITCH,
#endif    
#if !defined(TNETV1020_BOARD)            
    USB_SERIAL,
    HWA_HRNDIS,      /* Host (PC) side RNDIS address */
#endif    
    REMOTE_USER,
    REMOTE_PASS,
    REMOTE_DIR,
    SYSFREQ,
    LINK_TIMEOUT,
#ifndef AVALANCHE     /* Avalanche boards use only one mac port */
    MAC_PORT,
#endif    
    PATH,
    HOSTNAME,
#ifdef WLAN
	 HW_REV_MAJOR,
	 HW_REV_MINOR,
	 HW_PATCH,
	 SW_PATCH,
	 SERIAL_NUMBER,
#endif
    TFTPCFG,
#if defined (TNETV1050_BOARD)    
    HWA_ESWITCH,
#endif
	/*
	 * Add new env variables here.
	 * NOTE: New environment variables should always be placed at the end, ie 
	 *       just before env_vars_end.
	 */

	env_vars_end
} ENV_VARS;

typedef struct ENVDESC {
	ENV_VARS   idx;
	char      *nm;
    char      *alias;
} ENVDESC;

#define ENVSTR(x)         #x
#define _ENV_ENTRY(x)  {x, ENVSTR(x), 0}
ENVDESC env_ns[] = {
	_ENV_ENTRY(env_vars_start), /* start. */
	_ENV_ENTRY(CPUFREQ),
	_ENV_ENTRY(MEMSZ),
	_ENV_ENTRY(FLASHSZ),
	_ENV_ENTRY(MODETTY0),
	_ENV_ENTRY(MODETTY1),
	_ENV_ENTRY(PROMPT),
	_ENV_ENTRY(BOOTCFG),
	_ENV_ENTRY(HWA_0),
#if !defined (AVALANCHE) || defined(TNETC401B)    
	_ENV_ENTRY(HWA_1),
#endif
#if !defined(TNETV1020_BOARD)        
	_ENV_ENTRY(HWA_RNDIS),
#endif    
#if defined (TNETD73XX_BOARD)    
	_ENV_ENTRY(HWA_3),
#endif    
	_ENV_ENTRY(IPA),
	_ENV_ENTRY(IPA_SVR),
	_ENV_ENTRY(IPA_GATEWAY),
	_ENV_ENTRY(SUBNET_MASK),
	_ENV_ENTRY(BLINE_MAC0),
#if !defined (AVALANCHE) || defined(TNETC401B)    
	_ENV_ENTRY(BLINE_MAC1),
#endif
#if !defined(TNETV1020_BOARD)    
	_ENV_ENTRY(BLINE_RNDIS),
#endif
#if defined (TNETD73XX_BOARD)    
	_ENV_ENTRY(BLINE_ATM),
#endif
#if !defined(TNETV1020_BOARD)            
	_ENV_ENTRY(USB_PID),
	_ENV_ENTRY(USB_VID),
	_ENV_ENTRY(USB_EPPOLLI),
#endif
#if defined (TNETV1050_BOARD)    
	_ENV_ENTRY(BLINE_ESWITCH),
#endif
#if !defined(TNETV1020_BOARD)            
	_ENV_ENTRY(USB_SERIAL),
	_ENV_ENTRY(HWA_HRNDIS),
#endif    
    _ENV_ENTRY(REMOTE_USER),
    _ENV_ENTRY(REMOTE_PASS),
    _ENV_ENTRY(REMOTE_DIR),
    _ENV_ENTRY(SYSFREQ),
    _ENV_ENTRY(LINK_TIMEOUT),
#ifndef AVALANCHE       /* Avalanche boards use only one mac port */
    _ENV_ENTRY(MAC_PORT),
#endif    
    _ENV_ENTRY(PATH),
    _ENV_ENTRY(HOSTNAME),
#ifdef WLAN
    _ENV_ENTRY(HW_REV_MAJOR),
    _ENV_ENTRY(HW_REV_MINOR),
    _ENV_ENTRY(HW_PATCH),
    _ENV_ENTRY(SW_PATCH),
    _ENV_ENTRY(SERIAL_NUMBER),
#endif
    _ENV_ENTRY(TFTPCFG),
#if defined (TNETV1050_BOARD)    
    _ENV_ENTRY(HWA_ESWITCH),    
#endif
	/*
	 * Add new entries below this.
	 */
    /* Adam2 environment name alias. Temporary. */
    {IPA,     "my_ipaddress"},
    {CPUFREQ, "cpufrequency"},    
    {SYSFREQ, "sysfrequency"}, 
    {HWA_0,   "maca"},
#ifndef AVALANCHE    
    {HWA_1,   "macb"},
#endif
    {MEMSZ,   "memsize"},
    
	_ENV_ENTRY(env_vars_end) /* delimiter. */
};

/* TODO: remove this */
t_env_var env_vars[10];

char * envVersion = "TIENV0.8";  /* string size should be <= sizeof(ENV_VAR) */

#define ENV_CELL_SIZE           16

/* control field decode */
#define ENV_GARBAGE_BIT			0x01    /* Env is garbage if this bit is off */
#define ENV_DYNAMIC_BIT			0x02    /* Env is dynamic if this bit is off */

typedef struct ENV_VAR_t {
    unsigned char   varNum;
    unsigned char   ctrl;
    unsigned short  chksum;
    unsigned char   numCells;
    unsigned char   data[ENV_CELL_SIZE - 5];    /* The data section starts 
                                                 * here, continues for
                                                 * numCells.
                                                 */
}ENV_VAR;

static unsigned int MaxEnvVarsNum;       /* Holds count for the maximum 
                                          * number of environment variable
                                          * that can be set. 
                                          */
static PSBL_REC* psbl_rec =  (PSBL_REC*)0x94000300;

/* Exported APIs */
int     sys_setenv  (char* env_nm, char* env_val);
char*   sys_getenv  (char* env_nm);
void    sh_printenv (void);
int     EnvInit     (void);

/* Internal functions */
static int IsPreDefinedVar(const char* var); /* Checks for variable in
                                              * pre-defined list. 
                                              * Returns Index if found - else 0.
                                              * Please note:
                                              * ZERO is illegal index.
                                              */
#ifdef _STANDALONE
#define enter_critical_section()
#define exit_critical_section()
#else
void enter_critical_section(void);
void exit_critical_section(void);
#endif

static ENVDESC* GetEnvDescForEnvVar(const char* var);
static int __sys_setenv(char *env_nm, char *env_val);

/* Internal macros */
#define IsEnvGarbage(var)       (((var)->ctrl & ENV_GARBAGE_BIT) == 0)
#define IsEnvDynamic(var)       (((var)->ctrl & ENV_DYNAMIC_BIT) == 0)
#define EnvGetNextBlock(var)    ((ENV_VAR*)( (char*)(var) + (var)->numCells * ENV_CELL_SIZE))

static int EnvMakeGarbage(ENV_VAR* pVar)
{
    int status;
   
    enter_critical_section();
    
    status = FWBOpen((int)&((pVar)->ctrl));
    FWBWriteByte((int)&((pVar)->ctrl), (pVar)->ctrl & ~ENV_GARBAGE_BIT);
    FWBClose();

    exit_critical_section();

    return (status == 0);
}

#include <asm/io.h>
#include <asm/addrspace.h>
extern int avalanche_mtd_ready;
static char* GetEnvBaseAndSize(unsigned int* size)
{
	*size = psbl_rec->env_size;

#ifdef ENV_SPACE_SIZE
    if(*size > ENV_SPACE_SIZE){
        *size = ENV_SPACE_SIZE;
    }    
#endif
  
    return( (char *) psbl_rec->env_base);	
#if 0    
    if(!avalanche_mtd_ready)
	return( (char *) psbl_rec->env_base);	
    else
    {
	//unsigned long env_base = ioremap_nocache(0x10000000, 0x800000) + 0xe000;
	//printk("enter avalanche_mtd_ready == 1\n");
	//return( (char *)env_base);	
	return( (char *)(KSEG0ADDR(psbl_rec->env_base)));	
    }
#endif 
}

#if ENV_DEBUG
static int GetChecksum(ENV_VAR* var) {
    unsigned short chksum = 0;
    unsigned int tmp, i;

    chksum = var->varNum + var->numCells;
    
    tmp = strlen(var->data);
    if(IsEnvDynamic(var)) {            
        tmp += strlen(var->data + tmp + 1) + 1;
    }

    for(i = 0; i < tmp; i++) {
        chksum += var->data[i]; 
    }
    
    return chksum;
}
#endif

/* returns the variable number if pre-defined, else return 0 */
static int IsPreDefinedVar(const char* var)
{
    ENVDESC* env;
    
    if((env = GetEnvDescForEnvVar(var)) != NULL) return env->idx;
      
    return 0;  
}

static ENVDESC* GetEnvDescForEnvVar(const char* var)
{
    int ii;        
    /* go over the list of pre-defined environment variables */
	for (ii = env_vars_start; env_ns[ii].idx != env_vars_end; ii++){   
        /* check if the env variable is listed */            
		if (strcmp(env_ns[ii].nm, var) == 0) 
        {
            return  &env_ns[ii];
        }

        /* if an alias is present, check if the alias matches
         * the description
         */
        if(env_ns[ii].alias != NULL) {
    		if (strcmp(env_ns[ii].alias, var) == 0)
            {
                return &env_ns[ii];
			}			
        }
    }
    return NULL;
}

static char* GetPreDefinedVarName(int index) {
    int ii;
        
    if ( index >= env_vars_end || index <= env_vars_start) {
        return NULL;
    }
    for(ii = env_vars_start; env_ns[ii].idx != env_vars_end; ii++) {
        if(env_ns[ii].idx == index) {
            if(env_ns[ii].alias != NULL) {
                return env_ns[ii].alias;
            } else {                        
            return env_ns[ii].nm;
        }
    }
    }
    return NULL;
}

/* Gives the nth non-garbage environment block. Indexed starting ZERO */
static ENV_VAR* GetEnvBlockByNumber(int index) {
    
    ENV_VAR* pVar;
    int count = 0, size;
    
    pVar = (ENV_VAR*)GetEnvBaseAndSize(&size);

#if ENV_DEBUG
    //sys_printf("%s: pVar -- %08x, size -- %d, index -- %d\n", __FUNCTION__, pVar, size, index);
#endif
    
    /* skip first block */
    pVar++;

    
    enter_critical_section();
    
    for(;pVar->varNum!=0xFF; pVar = EnvGetNextBlock(pVar)) {

#if ENV_DEBUG
//    sys_printf("%s: pVar -- %08x varNum -- %02x ctrl -- %02x chksum -- %02x numCells -- %02x count %d index %d\n", __FUNCTION__, pVar, pVar->varNum, pVar->ctrl, pVar->chksum, pVar->numCells, count, index);
#endif
        if(!IsEnvGarbage(pVar)) {

#if ENV_DEBUG
//    sys_printf("%s: pVar -- %08x count -- %d index -- %d isnot envGarbage\n", __FUNCTION__, pVar, count, index);
#endif
            if(count == index){
                exit_critical_section();                    
                return pVar; 
            }
            else count++;
        }
    }    

    exit_critical_section();
    
#if ENV_DEBUG
    sys_printf("%s: is envGarbage\n", __FUNCTION__);
#endif
    return NULL;
}
/*
 * Gets the name and value from a given environment block. Also checks the 
 * checksum while doing so. If it finds that the checksum is invalid, it 
 * marks the environment as garbage, and returns NULL for the name and value.
 */
static void GetNameAndValueFromEnvVar(ENV_VAR* pVar, char** ppName, 
                                                            char** ppValue)
{
    unsigned short chksum = 0;
    int i;
        
    enter_critical_section();
    chksum += (pVar->varNum + pVar->ctrl + pVar->numCells);        
            
    if(IsEnvDynamic(pVar)) {
        *ppName  = pVar->data;
        *ppValue = pVar->data + strlen(pVar->data) + 1;
    } else {
        *ppName  = GetPreDefinedVarName(pVar->varNum);
        *ppValue = pVar->data;                
    }        

    for(i = 0; i < strlen(*ppValue); i++) {
        chksum += (unsigned char) (*ppValue)[i];
    }

    if(IsEnvDynamic(pVar)) {
        for(i = 0; i < strlen(*ppName); i++) {
            chksum += (unsigned char) (*ppName)[i];
        }
    }    
    
    chksum += pVar->chksum;

    if(chksum > 0xFFFF) {
        chksum -= 0xFFFF;
    }
    chksum  = ~(chksum);
    
    /* bad checksum */
    if(chksum != 0) {
        *ppName = *ppValue = NULL;            
        EnvMakeGarbage(pVar);
    }                
    
    exit_critical_section();

    return;
}

/* returns the non-garbage block corresponding to the asked var. */
static ENV_VAR* GetEnvBlockByName(char *var) {
        
    ENV_VAR* pVar;
    int index = IsPreDefinedVar(var);
    int i;
    
#if ENV_DEBUG
//    sys_printf("%s: index -- %d\n", __FUNCTION__, index);
#endif

    for(i = 0; i < MaxEnvVarsNum; i++) {
            
        if( !(pVar = GetEnvBlockByNumber(i)) ) return NULL;
    

        enter_critical_section();
        
        if(index) { /* Pre-defined environment variables */
            if(pVar->varNum == index) {
                exit_critical_section();
                return pVar;           
            }
        } else {    /* Dynamic environment variables */
            if(!strcmp(var, pVar->data)) {
                exit_critical_section();
                return pVar;
            }
        }        

        exit_critical_section();
    }
    return NULL;
}

static int FormatEnvBlock(void) 
{
    unsigned int size, i;
    unsigned char* pFlash = GetEnvBaseAndSize(&size);
   
#ifdef ENV_SPACE_SIZE
    char *pExtraSpace;
    
    if(!(pExtraSpace = _malloc(psbl_rec->env_size - size))) {
        return SBL_EFAILURE;
    }
    
    memset(pExtraSpace, 0xFF, psbl_rec->env_size - size);
    memcpy(pExtraSpace, (char*)psbl_rec->env_base + ENV_SPACE_SIZE, 
                    psbl_rec->env_size - size);    
#endif    
   
    enter_critical_section();

    /* If Erase returns 0 => Flash has gone bad. Return error */
    if(FWBErase((unsigned int)pFlash, size, 0) == 0) {
        exit_critical_section();
        return SBL_EFAILURE;
    }
            
    FWBOpen((int)pFlash);
        
	for (i = 0; i <= strlen(envVersion) ;i++) {
	    FWBWriteByte( (int)(pFlash++), envVersion[i]);
    }

#ifdef ENV_SPACE_SIZE
    pFlash = (char*)psbl_rec->env_base + ENV_SPACE_SIZE;
	for (i = 0; i < psbl_rec->env_size - size ;i++) {
	    FWBWriteByte( (int)(pFlash++), pExtraSpace[i]);
    }
    _free(pExtraSpace); 
#endif    
    
	FWBClose();	

    exit_critical_section();

    return SBL_SUCCESS;
}

#if 0
int EnvInit(void)
{
    unsigned char* pFlash;
    unsigned int size;

    pFlash = GetEnvBaseAndSize(&size);

    MaxEnvVarsNum = (size)/(ENV_CELL_SIZE) - 1; /* Ignore the header */
    
    if(strcmp(pFlash, envVersion) == 0) {
        /* TODO: Visit each environment variable and calculate the checksum. 
         * If incorrect, mark as garbage
         */
    } else {
        return FormatEnvBlock();            
    }
    
    return SBL_SUCCESS;        
}
#endif

//#include <asm/addrspace.h>

//junzhao 2005.1.4
#ifdef JUNZHAO_DEBUG
unsigned char buff[10];
unsigned int myaddress = 0;
unsigned int mysize = 0;
unsigned int myptr = 0;
#endif

#include <code_pattern.h>
#if (FLASHTYPE == INTELJ3)
static int flash_init_flag = 0;
//junzhao 2005.2.16 
int flash_type = 0; //flash type: C3 or J3
#endif

int EnvInit(void)
{
    unsigned char* pFlash;
    unsigned int *ptr;
    //PSBL_REC* psbl = (PSBL_REC *)((unsigned long)ioremap_nocache(0x10000000, 0x800000) + 0x4000300);
    //PSBL_REC* psbl = (PSBL_REC *)((unsigned long)(0x94000300 & 0x1FFFFFFF | 0xA0000000));
    PSBL_REC* psbl = (PSBL_REC *)(KSEG1ADDR(0x94000300));
    ptr = KSEG1ADDR(psbl->env_base);
    //ptr = psbl->env_base;
    pFlash = (char *)(ptr);	
  
#ifdef JUNZHAO_DEBUG
    myaddress = psbl->env_base;
    mysize = psbl->env_size;
    myptr = ptr;
    memcpy(buff, pFlash, 10);
#endif
    
    //return 1;    

#if (FLASHTYPE == INTELJ3)
//junzhao 2005.2.16 check flash type first
	if (flash_init_flag == 0)
	{
		char str[43];
		flash_init(CS0_BASE, str);
		flash_init_flag = 1;
		printk("FlashType: %s\n", str);
		if(strstr(str, "C3"))
			flash_type = INTELC3;  //INTEL C3 TYPE
	}	
#endif
    
    if(strcmp(pFlash, envVersion) == 0) //PSPBOOT
    {
        /* TODO: Visit each environment variable and calculate the checksum. 
         * If incorrect, mark as garbage
         */
     	    unsigned int size;
    	    size = psbl->env_size;
    	    MaxEnvVarsNum = (size)/(ENV_CELL_SIZE) - 1; /* Ignore the header */
	    return 0;
    } 
    else  //ADAM2
    {
	    return 1;
       // return FormatEnvBlock();            
    }
    //return SBL_SUCCESS;        
}

int sys_setenv(char *env_nm, char *env_val) 
{   
    /*
     * CPUFREQ and SYSFREQ should not be modified 
     */
#ifndef _STANDALONE
    if (strcmp("CPUFREQ", env_nm) == 0) {
	    sys_printf("Env: CPUFREQ is read-only.\n");
		return SBL_SUCCESS;       
	}
	if (strcmp("SYSFREQ", env_nm) == 0) {
        sys_printf("Env: SYSFREQ is read-only.\n");
	    return SBL_SUCCESS;       
	}
#endif

#if (FLASHTYPE == INTELJ3)
	if (flash_init_flag == 0)
	{
		char str[43];
		flash_init(CS0_BASE, str);
		flash_init_flag = 1;
		printk("FlashType: %s\n", str);
		if(strstr(str, "C3"))
			flash_type = INTELC3;  //INTEL C3
	}	
#endif

    return __sys_setenv(env_nm, env_val);
}
        
/* Returns:
 * SBL_SUCCESS: successfully configured.
 * SBL_ERESCRUNCH: env block is exhausted, env set failed.
 */
static int __sys_setenv(char *env_nm, char *env_val)
{
    ENV_VAR *pVar, new, *pBase, *pOldEnv = NULL;
    unsigned int size, i, newsize;
    char *pTmp;
#if (AUTO_DEFRAG_ENVIRONMENT == TRUE)
    int IsGarbagePresent = FALSE;
#endif

    /* Check for pre-existance of the variable */
    if((pTmp = sys_getenv(env_nm))) {
        /* Env Exists. See if the value to be set is same as old one */
        if(!strcmp(pTmp, env_val)) {
            return SBL_SUCCESS;
        }
        /* Env Exists but is a different value. Make old one garbage */ 
        pOldEnv = GetEnvBlockByName(env_nm);
#if (AUTO_DEFRAG_ENVIRONMENT == TRUE)
        IsGarbagePresent = TRUE;
#endif
    }
    
    pBase = pVar = (ENV_VAR*)GetEnvBaseAndSize(&size);

    /* skip first block */
    pVar++;    
   
    enter_critical_section();
    
    /* Go to the end of Available Flash space */
    for( ; pVar->varNum != 0xFF ; pVar = EnvGetNextBlock(pVar) ) {
#if (AUTO_DEFRAG_ENVIRONMENT == TRUE)
        if(IsEnvGarbage(pVar)) {
            IsGarbagePresent = TRUE;
        }
#endif        
    }

    exit_critical_section();
    
#if ENV_DEBUG
    sys_printf("New Environment to be written at %x", pVar);
#endif
    
    memset((char*)&new, 0xFF, sizeof(new));
    new.chksum = 0;                
    
    if(!(new.varNum = IsPreDefinedVar(env_nm))) {       
        /* Dynamic variable */
        new.ctrl &= ~(ENV_DYNAMIC_BIT);
    }
    new.chksum += (new.varNum + new.ctrl);
   
    newsize = sizeof(ENV_VAR) - sizeof(new.data) + strlen(env_val) + 1;
   
    enter_critical_section();
    
    if(IsEnvDynamic(&new)) {
        newsize  += strlen(env_nm) + 1;
        for(i=0; i < strlen(env_nm); i++) {
            new.chksum += (unsigned char) env_nm[i];
        }
    }

    exit_critical_section();
    
    new.numCells = ((newsize)/ENV_CELL_SIZE)+((newsize%ENV_CELL_SIZE)?1:0);
    new.chksum += new.numCells;
    
    for(i = 0; i <= strlen(env_val); i++) {
        new.chksum += (unsigned char) env_val[i];            
    }            
  
    if(new.chksum > 0xFFFF) {
        new.chksum -= 0xFFFF;
    }
    new.chksum = ~(new.chksum);
    
    /* Check if enough space is available to store the env variable */
    if(((char*)pVar + (new.numCells*ENV_CELL_SIZE)) > ((char*)pBase + size)) {
#if (AUTO_DEFRAG_ENVIRONMENT == TRUE)
        if(IsGarbagePresent){
            if (sys_defragenv() == SBL_SUCCESS) {
                return __sys_setenv(env_nm, env_val);
            }
        }
#endif        
        sys_printf("Error: Out of Environment space\n");
        return SBL_ERESCRUNCH;
    }
    
#if ENV_DEBUG   
    sys_printf("newsize = %d\n", newsize);
    sys_printf("new.numCells = %d\n", new.numCells);
#endif
   
    enter_critical_section();
    
    /* Write to flash */
    FWBOpen((int)pVar);
    
    for(i = 0; i < sizeof(ENV_VAR) - sizeof(new.data); i++) {
        FWBWriteByte(((int)pVar)++, ((char*)&new)[i]);
    }

    if(IsEnvDynamic(&new)) {
        for(i = 0; i <= strlen(env_nm); i++) {
            FWBWriteByte(((int)pVar)++, env_nm[i]);
        }
    }

    for(i = 0; i <= strlen(env_val); i++) {
        FWBWriteByte(((int)pVar)++, env_val[i]);
    }        
    
    FWBClose();
    
    exit_critical_section();
    
    if(pOldEnv) {
        EnvMakeGarbage(pOldEnv);
    }

    
    return SBL_SUCCESS;
}

int sys_unsetenv(char *env_nm)
{
    ENV_VAR* pVar;        

	if (strcmp("CPUFREQ", env_nm) == 0) {
        goto fail;            
	}	
	if (strcmp("SYSFREQ", env_nm) == 0) {
        goto fail;            
	}		
    
    if( !(pVar = GetEnvBlockByName(env_nm)) ) return SBL_EFAILURE;
   
    EnvMakeGarbage(pVar); 
    
    return SBL_SUCCESS;  

fail:    
    sys_printf("Env: %s is read-only.", env_nm);	
    return SBL_EFAILURE;       
 
}

void echo(int argc, char **argv)
{
    int ii;
    if (argc == 1) {
        sh_printenv();
    } else {
        if (strcmp(argv[1], "envlist") == 0) {
            sys_printf("Pre-defined Variable list:\n\n");                
            for ( ii = env_vars_start + 1; ii < env_vars_end; ii++) {
                sys_printf("%-13s\n", GetPreDefinedVarName(ii));
            }
        }
        else {  /* user gave some unsupported echo request */
            sh_printenv();
        }
    }
}

#ifndef _STANDALONE
int FWBGet_flash_type(void);
int sys_initenv(void)
{
    unsigned int size;    
#ifdef TNETV1050SDB
    FWBGet_flash_type();
#endif
    
    GetEnvBaseAndSize(&size);
    MaxEnvVarsNum = (size)/(ENV_CELL_SIZE) - 1; /* Ignore the header */
    return TRUE;
}
#endif

void sh_printenv(void)
{
    ENV_VAR* pVar;
    int i;
    char *pName, *pValue;
    
        
    for(i = 0; i < MaxEnvVarsNum; i++) {

#if ENV_DEBUG
    sys_printf("%s: before GetEnvBlockByNumber\n", __FUNCTION__);
#endif
    
        if( !(pVar = GetEnvBlockByNumber(i)) ) goto out;
#if ENV_DEBUG
        sys_printf("%s %x\n", "sh_printenv", pVar);
#endif
        GetNameAndValueFromEnvVar(pVar, &pName, &pValue);
        if(pName == NULL) continue;
        sys_printf("\n%-13s\t%s", pName, pValue);
    }
out:    
    sys_printf("\n");
    return;        
}

char* sys_getenv(char *var)
{
    ENV_VAR* pVar;
    char *pName, *pValue;
	
#if ENV_DEBUG
    sys_printf("%s: var -- %s\n", __FUNCTION__, var);
#endif
    
    if( !(pVar = GetEnvBlockByName(var)) ) return NULL;

#if ENV_DEBUG
//    sys_printf("%s %x\n", "sys_getenv", pVar);
#endif
    
    GetNameAndValueFromEnvVar(pVar, &pName, &pValue);
    
#if ENV_DEBUG
    sys_printf("get env %s %x\n", pValue, pVar);
#endif
    
    return pValue;
}

int sys_defragenv(void)
{
    char **ppRamStore = NULL;
    char *pName, *pValue;
    unsigned int i;
    ENV_VAR* pVar;
    int defragFail=FALSE;

    if( !(ppRamStore = (char**)_malloc(sizeof(char*)*MaxEnvVarsNum)) ) {
        defragFail = TRUE;                
        goto defragerror;            
    }
    memset ((char*)ppRamStore, 0, sizeof(char*)*MaxEnvVarsNum);

    for(i = 0; i < MaxEnvVarsNum; i++) {
            
#if ENV_DEBUG
    sys_printf("%s: before GetEnvBlockByNumber\n", __FUNCTION__);
#endif
        if( !(pVar = GetEnvBlockByNumber(i)) ) continue;  

        GetNameAndValueFromEnvVar(pVar, &pName, &pValue);        
        
        if(pName == NULL) continue;
        
        if( !(ppRamStore[i] = _malloc(strlen(pName) + strlen(pValue) + 2)) ) {
            defragFail = TRUE;                
            goto defragerror;            
        }

        /* store name and value in RAM */
        memcpy((char*)ppRamStore[i], pName, strlen(pName) + 1);
        memcpy((char*)ppRamStore[i] + strlen(pName) + 1, 
                        pValue,strlen(pValue) + 1);
    }
    
    if(FormatEnvBlock() != SBL_SUCCESS) {
        defragFail = TRUE;
    }
    
defragerror:
    if(ppRamStore) {
        for(i = 0; i < MaxEnvVarsNum; i++) {
            if(ppRamStore[i]) {
                __sys_setenv(ppRamStore[i], 
                                ppRamStore[i]+strlen(ppRamStore[i]) + 1);
                _free(ppRamStore[i]);
            }
        }
        _free(ppRamStore);
    }
    
    if(defragFail) {
        sys_printf("Out of memory. Defragment aborted.\n");
        return SBL_ERESCRUNCH;
    } 

    return SBL_SUCCESS;
}

int get_envstring(int index, char *buffer)
{        
    ENV_VAR* pVar;

#if ENV_DEBUG
    sys_printf("%s: before GetEnvBlockByNumber\n", __FUNCTION__);
#endif
    if( !(pVar = GetEnvBlockByNumber(index)) ) return 0;
            
#ifndef _STANDALONE /* OS context */
    return sprintf(buffer, "%-13s\t%s\n", 
                    sys_getivar(index), sys_getienv(index));
#else
    return sys_sprintf(buffer, "%-20s  %s\r\n", 
                    sys_getivar(index), sys_getienv(index));
#endif
}

char* sys_getienv(int var_num)
{
    ENV_VAR* pVar;
    char* pName, *pValue;
        
#if ENV_DEBUG
    sys_printf("%s: before GetEnvBlockByNumber\n", __FUNCTION__);
#endif
    if( !(pVar = GetEnvBlockByNumber(var_num)) ) return 0;
    
    GetNameAndValueFromEnvVar(pVar, &pName, &pValue);

    return pValue;
}

char* sys_getivar (int var_num) {
    ENV_VAR* pVar;
    char* pName, *pValue;
        
#if ENV_DEBUG
    sys_printf("%s: before GetEnvBlockByNumber\n", __FUNCTION__);
#endif
    if( !(pVar = GetEnvBlockByNumber(var_num)) ) return 0;
    
    GetNameAndValueFromEnvVar(pVar, &pName, &pValue);

    return pName;
}

int EnvAddAlias(char* orig, char* alias)
{
    ENVDESC* env;
            
    if((env = GetEnvDescForEnvVar(orig)) != NULL)
    {
        env->alias = alias;
        return SBL_SUCCESS;
    }
    return SBL_EFAILURE;
}

char* GetResolvedEnvName(char *envName)
{
    ENVDESC* env;

    if((env = GetEnvDescForEnvVar(envName)) != NULL)
    {
        if(env->alias != NULL) {
            return env->alias;
        } else {
            return env->nm;
        }
    }
    return NULL;
}
