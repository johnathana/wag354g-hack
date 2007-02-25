#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <assert.h>
/* 
 * Define some nice color stuff
 */
#define  MOVE_TO_COL(col) "\033["#col"G"
#define      SETCOLOR_SUCCESS "\033[1;32m"
#define      SETCOLOR_FAILURE "\033[1;31m"
#define      SETCOLOR_WARNING "\033[1;33m"
#define      SETCOLOR_NORMAL "\033[0;39m"

#undef __assert_fail
# define __assert_fail(exp,file,line,fun)	\
({fprintf(stderr,SETCOLOR_FAILURE ": %s:%d: %s:assert(%s) failed\n"SETCOLOR_NORMAL,file,line,fun,exp);\
global_error++;})
//#define NSEC_PER_SEC 1000000000
#define timerplusnsec(c,d) (c)->tv_nsec +=(d);             \
                           if ((c)->tv_nsec >NSEC_PER_SEC){ \
                           (c)->tv_nsec -= NSEC_PER_SEC;(c)->tv_sec++;}

#define timerdiff(a,b) (((a)->tv_sec - (b)->tv_sec)*NSEC_PER_SEC + \
                         (a)->tv_nsec - (b)->tv_nsec)

#define timersum(c,a,b) (c)->tv_sec = ((a)->tv_sec + (b)->tv_sec); \
                       (c)->tv_nsec = ((a)->tv_nsec + (b)->tv_nsec); \
                       if ((c)->tv_nsec > NSEC_PER_SEC){ \
                           (c)->tv_nsec -= NSEC_PER_SEC;(c)->tv_sec++;}
#define timer_gt(a,b) (((a)->tv_sec > (b)->tv_sec) ? 1 :  \
                      (((a)->tv_sec < (b)->tv_sec) ? 0 :  \
                      ((a)->tv_nsec > (b)->tv_nsec)))
#define timeval_to_timespec(a,b) (b)->tv_sec = (a)->tv_sec; \
                                 (b)->tv_nsec = (a)->tv_usec * 1000
#define timespec_to_timeval(a,b) (b)->tv_sec = (a)->tv_sec; \
                                 (b)->tv_usec = (a)->tv_nsec / 1000
#define timevaldiff(a,b) (((a)->tv_sec - (b)->tv_sec)*1000000 + \
                         (a)->tv_usec - (b)->tv_usec)

int global_error;
#define by_now() { char * excolor = (global_error) ? SETCOLOR_FAILURE : \
                         SETCOLOR_WARNING;                     \
                         fprintf(stderr, \
                         "%sEnd of test, %d error(s) found.\n"  \
                         SETCOLOR_NORMAL,                      \
                         excolor,global_error);                       \
                         exit(global_error);}
#ifdef debug
#undef debug
#define debug(a) do {a}while (0)
#else
#define debug(a) do {} while (0)
#endif
#define MASK(x) (1<<(x))
#define myperror(s) MYperror(__BASE_FILE__,__LINE__,s)
int MYperror(char * where,int line,char *what)
{
        fprintf(stderr,SETCOLOR_FAILURE"%s,%d:",where,line);
        perror(what);
        fprintf(stderr,SETCOLOR_NORMAL);
	fflush(stderr);
	fflush(stdout);
        exit(++global_error);
}
#define my_c_perror(e,s) MY_c_perror(e,__BASE_FILE__,__LINE__,s)
int MY_c_perror(int e,char * where,int line,char *what)
{
        char *fmt;
        fmt = e ? "expected" :SETCOLOR_FAILURE"UNEXPECTED";
        fprintf(stderr,"%s %s,%d:",fmt,where,line);
        perror(what);
        if ( ! e) {
                global_error++;
                fprintf(stderr,SETCOLOR_NORMAL);
        }
	fflush(stderr);
	fflush(stdout);
        return e ? 0:-1;
}
#define no_error(e,s) No_error(e,__BASE_FILE__,__LINE__,s)
int No_error(int e,char * where,int line,char *what)
{
        char *fmt;
        fmt = e ? SETCOLOR_FAILURE"ERROR expected but not found" : "Cool";
        fprintf(stderr,"%s %s,%d:%s\n"SETCOLOR_NORMAL,fmt,where,line,what);
        if ( e) {
                global_error++;
                fprintf(stderr,SETCOLOR_NORMAL);
        }
	fflush(stderr);
	fflush(stdout);
        return e ? -1 : 0;
}
/*
 * Try() is a macro for making system calls where an error return is -1.
 * Used as a function, it returns any value other than -1.  Mostly we
 * use it as a straight call, but one never knows.  Also can be called
 * with some expression which will exit the program with an error message
 * if it ever gets to -1.  For example:
 * count = 100;
 * ---loop stuff ---
 * Try (--count);
 * ---end of loop ---
 * Will stop the loop with an error after 101 times around.  Great for 
 * stopping run away while loops.
 * Since it exits via myperror, above, it will report where the exit
 * came from (source and line number).  Error text will be the expression.
 */
#define Try(f) ((f) != -1 ? : myperror( #f ))
#define try(e,f) ((f) != -1 ? no_error(e,#f): my_c_perror( e,#f ))
/* These two printf() calls take a string with "%d.%03d" somewhere in it
 * and an integer.  They print the interger as a decimal with 3 digits
 * to the right of the decimal point.  Use to print microsecond values
 * as milliseconds.  The second version is used to print two such values.
 */
#define printf1(s, d) printfx(s,d,1000)
void printfx(char *s,int d, int x){printf(s,d/x,d%x);}
void printf2(char *s,int d, int e){ printf(s,d/1000,d%1000,e/1000,e%1000);}
/* Bit map routines.  These routines work on a bit map of N words.  We assume
 * the bit width of a word is 32.
 * Here we only need test, set and clear.  There are others to be found in
 * the bitops.h headers.
 */
#ifdef ASM_BITOPS
#include <asm/bitops.h>
#define BIT_SHIFT 5
#define WORD_MASK (int)(-1<<BIT_SHIFT)
#define BIT_MASK  (~WORD_MASK)
#define clearbit(bit, add) clear_bit((bit)&BIT_MASK,(add)+((bit)>>BIT_SHIFT))
#define setbit(bit, add) set_bit((bit)&BIT_MASK,(add)+((bit)>>BIT_SHIFT))
#define testbit(bit, add) __test_bit((bit)&BIT_MASK,(add)+((bit)>>BIT_SHIFT))
#else
#define BIT_SHIFT 5
#define WORD_MASK (int)(-1<<BIT_SHIFT)
#define BIT_MASK  (~WORD_MASK)
#define clearbit(bit, add) *((add)+((bit)>>BIT_SHIFT)) &= ~(1<<((bit)&BIT_MASK))
#define setbit(bit, add)   *((add)+((bit)>>BIT_SHIFT)) |= 1<<((bit)&BIT_MASK)
#define testbit(bit, add) (*((add)+((bit)>>BIT_SHIFT)) &  1<<((bit)&BIT_MASK))
#endif
#define NO_BITS 10000
#define NO_WDS ((NO_BITS/32)+1)

