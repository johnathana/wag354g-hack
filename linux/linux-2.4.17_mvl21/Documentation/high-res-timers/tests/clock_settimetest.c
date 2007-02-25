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
	try(0,clock_gettime(CLOCK_REALTIME, &ts));
		
	printf("clock_gettime(CLOCK_REALTIME) tv_sec == %ld, tv_nsec == %ld\n",
               ts.tv_sec, ts.tv_nsec);		

	try(0,clock_settime(CLOCK_REALTIME, &ts));

	try(0,clock_gettime(CLOCK_REALTIME, &ts));
	printf("clock_gettime(CLOCK_REALTIME) tv_sec == %ld, tv_nsec == %ld\n",
               ts.tv_sec, ts.tv_nsec);		

	try(1,clock_settime(33, &ts));

	try(1,clock_settime(CLOCK_REALTIME, NULL));

	try(1,clock_settime(CLOCK_REALTIME, (struct timespec*)1));

	try(0,clock_gettime(CLOCK_MONOTONIC, &ts));
		
	printf("clock_gettime(CLOCK_MONOTONIC) tv_sec == %ld, tv_nsec == %ld\n",
               ts.tv_sec, ts.tv_nsec);		
	try(1,clock_settime(CLOCK_MONOTONIC, &ts));

	try(1,clock_settime(33, &ts));

	try(1,clock_settime(CLOCK_MONOTONIC, NULL));

	try(1,clock_settime(CLOCK_MONOTONIC, (struct timespec*)1));

	by_now();
}




