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

/* clock_getrestest.c
 *
 * This program simply calls clock_getres().
 *
 * Author: Shyam Pather 
 */

#include <stdio.h>
#include <time.h>
#ifdef __linux__
#include <posix_time.h>
#endif
#include "utils.h"

int main() {
	struct timespec ts;
	
	Try(clock_getres(CLOCK_REALTIME, &ts));
	
	printf("(Resolution of CLOCK_REALTIME) tv_sec = %ld, tv_nsec = %ld\n", 
	       ts.tv_sec, ts.tv_nsec);		

	
	try(1,clock_getres(33, &ts));

	try(0,clock_getres(CLOCK_REALTIME, NULL));

	try(1,clock_getres(CLOCK_REALTIME, (struct timespec*)1));

	Try(clock_getres(CLOCK_MONOTONIC, &ts));
	
	printf("(Resolution of CLOCK_MONOTONIC) tv_sec = %ld, tv_nsec = %ld\n", 
	       ts.tv_sec, ts.tv_nsec);		

	
	Try(clock_getres(CLOCK_REALTIME_HR, &ts));
	
	printf("(Resolution of CLOCK_REALTIME_HR) tv_sec = %ld, tv_nsec = %ld\n", 
	       ts.tv_sec, ts.tv_nsec);		


	try(1,clock_getres(33, &ts));

	try(0,clock_getres(CLOCK_REALTIME_HR, NULL));

	try(1,clock_getres(CLOCK_REALTIME_HR, (struct timespec*)1));

	Try(clock_getres(CLOCK_MONOTONIC_HR, &ts));

	printf("(Resolution of CLOCK_MONOTONIC_HR) tv_sec = %ld, tv_nsec = %ld\n", 
	       ts.tv_sec, ts.tv_nsec);		

	return 0;
}
