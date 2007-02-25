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
**|         Copyright (c) 1998-2004 Texas Instruments Incorporated           |**
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

/** \file   pal_sysMisc.c
    \brief  PAL miscallaneous code file
	
    (C) Copyright 2004, Texas Instruments, Inc

    \author     Ajay Singh
    \version    0.1
 */
#include <pal.h>

unsigned int PAL_sysGetChipVersionInfo()
{
    return(*(volatile unsigned int*)AVALANCHE_CVR);
}

/*******************************************************************************
 * FUNCTION:    avalanche_get_cpu_type
 *******************************************************************************
 * DESCRIPTION: Function to get the cpu type
 *
 * RETURNS:     cpu type 
 *
 ******************************************************************************/
AVALANCHE_CPU_TYPE_T avalanche_get_cpu_type(void)
{
    volatile UINT32 cvr;
    volatile UINT32 cvr2;
    AVALANCHE_CPU_TYPE_T cpu_type;
   
    cpu_type =  CPU_UNIDENT;
   
    /* Normal read */
    cvr =  PAL_sysGetChipVersionInfo() & 0xffff;
   
    /* 
     * Avalanche I errata; Try writing to the CVR, it changes on the Avalanche I
     * though it should not 
     */
    REG32_WRITE(AVALANCHE_CVR, 0xffff);
   
    cvr2 = (REG32_DATA(AVALANCHE_CVR) & 0xffff);
   
    if(cvr == cvr2)
    {
        switch(cvr)
        {
            case CPU_AVALANCHE_I:
            case 0xffff:   /* This is because of the Avalanche I errata */
                cpu_type = CPU_AVALANCHE_I;
            break;
            
            case CPU_AVALANCHE_D:
            case CPU_PUMA:
            case CPU_PUMAS:
            case CPU_SANGAM:
            case CPU_TITAN:
			case CPU_APEX:
			case CPU_OHIO_7100:
			case CPU_OHIO_7200:
                cpu_type = cvr;
            break;
         
            /* TODO: More CPUs here */
         
            default:
                cpu_type = CPU_UNIDENT;
        }
    }
    else
    {
        /* CVR can be written, this is the Avalanche I */
        cpu_type = CPU_AVALANCHE_I;
    }
   
    return cpu_type;    
}

/*******************************************************************************
 * FUNCTION:    avalanche_get_cpu_name
 *******************************************************************************
 * DESCRIPTION: Function to get the cpu name
 *
 * RETURNS:     cpu name
 *
 ******************************************************************************/
const char* avalanche_get_cpu_name
(
    AVALANCHE_CPU_TYPE_T cpu_type
)
{
    char* name = NULL;
    static char* cpu_name[]=
    {
       "Unknown"    ,   /* 0 */ 
       "Avalanche I",   /* 1 */ 
       "Avalanche D",   /* 2 */ 
       "Puma"       ,   /* 3 */ 
       "Puma S"     ,   /* 4 */
       "Sangam"     ,   /* 5 */
       "Titan"      ,   /* 6 */
       "Apex"       ,   /* 7 */
	   "Ohio"       ,   /* 8 */
       
       /* TODO: Add more CPUs here */
       0
    };
    
    switch(cpu_type)
    {
        case CPU_AVALANCHE_I: 
            name = cpu_name[1];  
        break;
        case CPU_AVALANCHE_D: 
            name = cpu_name[2];  
        break;
        case CPU_PUMA : 
            name = cpu_name[3];
        break;
        case CPU_PUMAS: 
            name = cpu_name[4];  
        break;
        case CPU_SANGAM: 
            name = cpu_name[5];  
        break;        
        case CPU_TITAN: 
            name = cpu_name[6];  
        break;               
        case CPU_APEX: 
            name = cpu_name[7];  
        break; 
		case CPU_OHIO_7200:
		case CPU_OHIO_7100:
			name = cpu_name[8];
		break;

		case CPU_UNIDENT:
        default:        
            name = cpu_name[0];
    }
    
    return name;
}

/* Avalanche MDIX specific functionality */
SET_MDIX_ON_CHIP_FN_T p_set_mdix_on_chip_fn = NULL;

int avalanche_set_mdix_on_chip(unsigned int base_addr, unsigned int operation)
{
    if(p_set_mdix_on_chip_fn)
        return (p_set_mdix_on_chip_fn(base_addr, operation));
    else
        return(-1);
}

unsigned int avalanche_is_mdix_on_chip(void)
{
    return(p_set_mdix_on_chip_fn ? 1:0);
}
