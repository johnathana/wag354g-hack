#include <linux/config.h>

void ticfg_lock_region(void);
void ticfg_unlock_region(void);

int enter_critical_section(void) {
#if defined (CONFIG_MIPS_AVALANCHE_TICFG)
    ticfg_lock_region();
#endif
    return 0;    
}

int exit_critical_section(void) {
#if defined (CONFIG_MIPS_AVALANCHE_TICFG)
    ticfg_unlock_region();        
#endif
    return 0;
}
