/* IPsec DOI and Oakley resolution routines
 * Copyright (C) 1997 Angelos D. Keromytis.
 * Copyright (C) 1998-2002  D. Hugh Redelmeier.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * RCSID $Id: ipsec_doi.c,v 1.1.1.3 2005/08/25 06:27:25 sparq Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <arpa/nameser.h>	/* missing from <resolv.h> on old systems */
#include <sys/time.h>		/* for gettimeofday */

#include <freeswan.h>
#include <syslog.h>

#include "constants.h"
#include "defs.h"
#include "state.h"
#include "id.h"
#include "x509.h"
#include "connections.h"	/* needs id.h */
#include "keys.h"
#include "packet.h"
#include "demux.h"	/* needs packet.h */
#include "adns.h"	/* needs <resolv.h> */
#include "dnskey.h"	/* needs keys.h and adns.h */
#include "kernel.h"
#include "log.h"
#include "cookie.h"
#include "server.h"
#include "spdb.h"
#include "timer.h"
#include "rnd.h"
#include "ipsec_doi.h"	/* needs demux.h and state.h */
#include "whack.h"
#include "pkcs.h"

#include "sha1.h"
#include "md5.h"
#include "crypto.h" /* requires sha1.h and md5.h */
#include "ike_alg.h"
#include "kernel_alg.h"
#include "alg_info.h"

#include "vendor.h"

#ifdef NAT_TRAVERSAL
#include "nat_traversal.h"
#endif
#ifdef VIRTUAL_IP
#include "virtual.h"
#endif

//zhangbin 2004.11.19
#if 1
	#define VPN_LOG
#else
	//#define ZB_DEBUG
#endif

#ifdef DPD
static void dpd_init(struct state *st);
#endif
static bool encrypt_message(pb_stream *pbs, struct state *st);	/* forward declaration */

typedef stf_status initiator_function(
    int whack_sock,
    struct connection *c,
    struct state *predecessor,
    lset_t policy,
    unsigned long try);

#define ZHS_LOG() zhs_log()


/* MAGIC: perform f, a function that returns notification_t
 * and return from the ENCLOSING stf_status returning function if it fails.
 */
#define RETURN_STF_FAILURE(f) \
    { int r = (f); if (r != NOTHING_WRONG) return STF_FAIL + r; }


/* create output HDR as replica of input HDR */
void
echo_hdr(struct msg_digest *md, bool enc, u_int8_t np)
{
    struct isakmp_hdr r_hdr = md->hdr;	/* mostly same as incoming header */

    r_hdr.isa_flags &= ~ISAKMP_FLAG_COMMIT;	/* we won't ever turn on this bit */
    if (enc)
	r_hdr.isa_flags |= ISAKMP_FLAG_ENCRYPTION;
    /* some day, we may have to set r_hdr.isa_version */
    r_hdr.isa_np = np;
    if (!out_struct(&r_hdr, &isakmp_hdr_desc, &md->reply, &md->rbody))
	impossible();	/* surely must have room and be well-formed */
}

/* Compute DH shared secret from our local secret and the peer's public value.
 * We make the leap that the length should be that of the group
 * (see quoted passage at start of ACCEPT_KE).
 */
static void
compute_dh_shared(struct state *st, const chunk_t g
, const struct oakley_group_desc *group)
{
    MP_INT mp_g, mp_shared;
    struct timeval tv0, tv1;
    unsigned long tv_diff;

    gettimeofday(&tv0, NULL);
    passert(st->st_sec_in_use);
    n_to_mpz(&mp_g, g.ptr, g.len);
    mpz_init(&mp_shared);
    mpz_powm(&mp_shared, &mp_g, &st->st_sec, group->modulus);
    mpz_clear(&mp_g);
    freeanychunk(st->st_shared);	/* happens in odd error cases */
    st->st_shared = mpz_to_n(&mp_shared, group->bytes);
    mpz_clear(&mp_shared);
    gettimeofday(&tv1, NULL);
    tv_diff=(tv1.tv_sec  - tv0.tv_sec) * 1000000 + (tv1.tv_usec - tv0.tv_usec);
    DBG(DBG_CRYPT, 
    	DBG_log("compute_dh_shared(): time elapsed (%s): %ld usec"
		, enum_show(&oakley_group_names, st->st_oakley.group->group)
		, tv_diff);
       );
    /* if took more than 200 msec ... */
    if (tv_diff > 200000) {
	loglog(RC_LOG_SERIOUS, "WARNING: compute_dh_shared(): for %s took "
			"%ld usec"
		, enum_show(&oakley_group_names, st->st_oakley.group->group)
		, tv_diff);
    }

#ifdef DODGE_DH_MISSING_ZERO_BUG
    if (st->st_shared.ptr[0] == 0)
	loglog(RC_LOG_SERIOUS, "shared DH secret has leading zero -- triggers Pluto 1.0 bug");
#endif
    DBG_cond_dump_chunk(DBG_CRYPT, "DH shared secret:\n", st->st_shared);
}

/* needed for PGPnet Vendor ID */
char pgp_vid[] = "OpenPGP10171";

#ifdef DPD
/* needed for Dead Peer detection */
char dpd_vid[] = {0xAF, 0xCA, 0xD7, 0x13, 0x68, 0xA1, 0xF1,
          0xC9, 0x6B, 0x86, 0x96, 0xFC, 0x77, 0x57, 0x01, 0x00};
#endif


/* if we haven't already done so, compute a local DH secret (st->st_sec) and
 * the corresponding public value (g).  This is emitted as a KE payload.
 * KLUDGE: if DODGE_DH_MISSING_ZERO_BUG and we're the responder,
 * this routine computes the shared secret to see if it would
 * have a leading zero.  If so, we try again.
 */
static bool
build_and_ship_KE(struct state *st, chunk_t *g
, const struct oakley_group_desc *group, pb_stream *outs, u_int8_t np)
{
    if (!st->st_sec_in_use)
    {
	u_char tmp[LOCALSECRETSIZE];
	MP_INT mp_g;

	get_rnd_bytes(tmp, LOCALSECRETSIZE);
	st->st_sec_in_use = TRUE;
	n_to_mpz(&st->st_sec, tmp, LOCALSECRETSIZE);

	mpz_init(&mp_g);
	mpz_powm(&mp_g, &groupgenerator, &st->st_sec, group->modulus);
	freeanychunk(*g);	/* happens in odd error cases */
	*g = mpz_to_n(&mp_g, group->bytes);
	mpz_clear(&mp_g);
#ifdef DODGE_DH_MISSING_ZERO_BUG
	if (g->ptr[0] == 0)
	{
	    /* generate a new secret to avoid this situation */
	    loglog(RC_LOG_SERIOUS, "regenerating DH private secret to avoid Pluto 1.0 bug"
		" handling public value with leading zero");
	    mpz_clear(&st->st_sec);
	    st->st_sec_in_use = FALSE;
	    return build_and_ship_KE(st, g, group, outs, np);
	}
	/* if we're the responder, we can compute the shared secret
	 * to see if it would turn out OK.
	 */
	if (g == &st->st_gr)
	{
	    compute_dh_shared(st, st->st_gi
		, IS_PHASE1(st->st_state)
		    ? st->st_oakley.group : st->st_pfs_group);
	    if (st->st_shared.ptr[0] == 0)
	    {
		/* generate a new secret to avoid this situation */
		loglog(RC_LOG_SERIOUS, "regenerating DH private secret to avoid Pluto 1.0 bug"
		    " handling shared secret with leading zero");
		freeanychunk(st->st_shared);
		mpz_clear(&st->st_sec);
		st->st_sec_in_use = FALSE;
		return build_and_ship_KE(st, g, group, outs, np);
	    }
	}
#endif

	DBG(DBG_CRYPT,
	    DBG_dump("Local DH secret:\n", tmp, LOCALSECRETSIZE);
	    DBG_dump_chunk("Public DH value sent:\n", *g));
    }
    return out_generic_chunk(np, &isakmp_keyex_desc, outs, *g, "keyex value");
}

/* accept_ke
 *
 * Check and accept DH public value (Gi or Gr) from peer's message.
 * According to RFC2409 "The Internet key exchange (IKE)" 5:
 *  The Diffie-Hellman public value passed in a KE payload, in either
 *  a phase 1 or phase 2 exchange, MUST be the length of the negotiated
 *  Diffie-Hellman group enforced, if necessary, by pre-pending the
 *  value with zeros.
 * ??? For now, if DODGE_DH_MISSING_ZERO_BUG is defined, we accept shorter
 *     values to interoperate with old Plutos.  This should change some day.
 */
static notification_t
accept_KE(chunk_t *dest, const char *val_name
, const struct oakley_group_desc *gr
, pb_stream *pbs)
{
    if (pbs_left(pbs) != gr->bytes)
    {
	loglog(RC_LOG_SERIOUS, "KE has %u byte DH public value; %u required"
	    , (unsigned) pbs_left(pbs), (unsigned) gr->bytes);
#ifdef DODGE_DH_MISSING_ZERO_BUG
	if (pbs_left(pbs) > gr->bytes)
#endif
	    return INVALID_KEY_INFORMATION;
    }
    clonereplacechunk(*dest, pbs->cur, pbs_left(pbs), val_name);
    DBG_cond_dump_chunk(DBG_CRYPT, "DH public value received:\n", *dest);
    return NOTHING_WRONG;
}

/* accept_PFS_KE
 *
 * Check and accept optional Quick Mode KE payload for PFS.
 * Extends ACCEPT_PFS to check whether KE is allowed or required.
 */
static notification_t
accept_PFS_KE(struct msg_digest *md, chunk_t *dest
, const char *val_name, const char *msg_name)
{
    struct state *st = md->st;
    struct payload_digest *const ke_pd = md->chain[ISAKMP_NEXT_KE];

    if (ke_pd == NULL)
    {
	if (st->st_pfs_group != NULL)
	{
	    loglog(RC_LOG_SERIOUS, "missing KE payload in %s message", msg_name);
	    return INVALID_KEY_INFORMATION;
	}
    }
    else
    {
	if (st->st_pfs_group == NULL)
	{
	    loglog(RC_LOG_SERIOUS, "%s message KE payload requires a GROUP_DESCRIPTION attribute in SA"
		, msg_name);
	    return INVALID_KEY_INFORMATION;
	}
	if (ke_pd->next != NULL)
	{
	    loglog(RC_LOG_SERIOUS, "%s message contains several KE payloads; we accept at most one", msg_name);
	    return INVALID_KEY_INFORMATION;	/* ??? */
	}
	return accept_KE(dest, val_name, st->st_pfs_group, &ke_pd->pbs);
    }
    return NOTHING_WRONG;
}

static bool
build_and_ship_nonce(chunk_t *n, pb_stream *outs, u_int8_t np
, const char *name)
{
    setchunk(*n, alloc_bytes(DEFAULT_NONCE_SIZE, name), DEFAULT_NONCE_SIZE);
    get_rnd_bytes(n->ptr, DEFAULT_NONCE_SIZE);
    return out_generic_chunk(np, &isakmp_nonce_desc, outs, *n, name);
}

//zhangbin for blockip --2004/04/27
extern struct blockip blockip[BLOCKIP_NUM];
static int conn_blockip(unsigned long ip,char *name,int *secs)
{
	int i ;
	log("in conn_blockip");
	for(i = 0;i < BLOCKIP_NUM;i++)
	{
		//log("ip=%08x,blockip=%08x,name=%s,timeout=%d,max_timeout=%d,cnt=%d,max_cnt=%d",ip,blockip[i].ip,blockip[i].name,blockip[i].timeout,blockip[i].max_timeout,blockip[i].cnt,blockip[i].max_cnt);
		//log("now=%d",now());
		if((now() - blockip[i].timeout < blockip[i].max_timeout) && ip == blockip[i].ip)
		{
			blockip[i].cnt++;
			if(blockip[i].cnt >= blockip[i].max_cnt)
			{
				blockip[i].timeout = now();
				*secs = blockip[i].max_timeout;
				//log("blockip[%d]:name=%s,timeout=%d,max_timeout=%d,cnt=%d,max_cnt=%d",i,blockip[i].name,blockip[i].timeout,blockip[i].max_timeout,blockip[i].cnt,blockip[i].max_cnt);
				return 1;
			}
			break;
		}
	}
	//log("not found entry,i=%d!",i);
	if(i == BLOCKIP_NUM)
	{
		//log("i==BLOCKIP_NUM!");
		for(i = 0;i < BLOCKIP_NUM;i++)
		{
			if(now() - blockip[i].timeout > blockip[i].max_timeout)
			{
				FILE *bfd;
				char block_buf[80],t_name[30],t_tries[5],t_timeout[10];
				int find_it = 0;
				memset(block_buf,0,sizeof(block_buf));
				memset(t_name,0,sizeof(t_name));
				memset(t_tries,0,sizeof(t_tries));
				memset(t_timeout,0,sizeof(t_timeout));
				if((bfd = fopen("/tmp/blockip","r")) != NULL)
				{
					while(fgets(block_buf,sizeof(block_buf),bfd))
					{
						sscanf(block_buf,"%30[^:]:%5[^:]:%10[^\n]",t_name,t_tries,t_timeout);
						log("name=%s,t_name=%s,t_tries=%s,t_timeout=%s",name,t_name,t_tries,t_timeout);
						if(!strcmp(t_name,name))
						{
							find_it = 1;
							break;
						}
					}
					fclose(bfd);
				}	
				if(find_it == 1)
				{
					blockip[i].name = name;
					blockip[i].ip = ip;
					blockip[i].cnt = 1;
					blockip[i].timeout = now();
					blockip[i].max_cnt = atoi(t_tries);
					blockip[i].max_timeout = atoi(t_timeout);
					//zhs remove log("last:blockip[%d]:ip=%08x,name=%s,cnt=%d,timeout=%d,max_cnt=%d,max_timeout=%d",i,blockip[i].ip,blockip[i].name,blockip[i].cnt,blockip[i].timeout,blockip[i].max_cnt,blockip[i].max_timeout);
				}
				break;
			}
		}
	}
		return 0;
}

/*
 * Send a notification to the peer. We could make a decision on
 * whether to send the notification, based on the type and the
 * destination, if we care to.
 * It doesn't handle DELETE notifications (see send_ipsec_delete)
 */
static void
send_notification(struct state *sndst, u_int16_t type, struct state *encst,
    msgid_t msgid, u_char *icookie, u_char *rcookie,
    u_char *spi, size_t spisize, u_char protoid)
{
    u_char buffer[1024];
    pb_stream pbs, r_hdr_pbs;
    u_char *r_hashval, *r_hash_start;

    //zhangbin for blockip --2004/04/27
    int secs = 0;
    int block_flag = 0;



    passert((sndst) && (sndst->st_connection));

    //zhangbin for blockip --2004/04/27
    //log("first:sndst->st_connection->name=%s",sndst->st_connection->name);
    
    if(type != 80 && sndst->st_connection->initiated == FALSE)
    {
	    FILE *fp;
	    char block_name[30];
	    int auth_flag = 0;
	    memset(block_name,0,sizeof(block_name));
	    sprintf(block_name,"/tmp/block/%s",sndst->st_connection->name);
	    //log("after block_name!");
	    if((fp = fopen(block_name,"r")) != NULL)
	    {
		    auth_flag = 1;
		    fclose(fp);
	    }
	    if(auth_flag == 1)
	    {
		    //log("will enter conn_blockip:ip=%08x,name=%s!!",sndst->st_connection->that.host_addr.u.v4.sin_addr.s_addr,sndst->st_connection->name);
   		//md by zhs for blockip log,2005-07-08
		conn_blockip(sndst->st_connection->that.host_addr.u.v4.sin_addr.s_addr,sndst->st_connection->name,&secs);
	    	/*if(conn_blockip(sndst->st_connection->that.host_addr.u.v4.sin_addr.s_addr,sndst->st_connection->name,&secs) == 1)
	    	{	
			block_flag = 1;		
	    	}*/
		//end by zhs
	   }
    }
    
   	/*rm by zhs for blockip log,2005-07-08
	if(block_flag == 1)
	{
		//log("loop:send BLOCKIP notification again!");
		send_notification(sndst,80,encst,msgid,icookie,rcookie,spi,secs,protoid);
		return;
	}
	end by zhs*/
//zhangbin for blockip
    if(type == 80)
    {
#ifdef VPN_LOG
	    char blockmsg[80];
	    char *conn_name;
	    memset(blockmsg,0,sizeof(blockmsg));
	    conn_name = (sndst->st_connection->name == NULL)?(char *)spi:sndst->st_connection->name;
	    sprintf(blockmsg,"IKE[\"%s\"] Block %s for %d sec!",conn_name,ip_str(&sndst->st_connection->that.host_addr),spisize);
	    send_log(blockmsg);
	    log(blockmsg);
#endif
	    return ;
     }

    log("sending %snotification %s to %s:%u",
	encst ? "encrypted " : "",
	enum_name(&ipsec_notification_names, type),
	ip_str(&sndst->st_connection->that.host_addr),
	(unsigned)sndst->st_connection->that.host_port);
	
    memset(buffer, 0, sizeof(buffer));
    init_pbs(&pbs, buffer, sizeof(buffer), "notification msg");

    /* HDR* */
    {
	struct isakmp_hdr hdr;

	hdr.isa_version = ISAKMP_MAJOR_VERSION << ISA_MAJ_SHIFT | ISAKMP_MINOR_VERSION;
	hdr.isa_np = encst ? ISAKMP_NEXT_HASH : ISAKMP_NEXT_N;
	hdr.isa_xchg = ISAKMP_XCHG_INFO;
	hdr.isa_msgid = msgid;
	hdr.isa_flags = encst ? ISAKMP_FLAG_ENCRYPTION : 0;
	if (icookie)
	    memcpy(hdr.isa_icookie, icookie, COOKIE_SIZE);
	if (rcookie)
	    memcpy(hdr.isa_rcookie, rcookie, COOKIE_SIZE);
	if (!out_struct(&hdr, &isakmp_hdr_desc, &pbs, &r_hdr_pbs))
	    impossible();
    }

    /* HASH -- value to be filled later */
    if (encst)
    {
	pb_stream hash_pbs;
	if (!out_generic(ISAKMP_NEXT_N, &isakmp_hash_desc, &r_hdr_pbs,
	    &hash_pbs))
	    impossible();
	r_hashval = hash_pbs.cur;  /* remember where to plant value */
	if (!out_zero(
#ifdef _IKE_ALG_H
	    encst->st_oakley.hasher->hash_digest_size,
#else
	    encst->st_oakley.hasher->hash_digest_len,
#endif
	    &hash_pbs, "HASH(1)"))
	    impossible();
	close_output_pbs(&hash_pbs);
	r_hash_start = r_hdr_pbs.cur; /* hash from after HASH(1) */
    }

    /* Notification Payload */
    {
	pb_stream not_pbs;
	struct isakmp_notification isan;

	isan.isan_doi = ISAKMP_DOI_IPSEC;
	isan.isan_np = ISAKMP_NEXT_NONE;
	isan.isan_type = type;
	isan.isan_spisize = spisize;
	isan.isan_protoid = protoid;

	if (!out_struct(&isan, &isakmp_notification_desc, &r_hdr_pbs, &not_pbs)
	    || !out_raw(spi, spisize, &not_pbs, "spi"))
	    impossible();
	close_output_pbs(&not_pbs);
    }

    /* calculate hash value and patch into Hash Payload */
    if (encst)
    {
	struct hmac_ctx ctx;
	hmac_init_chunk(&ctx, encst->st_oakley.hasher, encst->st_skeyid_a);
	hmac_update(&ctx, (u_char *) &msgid, sizeof(msgid_t));
	hmac_update(&ctx, r_hash_start, r_hdr_pbs.cur-r_hash_start);
	hmac_final(r_hashval, &ctx);

#ifdef _IKE_ALG_H
	DBG(DBG_CRYPT,
	    DBG_log("HASH(1) computed:");
	    DBG_dump("", r_hashval, ctx.hmac_digest_size);
	)
    }
#else
	DBG(DBG_CRYPT,
	    DBG_log("HASH(1) computed:");
	    DBG_dump("", r_hashval, ctx.hmac_digest_len);
	)
    }
#endif

    /* Encrypt message (preserve st_iv) */
    if (encst)
    {
	u_char old_iv[MAX_DIGEST_LEN];
	if (encst->st_iv_len > MAX_DIGEST_LEN)
	    impossible();
	memcpy(old_iv, encst->st_iv, encst->st_iv_len);
	init_phase2_iv(encst, &msgid);
	if (!encrypt_message(&r_hdr_pbs, encst))
	    impossible();
	memcpy(encst->st_iv, old_iv, encst->st_iv_len);
    }
    else
    {
	close_output_pbs(&r_hdr_pbs);
    }

    /* Send packet (preserve st_tpacket) */
    {
	chunk_t saved_tpacket = sndst->st_tpacket;

	setchunk(sndst->st_tpacket, pbs.start, pbs_offset(&pbs));
	//zhangbin for vpn-log
	{
		char nt_msg[100],nt_type[100];
		memset(nt_msg,0,sizeof(nt_msg));
		memset(nt_type,0,sizeof(nt_type));
		switch(type)
		{
			case INVALID_PAYLOAD_TYPE:
			{
				sprintf(nt_msg,"2rIKE[\"%s\"] **Check your ISAKMP setting !",sndst->st_connection->name);
				send_log(nt_msg);
				sprintf(nt_type,"2nIKE[\"%s\"] Tx >> Notify : INVALID_PAYLOAD_TYPE",sndst->st_connection->name);
				send_log(nt_type);
				break;
			}
			case PAYLOAD_MALFORMED:
			{
				sprintf(nt_msg,"2rIKE[\"%s\"] **Check your PFS and Pre-share Key settings !",sndst->st_connection->name);
				send_log(nt_msg);
				sprintf(nt_type,"2nIKE[\"%s\"] Tx >> Notify : PAYLOAD_MALFORMED",sndst->st_connection->name);
				send_log(nt_type);
				break;
			}
			case NO_PROPOSAL_CHOSEN:
			{
				sprintf(nt_msg,"2rIKE[\"%s\"] **Check your Encryption,Authentication method and PFS settings !",sndst->st_connection->name);
				send_log(nt_msg);
				sprintf(nt_type,"2nIKE[\"%s\"] Tx >> Notify : NO_PROPOSAL_CHOSEN",sndst->st_connection->name);
				send_log(nt_type);
				break;
			}
			case INVALID_ID_INFORMATION:
			{
				sprintf(nt_msg,"2rIKE[\"%s\"] **Check your Local/Remote Secure Group settings !",sndst->st_connection->name);
				send_log(nt_msg);
				sprintf(nt_type,"2nIKE[\"%s\"] Tx >> Notify : INVALID_ID_INFORMATION",sndst->st_connection->name);
				send_log(nt_type);
				break;
			}
			default:
				break;
		}
				
	} //end if for vpn-log
	send_packet(sndst, "notification packet");
	sndst->st_tpacket = saved_tpacket;
    }
}

void
send_notification_from_state(struct state *st, enum state_kind state,
    u_int16_t type)
{
    struct state *p1st;

    passert(st);

    if (state == STATE_UNDEFINED)
	state = st->st_state;

    if (IS_QUICK(state)) {
	p1st = find_phase1_state(st->st_connection, TRUE);
	if ((p1st == NULL) || (!IS_ISAKMP_SA_ESTABLISHED(p1st->st_state))) {
	    loglog(RC_LOG_SERIOUS,
		"no Phase1 state for Quick mode notification");
	    return;
	}
	send_notification(st, type, p1st, generate_msgid(p1st),
	    st->st_icookie, st->st_rcookie, NULL, 0, PROTO_ISAKMP);
    }
    else if (IS_ISAKMP_SA_ESTABLISHED(state)) {
	send_notification(st, type, st, generate_msgid(st),
	    st->st_icookie, st->st_rcookie, NULL, 0, PROTO_ISAKMP);
    }
    else {
	/* no ISAKMP SA established - don't encrypt notification */
	send_notification(st, type, NULL, 0,
	    st->st_icookie, st->st_rcookie, NULL, 0, PROTO_ISAKMP);
    }
}

void
send_notification_from_md(struct msg_digest *md, u_int16_t type)
{
    /**
     * Create a dummy state to be able to use send_packet in
     * send_notification
     *
     * we need to set:
     *   st_connection->that.host_addr
     *   st_connection->that.host_port
     *   st_connection->interface
     */
    struct state st;
    struct connection cnx;

    passert(md);

    memset(&st, 0, sizeof(st));
    memset(&cnx, 0, sizeof(cnx));
    st.st_connection = &cnx;
    cnx.that.host_addr = md->sender;
    cnx.that.host_port = md->sender_port;
    cnx.interface = md->iface;

    send_notification(&st, type, NULL, 0,
	md->hdr.isa_icookie, md->hdr.isa_rcookie, NULL, 0, PROTO_ISAKMP);
}

//zhangbin for blockip --2004/04/27
void
send_blockip(struct msg_digest *md,u_int16_t type,char *name,int secs)
{
    struct state st;
    struct connection cnx;

    passert(md);
    memset(&st, 0, sizeof(st));
    memset(&cnx, 0, sizeof(cnx));
    st.st_connection = &cnx;
    cnx.that.host_addr = md->sender;
    cnx.that.host_port = md->sender_port;
    cnx.interface = md->iface;

	//log("type=%d,name=%s,secs=%d",type,name,secs);
    	send_notification(&st, type, NULL, 0,NULL,NULL,name,secs,PROTO_ISAKMP);
}


/* Send a Delete Notification to announce deletion of inbound IPSEC/ISAKMP SAs.
 * Ignores states that don't have any.
 * Delete Notifications cannot announce deletion of outbound IPSEC/ISAKMP SAs.
 */
