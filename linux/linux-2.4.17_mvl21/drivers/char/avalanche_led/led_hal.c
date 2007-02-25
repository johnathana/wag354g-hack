/******************************************************************************
 * FILE PURPOSE:     - LED HAL module source
 ******************************************************************************
 * FILE NAME:     led_hal.c
 *
 * DESCRIPTION:    LED HAL API's source.
 *
 * REVISION HISTORY:
 * 27 Aug 2003    Initial Creation                      Sharath Kumar<krs@ti.com>  
 * 
 * 16 Dec 2003    Updates for 5.7                       Sharath Kumar<krs@ti.com>       
 *
 * 07 Jan 2004    Wrapper for DSL                       Sharath Kumar<krs@ti.com>                                                   
 *
 * (C) Copyright 2002, Texas Instruments, Inc
 *******************************************************************************/

#include "led_platform.h"
#include "led_config.h"
#include "led_hal.h"

/* include for gpio APIs */
#include "avalanche_misc.h"


#define REQUIRES_TIMER(x) ( (x == LED_ONESHOT_ON) || (x == LED_ONESHOT_OFF) || (x == LED_FLASH) )

/*******************TYPEDEFs**************************************************/
typedef struct
{
    unsigned int state;
    unsigned int module_map;
    unsigned int pos_map[2];
}
LED_OBJ_T;

typedef struct state_entry STATE_ENTRY_T;

struct state_entry
{
    void (*handler) (STATE_ENTRY_T * state);
    unsigned int timer_running;
    LED_OBJ_T *led;
    unsigned int map1[2];
    unsigned int map2[2];
    void *os_timer;
    unsigned int param1;
    unsigned int param2;
    unsigned int module_id;
    unsigned int mode;
};

typedef struct module_instance
{
    int module_id;
    int instance;
    STATE_ENTRY_T *states[MAX_STATE_ENTRIES];
}
MODULE_INSTANCE_T;

typedef struct module_entry
{
    unsigned char *name;
    MODULE_INSTANCE_T *module_instance[MAX_MODULE_INSTANCES];
}
MODULE_ENTRY_T;



/******************variable defn/declns***************************************/

static LED_OBJ_T *leds[MAX_LED_ENTRIES];
static MODULE_ENTRY_T *modules[MAX_MODULE_ENTRIES];

/* Declare Mutex lock */
MUTEX_DECLARE (led_lock);

/*  GPIO OFF STATE  */
static unsigned int gpio_offstate_map[2];

/*  Number of GPIO pins in the system  */
static unsigned int num_gpios;

/* LED handlers */
static void (*led_mode_handler[NUM_LED_MODES]) (STATE_ENTRY_T * state);



/******************static functions*****************************************/

static void *led_malloc (int n)
{
    void *p;
    p = os_malloc (n);

    if (p)
        os_memset (p, 0, n);

    return p;
}

static void avalanche_gpio_set(int * set_map,int *pos_map)
{
    int i;
    
    for(i = 0;i <num_gpios ;i+=32)
    {
        int index = i/32;
        avalanche_gpio_out_value(set_map[index],pos_map[index],index);
        
    } 

}


static MODULE_INSTANCE_T *get_module (char *name, int instance)
{
    int module_id;
    MODULE_INSTANCE_T *mod_inst;


    if (instance >= MAX_MODULE_INSTANCES)
        return NULL;

    for (module_id = 0; module_id < MAX_MODULE_ENTRIES; module_id++)
      {
          if (modules[module_id]
              && !os_strcmp (name, modules[module_id]->name))
              break;
      }

    if (module_id == MAX_MODULE_ENTRIES)
      {
          for (module_id = 0;
               (module_id < MAX_MODULE_ENTRIES) && modules[module_id];
               module_id++);

          if (module_id < MAX_MODULE_ENTRIES)
            {
                modules[module_id] = led_malloc (sizeof (MODULE_ENTRY_T));

                if (!modules[module_id])
                    return NULL;

                modules[module_id]->name = led_malloc (os_strlen (name) + 1);

                if (!modules[module_id]->name)
                    return NULL;

                os_strcpy (modules[module_id]->name, name);
            }
          else
            {
                log_msg ("ERROR:Module Count exceeded\n");
                return NULL;
            }
      }

    if (!modules[module_id]->module_instance[instance])
        modules[module_id]->module_instance[instance] =
            led_malloc (sizeof (MODULE_INSTANCE_T));

    if (!modules[module_id]->module_instance[instance])
        return NULL;

    mod_inst = modules[module_id]->module_instance[instance];
    mod_inst->module_id = module_id;
    mod_inst->instance = instance;

    return mod_inst;
}


