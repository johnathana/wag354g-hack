#include <code_pattern.h>

#define RESERVE_MAC     8
#define PER_MAC_LEN     18      // contain '\0'

#define RESERVE_SN      8
#define PER_SN_LEN      20

#define PMON_MAC_START_ADDRESS  0x2000
#define PMON_VER_START_ADDRESS  0x2100

#define CFE_MAC_START_ADDRESS  0x1E00
#define CFE_VER_START_ADDRESS  0x1F00

#define FLASH_BASE	0xBFC00000

#define NOT_NULL(var,m,c) ( \
        var[m] != c && var[m+1] != c && var[m+2] != c && var[m+3] != c && var[m+4] != c && var[m+5] != c \
)

#define IS_NULL(var,m,c) ( \
        var[m] == c && var[m+1] == c && var[m+2] == c && var[m+3] == c && var[m+4] == c && var[m+5] == c \
)

#define MAC_ADD(mac) ({\
                int i,j; \
                unsigned char m[6]; \
                /* sscanf(mac,"%x:%x:%x:%x:%x:%x",&m[0],&m[1],&m[2],&m[3],&m[4],&m[5]);   will error */ \
                for(j=0,i=0 ; i<PER_MAC_LEN ; i+=3,j++) { \
                        if(mac[i] >= 'A' && mac[i] <= 'F')              mac[i] = mac[i] - 55;\
                        if(mac[i+1] >= 'A' && mac[i+1] <= 'F')  mac[i+1] = mac[i+1] - 55;\
                        if(mac[i] >= 'a' && mac[i] <= 'f')              mac[i] = mac[i] - 87;\
                        if(mac[i+1] >= 'a' && mac[i+1] <= 'f')  mac[i+1] = mac[i+1] - 87;\
                        if(mac[i] >= '0' && mac[i] <= '9')              mac[i] = mac[i] - 48;\
                        if(mac[i+1] >= '0' && mac[i+1] <= '9')  mac[i+1] = mac[i+1] - 48;\
                        m[j] = mac[i]*16 + mac[i+1]; \
                } \
                for(i=5 ; i>=3 ; i--){ \
                        if( m[i] == 0xFF)       { m[i] = 0x0; continue; } \
                        else                    { m[i] = m[i] + 1; break; } \
                } \
                sprintf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",m[0],m[1],m[2],m[3],m[4],m[5]); \
})

