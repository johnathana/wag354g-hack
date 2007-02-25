/******************************************************************************
 * FILE PURPOSE:    PSP Config Manager - Parse API Source
 ******************************************************************************
 * FILE NAME:       psp_config_parse.c
 *
 * DESCRIPTION:     These APIs should be used only for scanvenging parameters which 
 *                  are stored in the following format.
 *
 *                  str[] = "module(id=[module], k1=v1, k2=[k3=v3, k4=v4], k5=v5)"
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#include <stdio.h>
//#include <ctype.h>

/*--------------------------------------------------
 * MACROS.
 *-------------------------------------------------*/
#define my_isdigit(c) (c >= '0' && c <= '9')
#define my_isoct(c)   (c >= '0' && c <= '7')  
#define my_xtod(c)    ((c) <= '9' ? (c) - '0' : (c) - 'a' +  10)
#define my_ifupper(c)    (c >= 'A' && c <= 'F')
#define XTOD(c)       ((c) - 'A' + 10)
#define my_ishex(c)   ((c >= 'a' && c <='f') || (c >= 'A' && c<='F') || my_isdigit(c) ) 

/*---------------------------------------------------
 * Local Functions.
 *--------------------------------------------------*/
static int p_get_substr_from_str(char *p_in_str, char begin_delimiter,
                                 char end_delimiter, int pair_flag,
                                 char **p_out_str);
static int p_get_u_int_from_str(char *p_in_str, char begin_delimiter,
                                char end_delimiter, unsigned long *out_val);

/*---------------------------------------------------
 * Return pointer to first instance of the char.
 *--------------------------------------------------*/
static char* psp_config_strchr(char *str, char chr)
{
    while(*str)
    {
        if(*str == chr)
            break;
        str++;    
    }
    
    return((*str) ? str : NULL);
}

/*------------------------------------------------------------------------
 * Convert the string upto delimiter to unsigned long.
 *-----------------------------------------------------------------------*/
unsigned long my_atoul(char *p, char end_delimiter, unsigned long *out_val)
{
    unsigned long n;
    int c;
 
    /* check the for null input */    
    if (!p)
        return -1;
     
    c = *p;    
    
    /* pass through the leading spaces */
    if (!my_isdigit(c)) 
    {
        while ( c == ' ')
            c = *++p;
            
    }
    
    if (c == '0')
    {
        if(*(p + 1) == 'x' || *(p+1) == 'X' ) 
        {
            /* string is in hex format */

            p += 2; 
            c = *p;
            
            if(my_ishex(c))
            {
               if(my_ifupper(c))
                   n = XTOD(c);
               else         
                       n = my_xtod(c);
            }
            else
                return -1; /* invalid hex string format */
                
            while ((c = *++p) && my_ishex(c)) 
            {
                n *= 16; 
                if(my_ifupper(c))
                   n += XTOD(c);
                else         
                       n += my_xtod(c);
            }
         }
        else
        {
            /* string is in octal format */

            if( my_isoct(c) )
            n = c - '0';
            else
                return -1; /* invalid octal string format */
 
            while ((c = *++p) && my_isoct(c)) 
            {
                n *= 8; 
                n += c - '0'; 
            }
        } 
         
    }      
    else 
    {
        /* string is in decimal format */
 
        if( my_isdigit(c) )
            n = c - '0';
        else
            return -1; /* invalid decimal string format */

        while ((c = *++p) && my_isdigit(c)) 
        {
            n *= 10; 
            n += c - '0'; 
        }
    }
   
    /* move through the trailing spaces */ 
    while(*p == ' ')
        p++;
        
    if(*p == end_delimiter)
       {
           *out_val = n;
           return 0;
       }   
        
    else
        return -1; /* invalid string format */
}

/*---------------------------------------------------------------------------------
 * Gets the substring de-limited by the 'begin_delimiter' and 'end_delimiter'.
 * and returns the size of the substring. 
 *
 * Parses the NULL terminated p_in_str for a character array delimited by 
 * begin_delimiter and end_delimiter, passes back the pointer to the character 
 * array in ' *p_out_str '. The passed pointer ' *p_out_str ' should point to 
 * the location next (byte) to the begin_delimiter. The function routine returns 
 * the number of characters excluding the begin_delimiter and end_delimiter, 
 * found in the array delimited by the said delimiters.
 *
 * If the pair_flag is set to 1, then, number of begin_delimiter and end_delimiter
 * found in the parsing should match (equal) and this routine passes back the 
 * pointer to the character array, starting at a location next (byte) to the 
 * first begin_delimiter,  inclusive of all intermediate matching delimiter 
 * characters found between outer delimiters. If the pair flag is set and if 
 * begin_delimiter and end_delimiter happens to be same, then error (-1) is 
 * returned.
 * 
 * Return: 0 or more to indicate the size of the substring, -1 on error.
 *-------------------------------------------------------------------------------*/
