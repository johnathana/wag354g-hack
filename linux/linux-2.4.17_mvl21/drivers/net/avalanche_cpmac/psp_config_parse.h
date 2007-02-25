/******************************************************************************
 * FILE PURPOSE:    PSP Config Manager - Parse API Header
 ******************************************************************************
 * FILE NAME:       psp_config_parse.h
 *
 * DESCRIPTION:     Parsing for params from string available in the NBU format.
 *                  These APIs should be used only for scanvenging parameters which 
 *                  are stored in the following format.
 *                  
 *                  str[] = "module(id=[module], k1=v1, k2=[k3=v3, k4=v4], k5=v5)"
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __PSP_CONFIG_PARSER_H__
#define __PSP_CONFIG_PARSER_H__

/*------------------------------------------------------------------
 * These APIs should be used only for scanvenging parameters which 
 * are stored in the following format.
 *
 * str[] = "module(id=[module], k1=v1, k2=[k3=v3, k4=v4], k5=v5)"
 *-----------------------------------------------------------------*/
int psp_config_get_param_uint(char *p_in_str, const char *param, 
                              unsigned int *out_val);
int psp_config_get_param_string(char *p_in_str, const char *param, 
                                char **out_val);

#endif /* __PSP_CONFIG_PARSER_H__ */