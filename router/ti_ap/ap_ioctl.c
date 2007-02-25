/*
 * WCFG         An implementation of the wcfglib library.
 *		See wcfg_lib companion document.
 *
 *
 * Version:     0.1.0
 *
 * Authors:     Alexandra Aronov, <alexandraa@ti.com>
 *              Felix Rubinstein, <felixr@ti.com>
 *
 */



#include "iwlib.h"
#include "dda_wext_ioctl.h"
#include "dda_wpaauth_ioctl.h"
//#include "cm_logger.h"

#define IW_MAX_PRIV_DEF	128

#define min(x,y) ((x)<(y) ? (x) : (y))
/*------------------------------------------------------------------*/
/*
 * Macro to handle errors when setting WE
 * Print a nice error message and exit...
 * We define them as macro so that "return" do the right thing.
 * The "do {...} while(0)" is a standard trick
 */




#define ERR_SET_EXT(rname, request) \
	fprintf(stderr, "Error for wireless request \"%s\" (%X) :\n", \
		rname, request)

#define ABORT_ARG_SIZE(rname, request, max) \
	do { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    argument too big (max %d)\n", max); \
		return(-3); \
	} while(0)

#define ABORT_NULL_ARG(rname, request) \
	do { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    argument is NULL\n"); \
		return(-3); \
	} while(0)

/*------------------------------------------------------------------*/
/*
 * Wrapper to push some Wireless Parameter in the driver
 * Use standard wrapper and add pretty error message if fail...
 */
#define IW_SET_EXT_ERR(ioctl_sock, ifname, request, wrq, rname) \
	do { \
	int ret; \
	if ((ret = iw_set_ext(ioctl_sock, ifname, request, wrq))) { \
		ERR_SET_EXT(rname, request); \
		fprintf(stderr, "    SET failed on device %-1.16s ; %s.\n", \
			ifname, strerror(errno)); \
		return ret; \
	} } while(0)


static int generic_ioctl_get_buf(char * buf, int buf_len, int cmd, char * ifname, int param1, int param2, char * mac);
static int read_priv_ioctls(int ioctl_sock,char *ifname,iwprivargs *priv);
int ioctl_set_hwaddr(const __u8 *hwaddr, int ioctl_sock, const char *ifname);
int ioctl_get_hwaddr(const char *ifname, __u8 *hwaddr);
int ioctl_set_essid(const char *essid, int ioctl_sock, char *ifname);
int ioctl_set_rts(const __s32 *rts, int ioctl_sock, char *ifname);
int ioctl_set_frag(const __s32 *frag, int ioctl_sock, char *ifname);
int ioctl_set_retry(const __s32 *retry, const int type, int ioctl_sock, char *ifname);
int ioctl_set_txpow(tiap_param_txpower_t *txpow, int ioctl_sock, char *ifname);



int ioctl_set_hwaddr(const __u8 *hwaddr, int ioctl_sock, const char *ifname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	memcpy(ifr.ifr_hwaddr.sa_data, hwaddr, ETH_ALEN);
		
	return ioctl(ioctl_sock, SIOCSIFHWADDR, &ifr);
}

int ioctl_get_hwaddr(const char *ifname, __u8 *hwaddr)
{
	int s;
	struct ifreq ifr;

	if( (s =  socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) {
		fprintf(stderr, "%d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

	if( ioctl(s, SIOCGIFHWADDR, &ifr) < 0) {
		fprintf(stderr, "%d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	close(s);

	return 0;
}


int ioctl_set_essid(const char *essid, int ioctl_sock, char *ifname)
{

	/* Check the size of what the user passed us to avoid
	 * buffer overflows */
	if (essid)
	{
		if(strlen(essid) > IW_ESSID_MAX_SIZE)
		  ABORT_ARG_SIZE(__func__, SIOCSIWESSID, IW_ESSID_MAX_SIZE);
		else
		{
			struct iwreq		wrq;

			wrq.u.essid.flags = 1;
			wrq.u.essid.pointer = (caddr_t) essid;
			wrq.u.essid.length = strlen(essid) + 1;
			
			IW_SET_EXT_ERR(ioctl_sock, ifname, SIOCSIWESSID, &wrq,__func__);
		}
	}
	else ABORT_NULL_ARG("Set ESSID", SIOCSIWESSID);
	
	return 0;

}

static int generic_ioctl_get_buf(char * buf, int buf_len, int cmd, char * ifname, int param1, int param2, char * mac) {
	int ioctl_sock, ret = 0;
	generic_ioctl_buf_t data;
	struct ifreq ifr;

	if ((ioctl_sock = iw_sockets_open()) < 0) {
//		logger(module, -1, LOG_DEBUG, "ERROR %s", __FUNCTION__);
		perror(__FUNCTION__);
		ret = -1;
		goto exit;
	}

	data.buf = buf;
	data.buf_len = buf_len;
	data.cmd = cmd;
	data.param1 = param1;
	data.param2 = param2;
	if (mac != NULL)
		memcpy(data.mac, mac, 6);

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_data = (char *)&data;

	ret = ioctl(ioctl_sock, SIOCDEVPRIVATE, &ifr);	

 exit:
	close(ioctl_sock);

	return ret;
}


int ioctl_get_tx_info(int Qid, int QprintEn, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_GET_TX_INFO, ifname, Qid, QprintEn, NULL);
}

int ioctl_get_tx_queue(int Qid, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_GET_TX_QUEUE, ifname, Qid, 0, NULL);
}

int ioctl_get_challenge(int index, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_TST_CHALLENGE, ifname, index, 0, NULL);
}

int ioctl_get_find_sta(char * mac, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_TST_FINDSTA, ifname, 0, 0, mac);
}

int ioctl_get_insert_sta(char * mac, int wone_index, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_TST_FIND_INS, ifname, wone_index, 0, mac);
}

int ioctl_get_eer(int addr, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_GET_EER, ifname, addr, 0, NULL);
}
 
int ioctl_get_mac_reg_read(int addr, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_GET_MAC_REGREAD, ifname, addr, 0, NULL);
}

