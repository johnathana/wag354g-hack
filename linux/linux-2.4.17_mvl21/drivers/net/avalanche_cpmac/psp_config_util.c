/******************************************************************************
 * FILE PURPOSE:    PSP Config Manager - Utilities API Source
 ******************************************************************************
 * FILE NAME:       psp_config_util.c
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

#include <stdio.h>
#include "psp_config_util.h"

/*---------------------------------------------
 * strlen.
 *-------------------------------------------*/
int psp_config_strlen(char *p)
{
    char *p_orig = p;
    while(*p)
        p++;
    return(p - p_orig);
}

/*--------------------------------------------
 * strcmp.
 *-------------------------------------------*/
int psp_config_strcmp(char *s1, char *s2)
{
    while(*s1 && *s2)
    {
        if(*s1 != *s2)
            break;
        s1++;
        s2++;
    }

    return(*s1 - *s2);
}

/*--------------------------------------------
 * strcpy.
 *------------------------------------------*/
char* psp_config_strcpy(char *dest, char *src)
{
    char *dest_orig = dest;

    while(*src)
    {
        *dest++ = *src++;
    }

    *dest = '\0';

    return(dest_orig);
}

/*----------------------------------------------
 * psp_config_memcpy.
 *--------------------------------------------*/
void* psp_config_memcpy(void* dest, void* src, unsigned int n)
{
    void *dest_orig = dest;

    while(n)
    {
        *(char *)dest++ = *(char *)src++;
        n--;
    }

    return (dest_orig);
}

/*---------------------------------------------------
 * Return pointer to first instance of the char.
 *--------------------------------------------------*/
char* psp_config_strchr(char *str, char chr)
{
    while(*str)
    {
        if(*str == chr)
            break;
        str++;    
    }
    
    return((*str) ? str : NULL);
}

#ifdef PSP_CONFIG_MGR_DEBUG_TEST

int main( )
{
    char s[] = "hello             ";
    printf("%d.\n", psp_config_strlen("hello\n"));
    printf("%d.\n", psp_config_strcmp("hells", "hellq"));
    printf("%s %s.\n", psp_config_strcpy(s + 6, "test1"), s);
}

#endif /* PSP_CONFIG_MGR_DEBUG_TEST */