void
send_delete(struct state *st)
{
    pb_stream reply_pbs;
    pb_stream r_hdr_pbs;
    msgid_t	msgid;
    u_char buffer[8192];
    struct state *p1st;
    ip_said said[EM_MAXRELSPIS];
    ip_said *ns = said;
    u_char
	*r_hashval,	/* where in reply to jam hash value */
	*r_hash_start;	/* start of what is to be hashed */
    bool isakmp_sa = FALSE;

    if (IS_IPSEC_SA_ESTABLISHED(st->st_state)) {
	p1st = find_phase1_state(st->st_connection, TRUE);
	if (p1st == NULL)
	{
	    DBG(DBG_CONTROL, DBG_log("no Phase 1 state for Delete"));
	    return;
	}

	if (st->st_ah.present)
	{
	    ns->spi = st->st_ah.our_spi;
	    ns->dst = st->st_connection->this.host_addr;
	    ns->proto = PROTO_IPSEC_AH;
	    ns++;
	}
	if (st->st_esp.present)
	{
	    ns->spi = st->st_esp.our_spi;
	    ns->dst = st->st_connection->this.host_addr;
	    ns->proto = PROTO_IPSEC_ESP;
	    ns++;
	}
	
	passert(ns != said);	/* there must be some SAs to delete */
    }
    else if (IS_ISAKMP_SA_ESTABLISHED(st->st_state)) {
	p1st = st;
	isakmp_sa = TRUE;
    }
    else {
	return; /* nothing to do */
    }

    msgid = generate_msgid(p1st);

    memset(buffer, '\0', sizeof(buffer));
    init_pbs(&reply_pbs, buffer, sizeof(buffer), "delete msg");

    /* HDR* */
    {
	struct isakmp_hdr hdr;

	hdr.isa_version = ISAKMP_MAJOR_VERSION << ISA_MAJ_SHIFT | ISAKMP_MINOR_VERSION;
	hdr.isa_np = ISAKMP_NEXT_HASH;
	hdr.isa_xchg = ISAKMP_XCHG_INFO;
	hdr.isa_msgid = msgid;
	hdr.isa_flags = ISAKMP_FLAG_ENCRYPTION;
	memcpy(hdr.isa_icookie, p1st->st_icookie, COOKIE_SIZE);
	memcpy(hdr.isa_rcookie, p1st->st_rcookie, COOKIE_SIZE);
	if (!out_struct(&hdr, &isakmp_hdr_desc, &reply_pbs, &r_hdr_pbs))
	    impossible();
    }

    /* HASH -- value to be filled later */
    {
	pb_stream hash_pbs;

	if (!out_generic(ISAKMP_NEXT_D, &isakmp_hash_desc, &r_hdr_pbs, &hash_pbs))
	    impossible();
	r_hashval = hash_pbs.cur;	/* remember where to plant value */
	if (!out_zero(p1st->st_oakley.hasher->hash_digest_size, &hash_pbs, "HASH(1)"))
	    impossible();
	close_output_pbs(&hash_pbs);
	r_hash_start = r_hdr_pbs.cur;	/* hash from after HASH(1) */
    }

    /* Delete Payloads */
    if (isakmp_sa) {
	pb_stream del_pbs;
	struct isakmp_delete isad;
	u_char isakmp_spi[2*COOKIE_SIZE];

	isad.isad_doi = ISAKMP_DOI_IPSEC;
	isad.isad_np = ISAKMP_NEXT_NONE;
	isad.isad_spisize = (2 * COOKIE_SIZE);
	isad.isad_protoid = PROTO_ISAKMP;
	isad.isad_nospi = 1;

	memcpy(isakmp_spi, st->st_icookie, COOKIE_SIZE);
	memcpy(isakmp_spi+COOKIE_SIZE, st->st_rcookie, COOKIE_SIZE);

	if (!out_struct(&isad, &isakmp_delete_desc, &r_hdr_pbs, &del_pbs)
	|| !out_raw(&isakmp_spi, (2*COOKIE_SIZE), &del_pbs, "delete payload"))
	    impossible();
	close_output_pbs(&del_pbs);
    }
    else while (ns != said) {
	pb_stream del_pbs;
	struct isakmp_delete isad;

	ns--;
	isad.isad_doi = ISAKMP_DOI_IPSEC;
	isad.isad_np = ns == said? ISAKMP_NEXT_NONE : ISAKMP_NEXT_D;
	isad.isad_spisize = sizeof(ipsec_spi_t);
	isad.isad_protoid = ns->proto;
	isad.isad_nospi = 1;

	if (!out_struct(&isad, &isakmp_delete_desc, &r_hdr_pbs, &del_pbs)
	|| !out_raw(&ns->spi, sizeof(ipsec_spi_t), &del_pbs, "delete payload"))
	    impossible();
	close_output_pbs(&del_pbs);
    }

    /* calculate hash value and patch into Hash Payload */
    {
	struct hmac_ctx ctx;
	hmac_init_chunk(&ctx, p1st->st_oakley.hasher, p1st->st_skeyid_a);
	hmac_update(&ctx, (u_char *) &msgid, sizeof(msgid_t));
	hmac_update(&ctx, r_hash_start, r_hdr_pbs.cur-r_hash_start);
	hmac_final(r_hashval, &ctx);

	DBG(DBG_CRYPT,
	    DBG_log("HASH(1) computed:");
	    DBG_dump("", r_hashval, ctx.hmac_digest_size);
	)
    }

    /* Do a dance to avoid needing a new state object.
     * We use the Phase 1 State.  This is the one with right
     * IV, for one thing.
     * The tricky bits are:
     * - we need to preserve (save/restore) st_iv (but not st_iv_new)
     * - we need to preserve (save/restore) st_tpacket.
     */
    {
	u_char old_iv[MAX_DIGEST_LEN];
	chunk_t saved_tpacket = p1st->st_tpacket;

	memcpy(old_iv, p1st->st_iv, p1st->st_iv_len);
	init_phase2_iv(p1st, &msgid);

	if(!encrypt_message(&r_hdr_pbs, p1st))
	    impossible();

	setchunk(p1st->st_tpacket, reply_pbs.start, pbs_offset(&reply_pbs));
#if 1 
//zhangbin for vpn-log
	{
		if(IS_IPSEC_SA_ESTABLISHED(st->st_state))
		{
			char delmsg[80];
			memset(delmsg,0,sizeof(delmsg));

			sprintf(delmsg,"2nIKE[\"%s\"] Tx >> Delete ESP_SA : spi = %08x\n",st->st_connection->name,ntohl(st->st_esp.our_spi));
			send_log(delmsg);
		}
		else if(IS_ISAKMP_SA_ESTABLISHED(st->st_state))
		{
			char del_msg[85],temp1[3],temp2[3],temp3[3],temp4[3],i_ck1[10],i_ck2[10],r_ck1[10],r_ck2[10];
			int i = 0,j = 0;
			memset(del_msg,0,sizeof(del_msg));
			memset(temp1,0,sizeof(temp1));
			memset(temp2,0,sizeof(temp2));
			memset(temp3,0,sizeof(temp3));
			memset(temp4,0,sizeof(temp4));
			memset(i_ck1,0,sizeof(i_ck1));
			memset(i_ck2,0,sizeof(i_ck2));
			memset(r_ck1,0,sizeof(r_ck1));
			memset(r_ck2,0,sizeof(r_ck2));

			for(i = 3; i >= 0; i--)
			{
				sprintf(temp1,"%02x",st->st_icookie[i]);
				strcat(i_ck1,temp1);
			}
			for(i = 7; i >= 4; i--)
			{
				sprintf(temp2,"%02x",st->st_icookie[i]);
				strcat(i_ck2,temp2);
			}
			for(j = 3; j >= 0; j--)
			{
				sprintf(temp3,"%02x",st->st_rcookie[j]);
				strcat(r_ck1,temp3);
			}
			for(j = 7; j >= 4; j--)
			{
				sprintf(temp4,"%02x",st->st_rcookie[j]);
				strcat(r_ck2,temp4);
			}

			sprintf(del_msg,"2nIKE[\"%s\"] Tx >> Delete ISAKMP_SA : cookie %s %s | %s %s\n",st->st_connection->name,i_ck1,i_ck2,r_ck1,r_ck2);
			send_log(del_msg);
		}

	}
	
#endif
	send_packet(p1st, "delete notify");
	p1st->st_tpacket = saved_tpacket;

	/* get back old IV for this state */
	memcpy(p1st->st_iv, old_iv, p1st->st_iv_len);
    }
}

void
accept_delete(struct state *st, struct msg_digest *md, struct payload_digest *p)
{
    struct isakmp_delete *d = &(p->payload.delete);
    size_t sizespi = 0;
    u_char *spi;
    struct state *dst = NULL;
    int i;

    if ((!st) && (!(md->hdr.isa_flags & ISAKMP_FLAG_ENCRYPTION))) {
	loglog(RC_LOG_SERIOUS, "ignoring Delete SA payload: not encrypted");
	return;
    }

    if (!IS_ISAKMP_SA_ESTABLISHED(st->st_state)) {
	/* can't happen (if msg is encrypt), but just to be sure */
	loglog(RC_LOG_SERIOUS, "ignoring Delete SA payload: "
	"ISAKMP SA not established");
	return;
    }

    if (d->isad_nospi == 0) {
	loglog(RC_LOG_SERIOUS, "ignoring Delete SA payload: no SPI");
	return;
    }
    if (pbs_left(&p->pbs) != ((unsigned)d->isad_spisize * d->isad_nospi)) {
	loglog(RC_LOG_SERIOUS, "ignoring Delete SA payload: invalid size");	//zhangbin
	/* for  according to wag54g */
	{
#if 1
		if(NULL != st)
		{
			char delmsg[200];
			struct state *dst;//add by zhs for deleate SA repeatedly bug,2005-4-22
			memset(delmsg,0,sizeof(delmsg));
			if(d->isad_protoid == PROTO_IPSEC_ESP)
			{
				bool bogus;//add by zhs for deleate SA repeatedly bug,2005-4-22
				u_char *spi = p->pbs.cur; 
				ipsec_spi_t ipsec_spi = *((ipsec_spi_t *)spi);
				sprintf(delmsg,"2nIKE[\"%s\"] Rx {{ Delete ESP_SA : spi = %08x",st->st_connection->name,ipsec_spi);
				send_log(delmsg);
				//add by zhs for deleate SA repeatedly bug,2005-4-22
				{
					 dst = find_phase2_state_to_delete(st,PROTO_IPSEC_ESP,ipsec_spi,&bogus);
                        	 	if(dst)
                         		{
                                		delete_state(dst);
	                         	}
        	                 	else
                	               		send_log("ignore delete IPSEC packet!\n");
                        	}//end by zhs

			
			}
			else if(d->isad_protoid == PROTO_ISAKMP)
			{
				char temp1[3],temp2[3],temp3[3],temp4[3],i_ck1[10],i_ck2[10],r_ck1[10],r_ck2[10];
				int i = 0,j = 0;
				memset(temp1,0,sizeof(temp1));
				memset(temp2,0,sizeof(temp2));
				memset(temp3,0,sizeof(temp3));
				memset(temp4,0,sizeof(temp4));
				memset(i_ck1,0,sizeof(i_ck1));
				memset(i_ck2,0,sizeof(i_ck2));
				memset(r_ck1,0,sizeof(r_ck1));
				memset(r_ck2,0,sizeof(r_ck2));

				for(i = 3; i >= 0; i--)
				{
					sprintf(temp1,"%02x",st->st_icookie[i]);
					strcat(i_ck1,temp1);
				}
				for(i = 7; i >= 4; i--)
				{
					sprintf(temp2,"%02x",st->st_icookie[i]);
					strcat(i_ck2,temp2);
				}
				for(j = 3; j >= 0; j--)
				{
					sprintf(temp3,"%02x",st->st_rcookie[j]);
					strcat(r_ck1,temp3);
				}
				for(j = 7; j >= 4; j--)
				{
					sprintf(temp4,"%02x",st->st_rcookie[j]);
					strcat(r_ck2,temp4);
				}

				sprintf(delmsg,"2nIKE[\"%s\"] Rx {{ Delete ISAKMP_SA : cookie %s %s | %s %s",st->st_connection->name,r_ck1,r_ck2,i_ck1,i_ck2);
				send_log(delmsg);
				
				{//add by zhs for deleate SA repeatedly bug,2005-4-22

         				dst=find_phase1_state_to_delete(st,st-> st_icookie,st-> st_rcookie);
	          			if (dst)
        	  			{
                				delete_state(dst);
          				}
          				else
	              				send_log("ignore delete IKE packet!\n");
      				}//end by zhs

			}
		}
#endif
		  //rm by zhs for deleate SA repeatedly bug,2005-4-22
		//terminate_connection(st->st_connection->name);
	}
	
	return; 
    }
    switch (d->isad_protoid) {
	case PROTO_ISAKMP:
	    sizespi = (2*COOKIE_SIZE);
	    break;
	case PROTO_IPSEC_AH:
	case PROTO_IPSEC_ESP:
	    sizespi = sizeof(ipsec_spi_t);
	    break;
	default:
	    loglog(RC_LOG_SERIOUS, "ignoring Delete SA payload: "
		"unknown Protocol ID (%s)",
		enum_show(&protocol_names, d->isad_protoid));
	    return;
	    break;
    }

    if (d->isad_spisize != sizespi) {
	loglog(RC_LOG_SERIOUS, "ignoring Delete SA payload: "
	    "bad size (%d) for Protocol (%s)",
	    d->isad_spisize,
	    enum_show(&protocol_names, d->isad_protoid));
	return;
    }

    for (i=0; i<d->isad_nospi; i++) {
	spi = p->pbs.cur + (i * sizespi);
	if (d->isad_protoid == PROTO_ISAKMP) {
	    /**
	     * ISAKMP
	     */
	    dst = find_phase1_state_to_delete(st, spi /*iCookie*/,
		spi+COOKIE_SIZE /*rCookie*/);
	    if (dst) {
#ifdef NAT_TRAVERSAL
		if (nat_traversal_enabled) {
		    nat_traversal_change_port_lookup(md, dst);
		}
#endif
		loglog(RC_LOG_SERIOUS, "received Delete SA payload: "
		    "deleting ISAKMP State #%lu", dst->st_serialno);
		delete_state(dst);
	    }
	    else {
		loglog(RC_LOG_SERIOUS, "ignoring Delete SA payload: "
		    "ISAKMP SA not found (maybe expired)");
	    }
	}
	else {
	    /**
	     * IPSEC (ESP/AH)
	     */
	    bool bogus;
	    ipsec_spi_t ipsec_spi = *((ipsec_spi_t *)spi);

	    dst = find_phase2_state_to_delete(st, d->isad_protoid, ipsec_spi, &bogus);
	    if (dst) {
		struct connection *rc = dst->st_connection;
#ifdef NAT_TRAVERSAL
		if (nat_traversal_enabled) {
		    nat_traversal_change_port_lookup(md, dst);
		}
#endif
		if ((rc) && (rc->newest_ipsec_sa == dst->st_serialno) &&
		    (rc->initiated)) {
		    /*
		     * Last IPSec SA for a permanent connection that we
		     * have initiated. Replace it in a few seconds.
		     *
		     * Usefull if the other peer is rebooting
		     */
#define DELETE_SA_DELAY  EVENT_RETRANSMIT_DELAY_0
		    if ((dst->st_event) &&
			(dst->st_event->ev_type == EVENT_SA_REPLACE) &&
			(dst->st_event->ev_time <= DELETE_SA_DELAY + now())) {
			/*
			 * Patch from Angus Lees to ignore retransmited Delete SA.
			 */
			loglog(RC_LOG_SERIOUS, "received Delete SA payload: "
			    "already replacing IPSEC State #%lu in %d seconds",
			    dst->st_serialno, (int)(dst->st_event->ev_time - now()));
		    }
		    else {
			loglog(RC_LOG_SERIOUS, "received Delete SA payload: "
			    "replace IPSEC State #%lu in %d seconds",
			    dst->st_serialno, DELETE_SA_DELAY);
			dst->st_margin = DELETE_SA_DELAY;
			delete_event(dst);
			event_schedule(EVENT_SA_REPLACE, DELETE_SA_DELAY, dst);
		    }
		}
		else {
		    loglog(RC_LOG_SERIOUS, "received Delete SA payload: "
			"deleting IPSEC State #%lu", dst->st_serialno);
		    delete_state(dst);
		}
	    }
	    else {
		loglog(RC_LOG_SERIOUS, "ignoring Delete SA payload: "
		    "IPSEC SA not found (%s)",
		    bogus ? "our spi - bogus implementation" : "maybe expired");
	    }
	}
    }
}

/* The whole message must be a multiple of 4 octets.
 * I'm not sure where this is spelled out, but look at
 * rfc2408 3.6 Transform Payload.
 * Note: it talks about 4 BYTE boundaries!
 */
static void
close_message(pb_stream *pbs)
{
    size_t padding =  pad_up(pbs_offset(pbs), 4);

    if (padding != 0)
	(void) out_zero(padding, pbs, "message padding");
    close_output_pbs(pbs);
}

/* Initiate an Oakley Main Mode exchange.
 * --> HDR;SA
 * Note: this is not called from demux.c
 */
static stf_status
main_outI1(int whack_sock
, struct connection *c
, struct state *predecessor
, lset_t policy
, unsigned long try)
{
    struct state *st = new_state();
    pb_stream reply;	/* not actually a reply, but you know what I mean */
    pb_stream rbody;

    /* set up new state */
    st->st_connection = c;
    set_cur_state(st);	/* we must reset before exit */
    st->st_policy = policy & ~POLICY_IPSEC_MASK;
    st->st_whack_sock = whack_sock;
    st->st_try = try;
    st->st_state = STATE_MAIN_I1;

    get_cookie(TRUE, st->st_icookie, COOKIE_SIZE, &c->that.host_addr);

    insert_state(st);	/* needs cookies, connection, and msgid (0) */

    if (HAS_IPSEC_POLICY(policy))
	add_pending(dup_any(whack_sock), st, c, policy, 1
	    , predecessor == NULL? SOS_NOBODY : predecessor->st_serialno);

    if (predecessor == NULL)
	log("initiating Main Mode");
    else
	log("initiating Main Mode to replace #%lu", predecessor->st_serialno);

    /* set up reply */
    init_pbs(&reply, reply_buffer, sizeof(reply_buffer), "reply packet");

    /* HDR out */
    {
	struct isakmp_hdr hdr;

	zero(&hdr);	/* default to 0 */
	hdr.isa_version = ISAKMP_MAJOR_VERSION << ISA_MAJ_SHIFT | ISAKMP_MINOR_VERSION;
	hdr.isa_np = ISAKMP_NEXT_SA;
	hdr.isa_xchg = ISAKMP_XCHG_IDPROT;
	memcpy(hdr.isa_icookie, st->st_icookie, COOKIE_SIZE);
	/* R-cookie, flags and MessageID are left zero */

	if (!out_struct(&hdr, &isakmp_hdr_desc, &reply, &rbody))
	{
	    reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}
    }

    /* SA out */
    {
	u_char *sa_start = rbody.cur;
	lset_t auth_policy = policy & POLICY_ID_AUTH_MASK;

	/* if we  use an ID_KEY_ID as own ID, we assume a
	 * PGPcert peer and have to send the Vendor ID
	 *  ++++ not sure, if this is an interop problem ++++++
	 */
	int np = (c->this.id.kind == ID_KEY_ID)? ISAKMP_NEXT_VID : ISAKMP_NEXT_NONE;

	if (!out_sa(&rbody
	, &oakley_sadb[auth_policy >> POLICY_ISAKMP_SHIFT]
	, st, TRUE, FALSE, np))
	{
	    reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}

	/* save initiator SA for later HASH */
	passert(st->st_p1isa.ptr == NULL);	/* no leak!  (MUST be first time) */
	clonetochunk(st->st_p1isa, sa_start, rbody.cur - sa_start
	    , "sa in main_outI1");
    }

    if (c->this.id.kind == ID_KEY_ID)
    {
	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_vendor_id_desc, &rbody
	    , pgp_vid, sizeof(pgp_vid)-1, "V_ID"))
    		 return STF_INTERNAL_ERROR;
    }

#ifdef NAT_TRAVERSAL
    if (nat_traversal_enabled) {
	/* Add supported NAT-Traversal VID */
	if (!nat_traversal_add_vid(ISAKMP_NEXT_NONE, &rbody)) {
	    reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}
    }
#endif

#ifdef DPD
    /* Announce our ability to do Dead Peer Detection to the peer */
    if(st->st_connection->dpd_delay && st->st_connection->dpd_timeout) {
    	if (!out_modify_previous_np(ISAKMP_NEXT_VID, &rbody))
            return STF_INTERNAL_ERROR;
    	if( !out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_vendor_id_desc,
		    &rbody, dpd_vid, sizeof(dpd_vid), "V_ID"))
	    return STF_INTERNAL_ERROR;
    }
#endif

    close_message(&rbody);
    close_output_pbs(&reply);

    clonetochunk(st->st_tpacket, reply.start, pbs_offset(&reply)
	, "reply packet for main_outI1");

    /* Transmit */

    send_packet(st, "main_outI1");

    /* Set up a retransmission event, half a minute henceforth */
    delete_event(st);
    event_schedule(EVENT_RETRANSMIT, EVENT_RETRANSMIT_DELAY_0, st);

    if (predecessor != NULL)
    {
	update_pending(predecessor, st);
	whack_log(RC_NEW_STATE + STATE_MAIN_I1
	    , "%s: initiate, replacing #%lu"
	    , enum_name(&state_names, st->st_state)
	    , predecessor->st_serialno);
    }
    else
    {
	whack_log(RC_NEW_STATE + STATE_MAIN_I1
	    , "%s: initiate", enum_name(&state_names, st->st_state));
    }
    reset_cur_state();

    //zhangbin for vpn-log
#ifdef VPN_LOG
    {
	    char tx_mi1[80];
	    memset(tx_mi1,0,sizeof(tx_mi1));
	    sprintf(tx_mi1, "2nIKE[\"%s\"] Tx >> MM_I1 : %s SA",c->name,ip_str(&c->that.host_addr));
	    send_log(tx_mi1);
   }

    //zhangbin 2004.10.27
    send_log("after Tx MM_I1!!");
#endif
    return STF_OK;
}


/* Initiate an Oakley Aggressive Mode exchange.
 * --> HDR, SA, KE, Ni, IDii
 */