int ioctl_get_phy_reg_read(int addr, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_GET_PHY_REGREAD, ifname, addr, 0, NULL);
}

int ioctl_get_trace_print(int num, int from, char * buf, int buf_len, char * ifname) {
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_TRACE_PRINT, ifname, num, from, NULL);
}

int ioctl_get_wlan_params(int wone_index, char *buf, int buf_len, char *ifname){
	return generic_ioctl_get_buf(buf, buf_len, TIAP_GENERIC_IOCTL_WLAN_PARAMS, ifname, wone_index, 0, NULL);
}

int ioctl_set_rts(const __s32 *rts, int ioctl_sock, char *ifname)
{
	 
	struct iwreq		wrq;
	  
	if (rts)
	{
		wrq.u.rts.value = *rts;			
		IW_SET_EXT_ERR(ioctl_sock, ifname, SIOCSIWRTS, &wrq, __func__);
	}
	else ABORT_NULL_ARG("Set RTS", SIOCSIWRTS);

	return 0;

}

int ioctl_set_frag(const __s32 *frag, int ioctl_sock, char *ifname)
{
	 
	struct iwreq		wrq;

	if (frag)
	{
		wrq.u.frag.value = *frag;
		IW_SET_EXT_ERR(ioctl_sock, ifname, SIOCSIWFRAG, &wrq, __func__);

	}
	else ABORT_NULL_ARG("Set FRAG", SIOCSIWFRAG);

	return 0;

}

int ioctl_set_retry(const __s32 *retry, const int type, int ioctl_sock, char *ifname)
{
	struct iwreq		wrq;

	if (retry)
	{
		wrq.u.retry.value = *retry;
		wrq.u.retry.flags = type;
		IW_SET_EXT_ERR(ioctl_sock, ifname, SIOCSIWRETRY, &wrq, __func__);

	}
	else ABORT_NULL_ARG(__func__, SIOCSIWRETRY);

	return 0;

}

int ioctl_set_txpow(tiap_param_txpower_t *txpow, int ioctl_sock, char *ifname)
{
	struct iwreq		wrq;
	
	memcpy (&(wrq.u.txpower), (struct iw_param *)txpow, sizeof (struct iw_param));

	IW_SET_EXT_ERR(ioctl_sock, ifname, SIOCSIWTXPOW, &wrq, __func__);

	return 0;
}

int exec_priv_ioctl_set(const __u32 we_id,
			       void *param,
			       int param_size,
			       int ioctl_sock, char *ifname)
{
	int		num_ioctls,
			k,j,
			offset = 0,
			subioctl = 0;
	iwprivargs	priv_tbl[IW_MAX_PRIV_DEF];
	u_char		buffer[4096]	;
	struct iwreq	wrq;

	num_ioctls = read_priv_ioctls(ioctl_sock, ifname, priv_tbl);
	if ( num_ioctls <= 0)
		return -1;

	 /* Search the correct ioctl */
	k = -1;
	while((++k < num_ioctls) && (priv_tbl[k].cmd != we_id));
	
	/* If not found... */
	if(k == num_ioctls)
	{
		fprintf(stderr, "No such WE ID : %d\n", we_id);
		return(-1);
	}

	/* Watch out for sub-ioctls ! */

	if(priv_tbl[k].cmd < SIOCDEVPRIVATE)
	{

		j = -1;
		/* Find the matching *real* ioctl */
		 while((++j < num_ioctls) && ((priv_tbl[j].name[0] != '\0') 
					  || (priv_tbl[j].set_args != priv_tbl[k].set_args)));

		/* If not found... */
		if(j == num_ioctls)
		{
			fprintf(stderr, "Invalid private ioctl definition for : %d\n",we_id);
			return(-1);
		}

		/* Save sub-ioctl ID */
		subioctl = priv_tbl[k].cmd;
		/* Reserve one int (simplify alignement issues) */
		offset = sizeof(__u32);
		/* Use real ioctl definition from now on */
		k = j;
	
	}

	memcpy(buffer, param, param_size);
	
	strncpy(wrq.ifr_name, ifname, IFNAMSIZ);

	/* ThIs test is important. It defines how the driver
	 * will have to handle the data */
	
	  if( (priv_tbl[k].set_args & IW_PRIV_SIZE_FIXED) &&
	      (iw_get_priv_size(priv_tbl[k].set_args) + offset <= IFNAMSIZ))
	  {
		  /* First case : all SET args fit within wrq */
		  if(offset)
			wrq.u.mode = subioctl;
		  memcpy(wrq.u.name + offset, buffer, IFNAMSIZ - offset);
	  }
	  else
	  {
		  
		  /* Second case : arg won't fit in wrq */
		  wrq.u.data.pointer = (caddr_t) buffer;
		  wrq.u.data.length = param_size;
		  wrq.u.data.flags = subioctl;
	  }

	  /* Perform the private ioctl */
	  if(ioctl(ioctl_sock, priv_tbl[k].cmd, &wrq))
	  {
		fprintf(stderr, "Interface doesn't accept private ioctl...\n");
		fprintf(stderr, "(%X): %d: %s\n", priv_tbl[k].cmd, subioctl, strerror(errno));
		return(-1);
	  }


	return 0;
}

