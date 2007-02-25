/*
 * ipsec_alg CAST cipher stubs
 *
 * Author: JuanJo Ciarlante <jjo-ipsec@mendoza.gov.ar>
 * 
 * $Id: ipsec_alg_cast.c,v 1.1.1.2 2005/03/28 06:57:06 sparq Exp $
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
 */
#include <linux/config.h>
#include <linux/version.h>

/*	
 *	special case: ipsec core modular with this static algo inside:
 *	must avoid MODULE magic for this file
 */
#if CONFIG_IPSEC_MODULE && CONFIG_IPSEC_ALG_CAST
#undef MODULE
#endif

#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/string.h>

/* Check if __exit is defined, if not null it */
#ifndef __exit
#define __exit
#endif

/*	Low freeswan header coupling	*/
#include "ipsec_alg.h"
#define cast_context cast_key
#include "libcast/cast.h"

#define ESP_CAST		6	/* quite constant :) */

#define ESP_CAST_KEY_SZ_MIN	5
#define ESP_CAST_KEY_SZ		16 	/* 128 bit secret key */
#define ESP_CAST_CBC_BLK_LEN	8	/* CAST-CBC block size */

MODULE_AUTHOR("JuanJo Ciarlante <jjo-ipsec@mendoza.gov.ar>");
static int debug=0;
MODULE_PARM(debug, "i");
static int test=0;
MODULE_PARM(test, "i");
static int excl=0;
MODULE_PARM(excl, "i");
static int keyminbits=0;
MODULE_PARM(keyminbits, "i");
static int keymaxbits=0;
MODULE_PARM(keymaxbits, "i");

#undef cast_context
#define cast_context CAST_KEY
static int _cast_set_key(struct ipsec_alg_enc *alg, __u8 * key_e, const __u8 * key, size_t keysize) {
	cast_context *ctx=(cast_context *)key_e;
	if (debug > 0)
		printk(KERN_DEBUG "klips_debug:_cast_set_key:"
				"key_e=%p key=%p keysize=%d\n",
				key_e, key, keysize);
	CAST_set_key(ctx, keysize, (u_int8_t *)key);
	return 0;
}
static int _cast_cbc_encrypt(struct ipsec_alg_enc *alg, __u8 * key_e, __u8 * in, int ilen, const __u8 * iv, int encrypt) {
	/* cast toasts passed IV */
	__u8    iv_buf[ESP_CAST_CBC_BLK_LEN];
	cast_context *ctx=(cast_context *)key_e;
	*((__u32*)&(iv_buf)) = ((__u32*)(iv))[0];
	*((__u32*)&(iv_buf)+1) = ((__u32*)(iv))[1];
	if (debug > 0)
		printk(KERN_DEBUG "klips_debug:_cast_cbc_encrypt:"
				"key_e=%p in=%p ilen=%d iv=%p encrypt=%d\n",
				key_e, in, ilen, iv, encrypt);
	CAST_cbc_encrypt(in, in, ilen, ctx, iv_buf, encrypt);
	return ilen;
}
static struct ipsec_alg_enc ipsec_alg_CAST = {
	ixt_version:	IPSEC_ALG_VERSION,
	ixt_module:	THIS_MODULE,
	ixt_refcnt:	ATOMIC_INIT(0),
	ixt_alg_type:	IPSEC_ALG_TYPE_ENCRYPT,
	ixt_alg_id: 	ESP_CAST,
	ixt_name: 	"cast",
	ixt_blocksize:	ESP_CAST_CBC_BLK_LEN, 
	ixt_keyminbits:	ESP_CAST_KEY_SZ_MIN*8,
	ixt_keymaxbits:	ESP_CAST_KEY_SZ*8,
	ixt_e_keylen:	ESP_CAST_KEY_SZ,
	ixt_e_ctx_size:	sizeof(cast_context),
	ixt_e_set_key:	_cast_set_key,
	ixt_e_cbc_encrypt:_cast_cbc_encrypt,
};
	
IPSEC_ALG_MODULE_INIT(ipsec_cast_init)
{
	int ret, test_ret;
	if (keyminbits)
		ipsec_alg_CAST.ixt_keyminbits=keyminbits;
	if (keymaxbits) {
		ipsec_alg_CAST.ixt_keymaxbits=keymaxbits;
		if (keymaxbits*8>ipsec_alg_CAST.ixt_keymaxbits)
			ipsec_alg_CAST.ixt_e_keylen=keymaxbits*8;
	}
	if (excl) ipsec_alg_CAST.ixt_state |= IPSEC_ALG_ST_EXCL;
	ret=register_ipsec_alg_enc(&ipsec_alg_CAST);
	printk("ipsec_cast_init(alg_type=%d alg_id=%d name=%s): ret=%d\n", 
			ipsec_alg_CAST.ixt_alg_type, 
			ipsec_alg_CAST.ixt_alg_id, 
			ipsec_alg_CAST.ixt_name, 
			ret);
	if (ret==0 && test) {
		test_ret=ipsec_alg_test(
				ipsec_alg_CAST.ixt_alg_type,
				ipsec_alg_CAST.ixt_alg_id, 
				test);
		printk("ipsec_cast_init(alg_type=%d alg_id=%d): test_ret=%d\n", 
				ipsec_alg_CAST.ixt_alg_type, 
				ipsec_alg_CAST.ixt_alg_id, 
				test_ret);
	}
	return ret;
}
IPSEC_ALG_MODULE_EXIT(ipsec_cast_fini)
{
	unregister_ipsec_alg_enc(&ipsec_alg_CAST);
	return;
}
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

EXPORT_NO_SYMBOLS;
