#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

#include <bcmnvram.h>
#include <rc.h>

int start_second_lan(void){
	if(nvram_match("telnet_enable","1"))
	{
		  char seclan_ipaddr[32];
		  char seclan_netmask[32];
		  char cmd[128];

		  if (nvram_invmatch("seclan_enable","1")) {
			    return 0;
		  }

		  strcpy(seclan_ipaddr , nvram_safe_get("seclan_ipaddr"));
		  strcpy(seclan_netmask , nvram_safe_get("seclan_netmask"));

		  if (seclan_netmask[0] == 0 || seclan_ipaddr[0] == 0)
			    return 0;

		  sprintf(cmd,"/sbin/ifconfig br0:1 %s netmask %s",seclan_ipaddr, seclan_netmask);
		  system(cmd);
	}
  
	return 0;
}

int stop_second_lan(void) {
  
  system("ifconfig br0:1 down");
  
  return 0;
}
