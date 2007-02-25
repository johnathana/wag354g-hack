#define LIB_ENTRY 0x90000480    /*Was 0x90 for cache!*/

#define quick_link(func,libnum,callnum)\
LEAF(func)			;\
	li	t0, LIB_ENTRY	;\
	li	t2, libnum	;\
	li	t3, callnum	;\
	j	t0		;\
	nop			;\
END(func)			;