static void assign_map(int *to, int *from)
{
    int i;
    
    for(i = 0;i <num_gpios ;i+=32)
    { 
        int index = i/32;
        to[index] = from[index];
    }
} 




static LED_OBJ_T *get_led (int * pos_map)
{
    int led_id;

    for (led_id = 0; led_id < MAX_LED_ENTRIES; led_id++)
      {
          if (leds[led_id])
          {
              int i;
              int flag=0;
    
               for(i = 0;i <num_gpios ;i+=32)
               { 
                   int index = i/32;
                   if(leds[led_id]->pos_map[index] != pos_map[index])
                       flag =1;
               } 
               if(flag == 0)
                   break;
          }
      }

    if (led_id == MAX_LED_ENTRIES)
      {
          for (led_id = 0; (led_id < MAX_LED_ENTRIES) && leds[led_id];
               led_id++);

          if (led_id < MAX_LED_ENTRIES)
            {
                leds[led_id] = led_malloc (sizeof (LED_OBJ_T));

                if (!leds[led_id])
                    return NULL;

                assign_map(leds[led_id]->pos_map,pos_map);
            }
          else
            {
                log_msg ("ERROR:Module Count exceeded\n");
                return NULL;
            }
      }

    return leds[led_id];
}

static void led_oneshot_on_timer_func (int arg)
{
    STATE_ENTRY_T *state = (STATE_ENTRY_T *) arg;
    LED_OBJ_T *led = state->led;

    state->timer_running = 0;
    MUTEX_GET (led_lock);
    if (led->state == LED_ONESHOT_ON)
    {    
        led->state = LED_OFF;
        avalanche_gpio_set (state->map2,led->pos_map);
    }
    MUTEX_RELEASE (led_lock);

}

static void led_oneshot_off_timer_func (int arg)
{
    STATE_ENTRY_T *state = (STATE_ENTRY_T *) arg;
    LED_OBJ_T *led = state->led;

    state->timer_running = 0;

    MUTEX_GET (led_lock);
    if (led->state == LED_ONESHOT_OFF)
    {
        led->state = LED_ON;
        avalanche_gpio_set(state->map2,led->pos_map);
    }
    MUTEX_RELEASE (led_lock);

}

static void led_flash_timer_func (int arg)
{
    STATE_ENTRY_T *state = (STATE_ENTRY_T *) arg;
    LED_OBJ_T *led = state->led;


    if (led->state != LED_FLASH)
        return;

    MUTEX_GET (led_lock);
    
    if (state->timer_running == 1)
      {
          state->timer_running = 2;
          avalanche_gpio_set(state->map2,led->pos_map);
          os_timer_add (state->os_timer, state->param2, (int)state);
      }
    else
      {
          state->timer_running = 1;
          avalanche_gpio_set(state->map1,led->pos_map);
          os_timer_add (state->os_timer, state->param1, (int)state);
      }

    MUTEX_RELEASE (led_lock);
}

static void led_on(STATE_ENTRY_T * state)
{
    LED_OBJ_T *led = state->led;

    led->state = LED_ON;
    avalanche_gpio_set(state->map1,led->pos_map);
    led->module_map |= (1 << (state->module_id));

}

static void led_off (STATE_ENTRY_T * state)
{
    LED_OBJ_T *led = state->led;

    led->module_map &= ~(1 << (state->module_id));
    if (!led->module_map)
      {
          led->state = LED_OFF;
          avalanche_gpio_set(state->map1,led->pos_map);
      }

}

static void led_oneshot_on (STATE_ENTRY_T * state)
{
    LED_OBJ_T *led = state->led;

    if (state->timer_running)
        return;

    state->timer_running = 1;
    led->state = LED_ONESHOT_ON;
    avalanche_gpio_set(state->map1,led->pos_map);
    os_timer_add (state->os_timer, state->param1,(int) state);
}

