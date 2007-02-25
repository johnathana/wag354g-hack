/******************************************************************************    
 * FILE PURPOSE:     - LED Wrapper file for DSL module support                                       
 ******************************************************************************    
 * FILE NAME:     led_wrapper.c                                                        
 *                                                                                 
 * DESCRIPTION:   LED Wrapper file for DSL module support     
 *                This is to provide backward compatibility to the ADSL module 
 *                using OLD LED driver. The numbers mapped for DSL LEDs in the
 *                previous implementation is  3,4,5,6. Since these numbers overlap 
 *                with the existing numbering scheme, the following numbers need to 
 *                be used in the led configuration file - 32,33,34,35.                                            
 *                                                                                 
 * (C) Copyright 2002, Texas Instruments, Inc                                      
 *******************************************************************************/  
#include <linux/config.h>
#include <linux/init.h>
#include <linux/module.h>
// #include <asm/avalanche/avalanche_map.h>
#include <asm/avalanche/sangam/sangam.h>
#include "led_platform.h"
#include "led_config.h" 

#define BITS_PER_INT               (8 * sizeof(int))                                      
#define GPIOS_PER_INT              BITS_PER_INT                                           
#define GPIO_MAP_LEN               ((MAX_GPIO_PIN_NUM + GPIOS_PER_INT -1)/GPIOS_PER_INT)  
#define MODULE_MAP_LEN             ((MAX_MODULE_ENTRIES + BITS_PER_INT -1)/BITS_PER_INT)  
                                                                                          
                                                                                               
#define REQUIRES_TIMER(x)         (x > 1)                                                      
                                                                                               
#define gpio_on(gpio)  do \
                    {  \
                          if(gpio >= 32 && adsl_led_objs[gpio - 32].onfunc) \
                              adsl_led_objs[gpio - 32].onfunc(adsl_led_objs[gpio - 32].param);\
                   } while(0)



#define gpio_off(gpio)  \
                do \
                {  \
                      if(gpio >= 32 && adsl_led_objs[gpio - 32].offfunc) \
                          adsl_led_objs[gpio - 32].offfunc(adsl_led_objs[gpio - 32].param);\
                } while(0)
                                                                                               
                                                                                               
                                                                                                                                                                                         
                                                                                          
                                                                                          
                                                                                          
/********************TYPEDEFS***********************************************/             
                                                                                          
typedef struct gpio_module                                                            
{                                                                                     
    volatile unsigned int *gpio_write_reg;                                            
    volatile unsigned int *gpio_dir_reg;                                              
    volatile unsigned int *gpio_mode_reg;                                             
}GPIO_REGS_T;                                                                         
                                                                                          
typedef struct {                                                                      
    unsigned int gpio_id;                                                             
    unsigned int gpio_state;                                                          
    int module_map[MODULE_MAP_LEN];                                                   
}GPIO_OBJ_T;                                                                          
                                                                                          
                                                                                          
typedef struct state_entry STATE_ENTRY_T;                                             
                                                                                          
struct state_entry{                                                                  
    unsigned int timer_running;                                                       
    STATE_ENTRY_T *next;                                                              
    void (*handler)(STATE_ENTRY_T *state);                                            
    GPIO_OBJ_T *gpio;                                                                 
    void  *os_timer;                                                                  
    unsigned int param;                                                               
    unsigned int module_id;                                                           
    unsigned int mode;                                                                
};                                                                                    
                                                                                          
                                                                                          
typedef struct module_instance{                                                       
    int module_id;                                                                    
    int instance;                                                                     
    STATE_ENTRY_T *states[MAX_STATE_ENTRIES];                                         
}MODULE_INSTANCE_T;                                                                   
                                                                                          
typedef struct module_entry{                                                          
    unsigned char *name;                                                              
    MODULE_INSTANCE_T *module_instance[MAX_MODULE_INSTANCES];                         
}MODULE_ENTRY_T;                                                                      
                                                                                          
                                                                                          

typedef struct led_reg{
    unsigned int param;
    void (*init)(unsigned long param);
    void (*onfunc)(unsigned long param);
    void (*offfunc)(unsigned long param);
}led_reg_t;



