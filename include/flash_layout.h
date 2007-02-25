/* Unit */
#define KB	1024
#define MB	(1024 * KB)

/* Image size */
#ifdef MULTILANG_SUPPORT
#define FL_LANG_LEN	(128 * KB)
#else
#define FL_LANG_LEN	0
#endif

#define FL_BOOT_LEN	(128 * KB)
#define FL_KERN_LEN	(768 * KB)
#define FL_CONF_LEN	((64 * KB) * (FLASH_SIZE/4))
#define FL_FS_LEN	((FLASH_SIZE * MB) - FL_BOOT_LEN - FL_KERN_LEN - FL_LANG_LEN - FL_CONF_LEN)

/* Flash layout: 1-Boot, 2-Kernel, 3-Filesystem, 4-Language, and 5-Configure */
#define FL_BASE_ADDR	(0x90000000)	
#define FL_BOOT_OFFSET	(FL_BASE_ADDR)
#define FL_KERN_OFFSET	(FL_BOOT_OFFSET + FL_BOOT_LEN)
#define FL_FS_OFFSET	(FL_KERN_OFFSET + FL_KERN_LEN)
#define FL_LANG_OFFSET	(FL_FS_OFFSET + FL_FS_LEN)
#define FL_CONF_OFFSET	(FL_LANG_OFFSET	+ FL_LANG_LEN)
#define FL_END_ADDR	(FL_CONF_OFFSET	+ FL_CONF_LEN)

