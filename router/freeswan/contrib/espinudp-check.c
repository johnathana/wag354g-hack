/* 
   Quick C app to test the kernel for support of ESP in UDP
   (aka NAT-Traversal)
   (c) 2003 Ken Bantoft <ken@freeswan.ca>

*/
#include <stdlib.h>
#include <errno.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#define IPPROTO_UDP 17
#define UDP_ESPINUDP    100

#define ESPINUDP_WITH_NON_IKE   1  /* draft-ietf-ipsec-nat-t-ike-00/01 */
#define ESPINUDP_WITH_NON_ESP   2  /* draft-ietf-ipsec-nat-t-ike-02    */


int check_for_espinudp (int sk, u_int32_t type)
{       
        int r;

        r = setsockopt(sk, IPPROTO_UDP, UDP_ESPINUDP, &type, sizeof(type));
        if ((r<0) && (errno == ENOPROTOOPT)) {
		printf("No\n");
        } else {
		printf("Yes\n");
	}
        return r;
}


int main () {
int fd;
int udpike,udpesp;

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	printf("Checking for ESP in UDP IKE Support (ESPinUDP(%d)): ",ESPINUDP_WITH_NON_IKE);
	udpike = check_for_espinudp(fd,ESPINUDP_WITH_NON_IKE);
	printf("Checking for ESP in UDP ESP Support (ESPinUDP(%d)): ",ESPINUDP_WITH_NON_ESP);
	udpesp = check_for_espinudp(fd,ESPINUDP_WITH_NON_ESP);
	printf("Kernel Supports NAT Traversal: ");
	if(udpike == 0 && udpesp == 0) {
		printf("Yes\n");
	} else {
		printf("No\n");
	}


}