/* Interface prototypes */                                                            
static int led_hal_init(GPIO_REGS_T gpio_mod, unsigned int *gpio_off_value,int num_gpio_pins);  
static int avalanche_led_set_config(LED_CONFIG_T *led_cfg);                                     
static void *avalanche_led_register(const char *module_name, int instance_num);                 
static int avalanche_led_action(void *handle,int state_id);                                     
int avalanche_led_config_get (LED_CONFIG_T *led_cfg,int module_id,int instance, int state);

led_reg_t adsl_led_objs[4];
MODULE_INSTANCE_T *dsl_mod = NULL; 
static int gpio_off_state[GPIO_MAP_LEN] = AVALANCHE_GPIO_OFF_MAP; 



static    unsigned int num_gpios;
static GPIO_OBJ_T *gpio_arr;
GPIO_REGS_T gpio_regs;

/*  GPIO OFF STATE  */
static unsigned int gpio_off_val[GPIO_MAP_LEN];

    
MODULE_ENTRY_T *modules[MAX_MODULE_ENTRIES];

/* LED handlers */
void (*led_mode_handler[NUM_LED_MODES])(STATE_ENTRY_T *state);


/******************static functions*****************************************/
static void *led_malloc(int n)
{
    void *p;
    p=kmalloc(n,GFP_ATOMIC);

    if(p)
        os_memset(p,0,n);
    return p;
}

static void free_state(STATE_ENTRY_T *state)
{

    STATE_ENTRY_T *prev = NULL;
    STATE_ENTRY_T *curr = NULL ;
    while(curr != state)
    {
        curr = state;
        prev = NULL;

        while(curr->next != NULL)
        {
            prev = curr;
            curr = curr->next;
        }
       
        os_free(curr);
        if(prev)
        {   
            prev->next = NULL;
        }
        
    }

}

static MODULE_INSTANCE_T* get_module(char * name,int instance)
{
    int module_id;
    MODULE_INSTANCE_T *mod_inst;

    if(instance >= MAX_MODULE_INSTANCES)
        return NULL;

    for(module_id=0;module_id <MAX_MODULE_ENTRIES;module_id++)
    {
        if(modules[module_id] && !os_strcmp(name,modules[module_id]->name))
                     break;
    }

    if(module_id == MAX_MODULE_ENTRIES)
    {
        for(module_id = 0; (module_id < MAX_MODULE_ENTRIES) && modules[module_id] ; module_id++);

        if(module_id < MAX_MODULE_ENTRIES)
        {
            modules[module_id]=led_malloc(sizeof(MODULE_ENTRY_T));
            modules[module_id]->name = led_malloc(os_strlen(name));
            os_strcpy(modules[module_id]->name,name);
        }
        else
        {
            log_msg("ERROR:Module Count exceeded\n");
            return NULL;
        }
    }

    if(!modules[module_id]->module_instance[instance])
        modules[module_id]->module_instance[instance] = led_malloc(sizeof(MODULE_INSTANCE_T));

    mod_inst = modules[module_id]->module_instance[instance];
    mod_inst->module_id = module_id;
    mod_inst->instance = instance;

    return mod_inst;
}


static void led_timer_func(int arg)
{
    STATE_ENTRY_T *state = (STATE_ENTRY_T *) arg;
    GPIO_OBJ_T *gpio;


    gpio = state->gpio;

    switch(gpio->gpio_state)
    {
        case LED_ONESHOT_ON:
            gpio->gpio_state = LED_OFF;
            gpio_off(gpio->gpio_id);
            break;

        case LED_ONESHOT_OFF:
            gpio->gpio_state = LED_ON;
            gpio_on(gpio->gpio_id);
            break;

        case LED_FLASH:
	    {
                  	
                if(state->timer_running == 1)
                {
                    state->timer_running = 2;
                    gpio_off(gpio->gpio_id);
                    os_timer_add(state->os_timer,(state->param >> 16),(int)state);
                }
                else
                {
                    state->timer_running = 1;
                    gpio_on(gpio->gpio_id);
                    os_timer_add(state->os_timer, (state->param & 0xffff),(int)state);
                }
                return;
            }
        default:
            /*log_msg("invalid state in timer func\n"); */

    }

    state->timer_running = 0;


}