static stf_status
aggr_outI1(
	int whack_sock,
	struct connection *c,
	struct state *predecessor,
	lset_t policy,
	unsigned long try)
{
    u_char space[8192];	/* NOTE: we assume 8192 is big enough to build the packet */
    pb_stream reply;	/* not actually a reply, but you know what I mean */
    pb_stream rbody;

    struct state *st;
//zhangbin	    
    char ike_buf[6];

    /* set up new state */
    cur_state = st = new_state();
    st->st_connection = c;
#ifdef DEBUG
    extra_debugging(c);
#endif
    st->st_policy = policy & ~POLICY_IPSEC_MASK;
    st->st_whack_sock = whack_sock;
    st->st_try = try;
    st->st_state = STATE_AGGR_I1;

    get_cookie(TRUE, st->st_icookie, COOKIE_SIZE, &c->that.host_addr);

    insert_state(st);	/* needs cookies, connection, and msgid (0) */

    if (HAS_IPSEC_POLICY(policy))
	add_pending(dup_any(whack_sock), st, c, policy, 1
	    , predecessor == NULL? SOS_NOBODY : predecessor->st_serialno);

    if (predecessor == NULL)
	log("initiating Aggressive Mode #%lu, connection \"%s\""
	    , st->st_serialno, st->st_connection->name);
    else
	log("initiating Aggressive Mode #%lu to replace #%lu, connection \"%s\""
	    , st->st_serialno, predecessor->st_serialno, st->st_connection->name);

    /* set up reply */
    init_pbs(&reply, space, sizeof(space), "reply packet");

    /* HDR out */
    {
	struct isakmp_hdr hdr;

	memset(&hdr, '\0', sizeof(hdr));	/* default to 0 */
	hdr.isa_version = ISAKMP_MAJOR_VERSION << ISA_MAJ_SHIFT | ISAKMP_MINOR_VERSION;
	hdr.isa_np = ISAKMP_NEXT_SA;
	hdr.isa_xchg = ISAKMP_XCHG_AGGR;
	memcpy(hdr.isa_icookie, st->st_icookie, COOKIE_SIZE);
	/* R-cookie, flags and MessageID are left zero */

	if (!out_struct(&hdr, &isakmp_hdr_desc, &reply, &rbody))
	{
	    cur_state = NULL;
	    return STF_INTERNAL_ERROR;
	}
    }

    //zhangbin 2004.10.27
#if 0
    {
	    char ikebuf[256];
	    memset(ikebuf,0,sizeof(ikebuf));
	    if(c->alg_info_ike)
		    alg_info_snprint(ikebuf,sizeof(ikebuf),(struct alg_info *)c->alg_info_ike);
	    send_log("ike string!!!!");
	    send_log(ikebuf);
    }
#endif
    {
	    memset(ike_buf,0,sizeof(ike_buf));
	    sprintf(ike_buf,"%d%d%d!!!",c->alg_info_ike->ike[0].ike_ealg,c->alg_info_ike->ike[0].ike_halg,c->alg_info_ike->ike[0].ike_modp);
#ifdef ZB_DEBUG
	    send_log("ike buf!!!!");
	    send_log(ike_buf);
#endif
    }
    /* SA out */
    {
	u_char *sa_start = rbody.cur;
	lset_t auth_policy = policy & POLICY_ISAKMP_MASK;

#if 0
	if (auth_policy == LEMPTY)
	{
	    /* unspecified: figure out what we can manage */
	    if (get_preshared_secret(c) != NULL)
		auth_policy |= POLICY_PSK;

	    if (get_RSA_private_key(c) != NULL
	    && get_RSA_public_key(&c->that.id) != NULL)
		auth_policy |= POLICY_RSASIG;
	    /* Not clear what we should do if neither is possible.
	     * Perhaps we should not have entered negotiations at all.
	     */
	    if (auth_policy == LEMPTY)
	    {
		loglog(RC_LOG_SERIOUS, "we don't know how to Authenticate this connection");
		cur_state = NULL;
		return STF_INTERNAL_ERROR;
	    }
	}
#endif
	//zhangbin 2004.10.27
	//init_st_oakley(st, auth_policy);
	init_st_oakley(st, auth_policy,ike_buf);

	if (!out_sa(&rbody, &oakley_sadb_am, st, TRUE, TRUE, ISAKMP_NEXT_KE))
	{
	    return STF_INTERNAL_ERROR;
	    cur_state = NULL;
	}

	/* save initiator SA for later HASH */
	passert(st->st_p1isa.ptr == NULL);	/* no leak! */
	clonetochunk(st->st_p1isa, sa_start, rbody.cur - sa_start,
		     "sa in aggr_outI1");
    }

    /* KE out */
    if (!build_and_ship_KE(st, &st->st_gi, st->st_oakley.group,
			   &rbody, ISAKMP_NEXT_NONCE))
	return STF_INTERNAL_ERROR;

    /* Ni out */
    if (!build_and_ship_nonce(&st->st_ni, &rbody, ISAKMP_NEXT_ID, "Ni"))
	return STF_INTERNAL_ERROR;

    /* IDii out */
    {
	struct isakmp_ipsec_id id_hd;
	chunk_t id_b;
	pb_stream id_pbs;

	build_id_payload(&id_hd, &id_b, &st->st_connection->this);
	id_hd.isaiid_np = ISAKMP_NEXT_NONE;
	if (!out_struct(&id_hd, &isakmp_ipsec_identification_desc, &rbody, &id_pbs)
	|| !out_chunk(id_b, &id_pbs, "my identity"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&id_pbs);
    }

#ifdef NAT_TRAVERSAL
    if (nat_traversal_enabled) {
	/* Add supported NAT-Traversal VID */
	if (!nat_traversal_add_vid(ISAKMP_NEXT_NONE, &rbody)) {
	    reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}
    }
#endif

    /* finish message */

    close_message(&rbody);
    close_output_pbs(&reply);

    clonetochunk(st->st_tpacket, reply.start, pbs_offset(&reply),
		 "reply packet from aggr_outI1");

    /* Transmit */

    DBG_cond_dump(DBG_RAW, "sending:\n",
		  st->st_tpacket.ptr, st->st_tpacket.len);

    send_packet(st, "aggr_outI1");

    //zhangbin for VPN-log
#ifdef VPN_LOG
	{
	   char tx_ai1[100];
	    memset(tx_ai1,0,sizeof(tx_ai1));
	    sprintf(tx_ai1,"2nIKE[\"%s\"] Tx >> AG_I1 : %s SA, KE, NONCE, ID\n",c->name,ip_str(&c->that.host_addr));
	    send_log(tx_ai1);
	}
#endif

    /* Set up a retransmission event, half a minute henceforth */
    delete_event(st);
    event_schedule(EVENT_RETRANSMIT, EVENT_RETRANSMIT_DELAY_0, st);

    whack_log(RC_NEW_STATE + STATE_AGGR_I1,
	      "%s: initiate", enum_name(&state_names, st->st_state));
    cur_state = NULL;
    return STF_IGNORE;
}


void
ipsecdoi_initiate(int whack_sock
, struct connection *c
, lset_t policy
, unsigned long try
, so_serial_t replacing)
{
    /* If there's already an ISAKMP SA established, use that and
     * go directly to Quick Mode.
     * Note: there is no way to initiate with a Road Warrior.
     */
    struct state *st = find_phase1_state(c, FALSE);

    if (st == NULL)
    {
	initiator_function *initiator = c->policy & POLICY_AGGRESSIVE
					? aggr_outI1 : main_outI1;
	(void) initiator(whack_sock, c, NULL, policy, try);
    }
    else if (HAS_IPSEC_POLICY(policy))
    {
	if (!IS_ISAKMP_SA_ESTABLISHED(st->st_state))
	{
	    /* leave our Phase 2 negotiation pending */
#ifdef ZB_DEBUG
		send_log("no isakmp sa!!!");
#endif
	    add_pending(whack_sock, st, c, policy, try, replacing);
	}
	else
	{
	    /* ??? we assume that peer_nexthop_sin isn't important:
	     * we already have it from when we negotiated the ISAKMP SA!
	     * It isn't clear what to do with the error return.
	     */
#ifdef ZB_DEBUG
		send_log("quickoutI1!!!");
#endif
	    (void) quick_outI1(whack_sock, st, c, policy, try, replacing);
	}
    }
    else
    {
	close_any(whack_sock);
    }
    //zhangbin 2004.10.27
#ifdef ZB_DEBUG
    send_log("initiate!!!");
#endif
}

/* Replace SA with a fresh one that is similar
 *
 * Shares some logic with ipsecdoi_initiate, but not the same!
 * - we must not reuse the ISAKMP SA if we are trying to replace it!
 * - if trying to replace IPSEC SA, use ipsecdoi_initiate to build
 *   ISAKMP SA if needed.
 * - duplicate whack fd, if live.
 * Does not delete the old state -- someone else will do that.
 */
void
ipsecdoi_replace(struct state *st, unsigned long try)
{
    int whack_sock = dup_any(st->st_whack_sock);
    lset_t policy = st->st_policy;

    if (IS_PHASE1(st->st_state))
    {
	initiator_function *initiator = policy & POLICY_AGGRESSIVE
					? aggr_outI1 : main_outI1;
	passert(!HAS_IPSEC_POLICY(policy));
	(void) initiator(whack_sock, st->st_connection, st, policy, try);
#ifdef ZB_DEBUG
	send_log("in ipsecdoi_replace,after initiate!!");
#endif
	//zhangbin 2004.11.01
	//
	    /* leave our Phase 2 negotiation pending */
	   //zhs remove for ike_sa_timeout_bug
	   // add_pending(whack_sock, st, st->st_connection, policy, try, st->st_serialno);
	     {//add by zhs for ike_sa_timeout_bug
        	  struct state *ph2_st = find_ph2_st_by_ph1_st(st,PROTO_IPSEC_ESP);
         	 if(NULL != ph2_st)
          	{
                	char deb[100];
                	time_t rd = st->st_margin;
                	memset(deb,0,sizeof(deb));
                	sprintf(deb,"ike sa replace,and find st2:#%ld,rd=%ld\n",ph2_st->st_serialno,rd);
                	send_log(deb);
                	if( (EVENT_SA_REPLACE==ph2_st->st_event->ev_type)
                   		&& (ph2_st->st_event->ev_time > rd+10))
                	{
                 	/*We must delete_event for the state before adding event,so it will no
                  	 *leak memory and avoid process aborting at event_schendle*/
                        	delete_event(ph2_st);
                        	event_schedule(EVENT_SA_REPLACE,rd,ph2_st);
               		// ph2_st->is_ipsecsa_exp_imm = 1;
                        	memset(deb,0,sizeof(deb));
                        	sprintf(deb,"st2 #%ld:ev_type=%d,delay %lds\n",ph2_st->st_serialno,
                           	ph2_st->st_event->ev_type,ph2_st->st_event->ev_time-now());
                        	send_log(deb);
               		}
          	}
       	   }//end by zhs

    }
    else
    {
	/* Add features of actual old state to policy.  This ensures
	 * that rekeying doesn't downgrade security.  I admit that
	 * this doesn't capture everything.
	 */

#ifdef ZB_DEBUG
	    send_log("is_phase2!!!");
#endif
	if (st->st_pfs_group != NULL)
	    policy |= POLICY_PFS;
	if (st->st_ah.present)
	{
	    policy |= POLICY_AUTHENTICATE;
	    if (st->st_ah.attrs.encapsulation == ENCAPSULATION_MODE_TUNNEL)
		policy |= POLICY_TUNNEL;
	}
	if (st->st_esp.present && st->st_esp.attrs.transid != ESP_NULL)
	{
	    policy |= POLICY_ENCRYPT;
	    if (st->st_esp.attrs.encapsulation == ENCAPSULATION_MODE_TUNNEL)
		policy |= POLICY_TUNNEL;
	}
	if (st->st_ipcomp.present)
	{
	    policy |= POLICY_COMPRESS;
	    if (st->st_ipcomp.attrs.encapsulation == ENCAPSULATION_MODE_TUNNEL)
		policy |= POLICY_TUNNEL;
	}
	passert(HAS_IPSEC_POLICY(policy));
	ipsecdoi_initiate(whack_sock, st->st_connection, policy, try
	    , st->st_serialno);
    }
    //zhangbin
#ifdef ZB_DEBUG
    send_log("end of replace!!!");
#endif
}

/* SKEYID for preshared keys.
 * See draft-ietf-ipsec-ike-01.txt 4.1
 */
static bool
skeyid_preshared(struct state *st)
{
    const chunk_t *pss = get_preshared_secret(st->st_connection);

    if (pss == NULL)
    {
	loglog(RC_LOG_SERIOUS, "preshared secret disappeared!");
	return FALSE;
    }
    else
    {
	struct hmac_ctx ctx;

	hmac_init_chunk(&ctx, st->st_oakley.hasher, *pss);
	hmac_update_chunk(&ctx, st->st_ni);
	hmac_update_chunk(&ctx, st->st_nr);
	hmac_final_chunk(st->st_skeyid, "st_skeyid in skeyid_preshared()", &ctx);
	return TRUE;
    }
}

static bool
skeyid_digisig(struct state *st)
{
    struct hmac_ctx ctx;
    chunk_t nir;

    /* We need to hmac_init with the concatenation of Ni_b and Nr_b,
     * so we have to build a temporary concatentation.
     */
    nir.len = st->st_ni.len + st->st_nr.len;
    nir.ptr = alloc_bytes(nir.len, "Ni + Nr in skeyid_digisig");
    memcpy(nir.ptr, st->st_ni.ptr, st->st_ni.len);
    memcpy(nir.ptr+st->st_ni.len, st->st_nr.ptr, st->st_nr.len);
    hmac_init_chunk(&ctx, st->st_oakley.hasher, nir);
    pfree(nir.ptr);

    hmac_update_chunk(&ctx, st->st_shared);
    hmac_final_chunk(st->st_skeyid, "st_skeyid in skeyid_digisig()", &ctx);
    return TRUE;
}

/* Generate the SKEYID_* and new IV
 * See draft-ietf-ipsec-ike-01.txt 4.1
 */
static bool
generate_skeyids_iv(struct state *st)
{
    /* Generate the SKEYID */
    switch (st->st_oakley.auth)
    {
	case OAKLEY_PRESHARED_KEY:
	    if (!skeyid_preshared(st))
		return FALSE;
	    break;

	case OAKLEY_RSA_SIG:
	    if (!skeyid_digisig(st))
		return FALSE;
	    break;

	case OAKLEY_DSS_SIG:
	    /* XXX */

	case OAKLEY_RSA_ENC:
	case OAKLEY_RSA_ENC_REV:
	case OAKLEY_ELGAMAL_ENC:
	case OAKLEY_ELGAMAL_ENC_REV:
	    /* XXX */

	default:
	    exit_log("generate_skeyids_iv(): unsupported authentication method %s",
		enum_show(&oakley_auth_names, st->st_oakley.auth));
    }

    /* generate SKEYID_* from SKEYID */
    {
	struct hmac_ctx ctx;

	hmac_init_chunk(&ctx, st->st_oakley.hasher, st->st_skeyid);

	/* SKEYID_D */
	hmac_update_chunk(&ctx, st->st_shared);
	hmac_update(&ctx, st->st_icookie, COOKIE_SIZE);
	hmac_update(&ctx, st->st_rcookie, COOKIE_SIZE);
	hmac_update(&ctx, "\0", 1);
	hmac_final_chunk(st->st_skeyid_d, "st_skeyid_d in generate_skeyids_iv()", &ctx);

	/* SKEYID_A */
	hmac_reinit(&ctx);
	hmac_update_chunk(&ctx, st->st_skeyid_d);
	hmac_update_chunk(&ctx, st->st_shared);
	hmac_update(&ctx, st->st_icookie, COOKIE_SIZE);
	hmac_update(&ctx, st->st_rcookie, COOKIE_SIZE);
	hmac_update(&ctx, "\1", 1);
	hmac_final_chunk(st->st_skeyid_a, "st_skeyid_a in generate_skeyids_iv()", &ctx);

	/* SKEYID_E */
	hmac_reinit(&ctx);
	hmac_update_chunk(&ctx, st->st_skeyid_a);
	hmac_update_chunk(&ctx, st->st_shared);
	hmac_update(&ctx, st->st_icookie, COOKIE_SIZE);
	hmac_update(&ctx, st->st_rcookie, COOKIE_SIZE);
	hmac_update(&ctx, "\2", 1);
	hmac_final_chunk(st->st_skeyid_e, "st_skeyid_e in generate_skeyids_iv()", &ctx);
    }

    /* generate IV */
    {
	union hash_ctx hash_ctx;
	const struct hash_desc *h = st->st_oakley.hasher;

	st->st_new_iv_len = h->hash_digest_size;
	passert(st->st_new_iv_len <= sizeof(st->st_new_iv));

        DBG(DBG_CRYPT,
            DBG_dump_chunk("DH_i:", st->st_gi);
            DBG_dump_chunk("DH_r:", st->st_gr);
        );
	h->hash_init(&hash_ctx);
	h->hash_update(&hash_ctx, st->st_gi.ptr, st->st_gi.len);
	h->hash_update(&hash_ctx, st->st_gr.ptr, st->st_gr.len);
	h->hash_final(st->st_new_iv, &hash_ctx);
    }

    /* Oakley Keying Material
     * Derived from Skeyid_e: if it is not big enough, generate more
     * using the PRF.
     * See draft-ietf-ipsec-isakmp-oakley-07.txt Appendix B
     */
    {
	/* const size_t keysize = st->st_oakley.encrypter->keydeflen/BITS_PER_BYTE; */
	const size_t keysize = st->st_oakley.enckeylen/BITS_PER_BYTE;
	u_char keytemp[MAX_OAKLEY_KEY_LEN + MAX_DIGEST_LEN];
	u_char *k = st->st_skeyid_e.ptr;

	if (keysize > st->st_skeyid_e.len)
	{
	    struct hmac_ctx ctx;
	    size_t i = 0;

	    hmac_init_chunk(&ctx, st->st_oakley.hasher, st->st_skeyid_e);
	    hmac_update(&ctx, "\0", 1);
	    for (;;)
	    {
		hmac_final(&keytemp[i], &ctx);
		i += ctx.hmac_digest_size;
		if (i >= keysize)
		    break;
		hmac_reinit(&ctx);
		hmac_update(&ctx, &keytemp[i - ctx.hmac_digest_size], ctx.hmac_digest_size);
	    }
	    k = keytemp;
	}
	clonereplacechunk(st->st_enc_key, k, keysize, "st_enc_key");
    }

    DBG(DBG_CRYPT,
	DBG_dump_chunk("Skeyid:  ", st->st_skeyid);
	DBG_dump_chunk("Skeyid_d:", st->st_skeyid_d);
	DBG_dump_chunk("Skeyid_a:", st->st_skeyid_a);
	DBG_dump_chunk("Skeyid_e:", st->st_skeyid_e);
	DBG_dump_chunk("enc key:", st->st_enc_key);
	DBG_dump("IV:", st->st_new_iv, st->st_new_iv_len));
    return TRUE;
}

/* Generate HASH_I or HASH_R for ISAKMP Phase I.
 * This will *not* generate other hash payloads (eg. Phase II or Quick Mode,
 * New Group Mode, or ISAKMP Informational Exchanges).
 * If the hashi argument is TRUE, generate HASH_I; if FALSE generate HASH_R.
 * If hashus argument is TRUE, we're generating a hash for our end.
 * See RFC2409 IKE 5.
 *
 * Generating the SIG_I and SIG_R for DSS is an odd perversion of this:
 * Most of the logic is the same, but SHA-1 is used in place of HMAC-whatever.
 * The extensive common logic is embodied in main_mode_hash_body().
 * See draft-ietf-ipsec-ike-01.txt 4.1 and 6.1.1.2
 */

typedef void (*hash_update_t)(union hash_ctx *, const u_char *, size_t) ;
static void
main_mode_hash_body(struct state *st
, bool hashi	/* Initiator? */
, const pb_stream *idpl	/* ID payload, as PBS */
, union hash_ctx *ctx
, void (*hash_update_void)(void *, const u_char *input, size_t))
{
#define HASH_UPDATE_T (union hash_ctx *, const u_char *input, unsigned int len)
    hash_update_t hash_update=(hash_update_t)  hash_update_void;
#if 0	/* if desperate to debug hashing */
#   define hash_update(ctx, input, len) { \
	DBG_dump("hash input", input, len); \
	(hash_update)(ctx, input, len); \
	}
#endif

#   define hash_update_chunk(ctx, ch) hash_update((ctx), (ch).ptr, (ch).len)

    if (hashi)
    {
	hash_update_chunk(ctx, st->st_gi);
	hash_update_chunk(ctx, st->st_gr);
	hash_update(ctx, st->st_icookie, COOKIE_SIZE);
	hash_update(ctx, st->st_rcookie, COOKIE_SIZE);
    }
    else
    {
	hash_update_chunk(ctx, st->st_gr);
	hash_update_chunk(ctx, st->st_gi);
	hash_update(ctx, st->st_rcookie, COOKIE_SIZE);
	hash_update(ctx, st->st_icookie, COOKIE_SIZE);
    }

    DBG(DBG_CRYPT, DBG_log("hashing %d bytes of SA"
	, (int)(st->st_p1isa.len - sizeof(struct isakmp_generic))));

    /* SA_b */
    hash_update(ctx, st->st_p1isa.ptr + sizeof(struct isakmp_generic)
	, st->st_p1isa.len - sizeof(struct isakmp_generic));

    /* Hash identification payload, without generic payload header.
     * We used to reconstruct ID Payload for this purpose, but now
     * we use the bytes as they appear on the wire to avoid
     * "spelling problems".
     */
    hash_update(ctx
	, idpl->start + sizeof(struct isakmp_generic)
	, pbs_offset(idpl) - sizeof(struct isakmp_generic));

#   undef hash_update_chunk
#   undef hash_update
}

static size_t	/* length of hash */
main_mode_hash(struct state *st
, u_char *hash_val	/* resulting bytes */
, bool hashi	/* Initiator? */
, const pb_stream *idpl)	/* ID payload, as PBS; cur must be at end */
{
    struct hmac_ctx ctx;

    hmac_init_chunk(&ctx, st->st_oakley.hasher, st->st_skeyid);
    main_mode_hash_body(st, hashi, idpl, &ctx.hash_ctx, ctx.h->hash_update);
    hmac_final(hash_val, &ctx);
    return ctx.hmac_digest_size;
}

#if 0	/* only needed for DSS */
static void
main_mode_sha1(struct state *st
, u_char *hash_val	/* resulting bytes */
, size_t *hash_len	/* length of hash */
, bool hashi	/* Initiator? */
, const pb_stream *idpl)	/* ID payload, as PBS */
{
    union hash_ctx ctx;

    SHA1Init(&ctx.ctx_sha1);
    SHA1Update(&ctx.ctx_sha1, st->st_skeyid.ptr, st->st_skeyid.len);
    *hash_len = SHA1_DIGEST_SIZE;
    main_mode_hash_body(st, hashi, idpl, &ctx
	, (void (*)(union hash_ctx *, const u_char *, unsigned int))&SHA1Update);
    SHA1Final(hash_val, &ctx.ctx_sha1);
}
#endif

/* Create an RSA signature of a hash.
 * Poorly specified in draft-ietf-ipsec-ike-01.txt 6.1.1.2.
 * Use PKCS#1 version 1.5 encryption of hash (called
 * RSAES-PKCS1-V1_5) in PKCS#2.
 */
static size_t
RSA_sign_hash(struct connection *c
, u_char sig_val[RSA_MAX_OCTETS]
, const u_char *hash_val, size_t hash_len)
{
    const struct RSA_private_key *k = get_RSA_private_key(c);
    size_t sz;
    u_char *p = sig_val;
    size_t padlen;
    mpz_t t1, t2;
    chunk_t ch;

    if (k == NULL)
	return 0;	/* failure: no key to use */
    sz = k->pub.k;
    passert(RSA_MIN_OCTETS <= sz && 4 + hash_len < sz && sz <= RSA_MAX_OCTETS);

    DBG(DBG_CONTROL | DBG_CRYPT
	, DBG_log("signing hash with RSA Key *%s", k->pub.keyid));

    /* PKCS#1 v1.5 8.1 encryption-block formatting */
    *p++ = 0x00;
    *p++ = 0x01;	/* BT (block type) 01 */
    padlen = sz - 3 - hash_len;
    memset(p, 0xFF, padlen);
    p += padlen;
    *p++ = 0x00;
    memcpy(p, hash_val, hash_len);
    passert(p + hash_len - sig_val == (ptrdiff_t)sz);

    /* PKCS#1 v1.5 8.2 octet-string-to-integer conversion */
    n_to_mpz(t1, sig_val, sz);	/* (could skip leading 0x00) */

    /* PKCS#1 v1.5 8.3 RSA computation y = x^c mod n
     * Better described in PKCS#1 v2.0 5.1 RSADP.
     * There are two methods, depending on the form of the private key.
     * We use the one based on the Chinese Remainder Theorem.
     */
    mpz_init(t2);

    mpz_powm(t2, t1, &k->dP, &k->p);	/* m1 = c^dP mod p */

    mpz_powm(t1, t1, &k->dQ, &k->q);	/* m2 = c^dQ mod Q */

    mpz_sub(t2, t2, t1);	/* h = qInv (m1 - m2) mod p */
    mpz_mod(t2, t2, &k->p);
    mpz_mul(t2, t2, &k->qInv);
    mpz_mod(t2, t2, &k->p);

    mpz_mul(t2, t2, &k->q);	/* m = m2 + h q */
    mpz_add(t1, t1, t2);

    /* PKCS#1 v1.5 8.4 integer-to-octet-string conversion */
    ch = mpz_to_n(t1, sz);
    memcpy(sig_val, ch.ptr, sz);
    pfree(ch.ptr);

    mpz_clear(t1);
    mpz_clear(t2);
    return sz;
}

/* Check a Main Mode RSA Signature against computed hash using RSA public key k.
 *
 * As a side effect, on success, the public key is copied into the
 * state object to record the authenticator.
 *
 * Can fail because wrong public key is used or because hash disagrees.
 * We distinguish because diagnostics should also.
 *
 * The result is NULL if the Signature checked out.
 * Otherwise, the first character of the result indicates
 * how far along failure occurred.  A greater character signifies
 * greater progress.
 *
 * Classes:
 * 0	reserved for caller
 * 1	SIG length doesn't match key length -- wrong key
 * 2-8	malformed ECB after decryption -- probably wrong key
 * 9	decrypted hash != computed hash -- probably correct key
 *
 * Although the math should be the same for generating and checking signatures,
 * it is not: the knowledge of the private key allows more efficient (i.e.
 * different) computation for encryption.
 */
static err_t
try_RSA_signature(const u_char hash_val[MAX_DIGEST_LEN], size_t hash_len
, const pb_stream *sig_pbs, const struct RSA_public_key *k
, struct state *st)
{
    const u_char *sig_val = sig_pbs->cur;
    size_t sig_len = pbs_left(sig_pbs);
    u_char s[RSA_MAX_OCTETS];	/* for decrypted sig_val */
    u_char *hash_in_s = &s[sig_len - hash_len];

    /* decrypt the signature -- reversing RSA_sign_hash */
    if (sig_len != k->k)
    {
	/* XXX notification: INVALID_KEY_INFORMATION */
	return "1" "SIG length does not match public key length";
    }

    /* actual exponentiation; see PKCS#1 v2.0 5.1 */
    {
	chunk_t temp_s;
	mpz_t c;

	n_to_mpz(c, sig_val, sig_len);
	mpz_powm(c, c, &k->e, &k->n);

	temp_s = mpz_to_n(c, sig_len);	/* back to octets */
	memcpy(s, temp_s.ptr, sig_len);
	pfree(temp_s.ptr);
	mpz_clear(c);
    }

    /* sanity check on signature: see if it matches
     * PKCS#1 v1.5 8.1 encryption-block formatting
     */
    {
	err_t ugh = NULL;

	if (s[0] != 0x00)
	    ugh = "2" "no leading 00";
	else if (hash_in_s[-1] != 0x00)
	    ugh = "3" "00 separator not present";
	else if (s[1] == 0x01)
	{
	    const u_char *p;

	    for (p = &s[2]; p != hash_in_s - 1; p++)
	    {
		if (*p != 0xFF)
		{
		    ugh = "4" "invalid Padding String";
		    break;
		}
	    }
	}
	else if (s[1] == 0x02)
	{
	    const u_char *p;

	    for (p = &s[2]; p != hash_in_s - 1; p++)
	    {
		if (*p == 0x00)
		{
		    ugh = "5" "invalid Padding String";
		    break;
		}
	    }
	}
	else
	    ugh = "6" "Block Type not 01 or 02";

	if (ugh != NULL)
	{
	    /* note: it might be a good idea to make sure that
	     * an observer cannot tell what kind of failure happened.
	     * I don't know what this means in practice.
	     */
	    /* We probably selected the wrong public key for peer:
	     * SIG Payload decrypted into malformed ECB
	     */
	    /* XXX notification: INVALID_KEY_INFORMATION */
	    return ugh;
	}
    }

    /* We have the decoded hash: see if it matches. */
    if (memcmp(hash_val, hash_in_s, hash_len) != 0)
    {
	/* good: header, hash, signature, and other payloads well-formed
	 * good: we could find an RSA Sig key for the peer.
	 * bad: hash doesn't match
	 * Guess: sides disagree about key to be used.
	 */
	DBG_cond_dump(DBG_CRYPT, "decrypted SIG", s, sig_len);
	DBG_cond_dump(DBG_CRYPT, "computed HASH", hash_val, hash_len);
	/* XXX notification: INVALID_HASH_INFORMATION */
	return "9" "authentication failure: received SIG does not match computed HASH, but message is well-formed";
    }

    /* Success: copy successful key into state.
     * There might be an old one if we previously aborted this
     * state transition.
     */
    free_public_keys(&st->st_peer_pubkey);
    st->st_peer_pubkey = public_key_from_rsa(k);

    return NULL;    /* happy happy */
}

/* Check signature against all RSA public keys we can find.
 * If we need keys from DNS KEY records, and they haven't been fetched,
 * return STF_SUSPEND to ask for asynch DNS lookup.
 *
 * Note: global keys_from_dns contains results of DNS lookup for key
 * or is NULL indicating lookup not yet tried.
 *
 * take_a_crack is a helper function.  Mostly forensic.
 * If only we had coroutines.
 */
struct tac_state {
    /* RSA_check_signature's args that take_a_crack needs */
    struct state *st;
    const u_char *hash_val;
    size_t hash_len;
    const pb_stream *sig_pbs;

    /* state carried between calls */
    err_t best_ugh;	/* most successful failure */
    int tried_cnt;	/* number of keys tried */
    char tried[50];	/* keyids of tried public keys */
    char *tn;	/* roof of tried[] */
};

static bool
take_a_crack(struct tac_state *s
, const struct RSA_public_key *k
, const char *story)
{
    err_t ugh = try_RSA_signature(s->hash_val, s->hash_len, s->sig_pbs
	, k, s->st);

    s->tried_cnt++;
    if (ugh == NULL)
    {
	DBG(DBG_CRYPT | DBG_CONTROL
	    , DBG_log("an RSA Sig check passed with *%s [%s]"
		, k->keyid, story));
	return TRUE;
    }
    else
    {
	DBG(DBG_CRYPT
	    , DBG_log("an RSA Sig check failure %s with *%s [%s]"
		, ugh + 1, k->keyid, story));
	if (s->best_ugh == NULL || s->best_ugh[0] < ugh[0])
	    s->best_ugh = ugh;
	if (ugh[0] > '0'
	&& s->tn - s->tried + KEYID_BUF + 2 < (ptrdiff_t)sizeof(s->tried))
	{
	    strcpy(s->tn, " *");
	    strcpy(s->tn + 2, k->keyid);
	    s->tn += strlen(s->tn);
	}
	return FALSE;
    }
}

static stf_status
RSA_check_signature(struct state *st
, const u_char hash_val[MAX_DIGEST_LEN]
, size_t hash_len
, const pb_stream *sig_pbs)
{
    const struct connection *c = st->st_connection;
    struct tac_state s;
    err_t dns_ugh = NULL;

    s.st = st;
    s.hash_val = hash_val;
    s.hash_len = hash_len;
    s.sig_pbs = sig_pbs;

    s.best_ugh = NULL;
    s.tried_cnt = 0;
    s.tn = s.tried;

    /* try all gateway records hung off c */
    if ((c->policy & POLICY_OPPO))
    {
	struct gw_info *gw;

	for (gw = c->gw_info; gw != NULL; gw = gw->next)
	{
	    /* only consider entries that have a key and are for our peer */
	    if (gw->gw_key_present
	    && same_id(&gw->gw_id, &c->that.id)
	    && take_a_crack(&s, &gw->gw_key, "key saved from DNS TXT"))
		return STF_OK;
	}
    }

    /* try all appropriate Public keys */
    {
	struct pubkeyrec *p, **pp;
	int pathlen;
	char buf[IDTOA_BUF];

	pp = &pubkeys;

	DBG(DBG_CONTROL,
	    dntoa_or_null(buf, IDTOA_BUF, c->that.ca, "%any");
	    DBG_log("required CA is '%s'", buf);
	)

	for (p = pubkeys; p != NULL; p = *pp)
	{
	    if (p->alg == PUBKEY_ALG_RSA && same_id(&c->that.id, &p->id)
	    && trusted_ca(p->issuer, c->that.ca, &pathlen))
	    {
		time_t now;

		DBG(DBG_CONTROL,
		    dntoa_or_null(buf, IDTOA_BUF, p->issuer, "%any");
		    DBG_log("key issuer CA is '%s'", buf);
		)

		/* check if found public key has expired */
		time(&now);
		if (p->until != UNDEFINED_TIME && p->until < now)
		{
		    loglog(RC_LOG_SERIOUS,
			"cached RSA public key has expired and has been deleted");
		    *pp = free_public_key(p);
		    continue; /* continue with next public key */
		}

		if (take_a_crack(&s, &p->u.rsa, "preloaded key"))
		return STF_OK;
	    }
	    pp = &p->next;
	}
    }

    /* if no key was found (evidenced by best_ugh == NULL)
     * and that side of connection is key_from_DNS_on_demand
     * then go search DNS for keys for peer.
     */
    if (s.best_ugh == NULL && c->that.key_from_DNS_on_demand)
    {
	struct pubkeyrec *kr;

	if (keys_from_dns == NULL)
	    return STF_SUSPEND;	/* ask for asynch DNS lookup */

	for (kr = keys_from_dns; kr != NULL; kr = kr->next)
	{
	    if (kr->alg == PUBKEY_ALG_RSA)
	    {
		if (take_a_crack(&s, &kr->u.rsa, "key from DNS KEY"))
		{
		    free_public_keys(&keys_from_dns);
		    return STF_OK;
		}
	    }
	}

	free_public_keys(&keys_from_dns);
    }

    /* no acceptable key was found: diagnose */
    {
	char id_buf[IDTOA_BUF];	/* arbitrary limit on length of ID reported */

	(void) idtoa(&st->st_connection->that.id, id_buf, sizeof(id_buf));

	if (s.best_ugh == NULL)
	{
	    if (dns_ugh == NULL)
		loglog(RC_LOG_SERIOUS, "no RSA public key known for '%s'"
		    , id_buf);
	    else
		loglog(RC_LOG_SERIOUS, "no RSA public key known for '%s'"
		    "; DNS search for KEY failed (%s)"
		    , id_buf, dns_ugh);

	    /* ??? is this the best code there is? */
	    return STF_FAIL + INVALID_KEY_INFORMATION;
	}

	if (s.best_ugh[0] == '9')
	{
	    loglog(RC_LOG_SERIOUS, "%s", s.best_ugh + 1);
	    return STF_FAIL + INVALID_HASH_INFORMATION;
	}
	else
	{
	    if (s.tried_cnt == 1)
	    {
		loglog(RC_LOG_SERIOUS
		    , "Signature check (on %s) failed (wrong key?); tried%s"
		    , id_buf, s.tried);
		DBG(DBG_CONTROL,
		    DBG_log("public key for %s failed:"
			" decrypted SIG payload into a malformed ECB (%s)"
			, id_buf, s.best_ugh + 1));
	    }
	    else
	    {
		loglog(RC_LOG_SERIOUS
		    , "Signature check (on %s) failed:"
		      " tried%s keys but none worked."
		    , id_buf, s.tried);
		DBG(DBG_CONTROL,
		    DBG_log("all %d public keys for %s failed:"
			" best decrypted SIG payload into a malformed ECB (%s)"
			, s.tried_cnt, id_buf, s.best_ugh + 1));
	    }
	    return STF_FAIL + INVALID_KEY_INFORMATION;
	}
    }
}

/* CHECK_QUICK_HASH
 *
 * This macro is magic -- it cannot be expressed as a function.
 * - it causes the caller to return!
 * - it declares local variables and expects the "do_hash" argument
 *   expression to reference them (hash_val, hash_pbs)
 */
#define CHECK_QUICK_HASH(md, do_hash, hash_name, msg_name) { \
	pb_stream *const hash_pbs = &md->chain[ISAKMP_NEXT_HASH]->pbs; \
	u_char hash_val[MAX_DIGEST_LEN]; \
	size_t hash_len = do_hash; \
	if (pbs_left(hash_pbs) != hash_len \
	|| memcmp(hash_pbs->cur, hash_val, hash_len) != 0) \
	{ \
	    DBG_cond_dump(DBG_CRYPT, "received " hash_name ":", hash_pbs->cur, pbs_left(hash_pbs)); \
	    loglog(RC_LOG_SERIOUS, "received " hash_name " does not match computed value in " msg_name); \
	    return STF_FAIL + INVALID_HASH_INFORMATION; \
	} \
    }

static notification_t
accept_nonce(struct msg_digest *md, chunk_t *dest, const char *name)
{
    pb_stream *nonce_pbs = &md->chain[ISAKMP_NEXT_NONCE]->pbs;
    size_t len = pbs_left(nonce_pbs);

    if (len < MINIMUM_NONCE_SIZE || MAXIMUM_NONCE_SIZE < len)
    {
	loglog(RC_LOG_SERIOUS, "%s length not between %d and %d"
	    , name , MINIMUM_NONCE_SIZE, MAXIMUM_NONCE_SIZE);
	return PAYLOAD_MALFORMED;	/* ??? */
    }
    clonereplacechunk(*dest, nonce_pbs->cur, len, "nonce");
    return NOTHING_WRONG;
}

