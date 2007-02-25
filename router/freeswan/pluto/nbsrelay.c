/**************************************************************************
 *nbsrelay.c :receive the NBNS request  packet, and broadcast to 
 *            remote secure group of IPSEC
 *
 *
 **************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<error.h>
#include<netdb.h>
#include<netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <glob.h>

#define SERVER_PORT	137
#define MAX_PKT_SIZE    350
#define NBSR_MAX_LEN    5//max length of the nbsr list
#define DIP_MAX         5//max number of remote IP
#define UDPHDR_LEN      8
#define IPHDR_LEN       20
//#define PC_TEST
#undef PC_TEST
#ifdef PC_TEST
#define NBS_PAIR_FL      "./nbsbrdc/*.nbs"
#else
#define NBS_PAIR_FL     "/tmp/nbsbrdc/*.nbs"
#endif
#define print_err printf
typedef  unsigned long  u32;
typedef  unsigned short u16;
#define clear_arr(a) memset(a,0,sizeof(a))

typedef struct udp_pkt{
	struct iphdr ip;
	struct udphdr udp;
	} udppkt;
typedef struct sub_brd{
	u32 sub_ip;/*sub net address,also the start ip, the network byte order*/
	u32 brd_ip;/*the broadcast ip address,also the end ip, the network byte order*/
	}subbrd_t;

typedef struct ip_mask{
	struct in_addr ip;
	struct in_addr mask;
} ipmask;

typedef struct nbsrelay{
	int valid;/*1--valid,0--empty*/
	subbrd_t lip;/*local secure group*/
	int  rip_cnt; /*the counter of remote secure group*/
	subbrd_t  rip[DIP_MAX];/*the ip list of remote secure group*/
	} nbsrelay_t;

nbsrelay_t nbsr_list[NBSR_MAX_LEN];
#define  clear_nbsrelay(nr) memset(nr,0,sizeof(nbsrelay_t))
#define get_ip(ipmsk) ((ipmsk)->ip.s_addr)
#define get_mask(ipmsk) ((ipmsk)->mask.s_addr)

//MACRO for debug
//#define ZHS_DEBUG
#ifdef  ZHS_DEBUG
#define zhs_printf printf
void show_nbsr(nbsrelay_t *nbsr)
{
	int j;	
	{
		if(nbsr->valid)
		{
			struct in_addr subip,brdip;
			subip.s_addr =nbsr->lip.sub_ip;
			brdip.s_addr =nbsr->lip.brd_ip;
			zhs_printf("  lid=%x(%s)--%x(%s)  ,rid_cnt=%d\n"
				,nbsr->lip.sub_ip,inet_ntoa(subip)
				,nbsr->lip.brd_ip,inet_ntoa(brdip),nbsr->rip_cnt);
			
			for(j=0;j<nbsr->rip_cnt;j++)
			{
				subip.s_addr =nbsr->rip[j].sub_ip;
				brdip.s_addr =nbsr->rip[j].brd_ip;
				zhs_printf("  rid[%d]=%x(%s)--%x(%s) \n"
					,j,nbsr->rip[j].sub_ip,inet_ntoa(subip)
					,nbsr->rip[j].brd_ip,inet_ntoa(brdip));
			}
		}
	}
}
void show_nbsr_list(void)
{
	int i;
	nbsrelay_t *nbsr = nbsr_list;
	for(i=0;i<NBSR_MAX_LEN;i++,nbsr ++)
	{
		zhs_printf("nbsr_list[%d].valid=%d \n ",i,nbsr->valid);
	#if 0
		if(nbsr->valid)
		{
			struct in_addr subip,brdip;
			subip.s_addr =nbsr->lip.sub_ip;
			brdip.s_addr =nbsr->lip.brd_ip;
			zhs_printf("  lid=%x(%s)--%x(%s)  ,rid_cnt=%d\n"
				,nbsr->lip.sub_ip,inet_ntoa(subip)
				,nbsr->lip.brd_ip,inet_ntoa(brdip),nbsr->rip_cnt);
			
			for(j=0;j<nbsr->rip_cnt;j++)
			{
				subip.s_addr =nbsr->rip[j].sub_ip;
				brdip.s_addr =nbsr->rip[j].brd_ip;
				zhs_printf("  rid[%d]=%x(%s)--%x(%s) \n"
					,j,nbsr->rip[j].sub_ip,inet_ntoa(subip)
					,nbsr->rip[j].brd_ip,inet_ntoa(brdip));
			}
		}
	#else 
		show_nbsr(nbsr);
	#endif
	}
}

