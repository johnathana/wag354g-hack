
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/types.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <linux/wireless.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <pty.h>
#include <sys/poll.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>


#include <bcmnvram.h>
#include <rc.h>
#include <shutils.h>


#include <ti_wireless/tiwlan_drv.h>    
#include <ti_wireless/ti_types.h>      
#include <ti_wireless/pform_types.h>   
#include <ti_wireless/whalDefs.h>      
#include <ti_wireless/app_driver_api.h>
#include <ti_wireless/mgmtAccess.h>    

#include <tiwlan_conf.h>

#define TIWLAN_CM_SOCKET_PATH "/var/tmp/cm_logic.ctl"

#define CONF_SIZE   TIWLAN_CONF_SIZE //3426 // 3162 // 3157 //3153 // 2350 // 2175
#define TEMP_SIZE  TIWLAN_NVRAM_SIZE //1024 

//#define OPTIMIZATION42		// WiFi Mode  

//wwzh 
int get_wireless_mac(char *hwaddr);
int get_wireless_version(char *version);

                                                                              
/* 0: success                                                                 
 * !=0 Error                                                                  
 */                                                                           
                                                                              
static int do_ioctl(tiwlan_ioctl_cmd_opcode_t opcode,                         
        tiwlan_ioctl_data_t type, unsigned long size, void *data,             
                tiwlan_ioctl_cmd_t *cmd)                                      
{                                                                             
        int result, dev_fd;                                                   
                                                                              
        if ((dev_fd = open("/dev/tiwlan", O_RDWR)) < 0)                       
        {                                                                     
                printf("Unable to open device log file!\n");                  
                return -1;                                                    
        }                                                                     
        cmd->opcode = opcode;                                                 
        cmd->data_type = type;                                                
        cmd->data_size = size;                                                
	
        if ((type == TIWLAN_IOCTL_DATA_PACKET) || (type == TIWLAN_IOCTL_DATA_CHAR_PTR))                                                                         
        {                                                                       
                cmd->packet = (char *)data;                                     
        }                                                                       
        else                                                                    
        {                                                                       
                memcpy((void *)cmd->data, data, cmd->data_size);                
        }                                                                       
                                                                                
        result = ioctl(dev_fd, TIWLAN_DRV_IOCTL_IOCTL, cmd);                    
        close(dev_fd);                                                          
        return result;                                                          
}                                                                               

/* 0: Disabled                                                                  
   1: Enabled                                                                   
 */                                                                             
int tiwlan_SetEnable(unsigned char enable)                                      
{                                                                               
        tiwlan_ioctl_cmd_t cmd;                                                 
        int result;                                                             
                                                                                
        result = do_ioctl(TIWLAN_CMD_IOCTL_SET_SERVICE_ENABLE, TIWLAN_IOCTL_DATA_U8, sizeof(unsigned char), (void *)&enable, &cmd);                             
        if (result != 0)                                                        
                printf("Set Wireless Enabled/Disabled Error!\n");               
        return result;                                                          
}                                                                               
                                                                                