/* START_HASH_PAYLOAD
 *
 * Emit a to-be-filled-in hash payload, noting the field start (r_hashval)
 * and the start of the part of the message to be hashed (r_hash_start).
 * This macro is magic.
 * - it can cause the caller to return
 * - it references variables local to the caller (r_hashval, r_hash_start, st)
 */
#define START_HASH_PAYLOAD(rbody, np) { \
    pb_stream hash_pbs; \
    if (!out_generic(np, &isakmp_hash_desc, &(rbody), &hash_pbs)) \
	return STF_INTERNAL_ERROR; \
    r_hashval = hash_pbs.cur;	/* remember where to plant value */ \
    if (!out_zero(st->st_oakley.hasher->hash_digest_size, &hash_pbs, "HASH")) \
	return STF_INTERNAL_ERROR; \
    close_output_pbs(&hash_pbs); \
    r_hash_start = (rbody).cur;	/* hash from after HASH payload */ \
}

/* encrypt message, sans fixed part of header
 * IV is fetched from st->st_new_iv and stored into st->st_iv.
 * The theory is that there will be no "backing out", so we commit to IV.
 * We also close the pbs.
 */
static bool
encrypt_message(pb_stream *pbs, struct state *st)
{
    const struct encrypt_desc *e = st->st_oakley.encrypter;
    u_int8_t *enc_start = pbs->start + sizeof(struct isakmp_hdr);
    size_t enc_len = pbs_offset(pbs) - sizeof(struct isakmp_hdr);

    DBG_cond_dump(DBG_CRYPT | DBG_RAW, "encrypting:\n", enc_start, enc_len);

    /* Pad up to multiple of encryption blocksize.
     * See the description associated with the definition of
     * struct isakmp_hdr in packet.h.
     */
    {
	size_t padding = pad_up(enc_len, e->enc_blocksize);

	if (padding != 0)
	{
	    if (!out_zero(padding, pbs, "encryption padding"))
		return FALSE;
	    enc_len += padding;
	}
    }

    DBG(DBG_CRYPT, DBG_log("encrypting using %s", enum_show(&oakley_enc_names, st->st_oakley.encrypt)));

    /* e->crypt(TRUE, enc_start, enc_len, st); */
    crypto_cbc_encrypt(e, TRUE, enc_start, enc_len, st);

    update_iv(st);
    DBG_cond_dump(DBG_CRYPT, "next IV:", st->st_iv, st->st_iv_len);
    close_message(pbs);
    return TRUE;
}

/* Compute HASH(1), HASH(2) of Quick Mode.
 * HASH(1) is part of Quick I1 message.
 * HASH(2) is part of Quick R1 message.
 * Used by: quick_outI1, quick_inI1_outR1 (twice), quick_inR1_outI2
 * (see draft-ietf-ipsec-isakmp-oakley-07.txt 5.5)
 */
static size_t
quick_mode_hash12(u_char *dest, const u_char *start, const u_char *roof
, const struct state *st, const msgid_t *msgid, bool hash2)
{
    struct hmac_ctx ctx;

#if 0	/* if desperate to debug hashing */
#   define hmac_update(ctx, ptr, len) { \
	DBG_dump("hash input", (ptr), (len)); \
	(hmac_update)((ctx), (ptr), (len)); \
    }
    DBG_dump("hash key", st->st_skeyid_a.ptr, st->st_skeyid_a.len);
#endif
    hmac_init_chunk(&ctx, st->st_oakley.hasher, st->st_skeyid_a);
    hmac_update(&ctx, (const void *) msgid, sizeof(msgid_t));
    if (hash2)
	hmac_update_chunk(&ctx, st->st_ni);	/* include Ni_b in the hash */
    hmac_update(&ctx, start, roof-start);
    hmac_final(dest, &ctx);

    DBG(DBG_CRYPT,
	DBG_log("HASH(%d) computed:", hash2 + 1);
	DBG_dump("", dest, ctx.hmac_digest_size));
    return ctx.hmac_digest_size;
#   undef hmac_update
}

/* Compute HASH(3) in Quick Mode (part of Quick I2 message).
 * Used by: quick_inR1_outI2, quick_inI2
 * See RFC2409 "The Internet Key Exchange (IKE)" 5.5.
 * NOTE: this hash (unlike HASH(1) and HASH(2)) ONLY covers the
 * Message ID and Nonces.  This is a mistake.
 */
static size_t
quick_mode_hash3(u_char *dest, struct state *st)
{
    struct hmac_ctx ctx;

    hmac_init_chunk(&ctx, st->st_oakley.hasher, st->st_skeyid_a);
    hmac_update(&ctx, "\0", 1);
    hmac_update(&ctx, (u_char *) &st->st_msgid, sizeof(st->st_msgid));
    hmac_update_chunk(&ctx, st->st_ni);
    hmac_update_chunk(&ctx, st->st_nr);
    hmac_final(dest, &ctx);
    DBG_cond_dump(DBG_CRYPT, "HASH(3) computed:", dest, ctx.hmac_digest_size);
    return ctx.hmac_digest_size;
}

/* Compute Phase 2 IV.
 * Uses Phase 1 IV from st_iv; puts result in st_new_iv.
 */
void
init_phase2_iv(struct state *st, const msgid_t *msgid)
{
    const struct hash_desc *h = st->st_oakley.hasher;
    union hash_ctx ctx;

    DBG_cond_dump(DBG_CRYPT, "last Phase 1 IV:"
	, st->st_iv, st->st_iv_len);

    st->st_new_iv_len = h->hash_digest_size;
    passert(st->st_new_iv_len <= sizeof(st->st_new_iv));

    h->hash_init(&ctx);
    h->hash_update(&ctx, st->st_iv, st->st_iv_len);
    passert(*msgid != 0);
    h->hash_update(&ctx, (const u_char *)msgid, sizeof(*msgid));
    h->hash_final(st->st_new_iv, &ctx);

    DBG_cond_dump(DBG_CRYPT, "computed Phase 2 IV:"
	, st->st_new_iv, st->st_new_iv_len);
}

/* Initiate quick mode.
 * --> HDR*, HASH(1), SA, Nr [, KE ] [, IDci, IDcr ]
 * (see draft-ietf-ipsec-isakmp-oakley-07.txt 5.5)
 * Note: this is not called from demux.c
 */
//zhangbin
static bool
emit_ipaddr_id(ip_subnet *net
, u_int8_t np, u_int8_t protoid, u_int16_t port, pb_stream *outs)
{
    struct isakmp_ipsec_id id;
    pb_stream id_pbs;
    ip_address ta;
    const unsigned char *tbp;
    size_t tal;

    id.isaiid_np = np;
    id.isaiid_idtype = aftoinfo(subnettypeof(net))->id_addr;
    id.isaiid_protoid = protoid;
    id.isaiid_port = port;

    //zhangbin 2004.10.27
#ifdef ZB_DEBUG
    {
	    char tbuf[40];
	    memset(tbuf,0,sizeof(tbuf));
	    sprintf(tbuf,"protoid=%d,port=%d",protoid,port);//zhs md port=%s to port=%d for warning msg,2005-4-18
	    send_log(tbuf);
    }
#endif

    if (!out_struct(&id, &isakmp_ipsec_identification_desc, outs, &id_pbs))
	return FALSE;

    networkof(net, &ta);
    tal = addrbytesptr(&ta, &tbp);
    //zhangbin 2004.10.27
#ifdef ZB_DEBUG
    {
	    char tmpbuf[40];
	    memset(tmpbuf,0,sizeof(tmpbuf));
	    sprintf(tmpbuf,"tal=%d,tbp=%s",tal,tbp);
	    send_log(tmpbuf);
    }
#endif
    if (!out_raw(tbp, tal, &id_pbs, "client network"))
	return FALSE;

    maskof(net, &ta);
    tal = addrbytesptr(&ta, &tbp);
    if (!out_raw(tbp, tal, &id_pbs, "client mask"))
	return FALSE;

    close_output_pbs(&id_pbs);
    return TRUE;
}

static bool
emit_subnet_id(ip_subnet *net
, u_int8_t np, u_int8_t protoid, u_int16_t port, pb_stream *outs)
{
    struct isakmp_ipsec_id id;
    pb_stream id_pbs;
    ip_address ta;
    const unsigned char *tbp;
    size_t tal;

    id.isaiid_np = np;
    id.isaiid_idtype = aftoinfo(subnettypeof(net))->id_subnet;
    id.isaiid_protoid = protoid;
    id.isaiid_port = port;

    if (!out_struct(&id, &isakmp_ipsec_identification_desc, outs, &id_pbs))
	return FALSE;

    networkof(net, &ta);
    tal = addrbytesptr(&ta, &tbp);
    if (!out_raw(tbp, tal, &id_pbs, "client network"))
	return FALSE;

    maskof(net, &ta);
    tal = addrbytesptr(&ta, &tbp);
    if (!out_raw(tbp, tal, &id_pbs, "client mask"))
	return FALSE;

    close_output_pbs(&id_pbs);
    return TRUE;
}

stf_status
quick_outI1(int whack_sock
, struct state *isakmp_sa
, struct connection *c
, lset_t policy
, unsigned long try
, so_serial_t replacing)
{
    struct state *st = duplicate_state(isakmp_sa);
    pb_stream reply;	/* not really a reply */
    pb_stream rbody;
    u_char	/* set by START_HASH_PAYLOAD: */
	*r_hashval,	/* where in reply to jam hash value */
	*r_hash_start;	/* start of what is to be hashed */
    bool has_client = c->this.has_client ||  c->that.has_client ||
		      c->this.protocol || c->that.protocol ||
		      c->this.port || c->that.port;
    st->st_whack_sock = whack_sock;
    st->st_connection = c;
    set_cur_state(st);	/* we must reset before exit */
    st->st_policy = policy;
    st->st_try = try;

    st->st_myuserprotoid = c->this.protocol;
    st->st_peeruserprotoid = c->that.protocol;
    st->st_myuserport = c->this.port;
    st->st_peeruserport = c->that.port;

    st->st_msgid = generate_msgid(isakmp_sa);
    st->st_state = STATE_QUICK_I1;

    insert_state(st);	/* needs cookies, connection, and msgid */

    if (replacing == SOS_NOBODY)
	log("initiating Quick Mode %s"
	    , bitnamesof(sa_policy_bit_names, policy));
    else
	log("initiating Quick Mode %s to replace #%lu"
	    , bitnamesof(sa_policy_bit_names, policy), replacing);

#ifdef NAT_TRAVERSAL
    if (isakmp_sa->nat_traversal & NAT_T_DETECTED) {
	/* Duplicate nat_traversal status in new state */
	st->nat_traversal = isakmp_sa->nat_traversal;
	if (isakmp_sa->nat_traversal & LELEM(NAT_TRAVERSAL_NAT_BHND_ME)) {
	    has_client = TRUE;
	}
	nat_traversal_change_port_lookup(NULL, st);
    }
    else {
	st->nat_traversal = 0;
    }
#endif

    /* set up reply */
    init_pbs(&reply, reply_buffer, sizeof(reply_buffer), "reply packet");

    /* HDR* out */
    {
	struct isakmp_hdr hdr;

	hdr.isa_version = ISAKMP_MAJOR_VERSION << ISA_MAJ_SHIFT | ISAKMP_MINOR_VERSION;
	hdr.isa_np = ISAKMP_NEXT_HASH;
	hdr.isa_xchg = ISAKMP_XCHG_QUICK;
	hdr.isa_msgid = st->st_msgid;
	hdr.isa_flags = ISAKMP_FLAG_ENCRYPTION;
	memcpy(hdr.isa_icookie, st->st_icookie, COOKIE_SIZE);
	memcpy(hdr.isa_rcookie, st->st_rcookie, COOKIE_SIZE);
	if (!out_struct(&hdr, &isakmp_hdr_desc, &reply, &rbody))
	{
	    reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}
    }

    /* HASH(1) -- create and note space to be filled later */
    START_HASH_PAYLOAD(rbody, ISAKMP_NEXT_SA);

    /* SA out */

    /* 
     * See if pfs_group has been specified for this conn,
     * if not, fallback to old use-same-as-P1 behaviour
     */
#ifndef NO_IKE_ALG
    if (st->st_connection)
	    st->st_pfs_group = ike_alg_pfsgroup(st->st_connection, policy);
    if (!st->st_pfs_group)
#endif
    /* If PFS specified, use the same group as during Phase 1:
     * since no negotiation is possible, we pick one that is
     * very likely supported.
     */
	    st->st_pfs_group = policy & POLICY_PFS? isakmp_sa->st_oakley.group : NULL;

    /* Emit SA payload based on a subset of the policy bits.
     * POLICY_COMPRESS is considered iff we can do IPcomp.
     */
    {
	lset_t pm = POLICY_ENCRYPT | POLICY_AUTHENTICATE;

	if (can_do_IPcomp)
	    pm |= POLICY_COMPRESS;

	if (!out_sa(&rbody
	, &ipsec_sadb[(st->st_policy & pm) >> POLICY_IPSEC_SHIFT]
	, st, FALSE, FALSE, ISAKMP_NEXT_NONCE))
	{
	    reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}
    }

    /* Ni out */
    if (!build_and_ship_nonce(&st->st_ni, &rbody
    , policy & POLICY_PFS? ISAKMP_NEXT_KE : has_client? ISAKMP_NEXT_ID : ISAKMP_NEXT_NONE
    , "Ni"))
    {
	reset_cur_state();
	return STF_INTERNAL_ERROR;
    }

    /* [ KE ] out (for PFS) */

    if (st->st_pfs_group != NULL)
    {
	if (!build_and_ship_KE(st, &st->st_gi, st->st_pfs_group
	, &rbody, has_client? ISAKMP_NEXT_ID : ISAKMP_NEXT_NONE))
	{
	    reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}
    }
//zhangbin
log("*this.maskbits=%d,*that.maskbits=%d",c->this.client.maskbits,c->that.client.maskbits);
    /* [ IDci, IDcr ] out */
//zhangbin
    if (c->this.client.maskbits == 32 && c->that.client.maskbits == 32)
    {
	//zhangbin
	log("IP---IP!!!");
	/* IDci (we are initiator), then IDcr (peer is responder) */
	if (!emit_ipaddr_id(&c->this.client
	  , ISAKMP_NEXT_ID, st->st_myuserprotoid, st->st_myuserport, &rbody)
	|| !emit_ipaddr_id(&c->that.client
	  , ISAKMP_NEXT_NONE, st->st_peeruserprotoid, st->st_peeruserport, &rbody))
	{
	    reset_cur_state();
	    //zhangbin
	    log("this-ip,that-ip:internal error");
	    return STF_INTERNAL_ERROR;
	}

} 
//zhangbin
    if (c->this.client.maskbits == 32 && c->that.client.maskbits != 32)
    {
//zhangbin
log("this-ipaddr,that-sbunet!!!");

	/* IDci (we are initiator), then IDcr (peer is responder) */
	if (!emit_ipaddr_id(&c->this.client
	  , ISAKMP_NEXT_ID, st->st_myuserprotoid, st->st_myuserport, &rbody)
	|| !emit_subnet_id(&c->that.client
	  , ISAKMP_NEXT_NONE, st->st_peeruserprotoid, st->st_peeruserport, &rbody))
	{
	    reset_cur_state();
	    //zhangbin
	    log("this-ip,that-sub:internal error");
	    return STF_INTERNAL_ERROR;
	}

} 

//zhangbin
    if (c->this.client.maskbits != 32 && has_client)
    {

    if (c->that.client.maskbits == 32)
	{
		//zhangbin
log("this-subnet,that-ipaddr!!!");

	if (!emit_subnet_id(&c->this.client
	  , ISAKMP_NEXT_ID, st->st_myuserprotoid, st->st_myuserport, &rbody)
	|| !emit_ipaddr_id(&c->that.client
	  , ISAKMP_NEXT_NONE, st->st_peeruserprotoid, st->st_peeruserport, &rbody))
	{
	    reset_cur_state();
	    //zhangbin
	    log("this-sub,that-ip:internal error");
	    return STF_INTERNAL_ERROR;
	}
	}
	else
	{
		//zhangbin
log("this-subnet,that-subnet!!!");
	/* IDci (we are initiator), then IDcr (peer is responder) */
	if (!emit_subnet_id(&c->this.client
	  , ISAKMP_NEXT_ID, st->st_myuserprotoid, st->st_myuserport, &rbody)
	|| !emit_subnet_id(&c->that.client
	  , ISAKMP_NEXT_NONE, st->st_peeruserprotoid, st->st_peeruserport, &rbody))
	{
    		//zhangbin
	    log("this-sub,that-sub:internal error");

	    reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}
	}

} 
#ifdef NAT_TRAVERSAL
    if ((st->nat_traversal & NAT_T_WITH_NATOA) &&
	(!(st->st_policy & POLICY_TUNNEL)) &&
	(st->nat_traversal & LELEM(NAT_TRAVERSAL_NAT_BHND_ME))) {
	/** Send NAT-OA if our address is NATed */
	if (!nat_traversal_add_natoa(ISAKMP_NEXT_NONE, &rbody, st)) {
        reset_cur_state();
	    return STF_INTERNAL_ERROR;
	}
    }
#endif

    /* finish computing  HASH(1), inserting it in output */
    (void) quick_mode_hash12(r_hashval, r_hash_start, rbody.cur
	, st, &st->st_msgid, FALSE);

    /* encrypt message, except for fixed part of header */

    init_phase2_iv(isakmp_sa, &st->st_msgid);
    st->st_new_iv_len = isakmp_sa->st_new_iv_len;
    memcpy(st->st_new_iv, isakmp_sa->st_new_iv, st->st_new_iv_len);

    if (!encrypt_message(&rbody, st))
    {
	reset_cur_state();
	return STF_INTERNAL_ERROR;
    }

    /* save packet, now that we know its size */
    clonetochunk(st->st_tpacket, reply.start, pbs_offset(&reply)
	, "reply packet from quick_outI1");

    /* send the packet */

    send_packet(st, "quick_outI1");
    //zhangbin for vpn-log
#ifdef VPN_LOG
{
	char tx_qi1[100];
    memset(tx_qi1,0,sizeof(tx_qi1));
    sprintf(tx_qi1,"2nIKE[\"%s\"] Tx >> QM_I1 : %s HASH, SA, NONCE, KE, ID, ID\n",st->st_connection->name,ip_str(&st->st_connection->that.host_addr));
    send_log(tx_qi1);
}
#endif

    delete_event(st);
    event_schedule(EVENT_RETRANSMIT, EVENT_RETRANSMIT_DELAY_0, st);

    if (replacing == SOS_NOBODY)
	whack_log(RC_NEW_STATE + STATE_QUICK_I1
	    , "%s: initiate"
	    , enum_name(&state_names, st->st_state));
    else
	whack_log(RC_NEW_STATE + STATE_QUICK_I1
	    , "%s: initiate to replace #%lu"
	    , enum_name(&state_names, st->st_state)
	    , replacing);
    reset_cur_state();
    return STF_OK;
}


/*
 * Decode the CERT payload of Phase 1.
 */
static void
decode_cert(struct msg_digest *md)
{
    struct payload_digest *p;

    for (p = md->chain[ISAKMP_NEXT_CERT]; p != NULL; p = p->next)
    {
	struct isakmp_cert *const cert = &p->payload.cert;
	chunk_t blob;
	time_t valid_until;
	blob.ptr = p->pbs.cur;
	blob.len = pbs_left(&p->pbs);
	if (cert->isacert_type == CERT_X509_SIGNATURE)
	{
	    x509cert_t cert = empty_x509cert;
	    if (parse_x509cert(blob, 0, &cert))
	    {
		if (verify_x509cert(&cert, strict_crl_policy, &valid_until))
		{
		    DBG(DBG_PARSING,
			DBG_log("Public key validated")
		    )
		    add_x509_public_key(&cert, valid_until, DAL_SIGNED);
		}
		else
		{
		    log("X.509 certificate rejected");
		}
		free_generalNames(cert.subjectAltName);
		free_generalNames(cert.crlDistributionPoints);
	    }
	    else
		log("Error in X.509 certificate");
	}
	else if (cert->isacert_type == CERT_PKCS7_WRAPPED_X509)
	{
	    x509cert_t *cert = NULL;

	    if (parse_pkcs7_cert(blob, &cert))
		store_x509certs(&cert, strict_crl_policy);
	    else
		log("Error in PKCS#7 wrapped X.509 certificates");
	}
	else
	{
	    loglog(RC_LOG_SERIOUS, "ignoring %s certificate payload",
		   enum_show(&cert_type_names, cert->isacert_type));
	    DBG_cond_dump_chunk(DBG_PARSING, "CERT:\n", blob);
	}
    }
}

/*
 * Decode the CR payload of Phase 1.
 */
static void
decode_cr(struct msg_digest *md, chunk_t *requested_ca)
{
    struct payload_digest *p;

    *requested_ca = empty_chunk;

    for (p = md->chain[ISAKMP_NEXT_CR]; p != NULL; p = p->next)
    {
	struct isakmp_cr *const cr = &p->payload.cr;

	requested_ca->len = pbs_left(&p->pbs);
	requested_ca->ptr = (requested_ca->len > 0)? p->pbs.cur : NULL;

	DBG_cond_dump_chunk(DBG_PARSING, "CR", *requested_ca);

	if (cr->isacr_type == CERT_X509_SIGNATURE)
	{
	    char buf[IDTOA_BUF];

	    DBG(DBG_PARSING | DBG_CONTROL,
		dntoa_or_null(buf, IDTOA_BUF, *requested_ca, "%any");
		DBG_log("requested CA: '%s'", buf);
	    )
	}
	else
	    loglog(RC_LOG_SERIOUS, "ignoring %s certificate request payload",
		   enum_show(&cert_type_names, cr->isacr_type));
    }
}

/* Decode the ID payload of Phase 1 (main_inI3_outR3 and main_inR3)
 * Note: we may change connections as a result.
 * We must be called before SIG or HASH are decoded since we
 * may change the peer's RSA key or ID.
 */
static bool
decode_peer_id(struct msg_digest *md, bool initiator, bool aggrmode)
{
    struct state *const st = md->st;
    struct payload_digest *const id_pld = md->chain[ISAKMP_NEXT_ID];
    const pb_stream *const id_pbs = &id_pld->pbs;
    struct isakmp_id *const id = &id_pld->payload.id;
    struct id peer;

    /* I think that RFC2407 (IPSEC DOI) 4.6.2 is confused.
     * It talks about the protocol ID and Port fields of the ID
     * Payload, but they don't exist as such in Phase 1.
     * We use more appropriate names.
     * isaid_doi_specific_a is in place of Protocol ID.
     * isaid_doi_specific_b is in place of Port.
     * Besides, there is no good reason for allowing these to be
     * other than 0 in Phase 1.
     */
#ifdef NAT_TRAVERSAL
    if ((st->nat_traversal & NAT_T_WITH_PORT_FLOATING) &&
	(id->isaid_doi_specific_a == IPPROTO_UDP) &&
	((id->isaid_doi_specific_b == 0) || (id->isaid_doi_specific_b == NAT_T_IKE_FLOAT_PORT))) {
	    DBG_log("protocol/port in Phase 1 ID Payload is %d/%d. "
		"accepted with port_floating NAT-T",
		id->isaid_doi_specific_a, id->isaid_doi_specific_b);
    }
    else
#endif
    if (!(id->isaid_doi_specific_a == 0 && id->isaid_doi_specific_b == 0)
    && !(id->isaid_doi_specific_a == IPPROTO_UDP && id->isaid_doi_specific_b == IKE_UDP_PORT))
    {
	loglog(RC_LOG_SERIOUS, "protocol/port in Phase 1 ID Payload must be 0/0 or %d/%d"
	    " but are %d/%d"
	    , IPPROTO_UDP, IKE_UDP_PORT
	    , id->isaid_doi_specific_a, id->isaid_doi_specific_b);
	return FALSE;
    }

    peer.kind = id->isaid_idtype;

    switch (peer.kind)
    {
    case ID_IPV4_ADDR:
    case ID_IPV6_ADDR:
	/* failure mode for initaddr is probably inappropriate address length */
	{
	    err_t ugh = initaddr(id_pbs->cur, pbs_left(id_pbs)
		, peer.kind == ID_IPV4_ADDR? AF_INET : AF_INET6
		, &peer.ip_addr);

	    if (ugh != NULL)
	    {
		loglog(RC_LOG_SERIOUS, "improper %s identification payload: %s"
		    , enum_show(&ident_names, peer.kind), ugh);
		return FALSE;
	    }
	}
	break;

    case ID_USER_FQDN:
	if (memchr(id_pbs->cur, '@', pbs_left(id_pbs)) == NULL)
	{
	    loglog(RC_LOG_SERIOUS, "peer's ID_USER_FQDN contains no @");
	    return FALSE;
	}
	/* FALLTHROUGH */
    case ID_FQDN:
	if (memchr(id_pbs->cur, '\0', pbs_left(id_pbs)) != NULL)
	{
	    loglog(RC_LOG_SERIOUS, "Phase 1 ID Payload of type %s contains a NUL"
		, enum_show(&ident_names, peer.kind));
	    return FALSE;
	}

	/* ??? ought to do some more sanity check, but what? */

	setchunk(peer.name, id_pbs->cur, pbs_left(id_pbs));
	break;

    case ID_KEY_ID:
	setchunk(peer.name, id_pbs->cur, pbs_left(id_pbs));
	DBG(DBG_PARSING,
 	    DBG_dump_chunk("KEY ID:", peer.name));
	break;	

    case ID_DER_ASN1_DN:
	setchunk(peer.name, id_pbs->cur, pbs_left(id_pbs));
 	DBG(DBG_PARSING,
 	    DBG_dump_chunk("DER ASN1 DN:", peer.name));
	break;	
	    	
    default:
	loglog(RC_LOG_SERIOUS, "Unacceptable identity type (%s) in Phase 1 ID Payload"
	    , enum_show(&ident_names, peer.kind));
	return FALSE;
    }

    {
	char buf[IDTOA_BUF];
	idtoa(&peer, buf, sizeof(buf));
	log("%s mode peer ID is %s: '%s'",
	    aggrmode ? "Aggressive" : "Main",
	    enum_show(&ident_names, id->isaid_idtype), buf);
    }

     /* check for certificates */
     decode_cert(md);

    /* Now that we've decoded the ID payload, let's see if we
     * need to switch connections.
     * We must not switch horses if we initiated:
     * - if the initiation was explicit, we'd be ignoring user's intent
     * - if opportunistic, we'll lose our HOLD info
     */
    if (initiator)
    {
	if (!same_id(&st->st_connection->that.id, &peer))
	{
	    char expect[IDTOA_BUF]
		, found[IDTOA_BUF];

	    idtoa(&st->st_connection->that.id, expect, sizeof(expect));
	    idtoa(&peer, found, sizeof(found));
	    loglog(RC_LOG_SERIOUS
		, "we require peer to have ID '%s', but peer declares '%s'"
		, expect, found);
	    return FALSE;
	}
    }
    //zhangbin 2004.12.13
#if 0
    else
    {
	chunk_t requested_ca;
	struct connection *c = st->st_connection;
	struct connection *r;

        /* check for certificate requests */
        decode_cr(md, &requested_ca);
  
        r = refine_host_connection(st, &peer, requested_ca, initiator, aggrmode);

	if (r == NULL)
	{
	    char buf[IDTOA_BUF];

	    idtoa(&peer, buf, sizeof(buf));
	    loglog(RC_LOG_SERIOUS, "no suitable connection for peer '%s'", buf);
	    return FALSE;
	}

	DBG(DBG_CONTROL,
	    char buf[IDTOA_BUF];

	    dntoa_or_null(buf, IDTOA_BUF, r->this.ca, "%none");
	    DBG_log("offered CA: '%s'", buf);
	)

	if (r != c)
	{
	    /* apparently, r is an improvement on c -- replace */

	    DBG(DBG_CONTROL
		, DBG_log("switched from \"%s\" to \"%s\"", c->name, r->name));
	    if (r->kind == CK_TEMPLATE)
	    {
		/* instantiate it, filling in peer's ID */
		r = rw_instantiate(r, &c->that.host_addr,
#ifdef NAT_TRAVERSAL
			c->that.host_port,
#endif
#ifdef VIRTUAL_IP
			NULL,
#endif
			&peer);
	    }

	    st->st_connection = r;	/* kill reference to c */
	    set_cur_connection(r);
	    connection_discard(c);
	}
	else if (c->that.has_id_wildcards)
	{
	    free_id_content(&c->that.id);
	    c->that.id = peer;
	    c->that.has_id_wildcards = FALSE;
	    unshare_id_content(&c->that.id);
	}
    }
#endif
    return TRUE;
}