static void led_oneshot_off (STATE_ENTRY_T * state)
{

    LED_OBJ_T *led = state->led;

    if (state->timer_running)
        return;

    state->timer_running = 1;
    led->state = LED_ONESHOT_OFF;
    avalanche_gpio_set(state->map1,led->pos_map);
    os_timer_add (state->os_timer, state->param1,(int) state);
}

static void led_flash (STATE_ENTRY_T * state)
{
    LED_OBJ_T *led = state->led;

    if (state->timer_running)
        return;

    state->timer_running = 1;
    led->state = LED_FLASH;
    avalanche_gpio_set(state->map1,led->pos_map);
    os_timer_add (state->os_timer, state->param1,(int) state);
}



static int led_get_mode(LED_CONFIG_T *led_cfg)
{
    int num_gpio = led_cfg->gpio_num;
    int i;
    int *led_mode = led_cfg->mode;
    int max = -1;

    /* Return Max of available modes */
    for(i = 0; i<num_gpio; i++)
    {
       max = (max > led_mode[i]) ? max : led_mode[i];
    }
  
    return max; 
}

static void led_assign_timer(STATE_ENTRY_T *state_entry)
{

     if (state_entry->os_timer)
     {
         os_timer_delete(state_entry->os_timer);
     }

     switch(state_entry->mode)
     {
         case LED_ONESHOT_ON:
              state_entry->os_timer = os_timer_init(led_oneshot_on_timer_func);
              break;

         case LED_ONESHOT_OFF:
              state_entry->os_timer = os_timer_init(led_oneshot_off_timer_func);
              break;

         case LED_FLASH:
              state_entry->os_timer = os_timer_init(led_flash_timer_func);
              break;

         default:
              log_msg("invalid mode in function led_assign timer\n");
     }

}

static int led_get_map(LED_CONFIG_T *led_cfg,int *p_pos_map,int *p_map1,int *p_map2)
{
    int i;
    int map1[2] = {0,0};
    int pos_map[2] = {0,0};
    int map2[2] = {0,0};
    int requires_timer = REQUIRES_TIMER(led_get_mode(led_cfg));    

    for (i = 0; i < led_cfg->gpio_num; i++)
    {
        int gpio_map;
        int index = led_cfg->gpio[i]/32;
        int pos = led_cfg->gpio[i] % 32;
        
        if (led_cfg->gpio[i] >= num_gpios)
        {
            log_msg ("Error: gpio number out of range\n");
            return -1;
        }

        gpio_map = 1 << pos;        

        pos_map[index] |= gpio_map;
        

        switch (led_cfg->mode[i])
        {
            case LED_OFF:
                if(gpio_offstate_map[index] & gpio_map)
                    map1[index] |= gpio_map;
                
                if (requires_timer && (gpio_offstate_map[index] & gpio_map))
                    map2[index] |= gpio_map;
            break;

            case LED_ON:

                if(!(gpio_offstate_map[index] & gpio_map))
                    map1[index] |= gpio_map;
                
                if (requires_timer && !(gpio_offstate_map[index] & gpio_map))
                    map2[index] |= gpio_map;
            break;

            case LED_ONESHOT_OFF:

                if ((gpio_offstate_map[index] & gpio_map))
                    map1[index] |= gpio_map;
                else
                    map2[index] |= gpio_map;
            break;

            case LED_ONESHOT_ON:
            case LED_FLASH:

                if (!(gpio_offstate_map[index] & gpio_map))
                    map1[index] |= gpio_map;
                else
                    map2[index] |= gpio_map;
            break;

            default:
                log_msg ("Error: Invalid mode\n");
                return -1;
        }
    }

    assign_map(p_pos_map,pos_map);
    assign_map(p_map1,map1);
    assign_map(p_map2,map2);
 
    return 0;
}