int  reset_wireless_defaults(void)
{
	int fd;
	char default_file_name[] = "/etc/ap.bin";
	int rd, wr;
	char conf_file_buf[CONF_SIZE];
	char *xml_config;
	char buff[128];
	int offset, size, count, index, xml_length;

	unsigned char tmpbuf[6];
	
	
	memset((void *)conf_file_buf, 0, CONF_SIZE);
	fd = open(default_file_name, O_RDONLY);
	if (fd < 0)
	{
		printf("open wireless default file faile\n");
		return -1;
	}	
	rd = read(fd, conf_file_buf, CONF_SIZE);
	if (rd != CONF_SIZE)
	{
		printf("read wireless default file size mismatch!\n");
		close(fd);
		return -1;
	}
	close(fd);
//wwzh add for modify wireless mac address
#if 0
	conf_file_buf[0] = 0x00;
	conf_file_buf[1] = 0x50;
	conf_file_buf[2] = 0xf1;
	conf_file_buf[3] = 0x12;
	conf_file_buf[4] = 0x0a;
	conf_file_buf[5] = 0xaa;
#endif
	macaddr_get(buff);
	ether_atoe(buff, tmpbuf);

	memcpy(conf_file_buf, tmpbuf, 6);
	printf("change to %02x:%02x:%02x:%02x:%02x:%02x", conf_file_buf[0], conf_file_buf[1], conf_file_buf[2], conf_file_buf[3], conf_file_buf[4], conf_file_buf[5]);

	xml_config = io_b64enc((char *)conf_file_buf, CONF_SIZE);

	xml_length = strlen(xml_config);
	index = 0;
	offset = 0;
	count = 0;
	size = 0;
	
	count = ((xml_length%TEMP_SIZE) == 0) ? (xml_length/TEMP_SIZE):((xml_length/TEMP_SIZE) + 1);
	memset((void *)buff, 0, sizeof(buff));
	sprintf(buff, "%d", count);
	nvram_set("wl_bin_config_total", buff);
	
	while(index < count)
	{
		char *temp_string;
		
		memset((void *)buff, 0, sizeof(buff));
		sprintf(buff, "wl_bin_config%d", index);
		size = ((xml_length - offset) > TEMP_SIZE) ? TEMP_SIZE:(xml_length - offset);
		if ((temp_string = strndup(xml_config + offset, size)) == NULL)
		{
			printf("Restore Wireless Defaults Error: strndup faild\n");
			free(xml_config);
			break;
	
		}	
		nvram_set(buff, temp_string);
		free(temp_string);
		offset += size;
		index++;
	}
		

	nvram_commit();
	free(xml_config);
	printf("restore wireless default successfully\n");
	return 0;
}
int write_wireless_mac(void)
{
	char xml_config[CONF_SIZE * 2];
	char *binary_config;
	char *nvram_config;
	int offset, index, size, count, xml_length, len;
	char buff[128];
	char config_buff[CONF_SIZE];
	unsigned char hwaddr[12];

	memset((void *)config_buff, 0, sizeof(config_buff));
	memset((void *)xml_config, 0, sizeof(xml_config));
	memset((void *)buff, 0, sizeof(buff));

	count = atoi(nvram_get("wl_bin_config_total"));
	index = 0;
	while(index < count)
	{	
		sprintf(buff, "wl_bin_config%d", index);
		if (index == 0)
			strcpy(xml_config, nvram_get(buff));
		else
			strcat(xml_config, nvram_get(buff));
		index++;
		memset((void *)buff, 0, sizeof(buff));
	}

	binary_config = io_b64dec((char *)xml_config, &len);
	printf("write wireless mac len = %d\n", len);
	// modify wireless mac address 
	memset((void *)buff, 0, sizeof(buff));
	macaddr_get(buff);
	ether_atoe(buff, hwaddr);

	memcpy((char *)config_buff, binary_config, len);
	memcpy(config_buff, hwaddr, 6);

	free(binary_config);

	// save config information to nvram 

	nvram_config = io_b64enc((char *)config_buff, CONF_SIZE);
	if (nvram_config == NULL)
	{
		printf("Write Wireless MAC Address Error\n");
		return -1;
	}

	xml_length = 0;
	xml_length = strlen(nvram_config);
	index = 0;
	count = 0;
	offset = 0;
	size = 0;

	count = ((xml_length % TEMP_SIZE) == 0) ? (xml_length/TEMP_SIZE) : ((xml_length/TEMP_SIZE) + 1);
	
	memset((void *)buff, 0, sizeof(buff));
	sprintf((char *)buff, "%d", count);
	nvram_set("wl_bin_config_total", buff);
	while(index < count)
	{
		char *temp_string;
		
		memset((void *)buff, 0, sizeof(buff));
		sprintf((char *)buff, "wl_bin_config%d", index);
		size = ((xml_length - offset) > TEMP_SIZE) ? TEMP_SIZE: (xml_length - offset);
		if ((temp_string = strndup(nvram_config + offset, size)) == NULL)
		{
			printf("Write Wireless MAC Error: strndup failed\n");
			break;
		}
		nvram_set(buff, temp_string);
		free(temp_string);
		offset += size;
		index++;
	}
	nvram_commit();
	free(nvram_config);
	printf("Write Wireless MAC address successfully\n");
	return 0;
}
int start_wireless(void)
{
	int mode = 0;
	char hwaddr[20];
	char version[10];
	int result = 0;
	mode = atoi(nvram_get("wl_gmode"));
	
	if(!nvram_match("mac_write", "0"))
	{
		printf("restore wireless defaults\n");
		write_wireless_mac();
		sleep(1);
	}
	
	eval("wlan.sh", "start");
	if (mode != -1)
	{
		result = get_wireless_mac(hwaddr);
		if (result != -1)
			nvram_set("wl0_hwaddr", hwaddr);
	#if 0
		result = get_wireless_version(version);
		if(result != -1)
	#endif
	#if (TIWLAN_VERSION == 5520)
		nvram_set("wl_firmware_version", "5.5.20");
	#elif (TIWLAN_VERSION == 554)
		nvram_set("wl_firmware_version", "5.5.4");
	#endif 

	}
	else
	{
		sleep(2);
		tiwlan_SetEnable(0);
		nvram_set("wl0_hwaddr", "");
		nvram_set("wl_firmware_version", "");
	}
#ifdef OPTIMIZATION42  //WiFi Mode
	system("echo 1 > /proc/avalanche/optimization42");
	system("echo 1 > /proc/tiwlan/optimization42");
#endif
	nvram_set("mac_write", "0");
	nvram_commit();
	return 0;
}
int stop_wireless(void)
{
	eval("wlan.sh", "stop");
	nvram_set("wl0_hwaddr", "");
	nvram_set("wl_firmware_version", "");
#ifdef OPTIMIZATION42  //WiFi Mode
	system("echo 0 > /proc/avalanche/optimization42");
#endif
	return 0;
}
int restart_wireless(void)
{
	int mode = 0;
	FILE *file = NULL;
	char buf[256];
	int flag = 0;
	char *p;
	int result;
	char hwaddr[20];
	char version[10];

	eval("wlan.sh", "restart");

	mode = atoi(nvram_get("wl_gmode"));
	if (mode != -1)
	{
		result = get_wireless_mac(hwaddr);
		if (result != -1)
			nvram_set("wl0_hwaddr", hwaddr);
	#if 0
		result = get_wireless_version(version);
		if(result != -1)
	#endif
	#if (TIWLAN_VERSION == 5520)
		nvram_set("wl_firmware_version", "5.5.20");
	#elif (TIWLAN_VERSION == 554)
		nvram_set("wl_firmware_version", "5.5.4");
	#endif
	}
	else
	{
		sleep(3);
		tiwlan_SetEnable(0);
		nvram_set("wl0_hwaddr", "");
		nvram_set("wl_firmware_version", "");
	}
#ifdef OPTIMIZATION42  //WiFi Mode
	system("echo 1 > /proc/avalanche/optimization42");
	system("echo 1 > /proc/tiwlan/optimization42");
#endif
	return 0;
}
int get_wireless_mac(char *hwaddr)
{
	int fd;
	int i;
	struct ifreq ifr;
	unsigned char arp[6];

	memset((void *)&ifr, 0, sizeof(struct ifreq));
	fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (fd < 0)
	{
		printf("create socket failed for getting wireless mac addr\n");
		return -1;
	}
	ifr.ifr_addr.sa_family = AF_INET;
	strcpy(ifr.ifr_name, "tiwlan0");
	
	if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
	{
		
		memcpy(arp, ifr.ifr_hwaddr.sa_data, 6);		
		sprintf(hwaddr, "%02x:%02x:%02x:%02x:%02x:%02x", arp[0], arp[1], arp[2], arp[3], arp[4], arp[5]);
		for (i = 0; i < strlen(hwaddr); i++)
			hwaddr[i] = toupper(hwaddr[i]);	
		close(fd);
		return 0;
	}
	close(fd);
	return -1;
}
int get_wireless_version(char *version)
{
	FILE *file;
	char *pos = NULL;
	char buffer[512];

	file = fopen("/proc/tiwlan/status", "r");
	if (NULL == file)
	{
		printf("Error: Failed to open /proc/tiwlan/status\n");
		return -1;
	}
	while(fgets(buffer, sizeof(buffer), file))
	{
		if ((pos = strstr(buffer, "APDK")) != NULL)
			break;
	}
	if (NULL == pos)
	{
		printf("Error: Failed to get APDK version\n");
		fclose(file);
		return -1;
	}
	else
	{
		sscanf((pos + 7), "%6c", version);
		version[6] = '\0';
	}
	fclose(file);
	return 0;
}