/* Decode the variable part of an ID packet (during Quick Mode).
 * This is designed for packets that identify clients, not peers.
 */
static bool
decode_net_id(struct isakmp_ipsec_id *id
, pb_stream *id_pbs
, ip_subnet *net
, const char *which)
{
    const struct af_info *afi = NULL;

    /* Note: the following may be a pointer into static memory
     * that may be recycled, but only if the type is not known.
     * That case is disposed of very early -- in the first switch.
     */
    const char *idtypename = enum_show(&ident_names, id->isaiid_idtype);

    switch (id->isaiid_idtype)
    {
	case ID_FQDN:
	case ID_IPV4_ADDR:
	case ID_IPV4_ADDR_SUBNET:
	case ID_IPV4_ADDR_RANGE:
	    afi = &af_inet4_info;
	    break;
	case ID_IPV6_ADDR:
	case ID_IPV6_ADDR_SUBNET:
	case ID_IPV6_ADDR_RANGE:
	    afi = &af_inet6_info;
	    break;

	default:
	    /* XXX support more */
	    loglog(RC_LOG_SERIOUS, "unsupported ID type %s"
		, idtypename);
	    return FALSE;
    }

    switch (id->isaiid_idtype)
    {
	case ID_IPV4_ADDR:
	case ID_IPV6_ADDR:
	{
	    ip_address temp_address;
	    err_t ugh;
	    
#ifdef ZB_DEBUG
	    {
		    char srcbuf[20];
		    memset(srcbuf,0,sizeof(srcbuf));
		    sprintf(srcbuf,"name=%s,srclen=%d",id_pbs->name,pbs_left(id_pbs));
		    send_log(srcbuf);
	    }
#endif
	    ugh = initaddr(id_pbs->cur, pbs_left(id_pbs), afi->af, &temp_address);
#ifdef ZB_DEBUG
		{
		    char srcbuf[20];
		    memset(srcbuf,0,sizeof(srcbuf));
		    sprintf(srcbuf,"tmpaddr=%0x",temp_address.u.v4.sin_addr.s_addr);
		    send_log(srcbuf);
	         }

#endif
	    //zhangbin 2004.10.27
#if 1
	    if (ugh != NULL)
	    {
		loglog(RC_LOG_SERIOUS, "%s ID payload %s has wrong length in Quick I1 (%s)"
		    , which, idtypename, ugh);
		send_log("IP_ADDR:wrong length!\n");
		return FALSE;
	    }
#endif
	    happy(initsubnet(&temp_address, afi->mask_cnt, '0', net));
	    DBG(DBG_PARSING | DBG_CONTROL,
		{
		    char temp_buff[SUBNETTOT_BUF];

		    subnettot(net, 0, temp_buff, sizeof(temp_buff));
		    DBG_log("%s is %s", which, temp_buff);
		});
	    break;
	}

	case ID_IPV4_ADDR_SUBNET:
	case ID_IPV6_ADDR_SUBNET:
	{
	    ip_address temp_address, temp_mask;
	    err_t ugh;

	    if (pbs_left(id_pbs) != 2 * afi->ia_sz)
	    {
		loglog(RC_LOG_SERIOUS, "%s ID payload %s wrong length in Quick I1"
		    , which, idtypename);

#ifdef ZB_DEBUG
		send_log("IP_SUBNET:wrong length!\n");
#endif
		return FALSE;
	    }
	    ugh = initaddr(id_pbs->cur
		, afi->ia_sz, afi->af, &temp_address);
	    if (ugh == NULL)
		ugh = initaddr(id_pbs->cur + afi->ia_sz
		    , afi->ia_sz, afi->af, &temp_mask);
	    if (ugh == NULL)
		ugh = initsubnet(&temp_address, masktocount(&temp_mask)
		    , '0', net);
	    if (ugh != NULL)
	    {
		loglog(RC_LOG_SERIOUS, "%s ID payload %s bad subnet in Quick I1 (%s)"
		    , which, idtypename, ugh);
#ifdef ZB_DEBUG
		send_log("SUBNET:Bad subnet!\n");
#endif
		return FALSE;
	    }
	    DBG(DBG_PARSING | DBG_CONTROL,
		{
		    char temp_buff[SUBNETTOT_BUF];

		    subnettot(net, 0, temp_buff, sizeof(temp_buff));
		    DBG_log("%s is subnet %s", which, temp_buff);
		});
	    break;
	}

	case ID_IPV4_ADDR_RANGE:
	case ID_IPV6_ADDR_RANGE:
	{
	    ip_address temp_address_from, temp_address_to;
	    err_t ugh;

	    if (pbs_left(id_pbs) != 2 * afi->ia_sz)
	    {
		loglog(RC_LOG_SERIOUS, "%s ID payload %s wrong length in Quick I1"
		    , which, idtypename);
		return FALSE;
	    }
	    ugh = initaddr(id_pbs->cur, afi->ia_sz, afi->af, &temp_address_from);
	    if (ugh == NULL)
		ugh = initaddr(id_pbs->cur + afi->ia_sz
		    , afi->ia_sz, afi->af, &temp_address_to);
	    if (ugh != NULL)
	    {
		loglog(RC_LOG_SERIOUS, "%s ID payload %s malformed (%s) in Quick I1"
		    , which, idtypename, ugh);
		return FALSE;
	    }

	    ugh = rangetosubnet(&temp_address_from, &temp_address_to, net);
	    if (ugh != NULL)
	    {
		char temp_buff1[ADDRTOT_BUF], temp_buff2[ADDRTOT_BUF];

		addrtot(&temp_address_from, 0, temp_buff1, sizeof(temp_buff1));
		addrtot(&temp_address_to, 0, temp_buff2, sizeof(temp_buff2));
		loglog(RC_LOG_SERIOUS, "%s ID payload in Quick I1, %s"
		    " %s - %s unacceptable: %s"
		    , which, idtypename, temp_buff1, temp_buff2, ugh);
		return FALSE;
	    }
	    DBG(DBG_PARSING | DBG_CONTROL,
		{
		    char temp_buff[SUBNETTOT_BUF];

		    subnettot(net, 0, temp_buff, sizeof(temp_buff));
		    DBG_log("%s is subnet %s (received as range)"
			, which, temp_buff);
		});
	    break;
	}
    }

    return TRUE;
}

/* like decode, but checks that what is received matches what was sent */
static bool

check_net_id(struct isakmp_ipsec_id *id
, pb_stream *id_pbs
, u_int8_t *protoid
, u_int16_t *port
, ip_subnet *net
, const char *which)
{
    ip_subnet net_temp;

    if (!decode_net_id(id, id_pbs, &net_temp, which))
	return FALSE;

    DBG(DBG_PARSING | DBG_CONTROL,
	DBG_log("%s protocol/port is %d/%d", which, id->isaiid_protoid, id->isaiid_port)
    )

    if (!samesubnet(net, &net_temp)
    || *protoid != id->isaiid_protoid || *port != id->isaiid_port)
    {
	loglog(RC_LOG_SERIOUS, "%s ID returned doesn't match my proposal", which);
	return FALSE;
    }
    return TRUE;
}

/*
 * look for the existence of a non-expiring preloaded public key
 */
static bool
has_preloaded_public_key(struct state *st)
{
    struct connection *c = st->st_connection;

    /* do not consider rw connections since
     * the peer's identity must be known
     */
    if (c->kind == CK_PERMANENT)
    {
	struct pubkeyrec *p;

	/* look for a matching RSA public key */
	for (p = pubkeys; p != NULL; p = p->next)
	{
	    if (p->alg == PUBKEY_ALG_RSA &&
		same_id(&c->that.id, &p->id) &&
		p->until == UNDEFINED_TIME)
	    {
		/* found a preloaded public key */
		return TRUE;
	    }
	}
    }
    return FALSE;
}

/*
 * Produce the new key material of Quick Mode.
 * draft-ietf-ipsec-isakmp-oakley-06.txt section 5.5
 * specifies how this is to be done.
 */
static void
compute_proto_keymat(struct state *st
, u_int8_t protoid
, struct ipsec_proto_info *pi)
{
    size_t needed_len; /* bytes of keying material needed */

    /* Add up the requirements for keying material
     * (It probably doesn't matter if we produce too much!)
     */
    switch (protoid)
    {
    case PROTO_IPSEC_ESP:
	    switch (pi->attrs.transid)
	    {
	    case ESP_NULL:
		needed_len = 0;
		break;
	    case ESP_DES:
		needed_len = DES_CBC_BLOCK_SIZE;
		break;
	    case ESP_3DES:
		needed_len = DES_CBC_BLOCK_SIZE * 3;
		break;
	    default:
#ifndef NO_KERNEL_ALG
		if((needed_len=kernel_alg_esp_enc_keylen(pi->attrs.transid))>0) {
			/* XXX: check key_len "coupling with kernel.c's */
			if (pi->attrs.key_len) {
				needed_len=pi->attrs.key_len/8;
				DBG(DBG_PARSING, DBG_log("compute_proto_keymat:"
						"key_len=%d from peer",
						(int)needed_len));
			}
			break;
		}
#endif
		exit_log("transform %s not implemented yet",
		    enum_show(&esp_transformid_names, pi->attrs.transid));
	    }
#ifndef NO_KERNEL_ALG
	    DBG(DBG_PARSING, DBG_log("compute_proto_keymat:"
				    "needed_len (after ESP enc)=%d",
				    (int)needed_len));
	    if (kernel_alg_esp_auth_ok(pi->attrs.auth, NULL)) {
		needed_len += kernel_alg_esp_auth_keylen(pi->attrs.auth);
	    } else
#endif
	    switch (pi->attrs.auth)
	    {
	    case AUTH_ALGORITHM_NONE:
		break;
	    case AUTH_ALGORITHM_HMAC_MD5:
		needed_len += HMAC_MD5_KEY_LEN;
		break;
	    case AUTH_ALGORITHM_HMAC_SHA1:
		needed_len += HMAC_SHA1_KEY_LEN;
		break;
	    case AUTH_ALGORITHM_DES_MAC:
	    default:
		exit_log("AUTH algorithm %s not implemented yet",
		    enum_show(&auth_alg_names, pi->attrs.auth));
	    }
	    DBG(DBG_PARSING, DBG_log("compute_proto_keymat:"
				    "needed_len (after ESP auth)=%d",
				    (int)needed_len));
	    break;

    case PROTO_IPSEC_AH:
	    switch (pi->attrs.transid)
	    {
	    case AH_MD5:
		needed_len = HMAC_MD5_KEY_LEN;
		break;
	    case AH_SHA:
		needed_len = HMAC_SHA1_KEY_LEN;
		break;
	    default:
		exit_log("transform %s not implemented yet",
		    enum_show(&ah_transformid_names, pi->attrs.transid));
	    }
	    break;

    default:
	exit_log("protocol %s not implemented yet",
	    enum_show(&protocol_names, protoid));
	break;
    }

    pi->keymat_len = needed_len;

    /* Allocate space for the keying material.
     * Although only needed_len bytes are desired, we
     * must round up to a multiple of ctx.hmac_digest_size
     * so that our buffer isn't overrun.
     */
    {
	struct hmac_ctx ctx_me, ctx_peer;
	size_t needed_space;	/* space needed for keying material (rounded up) */
	size_t i;

	hmac_init_chunk(&ctx_me, st->st_oakley.hasher, st->st_skeyid_d);
	ctx_peer = ctx_me;	/* duplicate initial conditions */

	needed_space = needed_len + pad_up(needed_len, ctx_me.hmac_digest_size);
	replace(pi->our_keymat, alloc_bytes(needed_space, "keymat in compute_keymat()"));
	replace(pi->peer_keymat, alloc_bytes(needed_space, "peer_keymat in quick_inI1_outR1()"));

	for (i = 0;; )
	{
	    if (st->st_shared.ptr != NULL)
	    {
		/* PFS: include the g^xy */
		hmac_update_chunk(&ctx_me, st->st_shared);
		hmac_update_chunk(&ctx_peer, st->st_shared);
	    }
	    hmac_update(&ctx_me, &protoid, sizeof(protoid));
	    hmac_update(&ctx_peer, &protoid, sizeof(protoid));

	    hmac_update(&ctx_me, (u_char *)&pi->our_spi, sizeof(pi->our_spi));
	    hmac_update(&ctx_peer, (u_char *)&pi->attrs.spi, sizeof(pi->attrs.spi));

	    hmac_update_chunk(&ctx_me, st->st_ni);
	    hmac_update_chunk(&ctx_peer, st->st_ni);

	    hmac_update_chunk(&ctx_me, st->st_nr);
	    hmac_update_chunk(&ctx_peer, st->st_nr);

	    hmac_final(pi->our_keymat + i, &ctx_me);
	    hmac_final(pi->peer_keymat + i, &ctx_peer);

	    i += ctx_me.hmac_digest_size;
	    if (i >= needed_space)
		break;

	    /* more keying material needed: prepare to go around again */

	    hmac_reinit(&ctx_me);
	    hmac_reinit(&ctx_peer);

	    hmac_update(&ctx_me, pi->our_keymat + i - ctx_me.hmac_digest_size,
		ctx_me.hmac_digest_size);
	    hmac_update(&ctx_peer, pi->peer_keymat + i - ctx_peer.hmac_digest_size,
		ctx_peer.hmac_digest_size);
	}
    }

    DBG(DBG_CRYPT,
	DBG_dump("KEYMAT computed:\n", pi->our_keymat, pi->keymat_len);
	DBG_dump("Peer KEYMAT computed:\n", pi->peer_keymat, pi->keymat_len));
}

static void
compute_keymats(struct state *st)
{
    if (st->st_ah.present)
	compute_proto_keymat(st, PROTO_IPSEC_AH, &st->st_ah);
    if (st->st_esp.present)
	compute_proto_keymat(st, PROTO_IPSEC_ESP, &st->st_esp);
}

/* State Transition Functions.
 *
 * The definition of state_microcode_table in demux.c is a good
 * overview of these routines.
 *
 * - Called from process_packet; result handled by complete_state_transition
 * - struct state_microcode member "processor" points to these
 * - these routine definitionss are in state order
 * - these routines must be restartable from any point of error return:
 *   beware of memory allocated before any error.
 * - output HDR is usually emitted by process_packet (if state_microcode
 *   member first_out_payload isn't ISAKMP_NEXT_NONE).
 *
 * The transition functions' functions include:
 * - process and judge payloads
 * - update st_iv (result of decryption is in st_new_iv)
 * - build reply packet
 */

/* Handle a Main Mode Oakley first packet (responder side).
 * HDR;SA --> HDR;SA
 */
stf_status
main_inI1_outR1(struct msg_digest *md)
{
    struct payload_digest *const sa_pd = md->chain[ISAKMP_NEXT_SA];
    struct state *st;
    struct connection *c = find_host_connection(&md->iface->addr, pluto_port
	, &md->sender, md->sender_port);

    pb_stream r_sa_pbs;

    if (c == NULL)
    {
	/* see if a wildcarded connection can be found */
	c = find_host_connection(&md->iface->addr, pluto_port
	    , (ip_address*)NULL, md->sender_port);
	if (c != NULL)
	{
	    /* Create a temporary connection that is a copy of this one.
	     * His ID isn't declared yet.
	     */
	    c = rw_instantiate(c, &md->sender,
#ifdef NAT_TRAVERSAL
			md->sender_port,
#endif
#ifdef VIRTUAL_IP
			NULL,
#endif
			NULL);
	}
	else
	{
	    loglog(RC_LOG_SERIOUS, "initial Main Mode message received on %s:%u"
		" but no connection has been authorized"
		, ip_str(&md->iface->addr), ntohs(portof(&md->iface->addr)));
	    /* XXX notification is in order! */
	    return STF_IGNORE;
	}
    }

    /* Set up state */
    md->st = st = new_state();
    st->st_connection = c;
    set_cur_state(st);	/* (caller will reset cur_state) */
    st->st_try = 0;	/* not our job to try again from start */
    st->st_policy = c->policy & ~POLICY_IPSEC_MASK;	/* only as accurate as connection */

    memcpy(st->st_icookie, md->hdr.isa_icookie, COOKIE_SIZE);
    get_cookie(FALSE, st->st_rcookie, COOKIE_SIZE, &md->sender);

    insert_state(st);	/* needs cookies, connection, and msgid (0) */

    st->st_doi = ISAKMP_DOI_IPSEC;
    st->st_situation = SIT_IDENTITY_ONLY; /* We only support this */

    if ((c->kind == CK_INSTANCE) && (c->that.host_port != pluto_port))
    {
	log("responding to Main Mode from unknown peer %s:%u"
	    , ip_str(&c->that.host_addr), c->that.host_port);
    }
    else if (c->kind == CK_INSTANCE)
    {
	log("responding to Main Mode from unknown peer %s"
	    , ip_str(&c->that.host_addr));
    }
    else
    {
	log("responding to Main Mode");
	//zhangbin for VPN-log
#ifdef VPN_LOG
	{
		char rx_mi1[80];
		memset(rx_mi1,0,sizeof(rx_mi1));
		sprintf(rx_mi1,"2nIKE[\"%s\"] Rx {{ MM_I1 : %s SA",c->name,ip_str(&c->that.host_addr));
		send_log(rx_mi1);
	}
#endif

    }

    /* parse_isakmp_sa also spits out a winning SA into our reply,
     * so we have to build our md->reply and emit HDR before calling it.
     */

    /* HDR out.
     * We can't leave this to comm_handle() because we must
     * fill in the cookie.
     */
    {
	struct isakmp_hdr r_hdr = md->hdr;

	r_hdr.isa_flags &= ~ISAKMP_FLAG_COMMIT;	/* we won't ever turn on this bit */
	memcpy(r_hdr.isa_rcookie, st->st_rcookie, COOKIE_SIZE);
	r_hdr.isa_np = ISAKMP_NEXT_SA;
	if (!out_struct(&r_hdr, &isakmp_hdr_desc, &md->reply, &md->rbody))
	    return STF_INTERNAL_ERROR;
    }

    /* start of SA out */
    {
	struct isakmp_sa r_sa = sa_pd->payload.sa;

	/* if we  use an ID_KEY_ID as own ID, we assume a
	 * PGPcert peer and have to send the Vendor ID
	 *  ++++ not sure, if this is an interop problem ++++++
	 */
	if (c->this.id.kind == ID_KEY_ID)
		r_sa.isasa_np = ISAKMP_NEXT_VID;
	else
	r_sa.isasa_np = ISAKMP_NEXT_NONE;
	if (!out_struct(&r_sa, &isakmp_sa_desc, &md->rbody, &r_sa_pbs))
	    return STF_INTERNAL_ERROR;
    }

    /* SA body in and out */
    RETURN_STF_FAILURE(parse_isakmp_sa_body(&sa_pd->pbs, &sa_pd->payload.sa, &r_sa_pbs
	    , FALSE, st));

    if (c->this.id.kind == ID_KEY_ID)
    {
	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_vendor_id_desc, &md->rbody
	    , pgp_vid, sizeof(pgp_vid)-1, "V_ID"))
    		 return STF_INTERNAL_ERROR;
    }

#ifdef NAT_TRAVERSAL
    if (md->nat_traversal_vid && nat_traversal_enabled) {
	/* reply if NAT-Traversal draft is supported */
	st->nat_traversal = nat_traversal_vid_to_method(md->nat_traversal_vid);
	if ((st->nat_traversal) && (!out_vendorid(ISAKMP_NEXT_NONE,
	    &md->rbody, md->nat_traversal_vid))) {
	    return STF_INTERNAL_ERROR;
	}
    }
#endif

#ifdef DPD
    /* Announce our ability to do Dead Peer Detection to the peer */
    if(st->st_connection->dpd_delay && st->st_connection->dpd_timeout) {
    	if (!out_modify_previous_np(ISAKMP_NEXT_VID, &md->rbody))
            return STF_INTERNAL_ERROR;
    	if( !out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_vendor_id_desc,
			    &md->rbody, dpd_vid, sizeof(dpd_vid), "V_ID"))
	    return STF_INTERNAL_ERROR;
    }
#endif

    close_message(&md->rbody);

    /* save initiator SA for HASH */
    clonereplacechunk(st->st_p1isa, sa_pd->pbs.start, pbs_room(&sa_pd->pbs), "sa in main_inI1_outR1()");
    
//zhangbin for VPN-log
#ifdef VPN_LOG
	{
		char tx_mr1[80];
		memset(tx_mr1,0,sizeof(tx_mr1));
	    sprintf(tx_mr1,"2nIKE[\"%s\"] Tx >> MM_R1 : %s SA",c->name,ip_str(&c->that.host_addr));
		send_log(tx_mr1);
	}	
#endif

    return STF_OK;
}

/* STATE_MAIN_I1: HDR, SA --> auth dependent
 * PSK_AUTH, DS_AUTH: --> HDR, KE, Ni
 *
 * The following are not yet implemented:
 * PKE_AUTH: --> HDR, KE, [ HASH(1), ] <IDi1_b>PubKey_r, <Ni_b>PubKey_r
 * RPKE_AUTH: --> HDR, [ HASH(1), ] <Ni_b>Pubkey_r, <KE_b>Ke_i,
 *                <IDi1_b>Ke_i [,<<Cert-I_b>Ke_i]
 *
 * We must verify that the proposal received matches one we sent.
 */
stf_status
main_inR1_outI2(struct msg_digest *md)
{
    struct state *const st = md->st;

    /* verify echoed SA */
    {
	struct payload_digest *const sapd = md->chain[ISAKMP_NEXT_SA];

	RETURN_STF_FAILURE(parse_isakmp_sa_body(&sapd->pbs
	    , &sapd->payload.sa, NULL, TRUE, st));
    }


#ifdef NAT_TRAVERSAL
    if (nat_traversal_enabled && md->nat_traversal_vid) {
	st->nat_traversal = nat_traversal_vid_to_method(md->nat_traversal_vid);
    }
#endif

    //zhangbin for VPN_log
#ifdef VPN_LOG
	{
		char rx_mr1[80];
		memset(rx_mr1,0,sizeof(rx_mr1));
	    sprintf(rx_mr1,"2nIKE[\"%s\"] Rx {{ MM_R1 : %s SA",st->st_connection->name,ip_str(&(st->st_connection->that.host_addr)));
	    send_log(rx_mr1);
	}
#endif

    /**************** build output packet HDR;KE;Ni ****************/

    /* HDR out.
     * We can't leave this to comm_handle() because the isa_np
     * depends on the type of Auth (eventually).
     */
    echo_hdr(md, FALSE, ISAKMP_NEXT_KE);

    /* KE out */
    if (!build_and_ship_KE(st, &st->st_gi, st->st_oakley.group
    , &md->rbody, ISAKMP_NEXT_NONCE))
	return STF_INTERNAL_ERROR;

#ifdef IMPAIR_MTU_BUST_MI2	/* ship a really big VID payload in MI2 message */
    /* Ni out */
    if (!build_and_ship_nonce(&st->st_ni, &md->rbody, ISAKMP_NEXT_VID, "Ni"))
	return STF_INTERNAL_ERROR;

    /* generate a pointless large VID payload to push message over MTU */
    {
	pb_stream vid_pbs;

	if (!out_generic(ISAKMP_NEXT_NONE, &isakmp_vendor_id_desc, &md->rbody
	    , &vid_pbs))
	    return STF_INTERNAL_ERROR;
	if (!out_zero(1500 /*MTU?*/, &vid_pbs, "Filler VID"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&vid_pbs);
    }
#else
    /* Ni out */
    if (!build_and_ship_nonce(&st->st_ni, &md->rbody, ISAKMP_NEXT_NONE, "Ni"))
	return STF_INTERNAL_ERROR;
#endif

#ifdef NAT_TRAVERSAL
    if (st->nat_traversal & NAT_T_WITH_NATD) {
	if (!nat_traversal_add_natd(ISAKMP_NEXT_NONE, &md->rbody, md))
	    return STF_INTERNAL_ERROR;
    }
#endif

    /* finish message */
    close_message(&md->rbody);


    /* Reinsert the state, using the responder cookie we just received */
    unhash_state(st);
    memcpy(st->st_rcookie, md->hdr.isa_rcookie, COOKIE_SIZE);
    insert_state(st);	/* needs cookies, connection, and msgid (0) */

    //zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char tx_mi2[80];
	    memset(tx_mi2,0,sizeof(tx_mi2));
		//openlog("", LOG_CONS | LOG_NDELAY, LOG_AUTHPRIV);
	    sprintf(tx_mi2,"2nIKE[\"%s\"] Tx >> MM_I2 : %s KE, NONCE\n",st->st_connection->name,ip_str(&(st->st_connection->that.host_addr)));
//	    log("tx_mi2=%s",tx_mi2);
	    send_log(tx_mi2);
	    //syslog(LOG_CRIT,"%s",tx_mi2);
	    //closelog();
	}
#endif

    return STF_OK;
}

/* STATE_MAIN_R1:
 * PSK_AUTH, DS_AUTH: HDR, KE, Ni --> HDR, KE, Nr
 *
 * The following are not yet implemented:
 * PKE_AUTH: HDR, KE, [ HASH(1), ] <IDi1_b>PubKey_r, <Ni_b>PubKey_r
 *	    --> HDR, KE, <IDr1_b>PubKey_i, <Nr_b>PubKey_i
 * RPKE_AUTH:
 *	    HDR, [ HASH(1), ] <Ni_b>Pubkey_r, <KE_b>Ke_i, <IDi1_b>Ke_i [,<<Cert-I_b>Ke_i]
 *	    --> HDR, <Nr_b>PubKey_i, <KE_b>Ke_r, <IDr1_b>Ke_r
 */
stf_status
main_inI2_outR2(struct msg_digest *md)
{
    struct state *const st = md->st;
    pb_stream *keyex_pbs = &md->chain[ISAKMP_NEXT_KE]->pbs;
 
    /* send CR if auth is RSA and no preloaded RSA public key exists*/
    bool send_cr = !no_cr_send && (st->st_oakley.auth == OAKLEY_RSA_SIG) &&
		   !has_preloaded_public_key(st);

    /* KE in */
    RETURN_STF_FAILURE(accept_KE(&st->st_gi, "Gi", st->st_oakley.group, keyex_pbs));

    /* Ni in */
    RETURN_STF_FAILURE(accept_nonce(md, &st->st_ni, "Ni"));

    //zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char rx_mi2[80];
	    memset(rx_mi2,0,sizeof(rx_mi2));
	    sprintf(rx_mi2,"2nIKE[\"%s\"] Rx {{ MM_I2 : %s KE, NONCE",st->st_connection->name,ip_str(&(st->st_connection->that.host_addr)));
	    send_log(rx_mi2);
  }
#endif

#ifdef NAT_TRAVERSAL
    if (st->nat_traversal & NAT_T_WITH_NATD) {
	nat_traversal_natd_lookup(md);
    }
    if (st->nat_traversal) {
	nat_traversal_show_result(st->nat_traversal, md->sender_port);
    }
    if (st->nat_traversal & NAT_T_WITH_KA) {
	nat_traversal_new_ka_event();
    }
#endif

    /**************** build output packet HDR;KE;Nr ****************/

    /* HDR out done */

    /* KE out */
    if (!build_and_ship_KE(st, &st->st_gr, st->st_oakley.group
    , &md->rbody, ISAKMP_NEXT_NONCE))
	return STF_INTERNAL_ERROR;

