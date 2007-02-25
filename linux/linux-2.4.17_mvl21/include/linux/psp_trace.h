#ifndef _TRACER_H
#define _TRACER_H

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

//#define INCLUDE_TRACE
//#define INCLUDE_MARK
#define TRACE_GET_TIMESTAMP(tmp) __asm__ __volatile__ ( "mfc0 %0,$9"  : "=r" (tmp))



#define TRACER_GENERAL_GROUP    0
#define TRACER_ATM_DRIVER_GROUP 1
#define TRACER_ETH_DRIVER_GROUP 2
#define TRACER_USB_DRIVER_GROUP 3
#define TRACER_BRIDGE_GROUP     4
#define TRACER_IP_STACK_GROUP   5

#ifdef INCLUDE_MARK
	#define IP_HEADER_SIZE 48
	#define	ENTRY_POINT_OFFSET	IP_HEADER_SIZE
	#define EXIT_POINT_OFFSET ENTRY_POINT_OFFSET + sizeof(int)


	#define MARK_POINT(addr) 	  \
	{								  \
		int tmp;					  \
		TRACE_GET_TIMESTAMP(tmp);  \
		*((int *) (addr)) = tmp; \
	}								  \

	#define MARK_ENTRY_POINT(buf) MARK_POINT(((unsigned char*)buf) + ENTRY_POINT_OFFSET)
	#define MARK_EXIT_POINT(buf) MARK_POINT(((unsigned char*)buf) + EXIT_POINT_OFFSET)
#else
	#define MARK_ENTRY_POINT(buf) 
	#define MARK_EXIT_POINT(buf) 
#endif

#ifdef CONFIG_PSP_TRACE
void trace_event(int event_id,int par);
int trace_init(void);
void trace_exit(void);
void trace_print_2_scr(void);
	#define bm_trace(x) trace_event(x,0)
	#define bm_trace_par(a,b) trace_event(a,b)
	#define bm_shutdown trace_exit
	#define bm_disable trace_disable
	#define bm_print trace_print_2_scr
#else
	#define bm_trace(a)
	#define bm_trace_par(a,b)
	#define bm_shutdown()
	#define bm_print()
	#define bm_disable() 
#endif

#endif /* _TRACER_H */