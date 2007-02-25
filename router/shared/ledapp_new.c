/*
 * ledapp.c -- leds manipulation *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "../../linux/linux/include/asm/avalanche/generic/led_config.h"
#include "../../linux/linux/include/asm/avalanche/generic/led_ioctl.h"

#define MAX_KEYWORDS    7
#define MAX_STATES      (MAX_MODULE_ENTRIES * MAX_STATE_ENTRIES)
#define MODULE_NAME_LEN 80


#define LED_DEV_FILE  "/dev/led"
#define CONF_FILE  "/etc/led.conf"


int read_mod(char *line);
int read_instance(char *line);
int read_state(char *line);
int read_gpio(char *line);
int read_mode(char *line);
int read_param1(char *line);
int read_param2(char *line);
int read_param(char *line);

typedef struct config_kw_handler{
	char *name;
  int (*handler)(char *line);
}config_kw_handler_t;

config_kw_handler_t kwArr[] = {
                              {"module",read_mod},
                              {"instance",read_instance},
                              {"state",read_state},
                              {"gpio",read_gpio},
                              {"mode",read_mode},
                              {"param1",read_param1},
                              {"param2",read_param2},
                              {"param",read_param}
};



char  curModule[MODULE_NAME_LEN];
int curInstance=0;
int gpio_count;
char curName[15];


LED_CONFIG_T *pTemp;
LED_CONFIG_T *ptrArr[MAX_STATES];
int ptrIndex = 0;

char arr[255];

int read_mod(char *line)
{
  /* Remember the new module's name */
  strcpy(curModule,line);
  curInstance = 0;
  return 1;
}



int read_instance(char *line)
{
  /* Remember the new instance id */
  unsigned int i;
  i = atoi(line);

  if(i < MAX_MODULE_INSTANCES)
  {
  	curInstance = i;
        return 1;
  }

  else
  {
    printf("Invalid module instance id\n");
    return 0;
  }

}




int read_state(char *line)
{

  unsigned int i;

  /* Malloc memory for new state info */
  pTemp = malloc( sizeof(LED_CONFIG_T));
  if(pTemp == NULL)
    return 0;
  /* Remember the pointer */
  ptrArr[ptrIndex++] = (LED_CONFIG_T *)pTemp;
  memset(pTemp,0x00,sizeof(LED_CONFIG_T));

  /* save the module name,instance and state var */
  strcpy(pTemp->name,curModule);
  pTemp->instance=curInstance;


  i = atoi(line);

  if(i < MAX_STATE_ENTRIES)
  { 
    pTemp->state = i;
    pTemp->gpio_num = 0;
  }

  else
  {
    printf("Invalid module state id\n");
    return 0;
  }
  return 1;
}

int read_gpio(char *line)
{
  unsigned int i;

  /* Save the LED number */
  i = atoi(line);
  
  if(i < MAX_GPIO_PIN_NUM && pTemp->gpio_num < MAX_GPIOS_PER_STATE)
  { 
     pTemp->gpio[pTemp->gpio_num++] = atoi(line);
     return 1;
  }
  else
  {
    printf("Invalid gpio id/count\n");
    return 0;
  }
}

int read_mode(char *line)
{

  unsigned int i;

  i = atoi(line);

  if(i >= NUM_LED_MODES)
  {
    printf(" invalid LED mode\n");
    return 0;
  }  

  /* Save the mode */
  if(pTemp->gpio_num >= 1)
  pTemp->mode[pTemp->gpio_num - 1] = i;
  return 1;
}

int read_param1(char *line)
{
  pTemp->param1 = atoi(line);
  return 1;
}



int read_param2(char *line)
{
  pTemp->param2 = atoi(line);
  return 1;
}


int read_param(char *line)
{
  pTemp->param1 = pTemp->param2 = atoi(line);
  return 1;
}


int read_config(char *file)
{
	FILE *in;
	char buffer[80], orig[80], *token, *line;
	int i, j, index;

	
	if (!(in = fopen(file, "r"))) {
    printf("unable to open config file\n");
		return 0;
	}
	index = -1;	
	while (fgets(buffer, 80, in)) {
		if (strchr(buffer, '\n')) *(strchr(buffer, '\n')) = '\0';
		strncpy(orig, buffer, 80);
		if (strchr(buffer, '#')) *(strchr(buffer, '#')) = '\0';
		token = buffer + strspn(buffer, " \t");
		if (*token == '\0') continue;
		line = token + strcspn(token, " \t=");
		if (*line == '\0') continue;
		*line = '\0';
		line++;
		
		/* eat leading whitespace */
		line = line + strspn(line, " \t=");
		/* eat trailing whitespace */
		for (i = strlen(line) ; i > 0 && isspace(line[i-1]); i--);
		line[i] = '\0';
    for(j=0 ; j<MAX_KEYWORDS ; j++)
    {
			if (!strcasecmp(token, kwArr[j].name))
      {
				if (!kwArr[j].handler(line))
        {
          printf("unable to parse %s\n",line);
				  return 0;
        }
      }
		}
	}
	fclose(in);
	return 1;
}

int diag_led_ctl(char *ctrl_led)
{
	int fp;
	LED_STATE_T led_state;

	fp = open(LED_DEV_FILE, O_RDWR);
	
	if(!fp) {
		printf("open file %s failed!\n", LED_DEV_FILE);
        	return -1;
	}
	if (sscanf(ctrl_led, "%d,%d", &led_state.handle, &led_state.state_id) <  2) return 0;
	ioctl(fp, LED_ACTION, &led_state);
	
	close(fp);
//junzhao
/*
	printf("before enter.......%s\n", ctrl_led);
	if(strstr(ctrl_led, "3,"))
	{
		if(strcmp(ctrl_led, PPP_ACT))
		{
			memset(origin_ledstatus, 0, sizeof(origin_ledstatus));
			strncpy(origin_ledstatus, ctrl_led, sizeof(origin_ledstatus));
		}
	}
*/	
	return 1;
}
#if 1
int adsl_led_init()
#else
int main(int argc, char *argv[])
#endif
{
	FILE *in;
  unsigned int size;
  int i;
  LED_CONFIG_T *ptr;
  int *pBuffer;

  int fp;
#if 0

  if( argc > 3)
	{
		printf("\nLedcfg usage:   ledcfg -f <configuration file path>  \n");
		return -1;
	}
	else
  {
		if( (argc == 3) && ( strcmp( argv[1] , "-f") == 0)  && ( argv[2] != NULL) )
		{
		printf( " Reading LED config file %s \n",argv[2]);
  		read_config( argv[2] );
		}
		else if(argc == 1)
		{
		    printf( " Reading Standard Configuration File %s\n",CONF_FILE);
  		    read_config(CONF_FILE);
		}

               else
		{
		    printf("\nLedcfg usage:   ledcfg -f <configuration file path>  \n");
		    return -1;
		}


  }



#endif
	read_config(CONF_FILE);
   if ((fp = open(LED_DEV_FILE, O_RDWR)) == -1) {
        printf("unable to open LED device file\n");
		return 0;
	}

  /* Configure element through ioctl system calls */
  for ( i = 0 ; i < ptrIndex ; i++ )
  {

   LED_CONFIG_T *temp=ptrArr[i];

   ioctl(fp,LED_CONFIG,(int)temp);

  }


  printf("\n Configured %d states \n",ptrIndex);

  close(fp);  
  return 1;
}


