#include <linux/kernel.h>
#include "../ipsec_alg.h"
extern int ipsec_1des_init(void);
extern int ipsec_null_init(void);
void ipsec_alg_static_init(void){ 
	int __attribute__ ((unused)) err=0;
	if ((err=ipsec_1des_init()) < 0)
		printk(KERN_WARNING "ipsec_1des_init() returned %d", err);
	if ((err=ipsec_null_init()) < 0)
		printk(KERN_WARNING "ipsec_null_init() returned %d", err);
}
