#include "asm/avalanche/apex/apex_clk_cntl.h"


/****************************************************************************
 * FUNCTION: avalanche_clkc_init
 ****************************************************************************
 * Description: The routine initializes the internal variables depending on
 *              on the sources selected for different clocks.
 ***************************************************************************/
void avalanche_clkc_init(unsigned int afeclk, unsigned int refclk,
                         unsigned int xtal3in)
{
     UINT32 choice;                                                  
     return 0;

}



/****************************************************************************
 * FUNCTION: avalanche_clkc_set_freq
 ****************************************************************************
 * Description: The above routine is called to set the output_frequency of the
 *              selected clock(using clk_id) to the  required value given
 *              by the variable output_freq.
 * 
 * Returns     0           if the required frequency is set exactly
 *             1           if the frequency set is not exact but an approximate value
 *                         of the requested frequency
 *
 *             -1          In case of an error.
 *
 ***************************************************************************/
int avalanche_clkc_set_freq
(
    AVALANCHE_CLKC_ID_T clk_id,
    unsigned int        output_freq
)
{
    return -1;
}


/****************************************************************************
 * FUNCTION: avalanche_clkc_get_freq
 ****************************************************************************
 * Description: The above routine is called to get the output_frequency of the
 *              selected clock( clk_id)
 * 
 * Returns    Frequency of the requested clock in case of valid argument
 *            A value of 0 is returned otherwise.
 ***************************************************************************/
unsigned int avalanche_clkc_get_freq
(
    AVALANCHE_CLKC_ID_T clk_id
)
{

    int ret = 0;
    char *cpu_freq_ptr;   
    int cpufreq;                                                 
                                                                       
    cpu_freq_ptr = prom_getenv("cpufrequency");                            
   
    if(cpu_freq_ptr) 
        cpufreq = CONFIG_CPU_FREQUENCY_AVALANCHE;  
    
    else   
        cpufreq = simple_strtol(cpu_freq_ptr,NULL,0);   

    switch(clk_id)
    {
        case CLKC_MIPS:
            ret = cpufreq;
        break;
     
        case CLKC_VBUS:
            ret = cpufreq/2;
        break;


        case CLKC_SYS:
            ret = cpufreq;
        break;

        default:
            ret = 0;  
    }

    return ret;

}
    

EXPORT_SYMBOL(avalanche_clkc_init);
EXPORT_SYMBOL(avalanche_clkc_set_freq);
EXPORT_SYMBOL(avalanche_clkc_get_freq);

