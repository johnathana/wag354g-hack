RCSID $Id: include.linux.in.h,v 1.1.1.2 2005/03/28 06:57:39 sparq Exp $
--- ./include/linux/in.h.preipsec	Thu Apr 29 23:13:17 1999
+++ ./include/linux/in.h	Fri Apr 30 09:38:50 1999
@@ -36,7 +36,10 @@
 
   IPPROTO_IPV6	 = 41,		/* IPv6-in-IPv4 tunnelling		*/
 
+  IPPROTO_ESP = 50,		/* Encapsulation Security Payload protocol */
+  IPPROTO_AH = 51,		/* Authentication Header protocol	*/
   IPPROTO_PIM    = 103,		/* Protocol Independent Multicast	*/
+  IPPROTO_COMP   = 108,		/* Compression Header protocol		*/
 
   IPPROTO_RAW	 = 255,		/* Raw IP packets			*/
   IPPROTO_MAX
