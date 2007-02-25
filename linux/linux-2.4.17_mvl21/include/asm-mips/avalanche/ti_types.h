#ifndef __TI_TYPES_H__
#define __TI_TYPES_H__

#ifndef __KERNEL__
#include <stdint.h>
#endif

typedef enum {
  OK = 0,
  ERROR = -1
} STATUS;

#define IMPORT extern
#define FAST   register

#define TRUE 1
#define FALSE 0

//#define UINT32_T uint32_t;

#ifndef __KERNEL__

typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;
typedef unsigned long ULONG;
typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef unsigned char BOOL;
typedef int32_t INT32;
typedef int16_t INT16;
typedef int8_t  INT8;

#else 

typedef __u8 UINT8;
typedef __u32 UINT32;
typedef __u16 UINT16;
typedef __u32 ULONG;
typedef __u8 UCHAR;
typedef __u32 UINT;
typedef __u16 USHORT;
typedef __u8 BOOL;

#endif // __KERNEL__

#ifdef  __cplusplus
//typedef int             (*FUNCPTR) (...);     
typedef void            (*VOIDFUNCPTR) (...); 
typedef double          (*DBLFUNCPTR) (...);  
typedef float           (*FLTFUNCPTR) (...);  
#else
//typedef int             (*FUNCPTR) ();    
typedef void            (*VOIDFUNCPTR) ();
typedef double          (*DBLFUNCPTR) (); 
typedef float           (*FLTFUNCPTR) (); 
#endif

#endif // __TI_TYPES_H__
