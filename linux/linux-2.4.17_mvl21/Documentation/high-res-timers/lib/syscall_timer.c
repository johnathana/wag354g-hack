//#include <signal.h>
//#include <time.h>
#include <linux/unistd.h>
#include "posix_time.h"
//#include "syscall_timer.h"

#define __NR___timer_create     __NR_timer_create
#define __NR___timer_gettime    __NR_timer_gettime
#define __NR___timer_settime    __NR_timer_settime
#define __NR___timer_getoverrun __NR_timer_getoverrun
#define __NR___timer_delete     __NR_timer_delete

#define __NR___clock_settime   __NR_clock_settime
#define __NR___clock_gettime   __NR_clock_gettime
#define __NR___clock_getres    __NR_clock_getres
#define __NR___clock_nanosleep __NR_clock_nanosleep
#define __NR___dump_cnsl __NR_dump_cnsl

/* This will expand into the timer_create system call stub. */
_syscall3(int, timer_create, clockid_t, which_clock, sigevent_t *,
	  timer_event_spec, timer_t *, created_timer_id)

/* This will expand into the timer_gettime system call stub. */
_syscall2(int, timer_gettime, 
	  timer_t, timer_id, 
	  struct itimerspec *, setting)

/* This will expand into the timer_settime system call stub. */
_syscall4(int, timer_settime, 
	  timer_t, timer_id, 
	  int, flags, 
	  const struct itimerspec *, new_setting,
	  struct itimerspec *, old_setting)

/* This will expand into the timer_gettime system call stub. */
_syscall1(int, timer_getoverrun, 
	  timer_t, timer_id)

/* This will expand into the timer_delete system call stub. */
_syscall1(int, timer_delete, 
	  timer_t, timer_id)

/* This will expand into the clock_getres system call stub. */
_syscall2(int, clock_getres, 
	  clockid_t, which_clock,
          struct timespec *,resolution)

/* This will expand into the clock_gettime system call stub. */
_syscall2(int, clock_gettime, 
	  clockid_t, which_clock,
          struct timespec *,time)

/* This will expand into the clock_settime system call stub. */
_syscall2(int, clock_settime, 
	  clockid_t, which_clock,
          const struct timespec *,time)

/* This will expand into the clock_nanosleep system call stub. */
_syscall4(int, clock_nanosleep, 
	  clockid_t, which_clock,
          int, flags,
          const struct timespec *,rqtp,
          struct timespec *,rmtp)
