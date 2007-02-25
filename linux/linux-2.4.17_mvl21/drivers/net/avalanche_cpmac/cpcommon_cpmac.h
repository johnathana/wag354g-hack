#ifndef _INC_CPCOMMON_H
#define _INC_CPCOMMON_H

#define VOLATILE32(addr)  (*(volatile bit32u *)(addr))
#ifndef dbgPrintf
#define dbgPrintf HalDev->OsFunc->Printf
#endif

#define ChannelUpdate(Field) if(HalChn->Field != 0xFFFFFFFF) HalDev->ChData[Ch].Field = HalChn->Field

#define DBG(level)  (HalDev->debug & (1<<(level)))
/*
#define DBG0()      DBG(0)
#define DBG1()      DBG(1)
#define DBG2()      DBG(2)
#define DBG3()      DBG(3)
#define DBG4()      DBG(4)
#define DBG5()      DBG(5)
#define DBG6()      DBG(6)
#define DBG7()      DBG(7)
*/

/*
 *  List of defined actions for use with Control().
 */
typedef enum 
  {
   enGET=0,     /**< Get the value associated with a key */
   enSET,       /**< Set the value associates with a key */
   enCLEAR,     /**<Clear the value */ 
   enNULL       /**< No data action, used to initiate a service or send a message */
  }ACTION;

/*
 *  Enumerated hardware states.
 */
typedef enum
  {
   enConnected=1, enDevFound, enInitialized, enOpened
  }DEVICE_STATE;

typedef enum 
  {
   enCommonStart=0,
   /* General  */
   enOff, enOn, enDebug, 
   /* Module General */
   enCpuFreq, 
   enStatus,
   enCommonEnd
   }COMMON_KEY;

typedef struct
  {
   const char  *strKey;
   int  enKey;
  }CONTROL_KEY;

typedef struct
  {
   char *StatName;
   unsigned int *StatPtr;
   int DataType; /* 0: int, 1: hex int, 2:channel data */
  }STATS_TABLE;

typedef struct
  {
   int NumberOfStats;
   STATS_TABLE *StatTable;
  }STATS_DB;

#define osfuncSioFlush()    HalDev->OsFunc->Control(HalDev->OsDev,"SioFlush",pszNULL,0)
#define osfuncSleep(Ticks)  HalDev->OsFunc->Control(HalDev->OsDev,pszSleep,pszNULL,Ticks)
#define osfuncStateChange() HalDev->OsFunc->Control(HalDev->OsDev,pszStateChange,pszNULL,0)

#define CHANNEL_NAMES {"Ch0","Ch1","Ch2","Ch3","Ch4","Ch5","Ch6","Ch7","Ch8","Ch9","Ch10","Ch11","Ch12","Ch13","Ch14","Ch15"}

#endif

