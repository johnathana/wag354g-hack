

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#ifdef _PRECISION_SIOCGSTAMP
#include <sys/ioctl.h>
#endif
#include <fcntl.h>
#define ENABLE_DEBUG

extern char *optarg;
unsigned long g_timestamp = 0;
#ifdef linux
#include <sys/utsname.h>
#include <sys/time.h>
typedef u_int32_t __u32;
#include <sys/timex.h>
#else
#define main ntpclient
extern struct hostent *gethostbyname(const char *name);
extern int h_errno;
#define herror(hostname) \
	fprintf(stderr,"Error %d looking up hostname %s\n", h_errno,hostname)
typedef uint32_t __u32;
#endif

#define JAN_1970        0x83aa7e90      /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT (123)

#define  DNS_NTP      "time.domain.net"
/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x) ( 4294*(x) + ( (1981*(x))>>11 ) )

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x) ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x) ( (x) * 15.2587890625 )

struct ntptime {
	unsigned int coarse;
	unsigned int fine;
};

/* server ip enbale/disable table   kirby 2004/07/21*/
#define   MAX_SERVER    4
#define   SV_ENABLE          1
#define   SV_DISABLE         0
struct SIPTABLE{
        struct in_addr  ip;
	char                 enable;	
};
struct SIPTABLE  ntp_server[MAX_SERVER];
struct in_addr  cur_sip;            /* current server ip kirby 2004/07/21 */

void send_packet(int usd);
int rfc1305print(char *data, struct ntptime *arrival);
/* kirby 2004/07/21 change function proto */
int udp_handle(int usd, char *data, int data_len, struct sockaddr *sa_source, int sa_len);

/* global variables (I know, bad form, but this is a short program) */
char incoming[1500];
struct timeval time_of_send;
int live=0;
int set_clock=0;   /* non-zero presumably needs root privs */

#ifdef ENABLE_DEBUG
int debug=0;
#define DEBUG_OPTION "d"
#else
#define debug 0
#define DEBUG_OPTION
#endif


int contemplate_data(unsigned int absolute, double skew, double errorbar, int freq);

/* set server enbale kirby 2004/07/21*/
int  set_server_enbale(struct in_addr addr,char enbale)
{
        int i;

	for(i=0;i<MAX_SERVER;i++){
		if(ntp_server[i].ip.s_addr == addr.s_addr){
			ntp_server[i].enable = enbale;
			return 1;
		}
	}

	return -1;
}

/* check server enbale kirby 2004/07/21 */
int check_server_enbale(struct in_addr addr)
{
        int i;

	for(i=0;i<MAX_SERVER;i++){
		if(ntp_server[i].ip.s_addr == addr.s_addr){
			return ntp_server[i].enable;
		}
	}

	return -1;
}
/* set valid server ip to table  kirby 2004/07/21*/
/* return   1     put server into table ok */
/*             0     server already in table  */
/*             -2   server can be permit    */
/*             -1   server table full            */
int set_server_table(struct in_addr addr)
{
         int i;
	 int ret;	 

         /* serach old  if find it return*/
	 ret = check_server_enbale(addr);
	 if(ret == SV_DISABLE)
	 	return -2;
         if(ret > 0){
		return 0;  	
         }
	 for(i=0;i<MAX_SERVER;i++){
		if(ntp_server[i].ip.s_addr== 0){
			ntp_server[i].ip.s_addr = addr.s_addr;
			ntp_server[i].enable = SV_ENABLE;
			return 1;
		}
	 }	 

	 return -1;
}

