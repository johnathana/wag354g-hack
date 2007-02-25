#ifndef _INC_CPREMAP_C
#define _INC_CPREMAP_C

#ifdef __ADAM2
static inline void osfuncDataCacheHitInvalidate(void *ptr, int Size)
  {
  asm(" cache  17, (%0)" : : "r" (ptr));
  }

static inline void osfuncDataCacheHitWriteback(void *ptr, int Size)
  { 
  asm(" cache  25, (%0)" : : "r" (ptr));
  }
  
static inline void osfuncDataCacheHitWritebackAndInvalidate(void *ptr, int Size)
  { 
  asm(" cache  21, (%0)" : : "r" (ptr));
  }

#else

#define osfuncDataCacheHitInvalidate(MemPtr, Size)               __asm__(" .set mips3; cache  17, (%0); .set mips0" : : "r" (MemPtr))
#define osfuncDataCacheHitWritebackAndInvalidate(MemPtr, Size)   __asm__(" .set mips3; cache  21, (%0); .set mips0" : : "r" (MemPtr))
#define osfuncDataCacheHitWriteback(MemPtr, Size)                __asm__(" .set mips3; cache  25, (%0); .set mips0" : : "r" (MemPtr))      
            
#endif

#endif
