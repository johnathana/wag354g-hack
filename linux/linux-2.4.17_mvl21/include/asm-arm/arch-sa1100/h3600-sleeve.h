/*
*
* Driver for H3600 Extension Packs
*
* Copyright 2000 Compaq Computer Corporation.
*
* Use consistent with the GNU GPL is permitted,
* provided that this copyright notice is
* preserved in its entirety in all copies and derived works.
*
* COMPAQ COMPUTER CORPORATION MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
* AS TO THE USEFULNESS OR CORRECTNESS OF THIS CODE OR ITS
* FITNESS FOR ANY PARTICULAR PURPOSE.
*
* Author: Jamey Hicks.
*
*/
#ifndef _INCLUDE_H3600_SLEEVE_H_
#define _INCLUDE_H3600_SLEEVE_H_

#ifdef CONFIG_DRIVERFS_FS
#include <linux/device.h>
#endif

enum sleeve_vendor {
        SLEEVE_NO_VENDOR     = 0,
        COMPAQ_VENDOR_ID     = 0x1125,
        TALON_VENDOR         = 0x0100,
        BIOCENTRIC_SOLUTIONS = 0x0101,
        HANVIT_INFO_TECH     = 0x0102,
        SYMBOL_VENDOR        = 0x0103,
        OMRON_CORPORATION    = 0x0104,
        HAGIWARA_SYSCOM_CO   = 0x0105,
        SLEEVE_ANY_ID = -1,
};

enum sleeve_device {
        SLEEVE_NO_DEVICE           = 0,

        /* COMPAQ_VENDOR_ID sleeve devices */
        SINGLE_COMPACTFLASH_SLEEVE = 0xCF11,
        SINGLE_PCMCIA_SLEEVE       = 0xD7C3,
        DUAL_PCMCIA_SLEEVE         = 0x0001,
        EXTENDED_BATTERY_SLEEVE    = 0x0002,
        GPRS_EXPANSION_PACK        = 0x0010,
        BLUETOOTH_EXPANSION_PACK   = 0x0011,
        MERCURY_BACKPAQ            = 0x0100
};

#define H3600_SLEEVE_ID(vendor,device) ((vendor)<<16 | (device))

struct sleeve_driver;
struct sleeve_dev {
        struct sleeve_driver  *driver;	        /* which driver has allocated this device */
        struct proc_dir_entry *procent;	        /* device entry in /proc/bus/sleeve */
        unsigned short	       vendor;          /* Copied from EEPROM */
        unsigned short	       device;  
        void		      *driver_data;	/* data private to the driver */
#ifdef CONFIG_DRIVERFS_FS
        struct device         dev;
#endif
};

struct sleeve_device_id {
        unsigned int vendor, device;		/* Vendor and device ID or SLEEVE_ANY_ID */
};

struct sleeve_driver {
        struct list_head               node;
        char                          *name;
        const struct sleeve_device_id *id_table;     /* NULL if wants all devices */
#ifdef CONFIG_DRIVERFS_FS
        struct device_driver          *driver;
#endif
        int  (*probe)(struct sleeve_dev *dev, const struct sleeve_device_id *did);  /* New device inserted */
        void (*remove)(struct sleeve_dev *dev);
        void (*suspend)(struct sleeve_dev *dev);
        void (*resume)(struct sleeve_dev *dev);

};

extern struct iobus h3600_sleeve_iobus;

extern int  h3600_current_sleeve( void );   /* returns H3600_SLEEVE_ID(vendor,dev) */
extern int  h3600_sleeve_register_driver(struct sleeve_driver *drv);
extern void h3600_sleeve_unregister_driver(struct sleeve_driver *drv);

#endif /* _INCLUDE_H3600_SLEEVE_H_ */