int p_get_substr_from_str(char *p_in_str, char begin_delimiter, 
                          char end_delimiter, int pair_flag, 
                          char **p_out_str) 
{
    int cnt,pos;

    if(pair_flag  && begin_delimiter == end_delimiter)
        return -1;
    
    if((p_in_str = psp_config_strchr(p_in_str, begin_delimiter)) == 0)
        return -1; /* no start delimiter found */
    
    p_in_str++;      
    *p_out_str = p_in_str;
     
    for(pos = 0,cnt =1; cnt && p_in_str[pos] ; pos++)
    {
        if(p_in_str[pos] == end_delimiter)
        {
            if(pair_flag == 0)
                return pos;
               
            cnt--;                  
        }             
        else if(p_in_str[pos] == begin_delimiter)
            cnt++;
        else 
            ; /* We do nothing */
      
    }
    
    if( cnt == 0)   
        return pos - 1;
    else
        return -1; /* no corresponding end delimiter found */
}

/*--------------------------------------------------------------------------    
 * Parses the NULL terminated p_in_str for unsigned long value delimited by 
 * begin_delimiter and end_delimiter, passes back the found in ' *out_val '.  
 * The function routine returns 0 on success and returns -1 on failure. 
 * The first instance of the de-limiter should be accounted for the parsing.
 *
 * The base for unsigned value would 10, octal and hex. The value passed back 
 * would be of the base 10. Spaces at the begining of the byte array are valid  
 * and should be ingnored in the calculation of the value. Space character in 
 * the middle of the byte array or any character other than the valid ones 
 * (based on base type) should return error. The octal value begins with '0', 
 * the hex value begins with "0x" or "0X", the base value can begin with 
 * '1' to '9'.
 * 
 * Returns: 0 on success, -1 on failure.
 *-------------------------------------------------------------------------*/
int p_get_u_int_from_str(char *p_in_str, char begin_delimiter, 
                         char end_delimiter, unsigned long *out_val) 
{
    char *start;
    unsigned long num;
    
    num = p_get_substr_from_str(p_in_str, begin_delimiter, end_delimiter,
                                0, &start);

    if(num == (unsigned long)-1)
        return -1;
    
    return my_atoul(start,end_delimiter,out_val);
}

/*--------------------------------------------------------------------------
 * Finds the first occurrence of the substring p_find_str in the string
 * p_in_str. 
 *-------------------------------------------------------------------------*/
char *my_strstr(char *p_in_str, const char *p_find_str)
{
    char *p = (char *)p_find_str;
    char *ret = NULL;

    while(*p_in_str)
    {
        if(!(*p))
            return (ret);
        else if(*p_in_str == *p)
        {
            if(!ret) ret = p_in_str;
            p++;
            p_in_str++;
        }
        else if(ret)
        {
            p = (char *)p_find_str;
            p_in_str = ret + 1;
            ret = NULL;
        }
        else
            p_in_str++;
    }

    if(*p_in_str != *p) ret = NULL;

    return (ret);

}

/*------------------------------------------------------------------------------
 * Gets the value of the config param in the unsigned int format. The value is 
 * stored in the following format in the string.
 * str[] = "module(id=[module], k1=v1, k2=[k3=v3, k4=v4], k5=v5)"
 *-----------------------------------------------------------------------------*/
int psp_config_get_param_uint(char *p_in_str, const char *param, unsigned int *out_val)
{
    int   ret_val = -1;
    char *p_strstr;

    if(!p_in_str || !param || !out_val) 
    {
        ;
    } 
    else if((p_strstr = my_strstr(p_in_str, param)) == NULL)
    {
        ;
    }
    else if(p_get_u_int_from_str(p_strstr, '=', ',', (unsigned long *)out_val) == 0)
    {
        ret_val = 0;
    }
    else if(p_get_u_int_from_str(p_strstr, '=', ']', (unsigned long*)out_val) == 0)
    {
        ret_val = 0;
    }
    else if(p_get_u_int_from_str(p_strstr, '=', ')', (unsigned long*)out_val) == 0)
    {
        ret_val = 0;
    }
    else
    {
        /* we failed */
    }

    return (ret_val);
}
 
/*------------------------------------------------------------------------------
 * Gets the value of the config param in the Non NULL terminated format. The value 
 * is stored in the following format in the string. 
 * str[] = "module(id=[module], k1=v1, k2=[k3=v3, k4=v4], k5=v5)"
 *-----------------------------------------------------------------------------*/
int psp_config_get_param_string(char *p_in_str, const char *param, char **out_val)
{
    int ret_val = -1;
    char *p_strstr;

    if(!p_in_str || !param || !(out_val))
        ;
    else if((p_strstr = my_strstr(p_in_str, param)) == NULL)
    {
        ;
    }
    else if((ret_val = p_get_substr_from_str(p_strstr, '[', ']', 1, out_val)) == -1)
    {
        ;
    }
    else
    {
        ; /* we got the value */
    }   
 
    return (ret_val); 
} 

#ifdef PSP_CONFIG_MGR_DEBUG_TEST
main()
{
    unsigned long num =999;
    int ret = 0;
    char *val1 = NULL;
    char val[30];
    char str1[] = "cpmac(id=[cpmac], k0=[a1=[a2=[test], a3=2], k1=100, k2=[k3=300, k4=200], k7=722)";

    psp_config_get_param_uint(str1, "k7", &num);
    printf("%u.\n", num);    
    ret = psp_config_get_param_string(str1, "a1", &val1); 
    if(ret >= 0) { printf("%d.\n", ret); strncpy(val, val1, ret); val[ret] = '\0';}
    
    printf("val = \"%s\", and size = %d \n", val, ret);

    if(val[ret]) ; else printf("jeee.\n");
}
#endif /* PSP_CONFIG_MGR_DEBUG_TEST */