#ifdef IMPAIR_MTU_BUST_MR2	/* ship a really big VID payload in MR2 message */
    /* Nr out */
    if (!build_and_ship_nonce(&st->st_nr, &md->rbody, ISAKMP_NEXT_VID, "Nr"))
	return STF_INTERNAL_ERROR;

    /* generate a pointless large VID payload to push message over MTU */
    {
	pb_stream vid_pbs;

	if (!out_generic((send_cr)? ISAKMP_NEXT_CR : ISAKMP_NEXT_NONE,
	    &isakmp_vendor_id_desc, &md->rbody, &vid_pbs))
	    return STF_INTERNAL_ERROR;
	if (!out_zero(1500 /*MTU?*/, &vid_pbs, "Filler VID"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&vid_pbs);
    }
#else
    /* Nr out */
    if (!build_and_ship_nonce(&st->st_nr, &md->rbody,
	(send_cr)? ISAKMP_NEXT_CR : ISAKMP_NEXT_NONE, "Nr"))
	return STF_INTERNAL_ERROR;
#endif

    /* CR out */
    if (send_cr)
    {
	pb_stream cr_pbs;
	chunk_t desired_ca = st->st_connection->that.ca;

	struct isakmp_cr cr_hd;
	cr_hd.isacr_np = ISAKMP_NEXT_NONE;
	cr_hd.isacr_type = CERT_X509_SIGNATURE;

	if (!out_struct(&cr_hd, &isakmp_ipsec_cert_req_desc, &md->rbody, &cr_pbs))
	    return STF_INTERNAL_ERROR;
	if (desired_ca.ptr != NULL && st->st_connection->kind == CK_PERMANENT)
	{
	    if (!out_chunk(desired_ca, &cr_pbs, "CA"))
		return STF_INTERNAL_ERROR;
	}
	close_output_pbs(&cr_pbs);
    }

#ifdef NAT_TRAVERSAL
    if (st->nat_traversal & NAT_T_WITH_NATD) {
	if (!nat_traversal_add_natd(ISAKMP_NEXT_NONE, &md->rbody, md))
	    return STF_INTERNAL_ERROR;
    }
#endif

    /* finish message */
    close_message(&md->rbody);
//zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char tx_mr2[80];
	    memset(tx_mr2,0,sizeof(tx_mr2));
	    sprintf(tx_mr2,"2nIKE[\"%s\"] Tx >> MM_R2 : %s KE, NONCE",st->st_connection->name,ip_str(&(st->st_connection->that.host_addr)));
	    send_log(tx_mr2);
	}
#endif
    
    /* next message will be encrypted, but not this one.
     * We could defer this calculation.
     */
#ifndef DODGE_DH_MISSING_ZERO_BUG
    compute_dh_shared(st, st->st_gi, st->st_oakley.group);
#endif
    if (!generate_skeyids_iv(st))
	return STF_FAIL + AUTHENTICATION_FAILED;
    update_iv(st);

    return STF_OK;
}

/* STATE_MAIN_I2:
 * SMF_PSK_AUTH: HDR, KE, Nr --> HDR*, IDi1, HASH_I
 * SMF_DS_AUTH: HDR, KE, Nr --> HDR*, IDi1, [ CERT, ] SIG_I
 *
 * The following are not yet implemented.
 * SMF_PKE_AUTH: HDR, KE, <IDr1_b>PubKey_i, <Nr_b>PubKey_i
 *	    --> HDR*, HASH_I
 * SMF_RPKE_AUTH: HDR, <Nr_b>PubKey_i, <KE_b>Ke_r, <IDr1_b>Ke_r
 *	    --> HDR*, HASH_I
 */
stf_status
main_inR2_outI3(struct msg_digest *md)
{
    struct state *const st = md->st;
    pb_stream *const keyex_pbs = &md->chain[ISAKMP_NEXT_KE]->pbs;
    int auth_payload = st->st_oakley.auth == OAKLEY_PRESHARED_KEY
	? ISAKMP_NEXT_HASH : ISAKMP_NEXT_SIG;
    pb_stream id_pbs;	/* ID Payload; also used for hash calculation */
    chunk_t requested_ca;
    cert_t mycert;

    /* send certificate if we have one and auth is RSA */
    bool send_cert = (st->st_oakley.auth == OAKLEY_RSA_SIG) &&
		      get_mycert(&mycert, st->st_connection->this.cert);

    /* send certificate request, if we don't have a preloaded RSA public key */
    bool send_cr = !no_cr_send && send_cert && !has_preloaded_public_key(st);

    /* KE in */
    RETURN_STF_FAILURE(accept_KE(&st->st_gr, "Gr", st->st_oakley.group, keyex_pbs));

    /* Nr in */
    RETURN_STF_FAILURE(accept_nonce(md, &st->st_nr, "Nr"));

    /* decode certificate requests */
    decode_cr(md, &requested_ca);

    /* done parsing; initialize crypto  */

    compute_dh_shared(st, st->st_gr, st->st_oakley.group);
#ifdef DODGE_DH_MISSING_ZERO_BUG
    if (st->st_shared.ptr[0] == 0)
	return STF_REPLACE_DOOMED_EXCHANGE;
#endif
    if (!generate_skeyids_iv(st))
	return STF_FAIL + AUTHENTICATION_FAILED;

#ifdef NAT_TRAVERSAL
	if (st->nat_traversal & NAT_T_WITH_NATD) {
	    nat_traversal_natd_lookup(md);
	}
	if (st->nat_traversal) {
	    nat_traversal_show_result(st->nat_traversal, md->sender_port);
	}
	if (st->nat_traversal & NAT_T_WITH_KA) {
	    nat_traversal_new_ka_event();
	}
#endif
//zhangbin for VPN_log
#ifdef VPN_LOG
{
	char rx_mr2[80];
	memset(rx_mr2,0,sizeof(rx_mr2));
	//openlog("", LOG_CONS | LOG_NDELAY | LOG_PID, LOG_AUTHPRIV);
	sprintf(rx_mr2,"2nIKE[\"%s\"] Rx {{ MM_R2 : %s KE, NONCE",st->st_connection->name,ip_str(&(st->st_connection->that.host_addr)));
	    send_log(rx_mr2);
	//log("rx_mr2=%s",rx_mr2);
	//syslog(LOG_CRIT,"%s",rx_mr2);
	//closelog();
 }
#endif

    /*************** build output packet HDR*;IDii;HASH/SIG_I ***************/
    /* ??? NOTE: this is almost the same as main_inI3_outR3's code */

    /* HDR* out done */

    /* IDii out */
    {
	struct isakmp_ipsec_id id_hd;
	chunk_t id_b;

	build_id_payload(&id_hd, &id_b, &st->st_connection->this);
	id_hd.isaiid_np = (send_cert)? ISAKMP_NEXT_CERT : auth_payload;	
	if (!out_struct(&id_hd, &isakmp_ipsec_identification_desc, &md->rbody, &id_pbs)
	|| !out_chunk(id_b, &id_pbs, "my identity"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&id_pbs);
    }

    /* CERT out */
    if (send_cert)
    {
	pb_stream cert_pbs;

	struct isakmp_cert cert_hd;
	cert_hd.isacert_np = (send_cr)? ISAKMP_NEXT_CR : ISAKMP_NEXT_SIG;
	cert_hd.isacert_type = mycert.type;

	if (!out_struct(&cert_hd, &isakmp_ipsec_certificate_desc, &md->rbody, &cert_pbs))
	    return STF_INTERNAL_ERROR;
	if (!out_chunk(mycert.cert, &cert_pbs, "CERT"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&cert_pbs);
    }

    /* CR out */
    if (send_cr)
    {
	pb_stream cr_pbs;
	chunk_t desired_ca = st->st_connection->that.ca;

	struct isakmp_cr cr_hd;
	cr_hd.isacr_np = ISAKMP_NEXT_SIG;
	cr_hd.isacr_type = mycert.type;

	if (!out_struct(&cr_hd, &isakmp_ipsec_cert_req_desc, &md->rbody, &cr_pbs))
	    return STF_INTERNAL_ERROR;
	if (desired_ca.ptr != NULL)
	{
	    if (!out_chunk(desired_ca, &cr_pbs, "CA"))
		return STF_INTERNAL_ERROR;
	}
	close_output_pbs(&cr_pbs);
    }

    /* HASH_I or SIG_I out */
    {
	u_char hash_val[MAX_DIGEST_LEN];
	size_t hash_len = main_mode_hash(st, hash_val, TRUE, &id_pbs);

	if (auth_payload == ISAKMP_NEXT_HASH)
	{
	    /* HASH_I out */
	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_hash_desc, &md->rbody
	    , hash_val, hash_len, "HASH_I"))
		return STF_INTERNAL_ERROR;
	}
	else
	{
	    /* SIG_I out */
	    u_char sig_val[RSA_MAX_OCTETS];
	    size_t sig_len = RSA_sign_hash(st->st_connection
		, sig_val, hash_val, hash_len);

	    if (sig_len == 0)
	    {
		loglog(RC_LOG_SERIOUS, "unable to locate my private key for RSA Signature");
		return STF_FAIL + AUTHENTICATION_FAILED;
	    }

	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_signature_desc
	    , &md->rbody, sig_val, sig_len, "SIG_I"))
		return STF_INTERNAL_ERROR;
	}
    }

    /* encrypt message, except for fixed part of header */

    /* st_new_iv was computed by generate_skeyids_iv */
    if (!encrypt_message(&md->rbody, st))
	return STF_INTERNAL_ERROR;	/* ??? we may be partly committed */
//zhangbin for VPN_log
#ifdef VPN_LOG
{
	char tx_mi3[100];
	memset(tx_mi3,0,sizeof(tx_mi3));	
	//openlog("", LOG_CONS | LOG_NDELAY | LOG_PID, LOG_AUTHPRIV);
	sprintf(tx_mi3,"2nIKE[\"%s\"] Tx >> MM_I3 : %s ID, HASH",st->st_connection->name,ip_str(&st->st_connection->that.host_addr));
    	send_log(tx_mi3);
   	//syslog(LOG_CRIT,"%s",tx_mi3);
 	//closelog();
}
#endif
    return STF_OK;
}

/* Shared logic for asynchronous lookup of DNS KEY records.
 * Used for STATE_MAIN_R2 and STATE_MAIN_I3.
 */

struct key_continuation {
    struct adns_continuation ac;	/* common prefix */
    struct msg_digest *md;
};

static void
report_key_dns_failure(struct id *id, err_t ugh)
{
    char id_buf[IDTOA_BUF];	/* arbitrary limit on length of ID reported */

    (void) idtoa(id, id_buf, sizeof(id_buf));
    loglog(RC_LOG_SERIOUS, "no RSA public key known for '%s'"
	"; DNS search for KEY failed (%s)", id_buf, ugh);
}

/* Processs the OAKLEY ID Payload and the Authenticator
 * (Hash or Signature Payload).
 * If a KEY lookup is needed, it is initiated and STF_SUSPEND is returned.
 * Note: global keys_from_dns contains results of DNS lookup for key
 * or is NULL indicating lookup not yet tried.
 */
static stf_status
oakley_id_and_auth(struct msg_digest *md
, bool initiator	/* are we the Initiator? */
, bool aggrmode		/* aggressive mode? */
, cont_fn_t cont_fn)	/* continuation function */
{
    struct state *st = md->st;
    u_char hash_val[MAX_DIGEST_LEN];
    size_t hash_len;
    stf_status r = STF_OK;

    /* ID Payload in.
     * Note: this may switch the connection being used!
     */
    if (!decode_peer_id(md, initiator, aggrmode))
{

#ifdef ZB_DEBUG	
	char errbuf[80];	
	memset(errbuf,0,sizeof(errbuf));
	strcpy(errbuf,"in oakely_id_and_auth!\n");
	send_log(errbuf);
#endif

		ZHS_LOG();
	return STF_FAIL + INVALID_ID_INFORMATION;
}

    /* Hash the ID Payload.
     * main_mode_hash requires idpl->cur to be at end of payload
     * so we temporarily set if so.
     */
    {
	pb_stream *idpl = &md->chain[ISAKMP_NEXT_ID]->pbs;
	u_int8_t *old_cur = idpl->cur;

	idpl->cur = idpl->roof;
	hash_len = main_mode_hash(st, hash_val, !initiator, idpl);
	idpl->cur = old_cur;
    }

    switch (st->st_oakley.auth)
    {
    case OAKLEY_PRESHARED_KEY:
	{
	    pb_stream *const hash_pbs = &md->chain[ISAKMP_NEXT_HASH]->pbs;

	    if (pbs_left(hash_pbs) != hash_len
	    || memcmp(hash_pbs->cur, hash_val, hash_len) != 0)
	    {
		DBG_cond_dump(DBG_CRYPT, "received HASH:"
		    , hash_pbs->cur, pbs_left(hash_pbs));
		loglog(RC_LOG_SERIOUS, "received Hash Payload does not match computed value");
		ZHS_LOG();
		r = STF_FAIL + INVALID_HASH_INFORMATION;
	    }
	}
	break;

    case OAKLEY_RSA_SIG:
	r = RSA_check_signature(st, hash_val, hash_len
	    , &md->chain[ISAKMP_NEXT_SIG]->pbs);

	if (r == STF_SUSPEND)
	{
	    /* initiate asynchronous DNS lookup for KEY record(s) */
	    struct key_continuation *kc
		= alloc_thing(struct key_continuation, "key continuation");
	    err_t ugh;

		ZHS_LOG();
	    /* Record that state is used by a suspended md */
	    passert(st->st_suspended_md == NULL);
	    kc->md = st->st_suspended_md = md;

	    ugh = start_adns_query(&st->st_connection->that.id
		, NULL	/* no sgw for KEY */
		, T_KEY
		, cont_fn
		, &kc->ac);

	    if (ugh != NULL)
	    {
		report_key_dns_failure(&st->st_connection->that.id, ugh);
		st->st_suspended_md = NULL;
		ZHS_LOG();
		r = STF_FAIL + INVALID_KEY_INFORMATION;
	    }
	}
	break;

    default:
	impossible();
    }
    if (r == STF_OK)
	DBG(DBG_CRYPT, DBG_log("authentication succeeded"));
    return r;
}

static inline stf_status
main_id_and_auth(struct msg_digest *md
, bool initiator	/* are we the Initiator? */
, cont_fn_t cont_fn)	/* continuation function */
{
    return oakley_id_and_auth(md, initiator, FALSE, cont_fn);
}

static inline stf_status
aggr_id_and_auth(struct msg_digest *md
, bool initiator	/* are we the Initiator? */
, cont_fn_t cont_fn)	/* continuation function */
{
    return oakley_id_and_auth(md, initiator, TRUE, cont_fn);
}

static void
key_continue(struct adns_continuation *cr, err_t ugh
, stf_status (*tail)(struct msg_digest *md))
{
    stf_status r;
    struct key_continuation *kc = (void *)cr;
    struct state *st = kc->md->st;

    passert(cur_state == NULL);
    /* if st == NULL, our state has been deleted -- just clean up */
    if (st != NULL)
    {
	passert(st->st_suspended_md == kc->md);
	st->st_suspended_md = NULL;	/* no longer connected or suspended */
	cur_state = kc->md->st;
	if (ugh != NULL)
	{
	    report_key_dns_failure(&st->st_connection->that.id, ugh);
	    r = STF_FAIL + INVALID_KEY_INFORMATION;
	}
	else
	{
	    passert(keys_from_dns != NULL);
	    r = (*tail)(kc->md);
	    passert(r != STF_OK || keys_from_dns == NULL);
	    free_public_keys(&keys_from_dns);
	}
	log("will enter complete_state");
	complete_state_transition(&kc->md, r);
    }
    release_md(kc->md);
    cur_state = NULL;
}

/* STATE_MAIN_R2:
 * PSK_AUTH: HDR*, IDi1, HASH_I --> HDR*, IDr1, HASH_R
 * DS_AUTH: HDR*, IDi1, [ CERT, ] SIG_I --> HDR*, IDr1, [ CERT, ] SIG_R
 * PKE_AUTH, RPKE_AUTH: HDR*, HASH_I --> HDR*, HASH_R
 *
 * Broken into parts to allow asynchronous DNS for KEY records.
 *
 * - main_inI3_outR3 to start
 * - main_inI3_outR3_tail to finish or suspend for DNS lookup
 * - main_inI3_outR3_continue to start main_inI3_outR3_tail again
 */
static stf_status main_inI3_outR3_tail(struct msg_digest *md);	/* forward */

stf_status
main_inI3_outR3(struct msg_digest *md)
{
    passert(keys_from_dns == NULL);
    return main_inI3_outR3_tail(md);
}

static void
main_inI3_outR3_continue(struct adns_continuation *cr, err_t ugh)
{
    key_continue(cr, ugh, main_inI3_outR3_tail);
}

static stf_status
main_inI3_outR3_tail(struct msg_digest *md)
{
    struct state *const st = md->st;
    u_int8_t auth_payload;
    pb_stream r_id_pbs;	/* ID Payload; also used for hash calculation */
    cert_t mycert;
    bool send_cert;

    /* ID and HASH_I or SIG_I in
     * Note: this may switch the connection being used!
     */
    {
	stf_status r = main_id_and_auth(md, FALSE
	    , main_inI3_outR3_continue);

	if (r != STF_OK)
	    return r;
    }

    /* send certificate if we have one and auth is RSA */
    send_cert = (st->st_oakley.auth == OAKLEY_RSA_SIG) &&
		get_mycert(&mycert, st->st_connection->this.cert);

    //zhangbin for VPN_log
#ifdef VPN_LOG
    {
	char rx_mi3[80];
    	memset(rx_mi3,0,sizeof(rx_mi3));	
    sprintf(rx_mi3,"2nIKE[\"%s\"] Rx {{ MM_I3 : %s ID, HASH",st->st_connection->name,ip_str(&(st->st_connection->that.host_addr)));
    send_log(rx_mi3);
    }
#endif


    /*************** build output packet HDR*;IDir;HASH/SIG_R ***************/
    /* proccess_packet() would automatically generate the HDR*
     * payload if smc->first_out_payload is not ISAKMP_NEXT_NONE.
     * We don't do this because we wish there to be no partially
     * built output packet if we need to suspend for asynch DNS.
     */
    /* ??? NOTE: this is almost the same as main_inR2_outI3's code */

    /* HDR* out
     * If auth were PKE_AUTH or RPKE_AUTH, ISAKMP_NEXT_HASH would
     * be first payload.
     */
    echo_hdr(md, TRUE, ISAKMP_NEXT_ID);

    auth_payload = st->st_oakley.auth == OAKLEY_PRESHARED_KEY
	? ISAKMP_NEXT_HASH : ISAKMP_NEXT_SIG;

    /* IDir out */
    {
	struct isakmp_ipsec_id id_hd;
	chunk_t id_b;

	build_id_payload(&id_hd, &id_b, &st->st_connection->this);
	id_hd.isaiid_np = (send_cert)? ISAKMP_NEXT_CERT : auth_payload;
	if (!out_struct(&id_hd, &isakmp_ipsec_identification_desc, &md->rbody, &r_id_pbs)
	|| !out_chunk(id_b, &r_id_pbs, "my identity"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&r_id_pbs);
    }

    /* CERT out, if we have one */
    if (send_cert)
    {
	pb_stream cert_pbs;

	struct isakmp_cert cert_hd;
	cert_hd.isacert_np = ISAKMP_NEXT_SIG;
	cert_hd.isacert_type = mycert.type;

	if (!out_struct(&cert_hd, &isakmp_ipsec_certificate_desc, &md->rbody, &cert_pbs))
	return STF_INTERNAL_ERROR;
	if (!out_chunk(mycert.cert, &cert_pbs, "CERT"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&cert_pbs);
    }

    /* HASH_R or SIG_R out */
    {
	u_char hash_val[MAX_DIGEST_LEN];
	size_t hash_len = main_mode_hash(st, hash_val, FALSE, &r_id_pbs);

	if (auth_payload == ISAKMP_NEXT_HASH)
	{
	    /* HASH_R out */
	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_hash_desc, &md->rbody
	    , hash_val, hash_len, "HASH_R"))
		return STF_INTERNAL_ERROR;
	}
	else
	{
	    /* SIG_R out */
	    u_char sig_val[RSA_MAX_OCTETS];
	    size_t sig_len = RSA_sign_hash(st->st_connection
		, sig_val, hash_val, hash_len);

	    if (sig_len == 0)
	    {
		loglog(RC_LOG_SERIOUS, "unable to locate my private key for RSA Signature");
		return STF_FAIL + AUTHENTICATION_FAILED;
	    }

	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_signature_desc
	    , &md->rbody, sig_val, sig_len, "SIG_R"))
		return STF_INTERNAL_ERROR;
	}
    }

    /* encrypt message, sans fixed part of header */

    if (!encrypt_message(&md->rbody, st))
	return STF_INTERNAL_ERROR;	/* ??? we may be partly committed */

    /* Last block of Phase 1 (R3), kept for Phase 2 IV generation */
    DBG_cond_dump(DBG_CRYPT, "last encrypted block of Phase 1:"
	, st->st_new_iv, st->st_new_iv_len);

    ISAKMP_SA_established(st->st_connection, st->st_serialno);

    /* ??? If st->st_connectionc->gw_info != NULL,
     * we should keep the public key -- it tested out.
     */
//zhangbin for VPN_log
#ifdef VPN_LOG
{
     char tx_mr3[80];
	memset(tx_mr3,0,sizeof(tx_mr3));     
    sprintf(tx_mr3,"2nIKE[\"%s\"] Tx >> MM_R3 : %s ID, HASH",st->st_connection->name,ip_str(&st->st_connection->that.host_addr));
    send_log(tx_mr3);
}
#endif
    return STF_OK;
}

/* STATE_MAIN_I3:
 * Handle HDR*;IDir;HASH/SIG_R from responder.
 *
 * Broken into parts to allow asynchronous DNS for KEY records.
 *
 * - main_inR3 to start
 * - main_inR3_tail to finish or suspend for DNS lookup
 * - main_inR3_continue to start main_inR3_tail again
 */

static stf_status main_inR3_tail(struct msg_digest *md);	/* forward */

stf_status
main_inR3(struct msg_digest *md)
{
    passert(keys_from_dns == NULL);
    return main_inR3_tail(md);
}

static void
main_inR3_continue(struct adns_continuation *cr, err_t ugh)
{
    key_continue(cr, ugh, main_inR3_tail);
}

static stf_status
main_inR3_tail(struct msg_digest *md)
{
    struct state *const st = md->st;

    /* ID and HASH_R or SIG_R in
     * Note: this may switch the connection being used!
     */
    {
	stf_status r = main_id_and_auth(md, TRUE, main_inR3_continue);

	if (r != STF_OK)
	    return r;
    }
    //zhangbin for vpn-log
#ifdef VPN_LOG
    {
	    char rx_mr3[80];
	    memset(rx_mr3,0,sizeof(rx_mr3));
	    sprintf(rx_mr3,"2nIKE[\"%s\"] Rx {{ MM_R3 : %s ID,HASH",st->st_connection->name,ip_str(&(st->st_connection->that.host_addr)));
	    send_log(rx_mr3);
	    //log("rx_mr3=%s",rx_mr3);
	   // syslog(LOG_CRIT,"%s",rx_mr3);
	}
#endif

    /**************** done input ****************/

    ISAKMP_SA_established(st->st_connection, st->st_serialno);

    /* ??? If c->gw_info != NULL,
     * we should keep the public key -- it tested out.
     */

    update_iv(st);	/* finalize our Phase 1 IV */

    return STF_OK;
}


/* STATE_AGGR_R0: HDR, SA, KE, Ni, IDii 
 *           --> HDR, SA, KE, Nr, IDir, HASH_R/SIG_R
 */
stf_status
aggr_inI1_outR1(struct msg_digest *md)
{
    /* With Aggressive Mode, we get an ID payload in this, the first
     * message, so we can use it to index the preshared-secrets
     * when the IP address would not be meaningful (i.e. Road
     * Warrior).  So our first task is to unravel the ID payload.
     */
    struct state *st;
    struct payload_digest *const sa_pd = md->chain[ISAKMP_NEXT_SA];
    pb_stream *keyex_pbs = &md->chain[ISAKMP_NEXT_KE]->pbs;
    struct connection *c = find_host_connection(&md->iface->addr, pluto_port
	, &md->sender, md->sender_port);
    pb_stream r_sa_pbs;
    int auth_payload;
    pb_stream r_id_pbs;	/* ID Payload; also used for hash calculation */

    if (c == NULL)
    {
	/* see if a wildcarded connection can be found */
	c = find_host_connection(&md->iface->addr, pluto_port
	    , (ip_address*)NULL, md->sender_port);
	if (c != NULL && c->policy & POLICY_AGGRESSIVE)
	{
	    /* Create a temporary connection that is a copy of this one.
	     * His ID isn't declared yet.
	     */
	    c = rw_instantiate(c, &md->sender,
#ifdef NAT_TRAVERSAL
			md->sender_port,
#endif
#ifdef VIRTUAL_IP
			NULL,
#endif
			NULL);
	}
	else
	{
	    loglog(RC_LOG_SERIOUS, "initial Aggressive Mode message from %s"
		" but no (wildcard) connection has been configured"
		, ip_str(&md->sender));
	    /* XXX notification is in order! */
	    return STF_IGNORE;
	}
    }

    //zhangbin for VPN_log
#ifdef VPN_LOG
    {    
	    char rx_ai1[100];
	    memset(rx_ai1,0,sizeof(rx_ai1));
    sprintf(rx_ai1,"2nIKE[\"%s\"] Rx {{ AG_I1 : %s  SA, KE, NONCE, ID, ID, HASH",c->name,ip_str(&c->that.host_addr));
    send_log(rx_ai1);
   }
#endif
    /* Set up state */
    cur_state = md->st = st = new_state();	/* (caller will reset cur_state) */
    st->st_connection = c;

    st->st_policy |= POLICY_AGGRESSIVE;

    /* KLUDGE: st_oakley determined by SA parse which wants the pre-
       shared secret already determinable by decode_peer_id! */
    /* we really need to peek into the SA to see if it is RSASIG
       or something else. */
    st->st_oakley.auth = OAKLEY_PRESHARED_KEY;  /* FIXME! */
    if (!decode_peer_id(md, FALSE, TRUE))
    {
	char buf[200];

	(void) idtoa(&st->st_connection->that.id, buf, sizeof(buf));
	loglog(RC_LOG_SERIOUS,
	     "initial Aggressive Mode packet claiming to be from %s"
	     " on %s but no connection has been authorized",
	    buf, ip_str(&md->sender));
	/* XXX notification is in order! */
	return STF_FAIL + INVALID_ID_INFORMATION;
    }

    c = st->st_connection;

#ifdef DEBUG
    extra_debugging(c);
#endif
    st->st_try = 0;	/* Not our job to try again from start */
    st->st_policy = c->policy & ~POLICY_IPSEC_MASK;	/* only as accurate as connection */

#if 0
    /* Copy identity from temporary state object */
    st->st_peeridentity = tempstate.st_peeridentity;
    st->st_peeridentity_type = tempstate.st_peeridentity_type;
    st->st_peeruserprotoid = tempstate.st_peeruserprotoid;
    st->st_peeruserport = tempstate.st_peeruserport;
#endif

    memcpy(st->st_icookie, md->hdr.isa_icookie, COOKIE_SIZE);
    get_cookie(FALSE, st->st_rcookie, COOKIE_SIZE, &md->sender);

    insert_state(st);	/* needs cookies, connection, and msgid (0) */

    st->st_doi = ISAKMP_DOI_IPSEC;
    st->st_situation = SIT_IDENTITY_ONLY; /* We only support this */
    st->st_state = STATE_AGGR_R1;

    log("responding to Aggressive Mode, state #%lu, connection \"%s\""
	" from %s"
	, st->st_serialno, st->st_connection->name
	, ip_str(&c->that.host_addr));

#ifdef NAT_TRAVERSAL
    if (md->nat_traversal_vid && nat_traversal_enabled) {
	/* reply if NAT-Traversal draft is supported */
	st->nat_traversal = nat_traversal_vid_to_method(md->nat_traversal_vid);
    }
#endif

    /* save initiator SA for HASH */
    clonereplacechunk(st->st_p1isa, sa_pd->pbs.start, pbs_room(&sa_pd->pbs),
		      "sa in aggr_inI1_outR1()");

    /* parse_isakmp_sa also spits out a winning SA into our reply,
     * so we have to build our md->reply and emit HDR before calling it.
     */

    /* HDR out */
    {
	struct isakmp_hdr r_hdr = md->hdr;

	memcpy(r_hdr.isa_rcookie, st->st_rcookie, COOKIE_SIZE);
	r_hdr.isa_np = ISAKMP_NEXT_SA;
	if (!out_struct(&r_hdr, &isakmp_hdr_desc, &md->reply, &md->rbody))
	    return STF_INTERNAL_ERROR;
    }

    /* start of SA out */
    {
	struct isakmp_sa r_sa = sa_pd->payload.sa;
	notification_t r;

	r_sa.isasa_np = ISAKMP_NEXT_KE;
	if (!out_struct(&r_sa, &isakmp_sa_desc, &md->rbody, &r_sa_pbs))
	    return STF_INTERNAL_ERROR;

	/* SA body in and out */
	r = parse_isakmp_sa_body(&sa_pd->pbs, &sa_pd->payload.sa,
				 &r_sa_pbs, FALSE, st);
	if (r != NOTHING_WRONG)
	    return STF_FAIL + r;
    }

    /* don't know until after SA body has been parsed */
    auth_payload = st->st_oakley.auth == OAKLEY_PRESHARED_KEY
	? ISAKMP_NEXT_HASH : ISAKMP_NEXT_SIG;


    /* KE in */
    RETURN_STF_FAILURE(accept_KE(&st->st_gi, "Gi", st->st_oakley.group, keyex_pbs));

    /* Ni in */
    RETURN_STF_FAILURE(accept_nonce(md, &st->st_ni, "Ni"));

    /************** build rest of output: KE, Nr, IDir, HASH_R/SIG_R ********/

    /* KE */
    if (!build_and_ship_KE(st, &st->st_gr, st->st_oakley.group,
			   &md->rbody, ISAKMP_NEXT_NONCE))
	return STF_INTERNAL_ERROR;

    /* Nr */
    if (!build_and_ship_nonce(&st->st_nr, &md->rbody, ISAKMP_NEXT_ID, "Nr"))
	return STF_INTERNAL_ERROR;

    /* IDir out */
    {
	struct isakmp_ipsec_id id_hd;
	chunk_t id_b;

	build_id_payload(&id_hd, &id_b, &st->st_connection->this);
	id_hd.isaiid_np = auth_payload;
	if (!out_struct(&id_hd, &isakmp_ipsec_identification_desc, &md->rbody, &r_id_pbs)
	|| !out_chunk(id_b, &r_id_pbs, "my identity"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&r_id_pbs);
    }

    compute_dh_shared(st, st->st_gi, st->st_oakley.group);
#ifdef DODGE_DH_MISSING_ZERO_BUG
    if (st->st_shared.ptr[0] == 0)
	return STF_REPLACE_DOOMED_EXCHANGE;
#endif
    if (!generate_skeyids_iv(st))
	return STF_FAIL + AUTHENTICATION_FAILED;
    update_iv(st);

#ifdef NAT_TRAVERSAL
    if (st->nat_traversal) {
	if ((st->nat_traversal) && (!out_vendorid(auth_payload,
	    &md->rbody, md->nat_traversal_vid))) {
	    return STF_INTERNAL_ERROR;
	}
    }

    if (st->nat_traversal & NAT_T_WITH_NATD) {
	if (!nat_traversal_add_natd(auth_payload, &md->rbody, md))
	    return STF_INTERNAL_ERROR;
    }
#endif

    /* HASH_R or SIG_R out */
    {
	u_char hash_val[MAX_DIGEST_LEN];
	size_t hash_len = main_mode_hash(st, hash_val, FALSE, &r_id_pbs);

	if (auth_payload == ISAKMP_NEXT_HASH)
	{
	    /* HASH_R out */
	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_hash_desc, &md->rbody
	    , hash_val, hash_len, "HASH_R"))
		return STF_INTERNAL_ERROR;
	}
	else
	{
	    /* SIG_R out */
	    u_char sig_val[RSA_MAX_OCTETS];
	    size_t sig_len = RSA_sign_hash(st->st_connection
		, sig_val, hash_val, hash_len);

	    if (sig_len == 0)
	    {
		loglog(RC_LOG_SERIOUS, "unable to locate my private key for RSA Signature");
		return STF_FAIL + AUTHENTICATION_FAILED;
	    }

	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_signature_desc
	    , &md->rbody, sig_val, sig_len, "SIG_R"))
		return STF_INTERNAL_ERROR;
	}
    }

    /* finish message */
    close_message(&md->rbody);

    //zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char tx_ar1[80];
	    memset(tx_ar1,0,sizeof(tx_ar1));
	    sprintf(tx_ar1,"2nIKE[\"%s\"] Tx >> AG_R1 : %s SA, KE, NONCE, ID, HASH\n",c->name,ip_str(&c->that.host_addr));
	    send_log(tx_ar1);
    }
