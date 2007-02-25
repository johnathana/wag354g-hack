#define DEBUG 
#include "smtp_ntp.h"

const char month_day_table[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
const char month_day_table_addition[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
#if 0
const struct NAME_ DAY_NAME[] = {"N/A", "Sun.","Mon.","Tue.","Wed.","Thu.","Fri.","Sat."};
const struct NAME_ MONTH_NAME[] = {"Jan.","Feb.","Mar.","Apr.","May","Jun.","Jul.","Aug.","Sep.","Oct.","Nov.","Dec."};
#else
const char DAY_NAME[8][5] = {"N/A", "Sun.","Mon.","Tue.","Wed.","Thu.","Fri.","Sat."};
const char MONTH_NAME[12][5] = {"Jan.","Feb.","Mar.","Apr.","May","Jun.","Jul.","Aug.","Sep.","Oct.","Nov.","Dec."};
#endif
const timeZoneTable tzEntry[] = {
	{"(GMT-12:00) Kwajalein",							 -3600*12,	0},
	{"(GMT-11:00) Midway Island, Samoa",                 -3600*11,	0},
	{"(GMT-10:00) Hawaii",                               -3600*10,	0},   
	{"(GMT-09:00) Alaska",                               -3600*9,	1},
	{"(GMT-08:00) Pacific Time(USA & Canada)",           -3600*8,	1},   
	{"(GMT-07:00) Arizona",                              -3600*7,	0},
	{"(GMT-07:00) Mountain Time(USA & Canada)",          -3600*7,	1},
	{"(GMT-06:00) Mexico",                               -3600*6,	0},
	{"(GMT-06:00) Central Time(USA & Canada)",           -3600*6,	1},
	{"(GMT-05:00) Indiana East, Colombia, Panama",       -3600*5,	0},
	{"(GMT-05:00) Eastern Time(USA & Canada)",           -3600*5,	1},
	{"(GMT-04:00) Bolivia, Venezuela",                   -3600*4,	0},
	{"(GMT-04:00) Atlantic Time(Canada), Brazil West",   -3600*4,	1},
	{"(GMT-03:00) Guyana",                               -3600*3,	0},
	{"(GMT-03:00) Brazil East, Greenland",               -3600*3,	1},
	{"(GMT-02:00) Mid-Atlantic",                         -3600*2,	0},
	{"(GMT-01:00) Azores",                               -3600,  	2},
	{"(GMT) Gambia, Liberia, Morocco",                   0 ,     	0},
	{"(GMT) England",                                    0 ,     	2},
	{"(GMT+01:00) Tunisia",                              3600,   	0},
	{"(GMT+01:00) France, Germany, Italy",               3600,   	2},
	{"(GMT+02:00) South Africa",                         3600*2, 	0},
	{"(GMT+02:00) Greece, Ukraine, Romania, Turkey",     3600*2, 	2},
	{"(GMT+03:00) Iraq, Jordan, Kuwait",                 3600*3, 	0},
	{"(GMT+04:00) Armenia",                              3600*4, 	0},
	{"(GMT+05:00) Pakistan, Russia",                     3600*5, 	0},
	{"(GMT+06:00) Bangladesh, Russia",                   3600*6, 	0},
	{"(GMT+07:00) Thailand, Russia",                     3600*7,    0},
	{"(GMT+08:00) China, Hong Kong, Australia Western",  3600*8,    0},
	{"(GMT+08:00) Singapore, Taiwan, Russia",            3600*8,    0},
	{"(GMT+08:00) Perth",            3600*8,    4},
	{"(GMT+09:00) Japan, Korea",                         3600*9,    0},
	{"(GMT+09:30) Adelaide",                         3600*9.5,    4},
	{"(GMT+10:00) Guam, Russia",                         3600*10,   0},
	{"(GMT+10:00) Australia",                            3600*10,   4},
	{"(GMT+10:00) Sydney, Melbourne, Canberra",                            3600*10,   4},
	{"(GMT+11:00) Solomon Islands",                      3600*11,   0},
	{"(GMT+12:00) Fiji",                                 3600*12,   0},
	{"(GMT+12:00) New Zealand",                          3600*12,   4},
	};
const int tzEntrySize = sizeof(tzEntry) / sizeof(timeZoneTable);

const dstTable dstEntry[] = {
	{{ },                                   { },                                    0, 0, 0, 0, 0},
		//4 first sun - 10 last sun
	{{1,7,6,4,3,2,1,6,5,4,3,1},             {28,27,26,31,30,29,28,26,25,31,30,28 }, 4, 10, 0, 0, 3600},
		//3 last sun - 10 last sun
	{{25,31,30,28,27,26,25,30,29,28,27,25}, {28,27,26,31,30,29,28,26,25,31,30,28 }, 3, 10, 0, 0, 3600},
	{{25,31,30,28,27,26,25,30,29,28,27,25}, {28,27,26,31,30,29,28,26,25,31,30,28 }, 3, 10, 0, 0, 3600},
		//10 last sun - 3 last sun
	{{28,27,26,31,30,29,28,26,25,31,30,28}, {25,31,30,28,27,26,25,30,29,28,27,25 }, 10, 3, 1, 0, 3600},
	};
volatile unsigned long LastTimeStamp;// updata by NTP server
volatile TIME LastSystemTime;// updata by system timer and the base time received by NTP server
					// at GMT time
volatile unsigned int TimeZone = 4;				

void SystemTimeInit()
{
	LastTimeStamp = GMT_1970_01_01;
	memset((void*)&LastSystemTime, 0, sizeof(TIME));
	LastSystemTime.year = 1970;
	LastSystemTime.month = 1;
	LastSystemTime.day = 1;
	LastSystemTime.hour = 0;
	LastSystemTime.minute = 0;
	LastSystemTime.second = 0;
}

void time_convert(unsigned long time, TIME *tm)
{
	unsigned int remain_time, remain_in_day;
	unsigned int i, j, day_index, nDay;
	unsigned char startMonth;
	unsigned char endMonth;
	unsigned char diffMonth;
	int dstBias;

	memset((void *)tm, 0, sizeof(tm));
	if(time < GMT_2001_01_01)
		return;
	remain_time = time - GMT_2001_01_01;
	DEBUG("remain_time = %08x\n", remain_time);
	remain_time = remain_time + tzEntry[TimeZone].gmtOffset;
	remain_in_day = (remain_time / DAY);
	day_index = remain_in_day % 7;
	
	i = 0; // month
	j = 0; // year
	DEBUG("remain_time = %08x\n", remain_time);
	nDay = month_day_table[i];
	DEBUG("day_index:%d\n", day_index);	
	while (remain_in_day >= nDay)
	{
		
		if ((j+1)%4)
		{
			remain_in_day = remain_in_day - month_day_table[i];
			i++;
			if (i >= 12) // 12 months per year
			{
				i = 0;
				j++;
			}
			nDay = month_day_table[i];
		}
		else
		{
			remain_in_day = remain_in_day - month_day_table_addition[i];
			i++;
			if (i >= 12 ) // 12 months per year
			{
				i = 0;
				j++;
			}
			nDay = month_day_table_addition[i];
		}
	}
	tm->year = REF_YEAR + j;
	tm->month = i + 1;
	tm->day = remain_in_day + 1;
	tm->hour = (remain_time % DAY) / HOUR;
	tm->minute = ((remain_time % DAY) % HOUR) / MINUTE;
	tm->second = ((remain_time % DAY) % HOUR) % MINUTE;
	if (day_index == 6)
		tm->dayIndex = 1;
	else 
		tm->dayIndex = day_index + 2;
	tm->dname = (char *)DAY_NAME[tm->dayIndex];	
#if 0 
	DEBUG("1:%d-%d-%d %d:%d:%d:%s\n",
			tm->year,
			tm->month,
			tm->day,
			tm->hour,
			tm->minute,
			tm->second,
			tm->dname);	
#endif
	
	// compensate for daylight time
	// daylight time begin from the first sunday of April to 
	// the last sunday of October.
	startMonth = (unsigned char)(dstEntry[tzEntry[TimeZone].dstFlag].startMonth);
	endMonth = dstEntry[tzEntry[TimeZone].dstFlag].endMonth;
	diffMonth = dstEntry[tzEntry[TimeZone].dstFlag].diffMonth;	// 0: endMonth > startMonth	1:startMonth > endMonth
	dstBias = dstEntry[tzEntry[TimeZone].dstFlag].dstBias;	//hour     
	if (tzEntry[TimeZone].dstFlag &&
	(((diffMonth == 0) && 
		((tm->month == startMonth && tm->day >= dstEntry[tzEntry[TimeZone].dstFlag].startDay[j]) || 
		(tm->month == endMonth && tm->day <= dstEntry[tzEntry[TimeZone].dstFlag].endDay[j]) || 
		(tm->month > startMonth && tm->month < endMonth))) ||
	((diffMonth == 1) && 
		((tm->month == startMonth && tm->day >= dstEntry[tzEntry[TimeZone].dstFlag].startDay[j]) || 
		(tm->month == endMonth && tm->day <= dstEntry[tzEntry[TimeZone].dstFlag].endDay[j]) || 
		(tm->month > startMonth || tm->month < endMonth))))
	)
	{
		if (dstEntry[tzEntry[TimeZone].dstFlag].dstBias % 3600 == 0)
			tm->hour = tm->hour + dstEntry[tzEntry[TimeZone].dstFlag].dstBias / 3600;
		else
		{
			tm->minute =  tm->minute + dstEntry[tzEntry[TimeZone].dstFlag].dstBias / 60;
			if (tm->minute >= 60)
			{
			tm->minute = tm->minute - 60;
			tm->hour ++;
			}
		}
		
		if (tm->hour >= 24)
		{
			tm->hour = 0;
			tm->day ++;
			day_index ++;
			day_index = day_index % 7;
			if (tm->day > month_day_table[tm->month-1])
				tm->month ++;
		}
	}
	
}


