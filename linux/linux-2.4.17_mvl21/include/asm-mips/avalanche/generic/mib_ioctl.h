#ifndef _MIB_IOCTL_H_
#define _MIB_IOCTL_H_

#include "ioctl_api.h"

typedef struct
{
    unsigned long cmd;
    unsigned long port;
    void *data;
} TI_SNMP_CMD_T;

/* Ioctl/Cmd value to be used by snmpd like applications */
#define SIOTIMIB2   SIOCDEVPRIVATE + 1


#endif /* _MIB_IOCTL_H_ */
