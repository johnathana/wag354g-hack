/******************************************************************************
 * FILE PURPOSE:    PSP Config Manager - Utilities API Header
 ******************************************************************************
 * FILE NAME:       psp_config_util.h
 *
 * DESCRIPTION:     These APIs provide the standard "C" string interfaces. 
 *                  Provided here to reduce dependencies on the standard libraries 
 *                  and for cases where psp_config would required to run before 
 *                  the whole system is loaded or outside the scope of the OS.
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/
 
#ifndef __PSP_CONFIG_UTIL_H__
#define __PSP_CONFIG_UTIL_H__

extern int   psp_config_strlen(char*);
extern int   psp_config_strcmp(char*, char*);
extern char* psp_config_strcpy(char*, char*);
extern void* psp_config_memcpy(void*, void*, unsigned int n);
extern char* psp_config_strchr(char*, char);

#endif /* __PSP_CONFIG_UTIL_H__ */
