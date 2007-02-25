/*
 * Database handling functions
 *
 * $Id: packet.h,v 1.1.1.1 2003/11/25 22:15:11 jharrell Exp $
 *
 */

#include "lane.h"
#include "connect.h"

/* Protos */
int send_arp_response(int fd, LaneControl_t *lc,
		      unsigned int status, Reg_t *found);

int send_register_response(int fd, LaneControl_t *lc,
			   unsigned int status, int reg);

int send_join_response(int fd, LaneControl_t *lc, int lecid, 
		       unsigned int status);

int send_control_frame(int fd, LaneControl_t *to_send);
int forward_arp_request(LaneControl_t *to_forward,
			Proxy_t *proxyl);