#else
#define zhs_printf(fmt,arg...) 
#define show_nbsr_list() 
#define show_nbsr(nbsr)
#endif	
/************************************************************
 *ipmask_to_subbrd:conver the ipmask to sub_bdr format.
 */
static void ipmask_to_subbrd(const ipmask *ipmsk, subbrd_t *sub)
{
	int sub_len;/* the value of ntohl(sub_len) is the sub length*/
	
	sub->sub_ip = get_ip(ipmsk) & get_mask(ipmsk);
	sub_len =~(get_mask(ipmsk));

	sub->brd_ip = sub->sub_ip + sub_len;
}
/********************************************************
 *init_nbsr_list:cleari all the nbsr to zero
 */
#define init_nbsr_list() memset(nbsr_list,0,NBSR_MAX_LEN*sizeof(nbsrelay_t))

/*********************************************************
 */
static int is_in_subnet(subbrd_t *sbbrd,u32 ip)
{
	u32 sub_s = ntohl(sbbrd->sub_ip);
	u32 sub_e = ntohl(sbbrd->brd_ip);
	u32 ip_h =  ntohl(ip);
	
	if(  sub_s <= ip_h
	   &&sub_e >= ip_h)
		return 1;
	return 0;
			
}
/*********************************************************
 *find_nbsr:
 */
static int find_nbsr(subbrd_t *lip)
{
	int i;
	nbsrelay_t *nbsr = nbsr_list;
	zhs_printf("%s start with lip(sub_ip=%x,brd_ip=%x) ..."
		   ,__FUNCTION__,lip->sub_ip,lip->brd_ip);
	for(i = 0;i<NBSR_MAX_LEN; i++,nbsr++)
	{
		if(  (nbsr->valid)
		   && !memcmp(&(nbsr->lip),lip,sizeof(subbrd_t))
		  ) 
		  // &&(nbsr->lip.brd_ip == lip.brd_ip) )
		{
			zhs_printf("%s find nbsr_list[%d]\n",__FUNCTION__,i);
			return i;
		}	
	}
	return -1;
}
/********************************************************
 *find_empty:
 *
 */
static int find_empty_nbsr(void)
{
	int i;
	
	zhs_printf("%s()....\n",__FUNCTION__);	
	for(i = 0;i<NBSR_MAX_LEN; i++)
	{
		if(0 == nbsr_list[i].valid)
		{
			zhs_printf("%s find empty nbsr_list[%d]\n",__FUNCTION__,i);
			return i;
		}
		
	}
	print_err("nbsr list if full,no space!!!!!!!!\n");
	return -1;
}

/*********************************************************
 *add_nbsr:add an nbsrelay_t to nbsr_strart list.if the lip
 *         if found in the list than add the rip to the rip[] 
 *	   of this nbsr,else add the pair to the list.
 *
 *RETURN:
 *       0  --OK
 *      -1  --ERROR
 */
