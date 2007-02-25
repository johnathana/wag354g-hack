/* Copyright 2002, Telogy Networks */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <posix_time.h>
#define TIME_SAMPLES 100
#define TMR_INTERVAL (1000*1000) /* In nano-seconds */

struct timespec sample_time[TIME_SAMPLES];
unsigned int tmr_cnt = 0;
 extern FILE *stdout;

void time_handler(int signo, siginfo_t *info, void *context)
{

	clock_gettime(CLOCK_REALTIME, &sample_time[tmr_cnt]);
	tmr_cnt++;
}
void print_stats()
{
	unsigned int i;
	unsigned long min = 99999999;
	unsigned min_cnt = 0;
	unsigned long max = 0;
	unsigned max_cnt = 0;
	unsigned long avg  = 0;
	unsigned long delta_sum = 0;
	long delta;
	unsigned int undershots = 0;
	unsigned int overshots = 0;

	printf("Results for %u nsec timers\n",TMR_INTERVAL);
	for(i = 1; i < tmr_cnt; i++)
	{
		delta = (sample_time[i].tv_sec - sample_time[i-1].tv_sec)*1000000000 + (sample_time[i].tv_nsec - sample_time[i-1].tv_nsec);
		delta -= TMR_INTERVAL;

		if(delta > 0)
		{
			overshots++;
		}
		else
		{
			if(delta < 0)
			{
				undershots++;
			}
		}
		/*printf("%i %ld \n",i,delta);*/
		delta = labs(delta);
		delta_sum += delta;
		if(delta > max)
		{
			max = delta;
			max_cnt = i;
		}
		if(delta < min)
		{
			min = delta;
			min_cnt = i;
		}
	}
	avg = delta_sum/(TIME_SAMPLES-1);
	printf("Jitter count: %u samples\n",i);
	printf("Min/Max/Avg: %lu(%u)/%lu(%u)/%lu\n",min,min_cnt,max,max_cnt,avg);
	printf("Min/Max/Avg: %f%%)/%f%%/%f%%\n",((float)min)/TMR_INTERVAL,((float)max)/TMR_INTERVAL,((float)avg)/TMR_INTERVAL);
	printf("Undershots: %u, Overshots: %u\n",undershots, overshots);
}
void main(void)
{
	struct itimerspec tmspec;
	struct sigaction sa;
	timer_t tmr_id;
	sigset_t set;
	sigevent_t signal1;

	tmspec.it_value.tv_sec = 1;
	tmspec.it_value.tv_nsec = 0;
	tmspec.it_interval.tv_sec = 0;
	tmspec.it_interval.tv_nsec = TMR_INTERVAL;
#if 1
	signal1.sigev_notify = SIGEV_SIGNAL;
	signal1.sigev_signo = SIGRTMIN;
	signal1.sigev_value.sival_ptr = &tmspec;

	sa.sa_sigaction = time_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	printf("Jitter tester - will take %u samples @ %u ns\n",
			TIME_SAMPLES,TMR_INTERVAL);

	if (sigaction(SIGALRM, &sa, NULL) )
	{
		perror("sigaction for SIGALRM failed");
		exit(1);
	}		
	if (sigaction(SIGRTMIN, &sa, NULL) )
	{
		perror("sigaction for SIGALRM failed");
		exit(1);
	}		
	if (timer_create( CLOCK_REALTIME_HR, &signal1, &tmr_id) )
	{
		perror("Failed to create timer");
	}

	sigemptyset(&set);
	sigaddset(&set, SIGRTMIN);
	sigprocmask(SIG_BLOCK, &set, NULL);

	printf("Timer settime returned :%u \n",timer_settime(tmr_id, 0, &tmspec, NULL));
	sigprocmask(SIG_UNBLOCK, &set, NULL);
	while(tmr_cnt < TIME_SAMPLES)
	{
		pause();
	}
	timer_delete(tmr_id);
	puts("\nTest complete");
	print_stats();
#endif	
#if 0
	printf("Testing nanosleep\n");
	tmspec.it_value.tv_sec = 0;
	tmspec.it_value.tv_nsec = TMR_INTERVAL;
	for(tmr_cnt = 0; tmr_cnt < TIME_SAMPLES; tmr_cnt++)
	{
		clock_gettime(CLOCK_REALTIME, &sample_time[tmr_cnt]);
		if(nanosleep(&tmspec.it_value, NULL) != 0)
		{
			printf("Nanosleep failed\n");
			exit(2);
		}	
	}
	print_stats();
#endif
	printf("Testing clock_nanosleep\n");
	tmspec.it_value.tv_sec = 0;
	tmspec.it_value.tv_nsec = TMR_INTERVAL;
	for(tmr_cnt = 0; tmr_cnt < TIME_SAMPLES; tmr_cnt++)
	{
		clock_gettime(CLOCK_REALTIME, &sample_time[tmr_cnt]);
		if(clock_nanosleep(CLOCK_MONOTONIC_HR,0, &tmspec.it_value, NULL) != 0)
		{
			printf("Nanosleep failed\n");
			exit(2);
		}	
	}
	print_stats();
}