#endif

    return STF_OK;
}

/* STATE_AGGR_I1: HDR, SA, KE, Nr, IDir, HASH_R/SIG_R
 *           --> HDR*, HASH_I/SIG_I
 */
static stf_status aggr_inR1_outI2_tail(struct msg_digest *md);	/* forward */
stf_status
aggr_inR1_outI2(struct msg_digest *md)
{
    /* With Aggressive Mode, we get an ID payload in this, the second
     * message, so we can use it to index the preshared-secrets
     * when the IP address would not be meaningful (i.e. Road
     * Warrior).  So our first task is to unravel the ID payload.
     */
    struct state *st = md->st;
    pb_stream *keyex_pbs = &md->chain[ISAKMP_NEXT_KE]->pbs;

    st->st_policy |= POLICY_AGGRESSIVE;

    if (!decode_peer_id(md, FALSE, TRUE))
    {
	char buf[200];

	(void) idtoa(&st->st_connection->that.id, buf, sizeof(buf));
	loglog(RC_LOG_SERIOUS,
	     "initial Aggressive Mode packet claiming to be from %s"
	     " on %s but no connection has been authorized",
	    buf, ip_str(&md->sender));
	/* XXX notification is in order! */
	return STF_FAIL + INVALID_ID_INFORMATION;
    }

    /* verify echoed SA */
    {
	struct payload_digest *const sapd = md->chain[ISAKMP_NEXT_SA];
	notification_t r = \
	    parse_isakmp_sa_body(&sapd->pbs, &sapd->payload.sa,
				 NULL, TRUE, st);

	if (r != NOTHING_WRONG)
	    return STF_FAIL + r;
    }

#ifdef NAT_TRAVERSAL
    if (nat_traversal_enabled && md->nat_traversal_vid) {
	st->nat_traversal = nat_traversal_vid_to_method(md->nat_traversal_vid);
    }
#endif

    /* KE in */
    RETURN_STF_FAILURE(accept_KE(&st->st_gr, "Gr", st->st_oakley.group, keyex_pbs));

    /* Ni in */
    RETURN_STF_FAILURE(accept_nonce(md, &st->st_nr, "Nr"));

    /* moved the following up as we need Rcookie for hash, skeyids */
    /* Reinsert the state, using the responder cookie we just received */
    unhash_state(st);
    memcpy(st->st_rcookie, md->hdr.isa_rcookie, COOKIE_SIZE);
    insert_state(st);	/* needs cookies, connection, and msgid (0) */

#ifdef NAT_TRAVERSAL
    if (st->nat_traversal & NAT_T_WITH_NATD) {
	nat_traversal_natd_lookup(md);
    }
    if (st->nat_traversal) {
	nat_traversal_show_result(st->nat_traversal, md->sender_port);
    }
    if (st->nat_traversal & NAT_T_WITH_KA) {
	nat_traversal_new_ka_event();
    }
#endif

    /* Generate SKEYID, SKEYID_A, SKEYID_D, SKEYID_E */
    compute_dh_shared(st, st->st_gr, st->st_oakley.group);
#ifdef DODGE_DH_MISSING_ZERO_BUG
    if (st->st_shared.ptr[0] == 0)
	return STF_REPLACE_DOOMED_EXCHANGE;
#endif
    if (!generate_skeyids_iv(st))
	return STF_FAIL + AUTHENTICATION_FAILED;
    update_iv(st);

    //zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char rx_ai1[80];
	    memset(rx_ai1,0,sizeof(rx_ai1));
	    sprintf(rx_ai1,"2nIKE[\"%s\"] Rx {{ AG_R1 : %s SA, KE, NONCE, ID",st->st_connection->name,ip_str(&(st->st_connection->that.host_addr)));
	    send_log(rx_ai1);
    }
#endif

    return aggr_inR1_outI2_tail(md);
}

static void
aggr_inR1_outI2_continue(struct adns_continuation *cr, err_t ugh)
{
    key_continue(cr, ugh, aggr_inR1_outI2_tail);
}

static stf_status
aggr_inR1_outI2_tail(struct msg_digest *md)
{
    struct state *const st = md->st;
    struct connection *c = st->st_connection;
    int auth_payload;

    /* HASH_R or SIG_R in */
    {
	stf_status r = aggr_id_and_auth(md, TRUE, aggr_inR1_outI2_continue);

	if (r != STF_OK)
	    return r;
    }

    auth_payload = st->st_oakley.auth == OAKLEY_PRESHARED_KEY
	? ISAKMP_NEXT_HASH : ISAKMP_NEXT_SIG;

    /**************** build output packet: HDR, HASH_I/SIG_I **************/

    /* HDR out */
    {
	struct isakmp_hdr r_hdr = md->hdr;

	memcpy(r_hdr.isa_rcookie, st->st_rcookie, COOKIE_SIZE);
	/* outputting should back-patch previous struct/hdr with payload type */
	r_hdr.isa_np = auth_payload;
	r_hdr.isa_flags |= ISAKMP_FLAG_ENCRYPTION;  /* KLUDGE */
	if (!out_struct(&r_hdr, &isakmp_hdr_desc, &md->reply, &md->rbody))
	    return STF_INTERNAL_ERROR;
    }

#ifdef NAT_TRAVERSAL
    if (st->nat_traversal & NAT_T_WITH_NATD) {
	if (!nat_traversal_add_natd(auth_payload, &md->rbody, md))
	    return STF_INTERNAL_ERROR;
    }
#endif

    /* HASH_I or SIG_I out */
    {
	u_char buffer[1024];
	struct isakmp_ipsec_id id_hd;
	chunk_t id_b;
	pb_stream id_pbs;
	u_char hash_val[MAX_DIGEST_LEN];
	size_t hash_len;

	build_id_payload(&id_hd, &id_b, &st->st_connection->this);
	init_pbs(&id_pbs, buffer, sizeof(buffer), "identity payload");
	id_hd.isaiid_np = ISAKMP_NEXT_NONE;
	if (!out_struct(&id_hd, &isakmp_ipsec_identification_desc, &id_pbs, NULL)
	|| !out_chunk(id_b, &id_pbs, "my identity"))
	    return STF_INTERNAL_ERROR;

	hash_len = main_mode_hash(st, hash_val, TRUE, &id_pbs);

	if (auth_payload == ISAKMP_NEXT_HASH)
	{
	    /* HASH_I out */
	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_hash_desc, &md->rbody
	    , hash_val, hash_len, "HASH_I"))
		return STF_INTERNAL_ERROR;
	}
	else
	{
	    /* SIG_I out */
	    u_char sig_val[RSA_MAX_OCTETS];
	    size_t sig_len = RSA_sign_hash(st->st_connection
		, sig_val, hash_val, hash_len);

	    if (sig_len == 0)
	    {
		loglog(RC_LOG_SERIOUS, "unable to locate my private key for RSA Signature");
		return STF_FAIL + AUTHENTICATION_FAILED;
	    }

	    if (!out_generic_raw(ISAKMP_NEXT_NONE, &isakmp_signature_desc
	    , &md->rbody, sig_val, sig_len, "SIG_I"))
		return STF_INTERNAL_ERROR;
	}
    }

    /* RFC2408 says we must encrypt at this point */

    /* st_new_iv was computed by generate_skeyids_iv */
    if (!encrypt_message(&md->rbody, st))
	return STF_INTERNAL_ERROR;	/* ??? we may be partly committed */

    c->newest_isakmp_sa = st->st_serialno;
//zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char tx_ai2[80];
	    memset(tx_ai2,0,sizeof(tx_ai2));
	    sprintf(tx_ai2,"2nIKE[\"%s\"] Tx >> AG_I2 : %s HASH\n",c->name,ip_str(&c->that.host_addr));
	    send_log(tx_ai2);
	}
#endif
    return STF_OK;
}

/* STATE_AGGR_R1: HDR*, HASH_I --> done
 */
static void
aggr_inI2_continue(struct adns_continuation *cr, err_t ugh)
{
    key_continue(cr, ugh, aggr_inR1_outI2_tail);
}

stf_status
aggr_inI2(struct msg_digest *md)
{
    struct state *const st = md->st;
    struct connection *c = st->st_connection;
    u_char buffer[1024];
    struct payload_digest id_pd;

#ifdef NAT_TRAVERSAL
    if (st->nat_traversal & NAT_T_WITH_NATD) {
	nat_traversal_natd_lookup(md);
    }
    if (st->nat_traversal) {
	nat_traversal_show_result(st->nat_traversal, md->sender_port);
    }
    if (st->nat_traversal & NAT_T_WITH_KA) {
	nat_traversal_new_ka_event();
    }
#endif

    /* Reconstruct the peer ID so the peer hash can be authenticated */
    {
	struct isakmp_ipsec_id id_hd;
	chunk_t id_b;
	pb_stream pbs;
	pb_stream id_pbs;
	build_id_payload(&id_hd, &id_b, &st->st_connection->that);
	init_pbs(&pbs, buffer, sizeof(buffer), "identity payload");
	id_hd.isaiid_np = ISAKMP_NEXT_NONE;
	if (!out_struct(&id_hd, &isakmp_ipsec_identification_desc, &pbs, &id_pbs)
		|| !out_chunk(id_b, &id_pbs, "my identity"))
	    return STF_INTERNAL_ERROR;
	close_output_pbs(&id_pbs);
	id_pbs.roof = pbs.cur;
	id_pbs.cur = pbs.start;
	in_struct(&id_pd.payload, &isakmp_identification_desc, &id_pbs, &id_pd.pbs);
    }
    md->chain[ISAKMP_NEXT_ID] = &id_pd;

    /* HASH_I or SIG_I in */
    {
	stf_status r = aggr_id_and_auth(md, FALSE, aggr_inI2_continue);

	if (r != STF_OK)
	    return r;
    }

    /* And reset the md to not leave stale pointers to our private id payload */
    md->chain[ISAKMP_NEXT_ID] = NULL;

    /**************** done input ****************/

    c->newest_isakmp_sa = st->st_serialno;

    update_iv(st);	/* Finalize our Phase 1 IV */
//zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char rx_ai2[80];
	    memset(rx_ai2,0,sizeof(rx_ai2));
	    sprintf(rx_ai2,"2nIKE[\"%s\"] Rx {{ AG_I2 : %s HASH",c->name,ip_str(&c->that.host_addr));
	    send_log(rx_ai2);
	}
#endif
    
    return STF_OK;
}



/* Handle first message of Phase 2 -- Quick Mode.
 * HDR*, HASH(1), SA, Ni [, KE ] [, IDci, IDcr ] -->
 * HDR*, HASH(2), SA, Nr [, KE ] [, IDci, IDcr ]
 * (see draft-ietf-ipsec-isakmp-oakley-07.txt 5.5)
 * Installs inbound IPsec SAs.
 * Although this seems early, we know enough to do so, and
 * this way we know that it is soon enough to catch all
 * packets that other side could send using this IPsec SA.
 *
 * Broken into parts to allow asynchronous DNS for TXT records:
 *
 * - quick_inI1_outR1 starts the ball rolling.
 *   It checks and parses enough to learn the Phase 2 IDs
 *
 * - quick_inI1_outR1_tail does the rest of the job
 *   unless DNS must be consulted.  In that case,
 *   it starts a DNS query, salts away what is needed
 *   to continue, and suspends.
 *
 * - quick_inI1_outR1_continue will restart quick_inI1_outR1_tail
 *   when DNS comes back with an answer.
 *
 * A big chunk of quick_inI1_outR1_tail is executed twice.
 * This is necessary because the set of connections
 * might change while we are awaiting DNS.
 * When first called, gateways_from_dns == NULL.  If DNS is
 * consulted asynchronously, gateways_from_dns != NULL the second time.
 * Remember that our state object might disappear too!
 */

/* hold anything we can handle of a Phase 2 ID */
struct p2id {
    ip_subnet net;
    u_int8_t proto;
    u_int16_t port;
};

struct verify_continuation {
    struct adns_continuation ac;	/* common prefix */
    struct msg_digest *md;
    struct p2id my, his;
    unsigned int new_iv_len;	/* p1st's might change */
    u_char new_iv[MAX_DIGEST_LEN];
    /* int whackfd; */	/* not needed */
};

static stf_status quick_inI1_outR1_tail(struct msg_digest *md
    , struct p2id *my, struct p2id *his
    , unsigned int new_iv_len
    , const u_char new_iv[MAX_DIGEST_LEN]);	/* forward */

stf_status
quick_inI1_outR1(struct msg_digest *md)
{
    const struct state *const p1st = md->st;
    struct connection *c = p1st->st_connection;
    struct payload_digest *const id_pd = md->chain[ISAKMP_NEXT_ID];
    struct p2id my, his;


    /* HASH(1) in */
    CHECK_QUICK_HASH(md
	, quick_mode_hash12(hash_val, hash_pbs->roof, md->message_pbs.roof
	    , p1st, &md->hdr.isa_msgid, FALSE)
	, "HASH(1)", "Quick I1");

    /* [ IDci, IDcr ] in
     * We do this now (probably out of physical order) because
     * we wish to select the correct connection before we consult
     * it for policy.
     */
    if (id_pd != NULL)
    {
	/* ??? we are assuming IPSEC_DOI */

	/* IDci (initiator is peer) */

	if (!decode_net_id(&id_pd->payload.ipsec_id, &id_pd->pbs
	, &his.net, "peer client"))
	{
#ifdef ZB_DEBUG
		send_log("in quick_inI1_outR1:peer client!\n");
#endif
	    	return STF_FAIL + INVALID_ID_INFORMATION;
	}

	his.proto = id_pd->payload.ipsec_id.isaiid_protoid;
	his.port = id_pd->payload.ipsec_id.isaiid_port;
	DBG(DBG_PARSING | DBG_CONTROL,
	    DBG_log("peer client protocol/port is %d/%d", his.proto, his.port)
	)

	/* IDcr (we are responder) */
#if 0
	//zhangbin 2004.10.27
	{
		char mbuf[60];
		memset(mbuf,0,sizeof(mbuf));
		sprintf(mbuf,"this.maskbits=%d,that.maskbits=%d\n",&c->this.client.maskbits,&c->that.client.maskbits);
		send_log(mbuf);
	}
	if(&c->this.client.maskbits != 32)
#endif
	{
		if (!decode_net_id(&id_pd->next->payload.ipsec_id, &id_pd->next->pbs
		, &my.net, "our client"))
		{
#ifdef ZB_DEBUG	
		send_log("In quick_inI1_outR1:our client!!\n");
#endif
	    	return STF_FAIL + INVALID_ID_INFORMATION;
		}
	}

	my.proto = id_pd->next->payload.ipsec_id.isaiid_protoid;
	my.port = id_pd->next->payload.ipsec_id.isaiid_port;
	DBG(DBG_PARSING | DBG_CONTROL,
	    DBG_log("our client protocol/port is %d/%d", my.proto, my.port)
	)
    }
    else
    {
	/* implicit IDci and IDcr: peer and self */
	if (!sameaddrtype(&c->this.host_addr, &c->that.host_addr))
	    return STF_FAIL;

	happy(addrtosubnet(&c->this.host_addr, &my.net));
	happy(addrtosubnet(&c->that.host_addr, &his.net));
	his.proto = my.proto = 0;
	his.port = my.port = 0;
    }
    passert(gateways_from_dns == NULL);
    return quick_inI1_outR1_tail(md, &my, &his
	, p1st->st_new_iv_len, p1st->st_new_iv);
}

static void
report_verify_failure(struct state *st, struct p2id *his, err_t ugh)
{
    char fgwb[ADDRTOT_BUF]
	, cb[ADDRTOT_BUF];
    ip_address peer_client;

    addrtot(&st->st_connection->that.host_addr, 0, fgwb, sizeof(fgwb));
    networkof(&his->net, &peer_client);
    addrtot(&peer_client, 0, cb, sizeof(cb));
    loglog(RC_OPPOFAILURE
	, "gateway %s claims client %s, but DNS for client fails to confirm: %s"
	, fgwb, cb, ugh);
}

static void
quick_inI1_outR1_continue(struct adns_continuation *cr, err_t ugh)
{
    stf_status r;
    struct verify_continuation *vc = (void *)cr;
    struct state *st = vc->md->st;

    passert(cur_state == NULL);
    /* if st == NULL, our state has been deleted -- just clean up */
    if (st != NULL)
    {
	passert(st->st_suspended_md == vc->md);
	st->st_suspended_md = NULL;	/* no longer connected or suspended */
	cur_state = vc->md->st;
	if (ugh != NULL)
	{
#ifdef ZB_DEBUG
		char errbuf[80];	
	memset(errbuf,0,sizeof(errbuf));
	strcpy(errbuf,"in quick_inI1_outR1_continue!\n");
	    report_verify_failure(st, &vc->his, ugh);

	send_log(errbuf);
#endif
	    r = STF_FAIL + INVALID_ID_INFORMATION;
	}
	else
	{
	    passert(gateways_from_dns != NULL);
	    r = quick_inI1_outR1_tail(vc->md, &vc->my, &vc->his
		, vc->new_iv_len, vc->new_iv);
	    /* Normally, at this point, gateways_from_dns == NULL.
	     * If another negotiation, in parallel, obviated the need
	     * for this info, it might not have been used and freed.
	     * So we have to free it to cover that case.
	     */
	    gw_delref(&gateways_from_dns);
	}
	log("&&&&complete!");
	complete_state_transition(&vc->md, r);
    }
    release_md(vc->md);
    cur_state = NULL;
}

static stf_status
quick_inI1_outR1_tail(struct msg_digest *md
, struct p2id *my, struct p2id *his
, unsigned int new_iv_len
, const u_char new_iv[MAX_DIGEST_LEN])
{
    struct state *const p1st = md->st;
    struct connection *c = p1st->st_connection;
    struct payload_digest *const id_pd = md->chain[ISAKMP_NEXT_ID];
    ip_subnet *our_net = &my->net
	, *his_net = &his->net;

    u_char	/* set by START_HASH_PAYLOAD: */
	*r_hashval,	/* where in reply to jam hash value */
	*r_hash_start;	/* from where to start hashing */

DB_ALG_INFO(c->alg_info_esp);//add by zhs for debug,2005-3-11

    /* Now that we have identities of client subnets, we must look for
     * a suitable connection (our current one only matches for hosts).
     */
	//zhangbin 2004.12.10
	if(c->that.client.addr.u.v4.sin_addr.s_addr != 0 && c->that.client.maskbits != 0)
    {
	struct connection *p = find_client_connection(c
	    , our_net, his_net, my->proto, my->port, his->proto, his->port);

	if (p == NULL)
	{
	    /* This message occurs in very puzzling circumstances
	     * so we must add as much information and beauty as we can.
	     */
	    struct end
		me = c->this,
		he = c->that;
	    char buf[2*SUBNETTOT_BUF + 2*ADDRTOT_BUF + 2*IDTOA_BUF + 2*ADDRTOT_BUF + 12]; /* + 12 for separating */
	    size_t l;

	    me.client = *our_net;
	    me.has_client = !subnetishost(our_net)
		|| !addrinsubnet(&me.host_addr, our_net);
	    me.protocol = my->proto;
	    me.port = my->port;

	    he.client = *his_net;
	    he.has_client = !subnetishost(his_net)
		|| !addrinsubnet(&he.host_addr, his_net);
	    he.protocol = his->proto;
	    he.port = his->port;

	    l = format_end(buf, sizeof(buf), &me, NULL, TRUE);
	    l += snprintf(buf + l, sizeof(buf) - l, "...");
	    (void)format_end(buf + l, sizeof(buf) - l, &he, NULL, FALSE);
	    log("cannot respond to IPsec SA request"
		" because no connection is known for %s"
		, buf);
		
#ifdef ZB_DEBUG
	    {
		    char errbuf[80];	
		memset(errbuf,0,sizeof(errbuf));
		strcpy(errbuf,"in quick_inI1_outR1_tail!\n");
		send_log(errbuf);
	    }
#endif
	  return STF_FAIL + INVALID_ID_INFORMATION;
	}
	else if (p != c)
	{
	    /* We've got a better connection: it can support the
	     * specified clients.  But it may need instantiation.
	     */
	    if (p->kind == CK_TEMPLATE)
	    {
		/* Yup, it needs instantiation.  How much?
		 * Is it a Road Warrior connection (simple)
		 * or is it an Opportunistic connection (needing gw validation)?
		 */
		if (p->policy & POLICY_OPPO)
		{
		    /* Opportunistic.
		     * We need to determine if this peer is authorized
		     * to negotiate for this client!  If the peer's
		     * client is the peer, we assume that it is authorized.
		     * Since p isn't yet instantiated, we need to look
		     * in c for description of peer.
		     */
		    ip_address our_client
			, his_client;

		    passert(subnetishost(our_net) && subnetishost(his_net));

		    networkof(our_net, &our_client);
		    networkof(his_net, &his_client);

		    /* refcounts for gateways_from_dns must balance,
		     * but NULL needs no count!
		     */
		    if (!sameaddr(&c->that.host_addr, &his_client))
		    {
			if (gateways_from_dns == NULL)
			{
			    /* initiate asynch DNS lookup and suspend */
			    struct verify_continuation *vc
				= alloc_thing(struct verify_continuation
				  , "verify continuation");
			    struct id pc_id;
			    err_t ugh;

			    /* Record that state is used by a suspended md */
			    passert(p1st->st_suspended_md == NULL);
			    vc->md = p1st->st_suspended_md = md;

			    vc->my = *my;
			    vc->his = *his;
			    vc->new_iv_len = p1st->st_new_iv_len;
			    memcpy(vc->new_iv, p1st->st_new_iv, p1st->st_new_iv_len);
			    iptoid(&his_client, &pc_id);
			    ugh = start_adns_query(&pc_id
				, &c->that.id
				, T_TXT
				, quick_inI1_outR1_continue
				, &vc->ac);
			    if (ugh != NULL)
			    {
				report_verify_failure(p1st, his, ugh);
				p1st->st_suspended_md = NULL;
		    
#ifdef ZB_DEBUG
				{
					char errbuf[80];	
					memset(errbuf,0,sizeof(errbuf));
					strcpy(errbuf,"in quick_inI1_outR1_tail!\n");
					send_log(errbuf);
				}
#endif
				return STF_FAIL + INVALID_ID_INFORMATION;
			    }
			    else
			    {
				return STF_SUSPEND;
			    }
			}
			else
			{
			    /* use result of asynch DNS lookup */
			    struct gw_info *gwp;

			    /* check that the public key that authenticated
			     * the ISAKMP SA (p1st) will do for this gateway.
			     */
			    for (gwp = gateways_from_dns; ; gwp = gwp->next)
			    {
				if (gwp == NULL)
				{
				    /* out of luck */
				    loglog(RC_OPPOFAILURE, "peer and client disagree about public key");
				    gw_delref(&gateways_from_dns);
				    return STF_FAIL + INVALID_ID_INFORMATION;
				}
				passert(same_id(&gwp->gw_id, &c->that.id));
				/* If there is a key from the TXT record,
				 * we count it as a win if we match the key.
				 * If there was no key, we claim a match since
				 * it implies fetching a KEY from the same
				 * place we must have gotten it.
				 */
				if (!gwp->gw_key_present
				|| same_RSA_public_key(&p1st->st_peer_pubkey->u.rsa
				, &gwp->gw_key))
				    break;	/* good! */
			    }
			}
		    }

		    /* Instantiate inbound Opportunistic connection,
		     * carrying over authenticated peer ID
		     * and filling in a few more details.
		     */
		    p = oppo_instantiate(p, &c->that.host_addr, &c->that.id
			, gateways_from_dns, &our_client, &his_client);
		    gw_delref(&gateways_from_dns);	/* gateways_from_dns is dead */
		}
		else
		{
		    /* Plain Road Warrior:
		     * instantiate, carrying over authenticated peer ID
		     */
		    p = rw_instantiate(p, &c->that.host_addr,
#ifdef NAT_TRAVERSAL
				md->sender_port,
#endif
#ifdef VIRTUAL_IP
				his_net,
#endif
				&c->that.id);
		}
	    }
#ifdef DEBUG
	    /* temporarily bump up cur_debugging to get "using..." message
	     * printed if we'd want it with new connection.
	     */
	    {
		unsigned int old_cur_debugging = cur_debugging;

		cur_debugging |= p->extra_debugging;
		DBG(DBG_CONTROL, DBG_log("using connection \"%s\"", p->name));
		cur_debugging = old_cur_debugging;
	    }
#endif
	    c = p;
	}
	/* fill in the client's true ip address/subnet */
	if (p->that.has_client_wildcard)
	{
	    p->that.client = *his_net;
	    p->that.has_client_wildcard = FALSE;
	}
#ifdef VIRTUAL_IP
	else if (is_virtual_connection(c))
	{
	    c->that.client = *his_net;
	    c->that.virt = NULL;
	    if (subnetishost(his_net) && addrinsubnet(&c->that.host_addr, his_net))
		c->that.has_client = FALSE;
	}
#endif
    }

    /* now that we are sure of our connection, create our new state */
    {
	struct state *const st = duplicate_state(p1st);

	/* first: fill in missing bits of our new state object
	 * note: we don't copy over st_peer_pubkey, the public key
	 * that authenticated the ISAKMP SA.  We only need it in this
	 * routine, so we can "reach back" to p1st to get it.
	 */

	if (st->st_connection != c)
	{
	    struct connection *t = st->st_connection;

	    st->st_connection = c;
	    set_cur_connection(c);
	    connection_discard(t);
	}

	st->st_try = 0;	/* not our job to try again from start */

	st->st_msgid = md->hdr.isa_msgid;

	st->st_new_iv_len = new_iv_len;
	memcpy(st->st_new_iv, new_iv, new_iv_len);

	set_cur_state(st);	/* (caller will reset) */
	md->st = st;	/* feed back new state */

	st->st_peeruserprotoid = his->proto;
	st->st_peeruserport = his->port;
	st->st_myuserprotoid = my->proto;
	st->st_myuserport = my->port;

	insert_state(st);	/* needs cookies, connection, and msgid */

	/* copy the connection's
	 * IPSEC policy into our state.  The ISAKMP policy is water under
	 * the bridge, I think.  It will reflect the ISAKMP SA that we
	 * are using.
	 */
	st->st_policy = (p1st->st_policy & POLICY_ISAKMP_MASK)
	    | (c->policy & ~POLICY_ISAKMP_MASK);

#ifdef NAT_TRAVERSAL
	if (p1st->nat_traversal & NAT_T_DETECTED) {
	    st->nat_traversal = p1st->nat_traversal;
	    nat_traversal_change_port_lookup(md, md->st);
	}
	else {
	    st->nat_traversal = 0;
	}
	if ((st->nat_traversal & NAT_T_DETECTED) &&
	    (st->nat_traversal & NAT_T_WITH_NATOA)) {
	    nat_traversal_natoa_lookup(md);
	}
#endif

	//zhangbin for VPN_log
#ifdef VPN_LOG
	{
		char rx_qi1[80];
		memset(rx_qi1,0,sizeof(rx_qi1));
		sprintf(rx_qi1,"2nIKE[\"%s\"] Rx {{ QM_I1 : %s HASH, SA, NONCE, KE, ID, ID",c->name,ip_str(&c->that.host_addr));
	    	send_log(rx_qi1);
	  }
#endif

	/* Start the output packet.
	 *
	 * proccess_packet() would automatically generate the HDR*
	 * payload if smc->first_out_payload is not ISAKMP_NEXT_NONE.
	 * We don't do this because we wish there to be no partially
	 * built output packet if we need to suspend for asynch DNS.
	 *
	 * We build the reply packet as we parse the message since
	 * the parse_ipsec_sa_body emits the reply SA
	 */

	/* HDR* out */
	echo_hdr(md, TRUE, ISAKMP_NEXT_HASH);

	/* HASH(2) out -- first pass */
	START_HASH_PAYLOAD(md->rbody, ISAKMP_NEXT_SA);

	/* process SA (in and out) */
	{
	    struct payload_digest *const sapd = md->chain[ISAKMP_NEXT_SA];
	    pb_stream r_sa_pbs;
	    struct isakmp_sa sa = sapd->payload.sa;

	    /* sa header is unchanged -- except for np */
	    sa.isasa_np = ISAKMP_NEXT_NONCE;
	    if (!out_struct(&sa, &isakmp_sa_desc, &md->rbody, &r_sa_pbs))
		return STF_INTERNAL_ERROR;

	    /* parse and accept body */
	    st->st_pfs_group = &unset_group;
	    RETURN_STF_FAILURE(parse_ipsec_sa_body(&sapd->pbs
		    , &sapd->payload.sa, &r_sa_pbs, FALSE, st));
	}

	passert(st->st_pfs_group != &unset_group);
        
        {//zhs add for PFS bug,Dec 29,2004
       
        //	if ((st->st_policy & POLICY_PFS) && st->st_pfs_group == NULL)
             int my_pfs = (st->st_policy & POLICY_PFS) ? 1:0;
             int his_pfs = (st->st_pfs_group == NULL) ? 0:1;
             if( my_pfs != his_pfs)
	    {
	       loglog(RC_LOG_SERIOUS, "we require PFS but Quick I1 SA specifies no GROUP_DESCRIPTION");
		ZHS_LOG();
	       return STF_FAIL + NO_PROPOSAL_CHOSEN;	/* ??? */
	    }
        }
	/* Ni in */
	RETURN_STF_FAILURE(accept_nonce(md, &st->st_ni, "Ni"));

	/* [ KE ] in (for PFS) */
	RETURN_STF_FAILURE(accept_PFS_KE(md, &st->st_gi, "Gi", "Quick Mode I1"));

	log("responding to Quick Mode");

	/**** finish reply packet: Nr [, KE ] [, IDci, IDcr ] ****/

	/* Nr out */
	if (!build_and_ship_nonce(&st->st_nr, &md->rbody
	, st->st_pfs_group != NULL? ISAKMP_NEXT_KE : id_pd != NULL? ISAKMP_NEXT_ID : ISAKMP_NEXT_NONE
	, "Nr"))
	    return STF_INTERNAL_ERROR;

	/* [ KE ] out (for PFS) */

	if (st->st_pfs_group != NULL)
	{
	    if (!build_and_ship_KE(st, &st->st_gr, st->st_pfs_group
	    , &md->rbody, id_pd != NULL? ISAKMP_NEXT_ID : ISAKMP_NEXT_NONE))
		    return STF_INTERNAL_ERROR;

	    /* MPZ-Operations might be done after sending the packet... */

    #ifndef DODGE_DH_MISSING_ZERO_BUG
	    compute_dh_shared(st, st->st_gi, st->st_pfs_group);
    #endif
	}

	/* [ IDci, IDcr ] out */
	if  (id_pd != NULL)
	{
	    struct isakmp_ipsec_id *p = (void *)md->rbody.cur;	/* UGH! */

	    if (!out_raw(id_pd->pbs.start, pbs_room(&id_pd->pbs), &md->rbody, "IDci"))
		return STF_INTERNAL_ERROR;
	    p->isaiid_np = ISAKMP_NEXT_ID;

	    p = (void *)md->rbody.cur;	/* UGH! */

	    if (!out_raw(id_pd->next->pbs.start, pbs_room(&id_pd->next->pbs), &md->rbody, "IDcr"))
		return STF_INTERNAL_ERROR;
	    p->isaiid_np = ISAKMP_NEXT_NONE;
	}

#ifdef NAT_TRAVERSAL
	if ((st->nat_traversal & NAT_T_WITH_NATOA) &&
	    (st->nat_traversal & LELEM(NAT_TRAVERSAL_NAT_BHND_ME)) &&
	    (st->st_esp.attrs.encapsulation == ENCAPSULATION_MODE_TRANSPORT)) {
	    /** Send NAT-OA if our address is NATed and if we use Transport Mode */
	    if (!nat_traversal_add_natoa(ISAKMP_NEXT_NONE, &md->rbody, md->st)) {
		return STF_INTERNAL_ERROR;
	    }
	}
	if ((st->nat_traversal & NAT_T_DETECTED) &&
	    (st->st_esp.attrs.encapsulation == ENCAPSULATION_MODE_TRANSPORT) &&
	    (c->that.has_client)) {
	    /** Remove client **/
	    addrtosubnet(&c->that.host_addr, &c->that.client);
	    c->that.has_client = FALSE;
	}
#endif

	/* Compute reply HASH(2) and insert in output */
	(void)quick_mode_hash12(r_hashval, r_hash_start, md->rbody.cur
	    , st, &st->st_msgid, TRUE);

	/* Derive new keying material */
	compute_keymats(st);

	/* Tell the kernel to establish the new inbound SA
	 * (unless the commit bit is set -- which we don't support).
	 * We do this before any state updating so that
	 * failure won't look like success.
	 */
	if (!install_inbound_ipsec_sa(st))
	    return STF_INTERNAL_ERROR;	/* ??? we may be partly committed */

	/* encrypt message, except for fixed part of header */

	if (!encrypt_message(&md->rbody, st))
	    return STF_INTERNAL_ERROR;	/* ??? we may be partly committed */

	//zhangbin for VPN_log
#ifdef VPN_LOG
	{
		char tx_qr1[80];
		memset(tx_qr1,0,sizeof(tx_qr1));
		sprintf(tx_qr1,"2nIKE[\"%s\"] Tx >> QM_R1 : %s HASH, SA, NONCE, KE, ID, ID",c->name,ip_str(&c->that.host_addr));
	    send_log(tx_qr1);
	  }
#endif

	return STF_OK;
    }
}