static int add_nbsr(ipmask *lip,ipmask *rip)
{
	int index;
	nbsrelay_t *nbsr;	
	subbrd_t lsub,rsub,*nbsr_sub;
		
	zhs_printf("%s start with:",__FUNCTION__);
	show_nbsr_list();
	ipmask_to_subbrd(lip,&lsub);
	ipmask_to_subbrd(rip,&rsub);

	if((index =find_nbsr(&lsub)) <0)
	{//not the same lip
		index = find_empty_nbsr();
        	if(-1 == index)
		{
			print_err("%s(): find index is %d and nbsr_list is full!!!\n"
				,__FUNCTION__ ,index);
			return -1;
		}
		{
			nbsr = &nbsr_list[index];
			nbsr->valid = 1;
			nbsr_sub = &(nbsr->lip);
			//ipmask_to_subbrd(lip,sub);
			nbsr_sub->brd_ip = lsub.brd_ip;
			nbsr_sub->sub_ip = lsub.sub_ip;
		}
		
	}
	
	nbsr = &nbsr_list[index];
	if(nbsr->rip_cnt < DIP_MAX)
	{
		nbsr_sub = &(nbsr->rip[nbsr->rip_cnt]);
		//ipmask_to_subbrd(rip,sub);
		nbsr_sub->brd_ip = rsub.brd_ip;
		nbsr_sub->sub_ip = rsub.sub_ip;
		nbsr->rip_cnt ++;
		zhs_printf(" %s end with :",__FUNCTION__);
		show_nbsr_list();
		return 0;
	}	
	print_err("%s(): nbsr_list[%d].rip_cnt=%d,and rip is full!!!\n"
		     ,__FUNCTION__ ,index ,nbsr->rip_cnt);
/*	for(i=0;i<DIP_MAX;i++)
	{
		if(0 == nbsr->rip[i].s_addr)
		{
			nbsr->rip[i].s_addr = rip.s_addr;
			return 0;
		}
	}
*/
	return -1;
}
/*
int inet_atou32(const char *na)
{
	struct in_addr addr;
	
	if(inet_aton(na,&addr))
	{
		return addr.s_addr;	
	}
	return 0;
}*/
/**********************************************************
 *get_nbsr_from_file:read the ip pair form file and add to 
 *                   nbsr list.The file stores as:
 *  xxx.xxx.xxx.xxx:xxx.xxx.xxx.xxx.xxx -> xxx.xxx.xxx.xxx:xxx.xxx.xxx.xxx.xxx
 *                                 lip(ip:mask)  ->   rip(ip:mask)
 */

static int get_nbsr_from_file(const char *fn)
{
	FILE *fd;
	char buf[80],*lip,*smsk,*rip,*dmsk;
	ipmask src,drc;

	zhs_printf("%s()....\n",__FUNCTION__);
	if(NULL==(fd = fopen(fn,"r")))
	{
		print_err("fopen file:%s failed!!!\n",fn);
		return -1;
	}
	clear_arr(buf);
	if(fread(buf,1,80,fd) ==0)
	{
		print_err("fread %s failed!!\n",fn);
		return -1;
	}
	/*clear_arr(lip);
	clear_arr(rip);
	clear_arr(smsk);
	clear_arr(dmsk);
	fscanf(fd,"%s:%s:%s:%s",lip,smsk,rip,dmsk);*/
	smsk = buf;
	lip = strsep(&smsk,":");
	rip = smsk;
	smsk = strsep(&rip,":");
	dmsk =rip;
	rip =strsep(&dmsk,":");

	zhs_printf("get lip %s:%s,rip %s:%s from file %s\n",lip,smsk,rip,dmsk,fn);

	inet_aton(lip,&(src.ip));
	inet_aton(smsk,&(src.mask));
	inet_aton(rip,&(drc.ip));
	inet_aton(dmsk,&(drc.mask));
	if(  get_ip(&src) && get_mask(&src)
	   &&get_ip(&drc) /*&& get_mask(&drc)*/)
	{/*lip and rip both valid*/
		return add_nbsr(&src,&drc);	
	}
	print_err("%s: lip %s,rip %s that get form %s are not valid ip!!!\n"
			          ,__FUNCTION__,lip,rip,fn);	
	return -1;
}
/***********************************************************
 *globerr:user for function glob
 */ 
static int globerr(const char *epath, int errno)
{
	print_err("error when glob with %s,errno=%d!!!\n",epath,errno);
	return 1;/*stop glob*/
}

/***********************************************************
 *fill_nbsr_list:read the files that store the netbios broadcast 
 * 	       pair and set to nbsr_list.
 */