static void led_on(STATE_ENTRY_T *state)
{
    int mod_index = state->module_id >> 5;
    GPIO_OBJ_T *gpio = state->gpio;	

    gpio->gpio_state = LED_ON;
    gpio_on(gpio->gpio_id);
    gpio->module_map[mod_index] |= (1 <<  (state->module_id % BITS_PER_INT));

}

static void led_off(STATE_ENTRY_T *state)
{

    int mod_index = state->module_id >> 5;
    GPIO_OBJ_T *gpio = state->gpio;	
    
    gpio->module_map[mod_index] &= ~(1 <<  (state->module_id % BITS_PER_INT) );
    if(!gpio->module_map[mod_index])
    {
        gpio->gpio_state = LED_OFF;
        gpio_off(gpio->gpio_id);
    }

}

static void led_oneshot_on(STATE_ENTRY_T *state)
{
    GPIO_OBJ_T *gpio = state->gpio;

    state->timer_running = 1; 
    gpio->gpio_state = LED_ONESHOT_ON;
    gpio_on(gpio->gpio_id);
    os_timer_add(state->os_timer,state->param,(int)state);
}

static void led_oneshot_off(STATE_ENTRY_T *state)
{

    GPIO_OBJ_T *gpio = state->gpio;

    state->timer_running = 1;
    gpio->gpio_state = LED_ONESHOT_OFF;
    gpio_off(gpio->gpio_id);
    os_timer_add(state->os_timer,state->param,(int)state);
}

static void led_flash(STATE_ENTRY_T *state)
{

    GPIO_OBJ_T *gpio = state->gpio;
 
    state->timer_running = 1;
    gpio->gpio_state = LED_FLASH;
    gpio_on(gpio->gpio_id);
    os_timer_add(state->os_timer,(state->param & 0xffff),(int)state);
}


/****************HAL APIS***********************************************/
int led_hal_init(GPIO_REGS_T gpio_reg,unsigned int *gpio_off_value,int num_gpio_pins)
{
        int i;
        unsigned int *p_gpio=gpio_off_val;

        gpio_regs = gpio_reg;
        num_gpios = num_gpio_pins;

        gpio_arr = led_malloc((num_gpio_pins + 4) * sizeof(GPIO_OBJ_T)); /* 4 added for ADSL gpio pins */

        /* get gpios off state */
        for(i=0; i < num_gpio_pins; i+=GPIOS_PER_INT)
        {
            *p_gpio = *gpio_off_value;
            gpio_off_value++;
            p_gpio++;
        }

        /* initialize gpio objects */
        for(i=0; i<num_gpio_pins + 4;i++) /* 4 added for ADSL gpio pins */
        {
            gpio_arr[i].gpio_id = i;
        }

        

	/* initialize led state function handlers */
	led_mode_handler[LED_ON] = led_on;
	led_mode_handler[LED_OFF] = led_off;
	led_mode_handler[LED_ONESHOT_ON] = led_oneshot_on;
	led_mode_handler[LED_ONESHOT_OFF] = led_oneshot_off;
	led_mode_handler[LED_FLASH] = led_flash;

 
        return 0;
}


    /*********************************************************/

