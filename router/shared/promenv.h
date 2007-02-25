#ifndef _promenv_h_
#define _promenv_h_

#define TICFG_BUF 10*1024
#define TICFG_ENV "/proc/ticfg/env"

#define prom_unsetenv(k) prom_setenv(k, NULL)

extern int prom_getenv(const char *key, char *val);
extern int prom_setenv(char *key, char *val);
extern int prom_setenv_va(char *env_nm, const char *format, ...);

#endif /* _promenv_h_ */