static int fill_nbsr_list(void)
{
#if 0 
	nbsrelay_t *nbsr =&nbsr_list[0];
	nbsr->valid =1;
	nbsr->lip.sub_ip = 0x20800a;
	nbsr->lip.brd_ip = 0xff20800a;

	nbsr->rip_cnt =1;
	nbsr->rip[0].sub_ip = 0x1a8c0;
	nbsr->rip[0].brd_ip = 0xff01a8c0;
	
	
#else
	glob_t globbuf;
	char **fnp;
	
	zhs_printf("%s()....\n",__FUNCTION__);
	init_nbsr_list();		
 /* do globbing */
    	{
       	  int r = glob(NBS_PAIR_FL, GLOB_ERR, globerr, &globbuf);
 
      #define loglog(level,fmt,arg...) print_err(fmt,##arg) 
	  if (r != 0)
       	  {
       		switch (r)
          	{
            	case GLOB_NOSPACE:
                	loglog(RC_LOG_SERIOUS, "out of space processing netbios pair filename \"%s\"\n", NBS_PAIR_FL);
                	break;
            	case GLOB_ABORTED:
                	break;  /* already logged */
            	case GLOB_NOMATCH:
                	loglog(RC_LOG_SERIOUS, "no netbios pair filename matched \"%s\"\n", NBS_PAIR_FL );
                	break;
            	default:
                	loglog(RC_LOG_SERIOUS, "unknown glob error %d \n", r);
                	break;
            	}
            	globfree(&globbuf);
            	return -1;
          }
    	}
	for (fnp = globbuf.gl_pathv; *fnp != NULL; fnp++)
	{
		get_nbsr_from_file(*fnp);	
	}
#endif
	return 0;
}	

static int listen_socket(unsigned int ip, int port, char *inf)
{
	struct ifreq interface;
	int fd;
	struct sockaddr_in addr;
	char  n = 1;

	zhs_printf("Opening listen socket on 0x%08x:%d %s\n", ip, port, inf);

	//create socket 
	if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <0) 
	//if ((fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP))) <0) 
	{
		print_err("socket  AF_PACKET call failed\n");
		return -1;
	}
	//set the sock option	
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &n, sizeof(int)) == -1) {
		
		close(fd);
		print_err("bad REUSERADDR!\n");
		return -1;
	}
	//allow broadcast operation
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *) &n, sizeof(int)) == -1) {
		close(fd);
		print_err("bad BROADCAST!\n");
		return -1;
	}
	
	strncpy(interface.ifr_ifrn.ifrn_name, inf, IFNAMSIZ);
	if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE,(char *)&interface, sizeof(interface)) < 0) {
		close(fd);
		print_err("bad BINDTODEVICE!\n");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(ip);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
		print_err("bind error,errno=%d(%s)!\n",errno,strerror(errno));
		close(fd);
		return -1;
	}
	return fd;
}
#if 0
static int create_socket(unsigned int ip, int port/*, char *inf*/)
{
	int fd;
	char  optval = 1;

	print_err("Opening listen socket on 0x%08x:%d \n", ip, port/*, inf*/);

	if ((fd = socket(AF_PACKET, SOCK_PACKET, htons(ETH_P_IP))) < 0) {
		print_err("socket call failed\n");
		return -1;
	}

	if(setsockopt(fd,SOL_SOCKET,SO_BROADCAST,(char *)&optval,sizeof(int)) < 0)
	{
		print_err("setsockopt  BROADCAST error!\n");
		close(fd);
		return -1;
	}

	return fd;
}
#endif
#ifdef ZHS_DEBUG
//xdump
//#define u_char unsigned char
#define isprint(a) ((a >=' ')&&(a<= '~'))
static void xdump( u_char*  cp, int  length, char*  prefix )
{
    int col, count;
    u_char prntBuf[120];
    u_char*  pBuf = prntBuf;
    count = 0;
    while(count < length){
        pBuf += sprintf( pBuf, "%s", prefix );
        for(col = 0;count + col < length && col < 16; col++){
            if (col != 0 && (col % 4) == 0)
                pBuf += sprintf( pBuf, " " );
            pBuf += sprintf( pBuf, "%02X ", cp[count + col] );
        }
        while(col++ < 16){      /* pad end of buffer with blanks */
            if ((col % 4) == 0)
                sprintf( pBuf, " " );
            pBuf += sprintf( pBuf, "   " );
        }
        pBuf += sprintf( pBuf, "  " );
        for(col = 0;count + col < length && col < 16; col++){
            if (isprint((int)cp[count + col]))
                pBuf += sprintf( pBuf, "%c", cp[count + col] );
            else
                pBuf += sprintf( pBuf, "." );
                }
        sprintf( pBuf, "\n" );
        // SPrint(prntBuf);
        printf(prntBuf);
        count += col;
        pBuf = prntBuf;
    }
                                                                                                                             
}  /* close xdump(... */
#else
#define xdump(cp,len,inf)
#endif