int avalanche_led_set_config(LED_CONFIG_T *led_cfg)
{
        MODULE_INSTANCE_T *module;
        module = get_module(led_cfg->name,led_cfg->instance);

        if(!module )
            goto config_failed;

        if(led_cfg->state < MAX_STATE_ENTRIES)
        {
            STATE_ENTRY_T *state_entry;
            int state=led_cfg->state;
            int i;

            if(!(module->states[state]))
            {
                module->states[state] = led_malloc(sizeof(STATE_ENTRY_T));
            }

            state_entry = module->states[state];

            for(i=0;i<led_cfg->gpio_num;i++)
            {
                if(led_cfg->gpio[i] >= (num_gpios + 4) ) /* 4 added for ADSL */
                {
                    log_msg("Error: gpio number out of range\n");
                    goto config_failed;
                }

                state_entry->gpio = &gpio_arr[led_cfg->gpio[i]];
                state_entry->mode = led_cfg->mode[i];
                state_entry->module_id = module->module_id;
                state_entry->handler = led_mode_handler[state_entry->mode]; 
                state_entry->timer_running = 0;

                if(REQUIRES_TIMER(led_cfg->mode[i])) /* requires timer */
                {

                    state_entry->param = led_cfg->param1;

                    if(led_cfg->mode[i] == LED_FLASH)
                        state_entry->param |= (led_cfg->param2 << 16);

                    if(!(state_entry->os_timer))
                        state_entry->os_timer = os_timer_init(led_timer_func);
                }

                if(i == led_cfg->gpio_num - 1)
                {
                   free_state(state_entry->next); 
                   state_entry->next = NULL;
                   break;
                }  
               
               
                /* allocate next node */
                else if( !(state_entry->next))
                {
                    state_entry->next = led_malloc(sizeof(STATE_ENTRY_T));
                }
                
                state_entry = state_entry->next;


            }

        }
        else
        {
            log_msg("ERROR:State Count exceeded\n");
            goto config_failed;
        }

        return 0;

        config_failed:
 
           return -1;



}

    /*********************************************************/

void *avalanche_led_register(const char * mod_name,int instance)
{
        void *p;
        p = get_module((void * )mod_name,instance);
        return p;
    }


int avalanche_led_action(void *module, int state_id)
{

        if(module && state_id < MAX_STATE_ENTRIES)
        {

            STATE_ENTRY_T *state =((MODULE_INSTANCE_T *)(module))->states[state_id];
            while(state)
            {
		if(state->timer_running == 0)
                    {
                        state->handler(state);  
                    }
                state = state->next; 
               
            }
        }
        return 0;
}



int led_get_dsl_config(void)
{
    
    int state_id = 0;
    LED_CONFIG_T led_cfg;
    int state_count = 0;
 
    os_strcpy(led_cfg.name,"adsl");

    for(state_id = 0; state_id < MAX_STATE_ENTRIES;state_id++)
    {
        if(avalanche_led_config_get(&led_cfg,-1,0,state_id) == 0)     
        {   
           /* call configure */
           avalanche_led_set_config(&led_cfg);    
           state_count++;
        }

    }
    return state_count;


}


void register_led_drv(int led_num,led_reg_t *led)
{

    /* DSL leds are numbered from 3  to 6 */
    int led_index = led_num - 3;

    if(led_index >=0 && led_index <= 2)
    {   
        adsl_led_objs[led_index] = *led;

        if(adsl_led_objs[led_index].init)
            adsl_led_objs[led_index].init(adsl_led_objs[led_index].param);
    }    

}

void deregister_led_drv( int led_num)
{
    /* DSL leds are numbered from 3  to 6 */
    int led_index = led_num - 3;
             
    if(led_index >=0 && led_index <= 2)
    {   
        adsl_led_objs[led_index].onfunc = NULL;
        adsl_led_objs[led_index].offfunc = NULL;
    }
    return;
}

void led_operation(int mod,int state_id)
{
    static int configured = 0;
  
    if(configured == 0)
    {
       configured = led_get_dsl_config();
    }
   
    avalanche_led_action(dsl_mod,state_id); 
}

static int __init  led_init(void)
{
    GPIO_REGS_T gpio_regs;  

    gpio_regs.gpio_write_reg = NULL;
    gpio_regs.gpio_dir_reg =  NULL;       
    gpio_regs.gpio_mode_reg = NULL;   
  
    led_hal_init(gpio_regs,gpio_off_state,AVALANCHE_GPIO_PIN_COUNT);     

    /* register instance 0 of adsl module */   
    dsl_mod = avalanche_led_register("adsl",0);
    return 0;

}

__initcall(led_init);



EXPORT_SYMBOL_NOVERS(led_init);
EXPORT_SYMBOL_NOVERS(led_operation);
EXPORT_SYMBOL_NOVERS(register_led_drv);
EXPORT_SYMBOL_NOVERS(deregister_led_drv);

