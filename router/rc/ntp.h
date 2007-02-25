
/*
 *********************************************************
 *   Copyright 2003, CyberTAN  Inc.  All Rights Reserved *
 *********************************************************

 This is UNPUBLISHED PROPRIETARY SOURCE CODE of CyberTAN Inc.
 the contents of this file may not be disclosed to third parties,
 copied or duplicated in any form without the prior written
 permission of CyberTAN Inc.

 This software should be used as a reference only, and it not
 intended for production use!


 THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
 KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.  CYBERTAN
 SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE
*/

struct timeZoneTable_ {
           unsigned char name[8];
           int           gmtOffset;
           char          dstFlag;
};

typedef struct timeZoneTable_ timeZoneTable;
 
struct dstTable_ {
           unsigned char  startDay[21];
           unsigned char  endDay[21];
           unsigned char  startMonth;
           unsigned char  endMonth;
           unsigned char  diffMonth;
           char           resv;
           int            dstBias;
};
typedef struct dstTable_ dstTable;
const timeZoneTable tzEntry[] = {
	{"-12",		-3600*12,	0},
	{"-11",		-3600*11,	0},
	{"-10", 	-3600*10,	0},   
	{"-09",          -3600*9,	1},
	{"-08",          -3600*8,	1},   
	{"-07 1",        -3600*7,	0},
	{"-07 2",        -3600*7,	1},
	{"-06 1",        -3600*6,	0},
	{"-06 2",        -3600*6,	1},
	{"-05 1",	-3600*5,	0},
	{"-05 2",        -3600*5,	1},
	{"-04 1",        -3600*4,	0},
	{"-04 2",	-3600*4,	1},
	{"-03.5",	-3600*3.5,	1},
	{"-03 1",        -3600*3,	0},
	{"-03 2",        -3600*3,	1},
	{"-02",          -3600*2,	0},
	{"-01",          -3600,  	2},
	{"+00 1",          0 ,     	0},
	{"+00 2",          0 ,     	2},
	{"+01 1",        3600,   	0},
	{"+01 2",        3600,   	2},
	{"+02 1",        3600*2, 	0},
	{"+02 2",     	3600*2, 	2},
	{"+03",          3600*3, 	0},
	{"+04",          3600*4, 	0},
	{"+05",          3600*5, 	0},
	{"+06",          3600*6, 	0},
	{"+07",          3600*7,		0},
	{"+08 1",  	3600*8,		0},
	{"+08 2",        3600*8,		0},
	{"+09",          3600*9,		0},
	{"+09.5",        3600*9.5,		0},
	{"+10 1",       3600*10,	0},
	{"+10 2",       3600*10,	4},
	{"+11",         3600*11,	0},
	{"+12 1",       3600*12,	0},
	{"+12 2",       3600*12,	4},
};
const  dstTable dstEntry[] = {
	{{ },                                   { },                                    0, 0, 0, 0, 0},
		//4 first sun - 10 last sun
	{{7,6,4,3,2,1,6,5,4,3,1,7,6,5,3,2,1,7,5,4,3}, {27,26,31,30,29,28,26,25,31,30,28,27,26,25,30,29,28,27,25,31,30}, 4, 10, 0, 0, 3600},
		//3 last sun - 10 last sun
	{{31,30,28,27,26,25,30,29,28,27,25,31,30,29,27,26,25,31,29,28,27}, {27,26,31,30,29,28,26,25,31,30,28,27,26,25,30,29,28,27,25,31,30}, 3, 10, 0, 0, 3600},
	{{31,30,28,27,26,25,30,29,28,27,25,31,30,29,27,26,25,31,29,28,27}, {27,26,31,30,29,28,26,25,31,30,28,27,26,25,30,29,28,27,25,31,30}, 3, 10, 0, 0, 3600},
		//10 last sun - 3 last sun
	{{27,26,31,30,29,28,26,25,31,30,28,27,26,25,30,29,28,27,25,31,30}, {31,30,28,27,26,25,30,29,28,27,25,31,30,29,27,26,25,31,29,28,27}, 10, 3, 1, 0, 3600},
	};

const int tzEntrySize = sizeof(tzEntry) / sizeof(timeZoneTable) ;