int get_current_freq()
{
	/* OS dependent routine to get the current value of clock frequency.
	 */
#ifdef linux
	struct timex txc;
	txc.modes=0;
	if (__adjtimex(&txc) < 0) {
		perror("adjtimex"); exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

int set_freq(int new_freq)
{
	/* OS dependent routine to set a new value of clock frequency.
	 */
#ifdef linux
	struct timex txc;
	txc.modes = ADJ_FREQUENCY;
	txc.freq = new_freq;
	if (__adjtimex(&txc) < 0) {
		perror("adjtimex"); exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

void send_packet(int usd)
{
	__u32 data[12];
	struct timeval now;
#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4 
#define PREC -6

	if (debug) fprintf(stderr,"Sending ...\n");
	if (sizeof(data) != 48) {
		fprintf(stderr,"size error\n");
		return;
	}
	bzero((char*)data,sizeof(data));
	data[0] = htonl (
		( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |
		( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );
#if 0
	data[1] = htonl(1<<16);  /* Root Delay (seconds) */
	data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */
	gettimeofday(&now,NULL);
	data[10] = htonl(now.tv_sec + JAN_1970); /* Transmit Timestamp coarse */
	data[11] = htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine   */
#endif
	send(usd,data,48,0);
        //fprintf(stderr,"send ntp packet ok!\n");
	time_of_send=now;
}


int udp_handle(int usd, char *data, int data_len, struct sockaddr *sa_source, int sa_len)
{
	struct timeval udp_arrival;
	struct ntptime udp_arrival_ntp;
	int ret;

#ifdef _PRECISION_SIOCGSTAMP
	if ( ioctl(usd, SIOCGSTAMP, &udp_arrival) < 0 ) {
		perror("ioctl-SIOCGSTAMP");
		gettimeofday(&udp_arrival,NULL);
	}
#else
	gettimeofday(&udp_arrival,NULL);
#endif
	udp_arrival_ntp.coarse = udp_arrival.tv_sec + JAN_1970;
	udp_arrival_ntp.fine   = NTPFRAC(udp_arrival.tv_usec);

	if (debug) {
		struct sockaddr_in *sa_in=(struct sockaddr_in *)sa_source;
		printf("packet of length %d received\n",data_len);
		if (sa_source->sa_family==AF_INET) {
			printf("Source: INET Port %d host %s\n",
				ntohs(sa_in->sin_port),inet_ntoa(sa_in->sin_addr));
		} else {
			printf("Source: Address family %d\n",sa_source->sa_family);
		}
	}
        /* check package length */
	if(data_len < 48){
		printf("The package length error\n");
	}
	
	/* check packge kirby 2004/07/21 */
	ret = rfc1305print(data,&udp_arrival_ntp);

	return ret;	
}

double ntpdiff( struct ntptime *start, struct ntptime *stop)
{
	int a;
	unsigned int b;
	a = stop->coarse - start->coarse;
	if (stop->fine >= start->fine) {
		b = stop->fine - start->fine;
	} else {
		b = start->fine - stop->fine;
		b = ~b;
		a -= 1;
	}
	
	return a*1.e6 + b * (1.e6/4294967296.0);
}

//#define SMTP_DEV "/dev/smtp_server_dev"
int rfc1305print(char *data, struct ntptime *arrival)
{
/* straight out of RFC-1305 Appendix A */
	int li, vn, mode, stratum, poll, prec;
	int delay, disp, refid;
	struct ntptime reftime, orgtime, rectime, xmttime;
	double etime,stime,skew1,skew2;
	int freq;
	time_t now;

#define Data(i) ntohl(((unsigned int *)data)[i])
	li      = Data(0) >> 30 & 0x03;
	vn      = Data(0) >> 27 & 0x07;
	mode    = Data(0) >> 24 & 0x07;
	stratum = Data(0) >> 16 & 0xff;
	poll    = Data(0) >>  8 & 0xff;
	prec    = Data(0)       & 0xff;
	if (prec & 0x80) prec|=0xffffff00;
	delay   = Data(1);
	disp    = Data(2);
	refid   = Data(3);
	reftime.coarse = Data(4);
	reftime.fine   = Data(5);
	orgtime.coarse = Data(6);
	orgtime.fine   = Data(7);
	rectime.coarse = Data(8);
	rectime.fine   = Data(9);
	xmttime.coarse = Data(10);
	xmttime.fine   = Data(11);
#undef Data
        /* check  stratum kirby 2004/07/21*/
	
        if(stratum == 0){
		set_server_enbale(cur_sip,SV_DISABLE);
		fprintf(stderr,"The server send package Stratum = 0\n");	
		return -1;	
        }		
	
	if (set_clock) {   /* you'd better be root, or ntpclient will crash! */
		struct timeval tv_set;
		if (mode != 4)
		{
			fprintf(stderr,"ntp response err ntp mode =%d !\n",mode);
			exit(1);
		}
         //  fprintf(stderr,"ntpc ntp mode = %d\n",mode);  
		/* it would be even better to subtract half the slop */
		tv_set.tv_sec  = xmttime.coarse - JAN_1970;
		/* divide xmttime.fine by 4294.967296 */
		tv_set.tv_usec = USEC(xmttime.fine);
		g_timestamp = xmttime.coarse;
		if (settimeofday(&tv_set,NULL)<0) {
			perror("settimeofday");
			exit(1);
		}
		if (debug) {
			fprintf(stderr,"ntpc set time to %lu.%.6lu\n", tv_set.tv_sec, tv_set.tv_usec);
			time(&now);
			fprintf(stderr,"ntpc setime is:%s\n",ctime(&now));
		}
	}

	if (debug) {
	fprintf(stderr,"LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d\n",
		li, vn, mode, stratum, poll, prec);
	fprintf(stderr,"Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u\n",
		sec2u(delay),sec2u(disp),
		refid>>24&0xff, refid>>16&0xff, refid>>8&0xff, refid&0xff);
	fprintf(stderr,"Reference %u.%.10u\n", reftime.coarse, reftime.fine);
	fprintf(stderr,"Originate %u.%.10u\n", orgtime.coarse, orgtime.fine);
	fprintf(stderr,"Receive   %u.%.10u\n", rectime.coarse, rectime.fine);
	fprintf(stderr,"Transmit  %u.%.10u\n", xmttime.coarse, xmttime.fine);
	fprintf(stderr,"Our recv  %u.%.10u\n", arrival->coarse, arrival->fine);
	}
	etime=ntpdiff(&orgtime,arrival);
	stime=ntpdiff(&rectime,&xmttime);
	skew1=ntpdiff(&orgtime,&rectime);
	skew2=ntpdiff(&xmttime,arrival);
	freq=get_current_freq();
	if (debug) {
	fprintf(stderr,"Total elapsed: %9.2f\n"
	       "Server stall:  %9.2f\n"
	       "Slop:          %9.2f\n",
		etime, stime, etime-stime);
	fprintf(stderr,"Skew:          %9.2f\n"
	       "Frequency:     %9d\n"
	       " day   second     elapsed    stall     skew  dispersion  freq\n",
		(skew1-skew2)/2, freq);
	}
	if (debug) {
	fprintf(stderr,"%d %5d.%.3d  %8.1f %8.1f  %8.1f %8.1f %9d\n",
		arrival->coarse/86400+15020, arrival->coarse%86400,
		arrival->fine/4294967, etime, stime,
		(skew1-skew2)/2, sec2u(disp), freq);
	fflush(stdout);
	}
/*	if (live) {
		int new_freq;
		new_freq = contemplate_data(arrival->coarse, (skew1-skew2)/2,
			etime+sec2u(disp), freq);
		if (!debug && new_freq != freq) set_freq(new_freq);
	}*/

        return 1;
}

int stuff_net_addr(struct in_addr *p, char *hostname)
{
	struct hostent *ntpserver;
        int ret;
	
	ntpserver=gethostbyname(hostname);
	if (ntpserver == NULL) {
		herror(hostname);
//		exit(1);
		return -1;
	}
	if (ntpserver->h_length != 4) {
		fprintf(stderr,"oops %d\n",ntpserver->h_length);
		return -1;
	}
	memcpy(&(p->s_addr),ntpserver->h_addr_list[0],4);
	/* get current server ip address  kirby 2004/07/21*/
	memcpy(&(cur_sip.s_addr),ntpserver->h_addr_list[0],4);
	ret = set_server_table(cur_sip);
	if(ret == -1)
	      fprintf(stderr,"Server Table Full\n");	
	else if(ret == -2)
	      fprintf(stderr,"Server must be permit\n");
	else if(ret ==0)
	      fprintf(stderr,"Server already in table\n");
	else
	     fprintf(stderr,"Server put in table ok!\n");	
	return 0;
}

void setup_receive(int usd, unsigned int interface, short port)
{
	struct sockaddr_in sa_rcvr;
	bzero((char *) &sa_rcvr, sizeof(sa_rcvr));
	sa_rcvr.sin_family=AF_INET;
	sa_rcvr.sin_addr.s_addr=htonl(interface);
	sa_rcvr.sin_port=htons(port);
	if(bind(usd,(struct sockaddr *) &sa_rcvr,sizeof(sa_rcvr)) == -1) {
		fprintf(stderr,"ntp could not bind to udp port %d\n",port);
		perror("bind");
              
		exit(1);
	}
	listen(usd,3);
}

int setup_transmit(int usd, char *host, short port)
{
	struct sockaddr_in sa_dest;
	bzero((char *) &sa_dest, sizeof(sa_dest));
	sa_dest.sin_family=AF_INET;
	if(stuff_net_addr(&(sa_dest.sin_addr),host) < 0)
		return -1;
	sa_dest.sin_port=htons(port);
	if (connect(usd,(struct sockaddr *)&sa_dest,sizeof(sa_dest))==-1)
		{perror("connect");fprintf(stderr,"ntp conn err!\n");return -1;}
	return 1;
}

int primary_loop(int usd, int num_probes, int cycle_time)
{
	fd_set fds;
	struct sockaddr_in sa_xmit;
	int i, pack_len, sa_xmit_len, probes_sent;
	struct timeval to;
	int ret;

	if (debug) printf("Listening...\n");

	probes_sent=0;
	sa_xmit_len=sizeof(sa_xmit);
	//to.tv_sec=0;
	//to.tv_usec=0;
	/* kirby 2004/07/26 */
	to.tv_sec=0;
	to.tv_usec=0;
	
	for (;;) {
		FD_ZERO(&fds);
		FD_SET(usd,&fds);
		i=select(usd+1,&fds,NULL,NULL,&to);  /* Wait on read or error */
		if ((i!=1)||(!FD_ISSET(usd,&fds))) {
			if (i==EINTR) continue;
			if (i<0) perror("select");
			if ((to.tv_sec == 0) || (to.tv_sec == cycle_time)) {
				if (probes_sent >= (num_probes + 1) &&
					num_probes != 0) break;
				/* only server enbale , the client can be permit send request */
				/* kirby 2004/07/21 */
				if(check_server_enbale(cur_sip) == SV_DISABLE){
					printf("Current Server Down!Do not send request package\n ");
					break;
				}
				send_packet(usd);
				++probes_sent;
				to.tv_sec=cycle_time;
				to.tv_usec=0;
			}	
			continue;
		}
		pack_len=recvfrom(usd,incoming,sizeof(incoming),0,
		                  (struct sockaddr_in*)(&sa_xmit),&sa_xmit_len);
		
		if (pack_len<0) {
			perror("recvfrom");
		} else if (pack_len>0 && pack_len<sizeof(incoming)){
		        /* The device SHOULD ignore time protocol response packets
		             with a source IP address other than that of the time server that
		             the modem queried. kirby 2004/07/21*/
		        fprintf(stderr,"Current server ip address is %s\n",inet_ntoa(cur_sip));
			fprintf(stderr,"Source server ip address is %s\n",inet_ntoa(sa_xmit.sin_addr));	
                        if(cur_sip.s_addr != sa_xmit.sin_addr.s_addr){
				probes_sent --;     /* probe again */
				continue;
                        }     	
		        /* check valid package kirby 2004/07/21*/
			ret = udp_handle(usd,incoming,pack_len,&sa_xmit,sa_xmit_len);
			if(ret > 0)	{   /* get time ok */
			     fprintf(stderr,"exit udp handle! server ip %s\n",inet_ntoa(cur_sip));	
			     return 0;
			}
		} else {
			printf("Ooops.  pack_len=%d\n",pack_len);
			fflush(stdout);
		}
		if (probes_sent >= num_probes && num_probes != 0) break;
	}
	return -1;
}

void do_replay(void)
{
	char line[100];
	int n, day, freq, absolute;
	float sec, etime, stime, disp;
	double skew, errorbar;
	int simulated_freq = 0;
	unsigned int last_fake_time = 0;
	double fake_delta_time = 0.0;

	while (fgets(line,sizeof(line),stdin)) {
		n=sscanf(line,"%d %f %f %f %lf %f %d",
			&day, &sec, &etime, &stime, &skew, &disp, &freq);
		if (n==7) {
			fputs(line,stdout);
			absolute=(day-15020)*86400+(int)sec;
			errorbar=etime+disp;
			if (debug) printf("contemplate %u %.1f %.1f %d\n",
				absolute,skew,errorbar,freq);
			if (last_fake_time==0) simulated_freq=freq;
			fake_delta_time += (absolute-last_fake_time)*((double)(freq-simulated_freq))/65536;
			if (debug) printf("fake %f %d \n", fake_delta_time, simulated_freq);
			skew += fake_delta_time;
			freq = simulated_freq;
			last_fake_time=absolute;
			simulated_freq = contemplate_data(absolute, skew, errorbar, freq);
		} else {
			fprintf(stderr,"Replay input error\n");
			exit(2);
		}
	}
}

void usage(char *argv0)
{
	fprintf(stderr,
	"Usage: %s [-c count] [-d] -h hostname [-i interval] [-l]\n"
	"\t[-p port] [-r] [-s] \n",
	argv0);
}

/* Copy each token in wordlist delimited by space into word */
#define foreach(word, wordlist, next) \
	for (next = &wordlist[strspn(wordlist, " ")], \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '); \
	     strlen(word); \
	     next = next ? &next[strspn(next, " ")] : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '))

unsigned long main(int argc, char *argv[]) {
	int usd;  /* socket */
	int c;
	/* These parameters are settable from the command line
	   the initializations here provide default behavior */
	short int udp_local_port=0;   /* default of 0 means kernel chooses */
	int cycle_time=2;          /* request timeout in seconds */
	int probe_count=0;            /* default of 0 means loop forever */
	/* int debug=0; is a global above */
	char *hostname=NULL;          /* must be set */
	int replay=0;                 /* replay mode overrides everything */
	char ntps[32], *next;
	int res=1; //barry add 20031009 BCM
	int limit;   // kirby 2004/07/21
	
	for (;;) {
		c = getopt( argc, argv, "c:" DEBUG_OPTION "h:i:p:lrs");
		if (c == EOF) break;
		switch (c) {
			case 'c':
				probe_count = atoi(optarg);
				break;
#ifdef ENABLE_DEBUG
			case 'd':
				++debug;
				break;
#endif
			case 'h':
				hostname = optarg;
				break;
			case 'i':
				cycle_time = atoi(optarg);
				break;
			case 'l':
				live++;
				break;
			case 'p':
				udp_local_port = atoi(optarg);
				break;
			case 'r':
				replay++;
				break;
			case 's':
				set_clock = 1;
				/* set search limit is 3  kirby 2004/07/21 */
				probe_count = 4;
				break;
			default:
				usage(argv[0]);
				exit(1);
		}
	}

	if (replay) {
		do_replay();
		exit(0);
	}
	if (hostname == NULL) {
		usage(argv[0]);
		exit(1);
	}
	if (debug) {
		printf("Configuration:\n"
		"  -c probe_count %d\n"
		"  -d (debug)     %d\n"
		"  -h hostname    %s\n"
		"  -i interval    %d\n"
		"  -l live        %d\n"
		"  -p local_port  %d\n"
		"  -s set_clock   %d\n",
		probe_count, debug, hostname, cycle_time,
		live, udp_local_port, set_clock);
	}

/* The time server discovery and selection stage used by the time client SHOULD 
     check each candidate time server in a round robin fashion 

     kirby 2004/07/21
*/
        /* init server table */
        memset(ntp_server,0,sizeof(ntp_server));
	cur_sip.s_addr = 0;
        for(limit = 0;limit < probe_count;limit++){
	       foreach(ntps, hostname, next) {
                   
		       /* Startup sequence */
		       if ((usd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1) {
		 	    perror ("socket");
			    exit(1);
		       }

		       setup_receive(usd, INADDR_ANY, udp_local_port);

		       if(setup_transmit(usd, ntps, NTP_PORT) < 0)
			       goto next_server;

		       /* each server discover 1 time for this cycle_time */			   
		       if (!primary_loop(usd, 1, cycle_time)) {
			      close(usd);
                              fprintf(stderr,"Host %s :ntp get time ok,timeout = %d\n",ntps,cycle_time);
			      res=0; //barry add 20031009 BCM
			      break;
		       }
		       else{
			      res=1; //barry add 20031009 BCM
			      fprintf(stderr,"Host %s: timeout,timeout = %d\n",ntps,cycle_time);
		       }
next_server:
		       close(usd);
	       }
	       /* if any server does not response cycle time double */
	       /* kirby 2004/07/21 */	  
	       if(res == 0)
		   	break;
	       cycle_time *= cycle_time;	   
        }

        /* support DNS select time server */
	if(res){
		fprintf(stderr,"Get ntp server from DNS\n");
		cycle_time = 2;
		for(limit = 0;limit < probe_count;limit++){
		     if ((usd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1) {
		 	    perror ("socket");
			    exit(1);
		     }

		     setup_receive(usd, INADDR_ANY, udp_local_port);
		     setup_transmit(usd,DNS_NTP,NTP_PORT);

		     /* each server discover 1 time for this cycle_time */			   
		     if (!primary_loop(usd, 1, cycle_time)) {
		          close(usd);
                          fprintf(stderr,"Host %s :ntp get time ok,timeout = %d\n",ntps,cycle_time);
		          res=0; //barry add 20031009 BCM
		          break;
		    }
	    else{
		          res=1; //barry add 20031009 BCM
		          fprintf(stderr,"Host %s: timeout,timeout = %d\n",ntps,cycle_time);
		    }
		    close(usd);

		    cycle_time *= cycle_time;	
		}
	}
	
	return res;
}
