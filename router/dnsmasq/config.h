/* dnsmasq is Copyright (c) 2000 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/

/* Author's email: simon@thekelleys.org.uk */

#define VERSION "1.12"

#define FTABSIZ 50 /* max number of outstanding requests */
#define TIMEOUT 40 /* drop queries after TIMEOUT seconds */
#define LOGRATE 120 /* log table overflows every LOGRATE seconds */
#define CACHESIZ 150 /* default cache size */
#define MAXTOK 50 /* token in DHCP leases */
#define SMALLDNAME 40 /* most domain names are smaller than this */
#define CONFFILE "/etc/dnsmasq.conf"
#define HOSTSFILE "/etc/hosts"
#ifdef __uClinux__
#define RESOLVFILE "/etc/config/resolv.conf"
#else
#define RESOLVFILE "/etc/resolv.conf"
#endif
#define RUNFILE "/var/run/dnsmasq.pid"
#define CHUSER "nobody"
#define IP6INTERFACES "/proc/net/if_inet6"

/* Decide if we're going to support IPv6 */
/* We assume that systems which don't have IPv6
   headers don't have ntop and pton either */

#if defined(INET6_ADDRSTRLEN)
#  define HAVE_IPV6
#  define ADDRSTRLEN INET6_ADDRSTRLEN
#elif defined(INET_ADDRSTRLEN)
#  undef HAVE_IPV6
#  define ADDRSTRLEN INET_ADDRSTRLEN
#else
#  undef HAVE_IPV6
#  define ADDRSTRLEN 16 /* 4*3 + 3 dots + NULL */
#endif


/* Follows system specific switches. If you run on a 
   new system, you may want to edit these. 
   May replace this with Autoconf one day. 


HAVE_LINUX_IPV6_PROC
   define this to do IPv6 interface discovery using
   proc/net/if_inet6 ala LINUX. 

HAVE_GETOPT_LONG
   define this if you have GNU libc or GNU getopt. 

HAVE_ARC4RANDOM
   define this if you have arc4random() to get better security from DNS spoofs
   by using really random ids (OpenBSD) 

HAVE_RANDOM
   define this if you have the 4.2BSD random() function (and its
   associated srandom() function), which is at least as good as (if not
   better than) the rand() function.

HAVE_DEV_RANDOM
   define this if you have the /dev/random device, which gives truly
   random numbers but may run out of random numbers.

HAVE_DEV_URANDOM
   define this if you have the /dev/urandom device, which gives
   semi-random numbers when it runs out of truly random numbers.

HAVE_SOCKADDR_SA_LEN
   define this if struct sockaddr has sa_len field (*BSD) 

NOTES:
   For Linux you should define 
      HAVE_LINUX_IPV6_PROC 
      HAVE_GETOPT_LONG
      HAVE_RANDOM
      HAVE_DEV_RANDOM
      HAVE_DEV_URANDOM
   you should NOT define 
      HAVE_ARC4RANDOM
      HAVE_SOCKADDR_SA_LEN

   For *BSD systems you should define 
     HAVE_SOCKADDR_SA_LEN
     HAVE_RANDOM
   you should NOT define  
     HAVE_LINUX_IPV6_PROC 
   and you MAY define  
     HAVE_ARC4RANDOM - OpenBSD and FreeBSD 
     HAVE_DEV_URANDOM - OpenBSD and FreeBSD
     HAVE_DEV_RANDOM - FreeBSD (OpenBSD with hardware random number generator)
     HAVE_GETOPT_LONG - only if you link GNU getopt. 

*/

/* Must preceed __linux__ since uClinux defines __linux__ too. */
#if defined(__uClinux__)
#undef HAVE_LINUX_IPV6_PROC
#define HAVE_GETOPT_LONG
#undef HAVE_ARC4RANDOM
#define HAVE_RANDOM
#define HAVE_DEV_URANDOM
#define HAVE_DEV_RANDOM
#undef HAVE_SOCKADDR_SA_LEN
/* Don't fork into background on uClinux */
#define NO_FORK

/* libc5 - must precede __linux__ too */
/* Note to build a libc5 binary on a modern Debian system:
   install the packages alt-gcc libc5 and libc5-altdev 
   then run "make CC=i486-linuxlibc1-gcc" */
#elif defined(__linux__) && \
      defined(_LINUX_C_LIB_VERSION_MAJOR) && \
      (_LINUX_C_LIB_VERSION_MAJOR == 5 )
#undef HAVE_IPV6
#undef HAVE_LINUX_IPV6_PROC
#define HAVE_GETOPT_LONG
#undef HAVE_ARC4RANDOM
#define HAVE_RANDOM
#define HAVE_DEV_URANDOM
#define HAVE_DEV_RANDOM
#undef HAVE_SOCKADDR_SA_LEN
/* Fix various misfeatures of libc5 headers */
#define T_SRV 33 
typedef unsigned long in_addr_t; 
typedef size_t socklen_t;

#elif defined(__linux__)
#define HAVE_LINUX_IPV6_PROC
#define HAVE_GETOPT_LONG
#undef HAVE_ARC4RANDOM
#define HAVE_RANDOM
#define HAVE_DEV_URANDOM
#define HAVE_DEV_RANDOM
#undef HAVE_SOCKADDR_SA_LEN
/* glibc2.0 has broken Sockaddr_in6 so we have to use our own. */
#if defined(_LINUX_C_LIB_VERSION_MINOR) && \
    (_LINUX_C_LIB_VERSION_MINOR == 0 )
#define HAVE_BROKEN_SOCKADDR_IN6
#endif

#elif defined(__FreeBSD__) || defined(__OpenBSD__)
#undef HAVE_LINUX_IPV6_PROC
#undef HAVE_GETOPT_LONG
#define HAVE_ARC4RANDOM
#define HAVE_RANDOM
#define HAVE_DEV_URANDOM
#define HAVE_SOCKADDR_SA_LEN

#elif defined(__NetBSD__)
#undef HAVE_LINUX_IPV6_PROC
#undef HAVE_GETOPT_LONG
#undef HAVE_ARC4RANDOM
#define HAVE_RANDOM
#undef HAVE_DEV_URANDOM
#undef HAVE_DEV_RANDOM
#define HAVE_SOCKADDR_SA_LEN
 
/* env "LIBS=-lsocket -lnsl" make */
#elif defined(__sun) || defined(__sun__)
#undef HAVE_LINUX_IPV6_PROC
#undef HAVE_GETOPT_LONG
#undef HAVE_ARC4RANDOM
#define HAVE_RANDOM
#undef HAVE_DEV_URANDOM
#undef HAVE_DEV_RANDOM
#undef HAVE_SOCKADDR_SA_LEN
#endif




