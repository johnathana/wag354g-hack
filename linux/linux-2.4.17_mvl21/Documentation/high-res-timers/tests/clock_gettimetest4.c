#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#ifdef __linux__
#include <posix_time.h>
#endif

#define MAX_SAMPLES 25

int timespec_diff(struct timespec *start, struct timespec *end)
{
  return (end->tv_sec - start->tv_sec) * 1000000000 +
    (end->tv_nsec - start->tv_nsec);
}


int main(void)
{
  struct timespec start_rt, end_rt;
  struct timespec start_tsc, end_tsc;
  int i;
  int delta_rt, delta_tsc;

  printf("Measured times to get times (in nano secs):\n");
  for (i = 0; i < MAX_SAMPLES; i++) {
    clock_gettime(CLOCK_REALTIME, &start_rt);
    clock_gettime(CLOCK_REALTIME, &end_rt);
    clock_gettime(CLOCK_MONOTONIC, &start_tsc);
    clock_gettime(CLOCK_MONOTONIC, &end_tsc);
    delta_rt = timespec_diff(&start_rt, &end_rt);
    delta_tsc = timespec_diff(&start_tsc, &end_tsc);

    printf("CLOCK_REALTIME = %9d\tCLOCK_MONOTONIC = %9d\n",
           delta_rt, delta_tsc);
  }
  return 0;
}