static int configure_state(STATE_ENTRY_T *state_entry,LED_CONFIG_T *led_cfg)
{
//    int state = led_cfg->state;
    int i;
    int map1[2] ;
    int pos_map[2];
    int map2[2];

    if((state_entry->mode = led_get_mode(led_cfg)) >=  NUM_LED_MODES)
    {
        log_msg ("Error: Invalid mode in func configure_state\n");
        return -1;
    }

    state_entry->handler = led_mode_handler[state_entry->mode];

 
    if(led_get_map(led_cfg,pos_map,map1,map2))
    {
        log_msg ("Error: gpio number out of range\n");
        return -1;
    }

    assign_map(state_entry->map1,map1);
    assign_map(state_entry->map2,map2);
    state_entry->led = get_led(pos_map);

    /* Check if the state requires timer */
    if(REQUIRES_TIMER(state_entry->mode))
    {
        state_entry->timer_running = 0;
        state_entry->param1 = led_cfg->param1;
        state_entry->param2 = led_cfg->param2;
        led_assign_timer(state_entry);
    }
   
    /* enable gpio pins */
    for(i = 0;i<led_cfg->gpio_num;i++)
    {
        int value;
        int index;
        int pos;
        avalanche_gpio_ctrl (led_cfg->gpio[i],GPIO_PIN,GPIO_OUTPUT_PIN);
        
        /* Turn off the led */
        index = led_cfg->gpio[i]/32;
        pos = led_cfg->gpio[i] % 32;
        value = (gpio_offstate_map[index] & (1 << pos))?1:0; 
        avalanche_gpio_out_bit(led_cfg->gpio[i],value);
    }
   
    return 0;
}


static void free_all_states(void)
{
    int module_id;

    for(module_id = 0; module_id < MAX_MODULE_ENTRIES; module_id++)
    {
        if(modules[module_id])
        {
            int i;
            for(i = 0; i< MAX_MODULE_INSTANCES; i++)
            {
                MODULE_INSTANCE_T *module_instance = modules[module_id]->module_instance[i];

                if(module_instance)
                {
                    int state_id;

                    for(state_id =0; state_id < MAX_STATE_ENTRIES; state_id++)
                    {
                        STATE_ENTRY_T *state= module_instance->states[state_id];

                        if(state)
                        {
                            if(state->os_timer)
                                os_timer_delete(state->os_timer);

                            os_free(state);
                            module_instance->states[state_id] = NULL;
                        }

                    }
                }

            }
            os_free(modules[module_id]->name);
            os_free(modules[module_id]);
            modules[module_id] = NULL;
        }
    }
}

/***********************************HAL APIS************************************/

/**************************************************************************
 * FUNCTION NAME : avalanche_led_hal_init
 **************************************************************************
 * DESCRIPTION   :
 *      The function initializes led hal module
 *
 * RETURNS               :
 *      0               on Success
 *      Negative value  on Error
 ***************************************************************************/


int avalanche_led_hal_init (int  *gpio_off_value, int num_gpio_pins)
{
    //int i;

    num_gpios = num_gpio_pins + 4;
    assign_map(gpio_offstate_map, gpio_off_value);

    MUTEX_INIT (led_lock);

    /* initialize led state function handlers */
    led_mode_handler[LED_ON] = led_on;
    led_mode_handler[LED_OFF] = led_off;
    led_mode_handler[LED_ONESHOT_ON] = led_oneshot_on;
    led_mode_handler[LED_ONESHOT_OFF] = led_oneshot_off;
    led_mode_handler[LED_FLASH] = led_flash;

    return 0;
}


/**************************************************************************
 * FUNCTION NAME : avalanche_led_config_set
 **************************************************************************
 * DESCRIPTION   :
 *      The function configures LED state object
 *
 * RETURNS               :
 *      0               on Success
 *      Negative value  on Error
 ***************************************************************************/

int avalanche_led_config_set(LED_CONFIG_T * led_cfg)
{
    MODULE_INSTANCE_T *module;
    MUTEX_GET (led_lock);
    module = get_module (led_cfg->name, led_cfg->instance);

    if (!module)
        goto config_failed;

    if (led_cfg->state < MAX_STATE_ENTRIES)
      {
          int state = led_cfg->state;

          if (!(module->states[state]))
            {
                module->states[state] = led_malloc (sizeof (STATE_ENTRY_T));
            }

          if (!(module->states[state]))
              goto config_failed;

          module->states[state]->module_id = module->module_id;	

          if(configure_state(module->states[state],led_cfg))
          {
              os_free(module->states[state]);
              module->states[state] = NULL;
              goto config_failed;
          }

      }
    else
      {
          log_msg ("ERROR:State Count exceeded\n");
          goto config_failed;
      }

    MUTEX_RELEASE (led_lock);
    return 0;

  config_failed:

    MUTEX_RELEASE (led_lock);
    return -1;

}

