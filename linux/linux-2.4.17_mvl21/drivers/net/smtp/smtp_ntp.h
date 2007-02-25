#ifndef __SMTP_NTP_H__
#define __SMTP_NTP_H__
unsigned long GMT_2001_01_01 =	3187296000;
unsigned long GMT_1970_01_01 =	0x83aa7e90;
#define REF_YEAR 	2001
#define DAY 		86400
#define HOUR 		3600
#define MINUTE 		60

struct timeZoneTable_ {
	unsigned char name[60];
	int gmtOffset;
	unsigned char dstFlag;
	unsigned char resv[3];
};
typedef struct timeZoneTable_ timeZoneTable;

struct dstTable_ { 
	unsigned char startDay[12];
	unsigned char endDay[12];
	unsigned char startMonth;
	unsigned char endMonth;
	unsigned char diffMonth;
	char resv;
	int dstBias;
};
typedef struct dstTable_ dstTable;

struct NAME_ {
	unsigned char name[50];
};

struct TIME_ {
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int dayIndex;
	char * dname;
};
typedef struct TIME_ TIME;
extern void time_convert(unsigned long time, TIME *tm);
#endif
