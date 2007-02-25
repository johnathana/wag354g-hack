#ifndef _INCLUDE_TICFG_H_
#define _INCLUDE_TICFG_H_

#define TICFG_MTD_OFFSET     (0)
#define TICFG_MTD_SIZE       (64*1024)

#define ENV_VAR_MTD          (3)
#define ENV_VAR_MTD_OFFSET   (0)
#define ENV_VAR_MTD_SIZE     (FLASH_ENV_ENTRY_SIZE * MAX_ENV_ENTRY)

#define CFGMAN_MTD           (3)
#define CFGMAN_MTD_OFFSET    (ENV_VAR_MTD_SIZE)
#define CFGMAN_MTD_SIZE      (TICFG_MTD_SIZE - ENV_VAR_MTD_SIZE)

#define TICFG_IOCTL_ERASE    0

#define MAX_ENV_DATA_LEN     (FLASH_ENV_ENTRY_SIZE)

extern int ticfg_erase_cfgman(struct mtd_info *mtd);
extern int ticfg_erase_env_vars(struct mtd_info *mtd);

extern void ticfg_lock_region(void);
extern void ticfg_unlock_region(void);

#endif