/* Handle (the single) message from Responder in Quick Mode.
 * HDR*, HASH(2), SA, Nr [, KE ] [, IDci, IDcr ] -->
 * HDR*, HASH(3)
 * (see draft-ietf-ipsec-isakmp-oakley-07.txt 5.5)
 * Installs inbound and outbound IPsec SAs, routing, etc.
 */
stf_status
quick_inR1_outI2(struct msg_digest *md)
{
    struct state *const st = md->st;
    const struct connection *c = st->st_connection;

    /* HASH(2) in */
    CHECK_QUICK_HASH(md
	, quick_mode_hash12(hash_val, hash_pbs->roof, md->message_pbs.roof
	    , st, &st->st_msgid, TRUE)
	, "HASH(2)", "Quick R1");

    /* SA in */
    {
	struct payload_digest *const sa_pd = md->chain[ISAKMP_NEXT_SA];

	RETURN_STF_FAILURE(parse_ipsec_sa_body(&sa_pd->pbs
	    , &sa_pd->payload.sa, NULL, TRUE, st));
    }

    /* Nr in */
    RETURN_STF_FAILURE(accept_nonce(md, &st->st_nr, "Nr"));

    /* [ KE ] in (for PFS) */
    RETURN_STF_FAILURE(accept_PFS_KE(md, &st->st_gr, "Gr", "Quick Mode R1"));

    if (st->st_pfs_group != NULL)
    {
	compute_dh_shared(st, st->st_gr, st->st_pfs_group);
#ifdef DODGE_DH_MISSING_ZERO_BUG
	if (st->st_shared.ptr[0] == 0)
	    return STF_REPLACE_DOOMED_EXCHANGE;
#endif
    }

    /* [ IDci, IDcr ] in; these must match what we sent */

    {
	struct payload_digest *const id_pd = md->chain[ISAKMP_NEXT_ID];

	if (id_pd != NULL)
	{
	    /* ??? we are assuming IPSEC_DOI */

	    /* IDci (we are initiator) */

	    if (!check_net_id(&id_pd->payload.ipsec_id, &id_pd->pbs
	    , &st->st_myuserprotoid, &st->st_myuserport
	    , &st->st_connection->this.client
	    , "our client"))
	{
#ifdef ZB_DEBUG
		char errbuf[80];	
		memset(errbuf,0,sizeof(errbuf));
		strcpy(errbuf,"in quick_inR1_outI2:our client!\n");
		send_log(errbuf);
#endif
		return STF_FAIL + INVALID_ID_INFORMATION;
	}

	    /* IDcr (responder is peer) */

	    if (!check_net_id(&id_pd->next->payload.ipsec_id, &id_pd->next->pbs
	    , &st->st_peeruserprotoid, &st->st_peeruserport
	    , &st->st_connection->that.client
	    , "peer client"))
		{
#ifdef ZB_DEBUG
			char errbuf[80];	
			memset(errbuf,0,sizeof(errbuf));
			strcpy(errbuf,"in quick_inR1_outI2:peer client!\n");
			send_log(errbuf);
#endif

		return STF_FAIL + INVALID_ID_INFORMATION;
		}
	}
	else
	{
	    /* No IDci, IDcr: we must check that the defaults match our proposal.
	     * Parallels a sequence of assignments in quick_outI1.
	     */
	    if (!subnetishost(&c->this.client) || !subnetishost(&c->that.client))
	    {
#ifdef ZB_DEBUG
		    char errbuf[80];	
			memset(errbuf,0,sizeof(errbuf));
			strcpy(errbuf,"in quick_inR1_outI2:subnetishost!\n");
			send_log(errbuf);
#endif

		loglog(RC_LOG_SERIOUS, "IDci, IDcr payloads missing in message"
		    " but default does not match proposal");
		return STF_FAIL + INVALID_ID_INFORMATION;
	    }
	}
    }

#ifdef NAT_TRAVERSAL
	if ((st->nat_traversal & NAT_T_DETECTED) &&
	    (st->nat_traversal & NAT_T_WITH_NATOA)) {
	    nat_traversal_natoa_lookup(md);
	}
#endif
//zhangbin for VPN-log
#ifdef VPN_LOG
	{
		char rx_qr1[80];
		memset(rx_qr1,0,sizeof(rx_qr1));
		sprintf(rx_qr1,"2nIKE[\"%s\"] Rx {{ QM_R1 : %s HASH, SA, NONCE, KE, ID, ID",st->st_connection->name,ip_str(&st->st_connection->that.host_addr));
		//send_log(messages);
//		log("rx_qr1=%s",rx_qr1);
		//syslog(LOG_CRIT,"%s",rx_qr1);
		send_log(rx_qr1);
	}
#endif
	
    /* ??? We used to copy the accepted proposal into the state, but it was
     * never used.  From sa_pd->pbs.start, length pbs_room(&sa_pd->pbs).
     */

    /**************** build reply packet HDR*, HASH(3) ****************/

    /* HDR* out done */

    /* HASH(3) out -- since this is the only content, no passes needed */
    {
	u_char	/* set by START_HASH_PAYLOAD: */
	    *r_hashval,	/* where in reply to jam hash value */
	    *r_hash_start;	/* start of what is to be hashed */

	START_HASH_PAYLOAD(md->rbody, ISAKMP_NEXT_NONE);
	(void)quick_mode_hash3(r_hashval, st);
    }
    //zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char tx_qi2[80];
	    memset(tx_qi2,0,sizeof(tx_qi2));
	    sprintf(tx_qi2,"2nIKE[\"%s\"] Tx >> QM_I2 : %s HASH",st->st_connection->name,ip_str(&st->st_connection->that.host_addr));
		send_log(tx_qi2);
//	    syslog(LOG_CRIT,"%s",tx_qi2);
	}
#endif

    /* Derive new keying material */
    compute_keymats(st);

    /* Tell the kernel to establish the inbound, outbound, and routing part
     * of the new SA (unless the commit bit is set -- which we don't support).
     * We do this before any state updating so that
     * failure won't look like success.
     */
    if (!install_ipsec_sa(st, TRUE))
	return STF_INTERNAL_ERROR;

    /* encrypt message, except for fixed part of header */

    if (!encrypt_message(&md->rbody, st))
	return STF_INTERNAL_ERROR;	/* ??? we may be partly committed */

    st->st_connection->newest_ipsec_sa = st->st_serialno;

    /* note (presumed) success */
    if (c->gw_info != NULL)
	c->gw_info->last_worked_time = now();

#ifdef DPD
    if (st->st_connection->dpd_delay & st->st_connection->dpd_timeout)
	    dpd_init(st);
#endif

    return STF_OK;
}

/* Handle last message of Quick Mode.
 * HDR*, HASH(3) -> done
 * (see draft-ietf-ipsec-isakmp-oakley-07.txt 5.5)
 * Installs outbound IPsec SAs, routing, etc.
 */
stf_status
quick_inI2(struct msg_digest *md)
{
    struct state *const st = md->st;

    /* HASH(3) in */
    CHECK_QUICK_HASH(md, quick_mode_hash3(hash_val, st)
	, "HASH(3)", "Quick I2");

    //zhangbin for VPN_log
#ifdef VPN_LOG
    {
	    char rx_qi2[80];
	    memset(rx_qi2,0,sizeof(rx_qi2));
	    sprintf(rx_qi2,"2nIKE[\"%s\"] Rx {{ QM_I2 : %s HASH",st->st_connection->name,ip_str(&st->st_connection->that.host_addr));
	    //log("rx_qi2=%s",rx_qi2);
	    //syslog(LOG_CRIT,"%s",rx_qi2);
	    send_log(rx_qi2);
	}
#endif

    /* Tell the kernel to establish the outbound and routing part of the new SA
     * (the previous state established inbound)
     * (unless the commit bit is set -- which we don't support).
     * We do this before any state updating so that
     * failure won't look like success.
     */
    if (!install_ipsec_sa(st, FALSE))
	return STF_INTERNAL_ERROR;

    st->st_connection->newest_ipsec_sa = st->st_serialno;

    update_iv(st);	/* not actually used, but tidy */

    /* note (presumed) success */
    {
	struct gw_info *gw = st->st_connection->gw_info;

	if (gw != NULL)
	    gw->last_worked_time = now();
    }

#ifdef DPD
    if(st->st_connection->dpd_delay && st->st_connection->dpd_timeout)
	    dpd_init(st);
#endif


    return STF_OK;
}

#ifdef DPD
static stf_status
send_isakmp_notification(struct state *st,
	u_int16_t type, const void *data, size_t len)
{
    msgid_t msgid;
    pb_stream reply;
    pb_stream rbody;
    u_char old_new_iv[MAX_DIGEST_LEN];
    u_char old_iv[MAX_DIGEST_LEN];
    u_char
	*r_hashval,     /* where in reply to jam hash value */
	*r_hash_start;  /* start of what is to be hashed */

    msgid = generate_msgid(st);

    init_pbs(&reply, reply_buffer, sizeof(reply_buffer), "ISAKMP notify");

    /* HDR* */
    {
	struct isakmp_hdr hdr;

	hdr.isa_version = ISAKMP_MAJOR_VERSION << ISA_MAJ_SHIFT | ISAKMP_MINOR_VERSION;
	hdr.isa_np = ISAKMP_NEXT_HASH;
	hdr.isa_xchg = ISAKMP_XCHG_INFO;
	hdr.isa_msgid = msgid;
	hdr.isa_flags = ISAKMP_FLAG_ENCRYPTION;
	memcpy(hdr.isa_icookie, st->st_icookie, COOKIE_SIZE);
	memcpy(hdr.isa_rcookie, st->st_rcookie, COOKIE_SIZE);
	if (!out_struct(&hdr, &isakmp_hdr_desc, &reply, &rbody))
	    impossible();
    }
  
    /* HASH -- create and note space to be filled later */
    START_HASH_PAYLOAD(rbody, ISAKMP_NEXT_N);

    /* NOTIFY */
    {
	pb_stream notify_pbs;
	struct isakmp_notification isan;

	isan.isan_np = ISAKMP_NEXT_NONE;
	isan.isan_doi = ISAKMP_DOI_IPSEC;
	isan.isan_protoid = PROTO_ISAKMP;
	isan.isan_spisize = COOKIE_SIZE * 2;
	isan.isan_type = type;
	if (!out_struct(&isan, &isakmp_notification_desc, &rbody, &notify_pbs))
	    return STF_INTERNAL_ERROR;
	if (!out_raw(st->st_icookie, COOKIE_SIZE, &notify_pbs, "notify icookie"))
	    return STF_INTERNAL_ERROR;
	if (!out_raw(st->st_rcookie, COOKIE_SIZE, &notify_pbs, "notify rcookie"))
	    return STF_INTERNAL_ERROR;
	if (data != NULL && len > 0)
	    if (!out_raw(data, len, &notify_pbs, "notify data"))
		return STF_INTERNAL_ERROR;
	close_output_pbs(&notify_pbs);
    }

    {
	/* finish computing HASH */
	struct hmac_ctx ctx;

	hmac_init_chunk(&ctx, st->st_oakley.hasher, st->st_skeyid_a);
	hmac_update(&ctx, (const u_char *) &msgid, sizeof(msgid_t));
	hmac_update(&ctx, r_hash_start, rbody.cur-r_hash_start);
	hmac_final(r_hashval, &ctx);

	DBG(DBG_CRYPT,
		DBG_log("HASH computed:");
		DBG_dump("", r_hashval, ctx.hmac_digest_size));
    }

    /* save old IV (this prevents from copying a whole new state object 
     * for NOTIFICATION / DELETE messages we don't need to maintain a state
     * because there are no retransmissions...
     */

    memcpy(old_new_iv, st->st_new_iv, st->st_new_iv_len);
    memcpy(old_iv, st->st_iv, st->st_iv_len);
    init_phase2_iv(st, &msgid);

    if (!encrypt_message(&rbody, st))
	return STF_INTERNAL_ERROR;

    {
        chunk_t saved_tpacket = st->st_tpacket;

        setchunk(st->st_tpacket, reply.start, pbs_offset(&reply));
        send_packet(st, "ISAKMP notify");
        st->st_tpacket = saved_tpacket;
    }    

    /* get back old IV for this state */
    memcpy(st->st_new_iv, old_new_iv, st->st_new_iv_len);
    memcpy(st->st_iv, old_iv, st->st_iv_len);

    return STF_IGNORE;
}

static void
dpd_init(struct state *st)
{
    struct state *p1st;

    /* find the related Phase 1 state */
    p1st = find_state(st->st_icookie, st->st_rcookie,
	    &st->st_connection->that.host_addr, 0);
    if (p1st == NULL)
	loglog(RC_LOG_SERIOUS, "could not find phase 1 state for DPD");
    else if (p1st->st_dpd) {
	    log("Dead Peer Detection (draft-ietf-ipsec-dpd-02) enabled");
	event_schedule(EVENT_DPD, st->st_connection->dpd_delay, st);
    }
}

bool was_eroute_idle(struct state *st, time_t since_when);

void
dpd_outI(struct state *p2st)
{
    struct state *st;
    time_t tm;
    u_int32_t seqno;
    time_t delay = p2st->st_connection->dpd_delay;
    time_t timeout = p2st->st_connection->dpd_timeout;

    /* find the related Phase 1 state */
    st = find_phase1_state(p2st->st_connection, FALSE);
    if (st == NULL)
    {
	loglog(RC_LOG_SERIOUS, "could not find newest phase 1 state for DPD");
	return;
    }
    if (!st->st_dpd)
	return;

    /* If an R_U_THERE has been sent or received recently, then
     * base the resend time on that. */
    tm = now();
    if (tm < st->st_last_dpd + delay)
    {
	event_schedule(EVENT_DPD, st->st_last_dpd + delay - tm, p2st);
	/* If there is still a timeout for the last R_U_THERE sent,
	 * and the timeout is greater than ours, then reduce it. */
	if (st->st_dpd_event != NULL
	&& st->st_dpd_event->ev_time > st->st_last_dpd + timeout)
	{
	    delete_dpd_event(st);
	    event_schedule(EVENT_DPD_TIMEOUT, st->st_last_dpd + timeout - tm, st);
	}
	return;
    }

    event_schedule(EVENT_DPD, delay, p2st);

    /* Make sure we really need to invoke DPD */
    if(was_eroute_idle(p2st, delay) == FALSE) {
	    return;
    }

    if (!IS_ISAKMP_SA_ESTABLISHED(st->st_state))
	return;

    if (!st->st_dpd_seqno)
    {
	/* Get a non-zero random value that has room to grow */
	get_rnd_bytes((u_char *)&st->st_dpd_seqno, sizeof(st->st_dpd_seqno));
	st->st_dpd_seqno &= 0x7fff;
	st->st_dpd_seqno++;
    }
    seqno = htonl(st->st_dpd_seqno);
    if (send_isakmp_notification(st, R_U_THERE, &seqno, sizeof(seqno)) != STF_IGNORE)
    {
	loglog(RC_LOG_SERIOUS, "could not send R_U_THERE");
	return;
    }

    st->st_dpd_expectseqno = st->st_dpd_seqno++;
    st->st_last_dpd = tm;
    /* Only schedule a new timeout if there isn't one currently,
     * or if it would be sooner than the current timeout. */
    if (st->st_dpd_event == NULL
    || st->st_dpd_event->ev_time > tm + timeout)
    {
	delete_dpd_event(st);
	event_schedule(EVENT_DPD_TIMEOUT, timeout, st);
    }
}

stf_status
dpd_inI_outR(struct state *st, struct isakmp_notification *const n, pb_stream *n_pbs)
{
    time_t tm = now();
    u_int32_t seqno;

    if (!IS_ISAKMP_SA_ESTABLISHED(st->st_state))
    {
	loglog(RC_LOG_SERIOUS, "received R_U_THERE for unestablished ISKAMP SA");
	return STF_IGNORE;
    }

    if (n->isan_spisize != COOKIE_SIZE * 2 || pbs_left(n_pbs) < COOKIE_SIZE * 2)
    {
	loglog(RC_LOG_SERIOUS, "R_U_THERE has invalid SPI length (%d)", n->isan_spisize);
	return STF_FAIL + PAYLOAD_MALFORMED;
    }

    if (memcmp(n_pbs->cur, st->st_icookie, COOKIE_SIZE) != 0)
    {
	loglog(RC_LOG_SERIOUS, "R_U_THERE has invalid icookie");
	/* zhangbin 2005.3.15 for Inter-op with Cisco IOS */
	//return STF_FAIL + INVALID_COOKIE;
	return;
    }
    n_pbs->cur += COOKIE_SIZE;

    if (memcmp(n_pbs->cur, st->st_rcookie, COOKIE_SIZE) != 0)
    {
	loglog(RC_LOG_SERIOUS, "R_U_THERE has invalid rcookie");
	/* zhangbin 2005.3.15 for Inter-op with Cisco IOS */
	//return STF_FAIL + INVALID_COOKIE;
	return;
    }
    n_pbs->cur += COOKIE_SIZE;

    if (pbs_left(n_pbs) != sizeof(seqno))
    {
	loglog(RC_LOG_SERIOUS, "R_U_THERE has invalid data length (%d)", pbs_left(n_pbs));
	return STF_FAIL + PAYLOAD_MALFORMED;
    }

    seqno = ntohl(*(u_int32_t *)n_pbs->cur);
    if (st->st_dpd_peerseqno && seqno <= st->st_dpd_peerseqno) {
	loglog(RC_LOG_SERIOUS, "received old or duplicate R_U_THERE");
	return STF_IGNORE;
    }

    st->st_dpd_peerseqno = seqno;
    delete_dpd_event(st);

    if (send_isakmp_notification(st, R_U_THERE_ACK, n_pbs->cur, pbs_left(n_pbs)) != STF_IGNORE)
    {
	loglog(RC_LOG_SERIOUS, "could not send R_U_THERE_ACK");
	return STF_IGNORE;
    }

    st->st_last_dpd = tm;
    return STF_IGNORE;
}

stf_status
dpd_inR(struct state *st, struct isakmp_notification *const n, pb_stream *n_pbs)
{
    u_int32_t seqno;

    if (!IS_ISAKMP_SA_ESTABLISHED(st->st_state))
    {
	loglog(RC_LOG_SERIOUS, "recevied R_U_THERE_ACK for unestablished ISKAMP SA");
	return STF_FAIL;
    }

    if (n->isan_spisize != COOKIE_SIZE * 2 || pbs_left(n_pbs) < COOKIE_SIZE * 2)
    {
	loglog(RC_LOG_SERIOUS, "R_U_THERE_ACK has invalid SPI length (%d)", n->isan_spisize);
	return STF_FAIL + PAYLOAD_MALFORMED;
    }

    if (memcmp(n_pbs->cur, st->st_icookie, COOKIE_SIZE) != 0)
    {
	loglog(RC_LOG_SERIOUS, "R_U_THERE_ACK has invalid icookie");
	/* zhangbin 2005.3.15 for Inter-op with Cisco IOS */
	//return STF_FAIL + INVALID_COOKIE;
	return ;
    }
    n_pbs->cur += COOKIE_SIZE;

    if (memcmp(n_pbs->cur, st->st_rcookie, COOKIE_SIZE) != 0)
    {
	loglog(RC_LOG_SERIOUS, "R_U_THERE_ACK has invalid rcookie");
	/* zhangbin 2005.3.15 for Inter-op with Cisco IOS */
	//return STF_FAIL + INVALID_COOKIE;
	return;
    }
    n_pbs->cur += COOKIE_SIZE;

    if (pbs_left(n_pbs) != sizeof(seqno))
    {
	loglog(RC_LOG_SERIOUS, "R_U_THERE_ACK has invalid data length (%d)", pbs_left(n_pbs));
	return STF_FAIL + PAYLOAD_MALFORMED;
    }

    seqno = ntohl(*(u_int32_t *)n_pbs->cur);
    if (!st->st_dpd_expectseqno && seqno != st->st_dpd_expectseqno) {
	loglog(RC_LOG_SERIOUS, "R_U_THERE_ACK has unexpected sequence number");
	return STF_FAIL + PAYLOAD_MALFORMED;
    }

    st->st_dpd_expectseqno = 0;
    delete_dpd_event(st);
    return STF_IGNORE;
}

void
dpd_timeout(struct state *st)
{
    int action;
    char *name;

    action = st->st_connection->dpd_action;
    name = strdup(st->st_connection->name);

    passert(action == DPD_ACTION_HOLD || action == DPD_ACTION_CLEAR);

    loglog(RC_LOG_SERIOUS, "DPD: No response from peer - declaring peer dead");

    release_connection(st->st_connection);

    if(action == DPD_ACTION_HOLD) {
	    struct connection *c = con_by_name(name, TRUE);
	    
	    if(c != NULL) {
		    set_cur_connection(c);
		    if(!trap_connection(c))
			    log("cannot trap");
		    reset_cur_connection();
	    } else
		    log("cannot find connection");
    }
    
    if(name != NULL)
	    free(name);
}
#endif
