#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

#include <bcmnvram.h>
#include <rc.h>

int mk_etc_passwd(void){
	FILE *passwd;

	passwd=fopen("/tmp/passwd","w");

	fprintf(passwd,"%s:",nvram_safe_get("http_username"));
	fprintf(passwd,"%s:0:0::/tmp:/bin/sh\n",crypt(nvram_safe_get("http_passwd"),"Zi"));
	fprintf(passwd,"%s:",nvram_safe_get("http_remote_username"));
	fprintf(passwd,"%s:0:0::/tmp:/bin/sh\n",crypt(nvram_safe_get("http_remote_passwd"),"Zi"));
	
	fclose(passwd);
	return 0;

};

