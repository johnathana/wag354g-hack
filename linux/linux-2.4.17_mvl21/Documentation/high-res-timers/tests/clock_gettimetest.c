/*
 * Copyright (C) 1997 by the University of Kansas Center for Research,
 * Inc.  This software was developed by the Information and
 * Telecommunication Technology Center (ITTC) at the University of
 * Kansas.  Partial funding for this project was provided by Sprint. This
 * software may be used and distributed according to the terms of the GNU
 * Public License, incorporated herein by reference.  Neither ITTC nor
 * Sprint accept any liability whatsoever for this product.
 *
 * This project was developed under the direction of Dr. Douglas Niehaus.
 *
 * Authors: Shyam Pather, Balaji Srinivasan 
 *
 * Please send bug-reports/suggestions/comments to posix@ittc.ukans.edu
 *
 * Further details about this project can be obtained at
 *    http://hegel.ittc.ukans.edu/projects/posix/
 */

/* clock_gettimetest.c
 *
 * This program simply calls clock_gettime() in a loop. Time values 
 * displayed should be monotonically increasing.
 */

#include <stdio.h>
#include <time.h>
#ifdef __linux__
#include <posix_time.h>
#endif
#include "utils.h"

int main() {
	struct timespec ts;
	int retval;

	retval = clock_gettime(CLOCK_REALTIME, &ts);
	if (retval) {
		myperror("clock_gettime(CLOCK_REALTIME) failed");
		exit(1);
	}
		
	printf("clock_gettime(CLOCK_REALTIME) tv_sec == %ld, tv_nsec == %ld\n",
               ts.tv_sec, ts.tv_nsec);		

	retval = clock_gettime(33, &ts);
	if (retval) {
		my_c_perror(1,"clock_gettime(bogus clock) failed as expected");
	}

#if 0
	retval = clock_gettime(CLOCK_REALTIME, NULL);
	if (retval) {
		myperror("clock_gettime(CLOCK_REALTIME) failed");
	}

	retval = clock_gettime(CLOCK_REALTIME, (struct timespec*)1);
	if (retval) {
		myperror("clock_gettime(CLOCK_REALTIME) failed");
	}
#endif
	retval = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (retval) {
		myperror("clock_gettime(CLOCK_MONOTONIC) failed");
	}

	printf("clock_gettime(CLOCK_MONOTONIC) tv_sec == %ld, tv_nsec == %ld\n",
               ts.tv_sec, ts.tv_nsec);		
	by_now();
}