static int get_packet(char *udp_pkt, int fd,struct sockaddr_in *from, socklen_t *addr_len)
{
	int bytes;
//	struct sockaddr from;
//	socklen_t addr_len=sizeof(struct sockaddr);
	
//	memset(&from,0,sizeof(struct sockaddr));
//	memset(packet, 0, sizeof(struct nbsMsg));
//	bytes = read(fd, packet, sizeof(struct nbsMsg));
	bytes = recvfrom(fd,udp_pkt,MAX_PKT_SIZE,0,(struct sockaddr *)from,addr_len);
	if (bytes < 0) {
		print_err("couldn't read on listening socket, ignoring.errno=%d(%s)\n",errno,strerror(errno));
		return -1;
	}
	/*if(get_src_ip(udp_pkt) !=from.sin_addr.s_addr)
	{
		from.sin_addr.s_addr = get_src_ip(udp_pkt);			
	}*/
	zhs_printf("get packet(%d bytes)from:%s:%x addr len is %d!\n"
			,bytes,inet_ntoa(from->sin_addr),ntohs(from->sin_port),*addr_len);
	
	xdump((u_char*)udp_pkt,bytes,"getpacket:");
	return bytes;
}
#if 0
/***********************************************
 *is_match_sip:is the sip in the range of nbsr_list local
 *             secure group.
 *             if it is,return the corresponding nbsr,
 *             else return NULL. 
 *	       
 *	       
 */
static nbsrelay_t *  is_match_sip(struct in_addr *sip)
{
	int i;
	nbsrelay_t * nbsr =nbsr_list;
	
	zhs_printf("%s() with sip =%x\n",__FUNCTION__, sip->s_addr);
	show_nbsr_list();
	for(i=0;i<NBSR_MAX_LEN;i++,nbsr++)
	{
		if(  nbsr->valid
			&& is_in_subnet(&(nbsr->lip),  sip->s_addr))
			return nbsr;
			
	} 
	return NULL;
	
}
#endif 
#if 0
/******************************************************
  *ipt_md_sip:modify the source ip of the packet sent to dip  to sip,
  *                 use the IPTABLES rules to implement.
  */
  int ipt_md_sip(u32 sip,u32 dip)
{
	char ipt_buf[300]; 
	char *a_dip,*a_sip;
	
	a_sip = inet_ntoa(sip);
	a_dip = inet_ntoa(dip);
	clear_arr(ipt_buf);
	sprintf(ipt_buf,"iptables -t nat -I POSTROUTING -p udp -d %s --dport 137 "
		"-j SNAT --to-source %s:137\n",a_dip,a_sip);
}
#endif
/**********************************************************
 *in_cksum:caculate the checksum
 */
static int in_cksum(u16 *addr,int len)
{
        register int sum = 0;
        register u16  *w = addr;
        register int nleft = len;
        while ( nleft > 1 )
        {
                sum += *w++;
                nleft -= 2;
        }
        if ( nleft == 1 )
        {
                u_char a = 0;
                memcpy(&a,w,1);
                sum += a;
         }
        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        return ~sum;
}
/****************************************************************
 *create_send_sock: create a IP RAW sock for send ip packet.
 */
