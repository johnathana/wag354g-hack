/******************************************************************************
 * FILE PURPOSE:    TNETD73xx Error Definations Header File
 ******************************************************************************
 * FILE NAME:       tnetd73xx_err.h
 *
 * DESCRIPTION:     Error definations for TNETD73XX
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

 
#ifndef __TNETD73XX_ERR_H__
#define __TNETD73XX_ERR_H__

typedef enum TNETD73XX_ERR_t
{
    TNETD73XX_ERR_OK        = 0,    /* OK or SUCCESS */
    TNETD73XX_ERR_ERROR     = -1,   /* Unspecified/Generic ERROR */

    /* Pointers and args */
    TNETD73XX_ERR_INVARG        = -2,   /* Invaild argument to the call */
    TNETD73XX_ERR_NULLPTR       = -3,   /* NULL pointer */
    TNETD73XX_ERR_BADPTR        = -4,   /* Bad (out of mem) pointer */

    /* Memory issues */
    TNETD73XX_ERR_ALLOC_FAIL    = -10,  /* allocation failed */
    TNETD73XX_ERR_FREE_FAIL     = -11,  /* free failed */
    TNETD73XX_ERR_MEM_CORRUPT   = -12,  /* corrupted memory */
    TNETD73XX_ERR_BUF_LINK      = -13,  /* buffer linking failed */

    /* Device issues */
    TNETD73XX_ERR_DEVICE_TIMEOUT    = -20,  /* device timeout on read/write */
    TNETD73XX_ERR_DEVICE_MALFUNC    = -21,  /* device malfunction */

    TNETD73XX_ERR_INVID     = -30   /* Invalid ID */

} TNETD73XX_ERR;

#endif /* __TNETD73XX_ERR_H__ */