/**************************************************************************
 * FUNCTION NAME : avalanche_led_register
 **************************************************************************
 * DESCRIPTION   :
 *      The function creates handle to the given module and returns it.
 *
 * RETURNS               :
 *      Handle to the module instance on success.
 *      NULL  on failure.
 ***************************************************************************/

void *avalanche_led_register (const char *mod_name, int instance)
{
    void *p;
    MUTEX_GET (led_lock);
    p = (void *)get_module ((char *)mod_name, instance);
    MUTEX_RELEASE (led_lock);
    return p;
}


/**************************************************************************
 * FUNCTION NAME : avalanche_led_action
 **************************************************************************
 * DESCRIPTION   :
 *      The function triggers action on LED
 *
 ***************************************************************************/

void avalanche_led_action (void *module, int state_id)
{

    MUTEX_GET (led_lock);
    if (module && (state_id < MAX_STATE_ENTRIES))
      {

          STATE_ENTRY_T *state =
              ((MODULE_INSTANCE_T *) (module))->states[state_id];
          if (state)
            {
                state->handler (state);
            }
      }
    MUTEX_RELEASE (led_lock);
    return;
}


/**************************************************************************
 * FUNCTION NAME : avalanche_led_unregister
 **************************************************************************
 * DESCRIPTION   :
 *      The function unregisters the module
 *
 * RETURNS               :
 *      0               on Success
 *      Negative value  on Error
 ***************************************************************************/

int avalanche_led_unregister (void *mod_inst)
{

    return 0;
}


/**************************************************************************
 * FUNCTION NAME : led_free_all
 **************************************************************************
 * DESCRIPTION   :
 *      The function frees the memory allocated for holding state
 *       configuration data
 *
 ***************************************************************************/

void avalanche_led_free_all()
{
    free_all_states();
}

/**************************************************************************
 * FUNCTION NAME : avalanche_led_hal_exit
 **************************************************************************
 * DESCRIPTION   :
 *      The function releases all the allocated memory
 *
 ***************************************************************************/

void avalanche_led_hal_exit ()
{
    free_all_states();
}

/*****************************************************************************
 * FUNCTION NAME : avalanche_led_config_get
 *****************************************************************************
 * DESCRIPTION   :
 *      The function returns configuration information corresponding to module
 *      state.
 *
 * RETURNS               :
 *      0               on Success
 *      Negative value  on Error
 ***************************************************************************/
int avalanche_led_config_get(LED_CONFIG_T *led_cfg,int module_id,int instance, int state_id)
{
    if(module_id == -1)
    {
        /* The module info is passed through  field of led_cfg */
        MODULE_INSTANCE_T *mod = get_module (led_cfg->name, instance);
        if(mod)
            module_id = mod->module_id;

    }
    if(module_id >= MAX_MODULE_ENTRIES ||  module_id < 0)
        return -1;

    if(state_id >= MAX_STATE_ENTRIES ||  module_id < 0)
        return -1;

    if(instance >= MAX_MODULE_INSTANCES ||  module_id < 0)
        return -1;

    if(modules[module_id])
    {
         MODULE_INSTANCE_T *module = modules[module_id]->module_instance[instance];
         if(module)
         {
             STATE_ENTRY_T *state = module->states[state_id];
             if(state)
             {
                 int i;
                 LED_OBJ_T *led;
                 strcpy(led_cfg->name, modules[module_id]->name);
                 led_cfg->state = state_id;
                 led_cfg->instance = instance;
                 led_cfg->param1 = state->param1;
                 led_cfg->param2 = state->param2;
                 led_cfg->mode[0] = state->mode;
                 led = state->led;
                 
                 /* needs to be modified for multi-pin leds */
                 for(i = 0;i < num_gpios && !(led->pos_map[i/32] & (1 << i)); i++);

                 led_cfg->gpio[0] = i;
                 led_cfg->gpio_num = 1;

                 return 0;
             }
         }
    }

    return -1;
}