static int create_send_sock(void)
{
	const char optval =1;
	struct timeval tv;
	int send_sk;
	
	zhs_printf("%s() start ...\n",__FUNCTION__);
	if ((send_sk = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) <0)  
	{
		print_err("socket call failed\n");
		return -1;
	}
	if(setsockopt(send_sk,SOL_SOCKET,SO_BROADCAST,&optval,sizeof(int)) < 0)
	{
		print_err("setsockopt SO_BROADCAST error!\n");
		close(send_sk);
		return -1;
	}
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	if(setsockopt(send_sk,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(struct timeval)) < 0)
	{
		print_err("setsockopt SO_SNDTIMEO error!\n");
		close(send_sk);
		return -1;
	}
	if(setsockopt(send_sk,IPPROTO_IP,IP_HDRINCL,&optval,sizeof(int)) < 0)
	{
		print_err("setsockopt IP_HDRINCL error!\n");
		close(send_sk);
		return -1;
	}
	zhs_printf("%s() return sock %d\n",__FUNCTION__,send_sk);
	return send_sk;
}
/******************************************************
 *
 */
static int nbsRelay(char  *udp_pkt,int len,struct sockaddr_in *from)
{
	struct sockaddr_in to;
	//socklen_t addr_len = sizeof(struct sockaddr);
	int send_sk;
	int i;
	udppkt *udp;
	char packet[MAX_PKT_SIZE];
	u32 sip = from->sin_addr.s_addr;
	int ip_len,udp_len;	
	nbsrelay_t *nbsr = nbsr_list;
	int j;

	zhs_printf("%s with sip=%x(%s):%x,len=%d\n",
		__FUNCTION__,sip,inet_ntoa(from->sin_addr),from->sin_port,len);
	xdump(udp_pkt,len,"udp pkt:");
	
	if(-1 ==(send_sk = create_send_sock()))
	{
		print_err("creat send socket failed!!!\n");
		return -1;
	}
	memset(&to,0,sizeof(struct sockaddr_in));
	to.sin_family = AF_INET;
	to.sin_port = htons(SERVER_PORT);

	/*creat the ip header and udp header. 
	 *the field the value is 0,set by the kernel 
	 */
	clear_arr(packet);	
//	strncpy(packet+sizeof(udppkt),udp_pkt,len);
	memcpy(packet+sizeof(udppkt),udp_pkt,len);
	udp = (udppkt *)packet;
	udp_len = len + UDPHDR_LEN;
	zhs_printf("udp_len =%d\n",len+UDPHDR_LEN); 
	udp->udp.len = htons(udp_len) ;
	udp->udp.source = from->sin_port;
	udp->udp.dest = to.sin_port;
	udp->udp.check = in_cksum((u16 *)(&(udp->udp)),udp_len);

	udp->ip.version = IPVERSION;
	udp->ip.ihl = 5;
	/*ip->tos =0;*/
	ip_len = len + UDPHDR_LEN +IPHDR_LEN;
	zhs_printf("ip_len=%d\n",ip_len);
	udp->ip.tot_len = htons(ip_len);
	/*ip->id=0;
	    ip->frag_off =0*/
	udp->ip.ttl = MAXTTL;
	udp->ip.protocol = IPPROTO_UDP;
	/*ip->checksum =0*/
	udp->ip.saddr = sip;
	for(j=0;j<NBSR_MAX_LEN;j++,nbsr++)
	{
		if((  nbsr->valid
		  && is_in_subnet(&(nbsr->lip),  sip)))
		{
			
			zhs_printf("find nbsr :\n");
			show_nbsr(nbsr);

			for(i=0;i< nbsr->rip_cnt;i++)
			{
		
				/*if(0 == inet_aton(dip,&to.sin_addr))
				{
					print_err("%s is a invalid IP address\n",dip);
					return -1;
				}*/
				u32 daddr = nbsr->rip[i].brd_ip;
				to.sin_addr.s_addr = daddr;
				udp->ip.daddr = daddr;
				zhs_printf("will sendto %8x(%s) %d bytes... \n",
					to.sin_addr.s_addr,inet_ntoa(to.sin_addr),ip_len);
				xdump((u_char*)packet,ip_len,"send data:");
				if(sendto(send_sk, packet,ip_len, 0,(struct sockaddr *)&to,sizeof(struct sockaddr))<0)
				{
					print_err("sendto %x(%s) error,errno=%d(%s)!!\n"
						,to.sin_addr,inet_ntoa(to.sin_addr),errno,strerror(errno));
					//close(send_sk);
					continue;
					/*return -1;*/
			
				}
			}
		}
	}
	close(send_sk);
	return 0;
}
#ifdef PC_TEST
char *path_netbios_pid ="./netb.pid";
#else
char *path_netbios_pid = "/tmp/netb.pid";
#endif
/****************************************
  *get_src_ip:get the source ip from ip packet
  */
