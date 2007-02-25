/******************************************************************************
 * FILE PURPOSE:    PSP Config Manager - Configuration Build Header
 ******************************************************************************
 * FILE NAME:       psp_config_build.h
 *
 * DESCRIPTION:     Configuration Build API's.
 *
 * REVISION HISTORY:
 * 27 Nov 02 - PSP TII  
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#ifndef __PSP_CONF_BUILD_H__
#define __PSP_CONF_BUILD_H__

/*------------------------------------------------------------------------------
 * Name: psp_conf_read_file
 *
 * Parameters: 
 *         in: p_file_name - the name of the file to read from.
 *
 * Description:
 *     Reads the entire file in one shot. This function opens  the 
 *     file, determines the size of the data to be read, allocates 
 *     the required memory, NULL terminates the data and closes the
 *     file.
 *
 *     It is responsibily of the callee to free the memory after it is
 *     done with that data.
 *    
 *
 * Returns:
 *     A NULL pointer, if failed to read the data otherwise, a valid
 *     pointer referring to the data read from the file.
 *
 * Example: 
 *   
 *     psp_conf_read_file("/etc/options.conf");
 *---------------------------------------------------------------------------*/
 char *psp_conf_read_file(char *p_file_name);

 /*----------------------------------------------------------------------------
  * Function : psp_conf_write_file
  *
  * Parameters:
  *         in: p_file_name - the file to which data is to be written.
  *         in: data        - the NULL terminated data string.
  *
  * Description:
  *     Write the indicated data into the file. This function opens the file,
  *     appends the data to end of the file, closes the file. 
  *
  * Returns:
  *
  *     The number of bytes on success. 
  *     0 on failure.
  *
  * Example:
  *
  *     psp_conf_write_file("/etc/outcon.conf", data);
  *--------------------------------------------------------------------------*/
 int   psp_conf_write_file(char *p_file_name, char *data);

 /*----------------------------------------------------------------------------
  * Function: psp_conf_get_line
  *
  * Parameters:
  *         in: data      - the data from which the line is to identified.
  *        out: next_line - the pointer to start of the next line. 
  *
  * Description:
  *     Expects the data to be '\n' separated segments and data is NULL 
  *     terminated. Parses the given data for '\n' or '\0'. Provides a pointer
  *     to the start of next line in the next_line. 
  *
  * Returns:
  *     -1 on error.
  *      0 or more to indicate the number of bytes in the line starting at 
  *      data.
  *--------------------------------------------------------------------------*/
 int psp_get_conf_line(char *p_in_data, char **next_line);

 /*----------------------------------------------------------------------------
  * Function: psp_conf_is_data_line
  *
  * Parameters:
  *         in: line - the array of bytes.
  *
  * Description:
  *     Tests the first byte in the array for '\0' or '\n' or '#'. Lines 
  *     starting with these characters are not considered data.
  *
  * Returns:
  *     1 if the line has data.
  *     0 otherwise.
  *
  *--------------------------------------------------------------------------*/
 int psp_conf_is_data_line(char *line);

 /*----------------------------------------------------------------------------
  * Function: psp_conf_eat_white_spaces
  *
  * Parameters:
  *         in: line - the array of bytes.
  *
  * Description:
  *     Eats white spaces at the begining of the line while looking out for 
  *     '\0' or '\n' or ' '.
  * 
  * Returns:
  *     Pointer to the begining of the non white space character. 
  *     NULL if '\0' or '\n' is found.
  *
  *--------------------------------------------------------------------------*/
  char *psp_conf_eat_white_spaces(char *line);

  /*---------------------------------------------------------------------------
   * Function: psp_conf_get_key_size
   *
   * Parameters:
   *         in: line - the array of bytes.
   *
   * Description:
   *     Identifies the size of the 'key' in array formatted as 
   *     key(id=[key1]....). This function also checks out for '\0' and '\n'.
   *
   * Returns:
   *     On success, The number of bytes that forms the key. 
   *     0 otherwise. 
   *     
   *-------------------------------------------------------------------------*/
  int psp_conf_get_key_size(char *line);



#endif /* __PSP_CONF_BUILD_H__ */

