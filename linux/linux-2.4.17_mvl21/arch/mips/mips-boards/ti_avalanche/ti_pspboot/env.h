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
 * FILE PURPOSE:    Environment variables header file.
 *******************************************************************************
 * FILE NAME:       env.h
 *
 * DESCRIPTION:     Environment variables header file.
 *
 * (C) Copyright 2003, Texas Instruments, Inc
 ******************************************************************************/

#ifndef _ENV_H_
#define	_ENV_H_

int  EnvInit(void);
char *env_get_base(unsigned int *block_size);
int  sys_unsetenv(char *);
int  sys_setenv(char *, char *);
char *sys_getenv(char *);
void sh_printenv(void);
void init_env(void);
int sys_defragenv(void);
int sys_initenv(void);
char* sys_getienv(int var_num);
char* sys_getivar (int var_num);
int get_envstring(int index, char *buffer);

typedef struct t_env_var {
	char      var;
	char  *	  val;
}t_env_var;

#ifdef FTP_SERVER_SUPPORT
#define ENV_SPACE_SIZE      (10 * 1024) 
#endif
#define AUTO_DEFRAG_ENVIRONMENT     TRUE

#endif /* _ENV_H_ */