#define  get_src_ip(ip) ((struct iphdr *)(ip))->daddr 

int main(void)
{
	int udp_sck = -1;
//	int log_perror;
	//fd_set rfds,wfds;
	//int max_sock,retval;
	//struct timeval tv;
	//struct udp_nbs_pkt packet;	
	//struct nbsMsg packet;	
	char udp_pkt[MAX_PKT_SIZE];

	/* Become a daemon... */
	{
		int pid;
		FILE *pf;
		int pfdesc;

//		log_perror = 0;
		if ((pid = fork()) < 0)
			print_err ("can't fork daemon");
		else if (pid)
			exit (0);
		zhs_printf("open %s\n",path_netbios_pid);
		pfdesc = open (path_netbios_pid,
			       O_CREAT | O_TRUNC | O_WRONLY, 0644);

		if (pfdesc < 0) {
			print_err("Can't create %s", path_netbios_pid);
		} else {
			pf = fdopen (pfdesc, "w");
			if (!pf)
				print_err("Can't fdopen %s",path_netbios_pid);
			else {
				fprintf (pf, "%ld\n", (long)getpid ());
				fclose (pf);
			}	
		}

		//close (0);
		//close (1);
		//close (2);
		pid = setsid ();
		fill_nbsr_list();

		while(1)
		{
			if(udp_sck < 0)
			{
			#ifdef PC_TEST
				if((udp_sck = listen_socket(INADDR_ANY,SERVER_PORT,"eth0")) < 0)
			#else
				if((udp_sck = listen_socket(INADDR_ANY,SERVER_PORT,"br0")) < 0)
			#endif
				{
					print_err("Cannot create server socket!\n");
					return -1;
				}	
			}
#if 0	
			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			FD_SET(udp_sck,&rfds);

			max_sock = udp_sck + 1;

			tv.tv_sec = 5;
			tv.tv_usec = 0;	

			//retval = select(max_sock,&rfds,NULL,NULL,&tv);
			retval = select(max_sock,&rfds,&wfds,NULL,&tv);

			if(retval < 0)
				print_err("select error!\n");

#endif			
//			if(FD_ISSET(udp_sck,&rfds))
			{
				int bytes;
				//unsigned char *ptr;
				struct sockaddr_in from;
				socklen_t addr_len;
				//nbsrelay_t *nbsr;

				memset(&from,0,sizeof(struct sockaddr_in));
				//from.sin_addr.s_addr =0xa920800a;
				//from.sin_port =0x89;
				clear_arr(udp_pkt);
				if((bytes = get_packet(udp_pkt,udp_sck,&from,&addr_len)) < 0)
				{
					print_err("get packet error!\n");
					close(udp_sck);
					udp_sck = -1;
					continue;
				}
				nbsRelay(udp_pkt,bytes,&from);
					continue;
				nbsRelay(udp_pkt,bytes,&from);
			}
		}
	}
	return 0;
}
