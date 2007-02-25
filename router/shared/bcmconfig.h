/*
 * Automatically generated C config: don't edit
 */

/*
 * Base Features
 */
#define __CONFIG_RC__ 1
#define __CONFIG_NVRAM__ 1
#define __CONFIG_SHARED__ 1
#define __CONFIG_BUSYBOX__ 1
#define __CONFIG_BUSYBOX_CONFIG__ "router"
#define __CONFIG_WLCONF__ 1
#define __CONFIG_BRIDGE__ 1
#define __CONFIG_VCONF__ 1
#define __CONFIG_HTTPD__ 1
#define __CONFIG_WWW__ 1
#define __CONFIG_NETCONF__ 1
#define __CONFIG_IPTABLES__ 1
#undef __CONFIG_LIBIPT__
#undef __CONFIG_GLIBC__
#define __CONFIG_UCLIBC__ 1
#define __CONFIG_LIBOPT__ 1

/*
 * Options
 */
#define __CONFIG_VENDOR__ "broadcom"
#define __CONFIG_UDHCPD__ 1
#define __CONFIG_DHCPRELAY__ 1
#undef __CONFIG_PPP__
#define __CONFIG_UPNP__ 1
#undef __CONFIG_WAN_UPGRADE__
#undef __CONFIG_DNS_ENTRY__
#undef __CONFIG_CPE__
#undef __CONFIG_NAS__
#define __CONFIG_NTP__ 1
#define __CONFIG_DNSMASQ__ 1
#undef __CONFIG_UTILS__
#undef __CONFIG_ETC__
#define __CONFIG_SNMPD__ 1
#define __CONFIG_ZEBRA__ 1
#define __CONFIG_FREESWAN__ 1
#define __CONFIG_TC__ 1
#define __CONFIG_PING__ 1
#define __CONFIG_EMAILALERT__ 1
#define __CONFIG_UPGRADEFIRMWARE__ 1
#undef __CONFIG_IGMPPROXY__

/*
 * Additional C libraries
 */
#define __CONFIG_LIBCRYPT__ 1
#define __CONFIG_LIBDL__ 1
#define __CONFIG_LIBM__ 1
#define __CONFIG_LIBNSL__ 1
#define __CONFIG_LIBPTHREAD__ 1
#define __CONFIG_LIBRESOLV__ 1
#define __CONFIG_LIBUTIL__ 1
#define __CONFIG_LIBATM__ 1

/*
 * Environment
 */
#define __PLATFORM__ "mipsel"
#define __LINUXDIR__ "$(SRCBASE)/linux/linux"
#define __LIBDIR__ "$(TOOLCHAIN)/lib"
#define __USRLIBDIR__ "$(TOOLCHAIN)/usr/lib"
#define __KERNELDIR__ "$(SRCBASE)/linux/linux-2.4.17_mvl21"

/*
 * Internal Options
 */