int exec_priv_ioctl_get(const __u32 we_id, /* private ioctl ID */
			       void *param,       /* ioctl parameter */
			       int param_size,	  /* parameter size */
			       int ioctl_sock,
			       char *ifname)
{
	int		num_ioctls,
			k,j, ret,
			offset = 0,
			subioctl = 0;
	iwprivargs	priv_tbl[IW_MAX_PRIV_DEF];
	u_char		buffer[4096]	;
	struct iwreq	wrq;

	num_ioctls = read_priv_ioctls(ioctl_sock, ifname, priv_tbl);
	if ( num_ioctls <= 0)
		return -1;

	/*for (k=0; k<num_ioctls; k++)
		printf("%X %s\n", priv_tbl[k].cmd, priv_tbl[k].name);*/
	 /* Search the correct ioctl */
	k = -1;
	while((++k < num_ioctls) && (priv_tbl[k].cmd != we_id ||
				     priv_tbl[k].get_args == 0));
	
	/* If not found... */
	if(k == num_ioctls)
	{
		fprintf(stderr, "No such WE ID : %d\n", we_id);
		return 1;
	}

	/* Watch out for sub-ioctls ! */
	if(priv_tbl[k].cmd < SIOCDEVPRIVATE)
	{

		j = -1;
		/* Find the matching *real* ioctl */
		 while((++j < num_ioctls) && ((priv_tbl[j].name[0] != '\0') 
					  || (priv_tbl[j].get_args != priv_tbl[k].get_args)));

		/* If not found... */
		if(j == num_ioctls)
		{
			fprintf(stderr, "Invalid private ioctl definition for : %d\n",we_id);
			return 1;
		}
	

		/* Save sub-ioctl ID */
		subioctl = priv_tbl[k].cmd;
		/* Reserve one int (simplify alignement issues) */
		offset = sizeof(__u32);
		/* Use real ioctl definition from now on */
		k = j;
	}
	strncpy(wrq.ifr_name, ifname, IFNAMSIZ);

	/* This test is important. It defines how the driver
	 * will have to handle the data */
	
	  if( (priv_tbl[k].get_args & IW_PRIV_SIZE_FIXED) &&
	      (iw_get_priv_size(priv_tbl[k].get_args) <= IFNAMSIZ))
	  {
		  /* First case : all GET args fit within wrq */
		  if(offset)
			wrq.u.mode = subioctl;
	  }
	  else
	  {
		  /* Second case : arg won't fit in wrq */
		  memcpy(buffer, param, min(sizeof(buffer),param_size));
		  wrq.u.data.pointer = (caddr_t) buffer;
		  wrq.u.data.flags = subioctl;
	  }

	  /* Perform the private ioctl */
	  if( (ret=ioctl(ioctl_sock, priv_tbl[k].cmd, &wrq)) )
	  {
		fprintf(stderr, "Interface doesn't accept private ioctl...\n");
		fprintf(stderr, "(%X): %s\n", priv_tbl[k].cmd, strerror(errno));
		return ret;
	  }


	   /* Check where is the returned data */
	  if((priv_tbl[k].get_args & IW_PRIV_SIZE_FIXED) &&
	       (iw_get_priv_size(priv_tbl[k].get_args) <= IFNAMSIZ))
	  {
		memcpy(buffer, wrq.u.name, IFNAMSIZ);
	  }
	  /* else the returned data is already in the buffer */

	  memcpy(param, buffer, param_size);
	
	 return 0;
}


/*
 * read the all the private ioctls of the given device into the table
 */
static int read_priv_ioctls( int ioctl_sock,   /* Socket */
				       char *ifname,     /* Dev name */
				       iwprivargs *priv) /* dest ioctls table */
{
  int		number;		/* Max of private ioctl */

  /* Read all the private ioctls */
  number = iw_get_priv_info(ioctl_sock, ifname, priv, IW_MAX_PRIV_DEF);

  /* Is there any ? */
  if(number <= 0)
    {
      /* Could skip this message ? */
      fprintf(stderr, "%-8.8s  no private ioctls.\n\n",ifname);
      return(-1);
    }

  return number;
}

