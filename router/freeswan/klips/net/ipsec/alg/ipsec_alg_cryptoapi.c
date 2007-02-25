/*
 * ipsec_alg to linux cryptoapi GLUE
 *
 * Authors: CODE.ar TEAM
 * 	Harpo MAxx <harpo@linuxmendoza.org.ar>
 * 	JuanJo Ciarlante <jjo-ipsec@mendoza.gov.ar>
 * 	Luciano Ruete <docemeses@softhome.net>
 * 
 * $Id: ipsec_alg_cryptoapi.c,v 1.1.1.2 2005/03/28 06:57:39 sparq Exp $
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
 * Example usage:
 *   modinfo -p ipsec_cryptoapi   (quite useful info, including supported algos)
 *   modprobe ipsec_cryptoapi
 *   modprobe ipsec_cryptoapi test=1
 *   modprobe ipsec_cryptoapi excl=1                     (exclusive cipher/algo)
 *   modprobe ipsec_cryptoapi latebind=1                 (defer cipher binding)
 *   modprobe ipsec_cryptoapi noauto=1  aes=1 twofish=1  (only these ciphers)
 *   modprobe ipsec_cryptoapi aes=128,128                (force these keylens)
 *   modprobe ipsec_cryptoapi des_ede3=0                 (everything but 3DES)
 */
#include <linux/config.h>
#include <linux/version.h>

/*	
 *	special case: ipsec core modular with this static algo inside:
 *	must avoid MODULE magic for this file
 */
#if CONFIG_IPSEC_MODULE && CONFIG_IPSEC_ALG_CRYPTOAPI
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

/* warn the innocent */
#if !defined (CONFIG_CRYPTO) && !defined (CONFIG_CRYPTO_MODULE)
#warning "No linux cryptoapi setup found... you *MUST*  install it ! (www.kerneli.org)"
#endif
/*	Low freeswan header coupling	*/
#include <ipsec_alg.h>
#include <linux/crypto.h>

#ifndef  CRYPTO_API_VERSION_CODE
#error "you *MUST*  install linux cryptoapi ! (www.kerneli.org)"
#endif
#define ESP_3DES		3
#define CIPHERNAME_3DES		"des_ede3-cbc"

#define ESP_AES			12
#define CIPHERNAME_AES		"aes-cbc"

#define ESP_BLOWFISH		7	/* truely _constant_  :)  */
#define CIPHERNAME_BLOWFISH	"blowfish-cbc"

#define ESP_CAST		6	/* quite constant :) */
#define CIPHERNAME_CAST		"cast5-cbc"

#define ESP_SERPENT		252	/* from ipsec drafts */
#define CIPHERNAME_SERPENT	"serpent-cbc"

#define ESP_TWOFISH		253	/* from ipsec drafts */
#define CIPHERNAME_TWOFISH	"twofish-cbc"

#define AH_MD5			2
#define DIGESTNAME_MD5		"md5"

#define AH_SHA			3
#define DIGESTNAME_SHA1		"sha1"


#ifdef CONFIG_IPSEC_ALG_NON_LIBRE
#define ESP_MARS		249	/* from ipsec drafts */
#define CIPHERNAME_MARS		"mars-cbc"

#define ESP_RC6			250	/* from ipsec drafts */
#define CIPHERNAME_RC6		"rc6-cbc"
#endif /* CONFIG_IPSEC_ALG_NON_LIBRE */

MODULE_AUTHOR("CODE.ar team: Harpo MAxx, Juanjo Ciarlante, Luciano Ruete");
static int debug=0;
MODULE_PARM(debug, "i");
static int test=0;
MODULE_PARM(test, "i");
static int excl=0;
MODULE_PARM(excl, "i");

static int noauto = 0;
MODULE_PARM(noauto,"i");
MODULE_PARM_DESC(noauto, "Dont try all known algos, just setup enabled ones");

static int latebind = 0;
MODULE_PARM(latebind,"i");
MODULE_PARM_DESC(latebind, "Bind cryptoapi ciphers at SA creation time");

static int des_ede3[] = {-1, -1};
static int aes[] = {-1, -1};
static int blowfish[] = {-1, -1};
static int cast[] = {-1, -1};
static int serpent[] = {-1, -1};
static int twofish[] = {-1, -1};
#ifdef CONFIG_IPSEC_ALG_NON_LIBRE
static int mars[] = {-1, -1};
static int rc6[] = {-1, -1};
#endif /* CONFIG_IPSEC_ALG_NON_LIBRE */

MODULE_PARM(des_ede3,"1-2i");
MODULE_PARM(aes,"1-2i");
MODULE_PARM(blowfish,"1-2i");
MODULE_PARM(cast,"1-2i");
MODULE_PARM(serpent,"1-2i");
MODULE_PARM(twofish,"1-2i");
#ifdef CONFIG_IPSEC_ALG_NON_LIBRE
MODULE_PARM(mars,"1-2i");
MODULE_PARM(rc6,"1-2i");
#endif /* CONFIG_IPSEC_ALG_NON_LIBRE */
MODULE_PARM_DESC(des_ede3, "0: disable | 1: force_enable | min,max: dontuse");
MODULE_PARM_DESC(aes, "0: disable | 1: force_enable | min,max: keybitlens");
MODULE_PARM_DESC(blowfish, "0: disable | 1: force_enable | min,max: keybitlens");
MODULE_PARM_DESC(cast, "0: disable | 1: force_enable | min,max: keybitlens");
MODULE_PARM_DESC(serpent, "0: disable | 1: force_enable | min,max: keybitlens");
MODULE_PARM_DESC(twofish, "0: disable | 1: force_enable | min,max: keybitlens");
#ifdef CONFIG_IPSEC_ALG_NON_LIBRE
MODULE_PARM_DESC(mars, "0: disable | 1: force_enable | min,max: keybitlens");
MODULE_PARM_DESC(rc6, "0: disable | 1: force_enable | min,max: keybitlens");
#endif /* CONFIG_IPSEC_ALG_NON_LIBRE */

struct ipsec_alg_capi_cipher {
	const char *ciphername;	/* cryptoapi's ciphername */
	int *parm;		/* lkm param for this cipher */
	struct cipher_implementation *ci;	/* actual ci */
	struct ipsec_alg_enc alg;	/* note it's not a pointer */
	atomic_t ci_usecnt;		/* ci use count */
};
static struct ipsec_alg_capi_cipher alg_capi_carray[] = {
	{ CIPHERNAME_AES,     aes    , NULL, { ixt_alg_id: ESP_AES, }},
	{ CIPHERNAME_TWOFISH, twofish, NULL, { ixt_alg_id: ESP_TWOFISH, }},
	{ CIPHERNAME_SERPENT, serpent, NULL, { ixt_alg_id: ESP_SERPENT, }},
	{ CIPHERNAME_CAST,    cast   , NULL, { ixt_alg_id: ESP_CAST, }},
	{ CIPHERNAME_BLOWFISH,blowfish,NULL, { ixt_alg_id: ESP_BLOWFISH, }},
	{ CIPHERNAME_3DES,    des_ede3,NULL, { ixt_alg_id: ESP_3DES, }},
#ifdef CONFIG_IPSEC_ALG_NON_LIBRE
	{ CIPHERNAME_MARS,    mars   , NULL, { ixt_alg_id: ESP_MARS, }},
	{ CIPHERNAME_RC6,     rc6    , NULL, { ixt_alg_id: ESP_RC6, }},
#endif /* CONFIG_IPSEC_ALG_NON_LIBRE */
	{ NULL, NULL, NULL, {} }
};
#ifdef NOT_YET
struct ipsec_alg_capi_digest {
	const char *digestname;	/* cryptoapi's digestname */
	struct digest_implementation *di;
	struct ipsec_alg_auth alg;	/* note it's not a pointer */
};
static struct ipsec_alg_capi_cipher alg_capi_darray[] = {
	{ DIGESTNAME_MD5,     NULL, { ixt_alg_id: AH_MD5, }},
	{ DIGESTNAME_SHA1,    NULL, { ixt_alg_id: AH_SHA, }},
	{ NULL, NULL, {} }
};
#endif
/*
 * 	"generic" linux cryptoapi setup_cipher() function
 */
static struct cipher_implementation *
setup_cipher (const char *ciphername)
{
	struct cipher_implementation *ci;
	/* 	1: atomic */
	ci = find_cipher_by_name (ciphername, 1); 
	if (debug > 0)
		printk(KERN_DEBUG "klips_debug:setup_cipher():"
				"ciphername=%s ci=%p\n"
				, ciphername, ci);
	if (!ci) {
		printk (KERN_INFO "cipher \"%s\" not found\n", ciphername);
		return NULL;
	}
	return ci;
}

/*
 * 	map cryptoapi's bitmaps to KLIPS min,max bitlens,
 * 	considering user passed parms for forced bitlens.
 * 	smells quite heuristic ...
 */
static void
setup_keylens(struct ipsec_alg_capi_cipher *cptr, struct cipher_implementation *ci,  int *keyminbitsp, int *keymaxbitsp)
{
	struct capi_keysz {
		int mask;
		int len;
	};
	struct capi_keysz *kz;
	struct capi_keysz capi_keysz_array[] = {
		{ CIPHER_KEYSIZE_40   , 40 },
		{ CIPHER_KEYSIZE_56   , 56 },
		{ CIPHER_KEYSIZE_64   , 64 },
		{ CIPHER_KEYSIZE_80   , 80 },
		{ CIPHER_KEYSIZE_96   , 96 },
		{ CIPHER_KEYSIZE_112  , 112 },
		{ CIPHER_KEYSIZE_128  , 128 },
		{ CIPHER_KEYSIZE_160  , 160 },
		{ CIPHER_KEYSIZE_168  , 168 },
		{ CIPHER_KEYSIZE_192  , 192 },
		{ CIPHER_KEYSIZE_256  , 256 },
		{ 0 , 0 }
	};
	if (cptr->parm[0] > 1 && cptr->parm[1] > 1) {
		*keyminbitsp=cptr->parm[0];
		*keymaxbitsp=cptr->parm[1];
	} else {
		*keyminbitsp=*keymaxbitsp=0;
		for (kz=capi_keysz_array; kz->mask; kz++) {
			if (kz->mask & ci->key_size_mask) {
				if (!*keyminbitsp) *keyminbitsp=kz->len;
				*keymaxbitsp=kz->len;
			}
		}
	}
}
/*
 * 	setups ipsec_alg_capi_cipher "hyper" struct components, calling
 * 	register_ipsec_alg for cointaned ipsec_alg object
 */
static void _capi_destroy_key (struct ipsec_alg_enc *alg, __u8 *key_e);
static __u8 * _capi_new_key (struct ipsec_alg_enc *alg, const __u8 *key, size_t keylen);
static int _capi_cbc_encrypt(struct ipsec_alg_enc *alg, __u8 * key_e, __u8 * in, int ilen, const __u8 * iv, int encrypt);

static int
setup_ipsec_alg_capi_cipher(struct ipsec_alg_capi_cipher *cptr, struct cipher_implementation *ci)
{
	int ret;
	int keyminbits, keymaxbits;
	cptr->ci = NULL;
	atomic_set (& cptr->ci_usecnt, 0);
	cptr->alg.ixt_version = IPSEC_ALG_VERSION;
	cptr->alg.ixt_module = THIS_MODULE;
	atomic_set (& cptr->alg.ixt_refcnt, 0);
	strncpy (cptr->alg.ixt_name , cptr->ciphername, sizeof (cptr->alg.ixt_name));

	cptr->alg.ixt_blocksize=ci->blocksize;
	setup_keylens(cptr, ci, &keyminbits, &keymaxbits);
	cptr->alg.ixt_state = 0;
	if (excl) cptr->alg.ixt_state |= IPSEC_ALG_ST_EXCL;
	cptr->alg.ixt_keyminbits=keyminbits;
	cptr->alg.ixt_keymaxbits=keymaxbits;
	cptr->alg.ixt_e_keylen=cptr->alg.ixt_keymaxbits/8;
	cptr->alg.ixt_e_ctx_size = 0;
	cptr->alg.ixt_alg_type = IPSEC_ALG_TYPE_ENCRYPT;
	cptr->alg.ixt_e_new_key = _capi_new_key;
	cptr->alg.ixt_e_destroy_key = _capi_destroy_key;
	cptr->alg.ixt_e_cbc_encrypt = _capi_cbc_encrypt;
	cptr->alg.ixt_data = cptr;

	ret=register_ipsec_alg_enc(&cptr->alg);
	printk("setup_ipsec_alg_capi_cipher(): " 
			"alg_type=%d alg_id=%d name=%s "
			"keyminbits=%d keymaxbits=%d, ret=%d\n", 
				cptr->alg.ixt_alg_type, 
				cptr->alg.ixt_alg_id, 
				cptr->alg.ixt_name, 
				cptr->alg.ixt_keyminbits,
				cptr->alg.ixt_keymaxbits,
				ret);
	return ret;
}
/*
 * 	bind cryptoapi cipher to ipsec_alg_capi_cipher "hyper" struct 
 * 	it will also create 1 lock() inside cipher
 */
static struct cipher_implementation *
_capi_ci_bind(struct ipsec_alg_capi_cipher *cptr, struct cipher_implementation *ci)
{
	if (atomic_read(&cptr->ci_usecnt) == 0) {
		cptr->ci=ci? ci : setup_cipher(cptr->ciphername);
	}
	if (cptr->ci) {
		atomic_inc(&cptr->ci_usecnt);
		cptr->ci->lock();
	}
	return cptr->ci;
}
/*
 * 	unbind cryptoapi cipher from ipsec_alg_capi_cipher "hyper" struct,
 * 	will do 1 unlock()
 */
static void
_capi_ci_unbind(struct ipsec_alg_capi_cipher *cptr)
{
	struct cipher_implementation *ci = cptr->ci;
	if (ci) {
		if (atomic_dec_and_test(&cptr->ci_usecnt) ) {
			cptr->ci = NULL;
		}
		ci->unlock();
	}
}
/*
 * 	called in ipsec_sa_wipe() time, will destroy key contexts
 * 	and do 1 unbind()
 */
static void 
_capi_destroy_key (struct ipsec_alg_enc *alg, __u8 *key_e)
{
	struct cipher_context *cx = (struct cipher_context *)key_e;
	struct cipher_implementation* ci=cx->ci;
	struct ipsec_alg_capi_cipher *cptr = alg->ixt_data;


	if (debug > 0)
		printk(KERN_DEBUG "klips_debug: _capi_destroy_key:"
				"name=%s key_e=%p ci=%p\n",
				alg->ixt_name, key_e, ci);
	if (!cx) {
		printk(KERN_ERR "klips_debug: _capi_destroy_key:"
				"name=%s NULL key_e!\n",
				alg->ixt_name);
		return;
	}

	ci->wipe_context(cx);
	ci->free_context(cx);
	/* ci->unlock(); */
	_capi_ci_unbind(cptr);
}
	
/*
 * 	create new key context, need alg->ixt_data to know which
 * 	(of many) cipher inside this module is the target
 */
static __u8 *
_capi_new_key (struct ipsec_alg_enc *alg, const __u8 *key, size_t keylen)
{
	struct cipher_context *cx = NULL;
	struct ipsec_alg_capi_cipher *cptr;
	struct cipher_implementation *ci;

	cptr = alg->ixt_data;
	if (!cptr) {
		printk(KERN_ERR "_capi_new_key(): "
				"NULL ixt_data (?!) for \"%s\" algo\n" 
				, alg->ixt_name);
		goto err;
	}
	if (debug > 0)
		printk(KERN_DEBUG "klips_debug:_capi_new_key:"
				"name=%s cptr=%p key=%p keysize=%d\n",
				alg->ixt_name, cptr, key, keylen);
	
	/* 
	 * 	sortof "cache": try to bind the cipher (will be noop if
	 *	already in use); if 1st SA using this cipher will
	 *	end doing find_cipher_by_name()
	 *	
	 *	this logic is needed to support "latebind"
	 */
	if (!(ci=_capi_ci_bind(cptr, NULL)))
		goto err;
	/* ci->lock (); */

	if (debug > 0)
		printk(KERN_DEBUG "klips_debug:_capi_new_key (after setup):"
				"name=%s ci=%p\n",
				alg->ixt_name, ci);
	/*	
	 *	alloc cipher context for this key
	 */
	cx = ci->realloc_context (NULL, ci, keylen);
	if (!cx) {
		printk(KERN_ERR "_capi_new_key(): "
				"NULL cx for \"%s\" cryptoapi algo\n" 
			, alg->ixt_name);
		/* ci->unlock (); */
		goto err;
	}
	if (ci->set_key (cx, key, keylen) < 0) {
		printk(KERN_ERR "_capi_new_key(): "
				"failed new_key() for \"%s\" cryptoapi algo\n" 
			, alg->ixt_name);
		ci->wipe_context (cx);
		ci->free_context (cx);
		/* ci->unlock (); */
		cx=NULL;
	}
err:
	/*
	 * 	unbind 1 lease if failed (bad key, ENOMEM, etc ...) 
	 */
	if (!cx)
		_capi_ci_unbind(cptr);

	if (debug > 0)
		printk(KERN_DEBUG "klips_debug:_capi_new_key:"
				"name=%s key=%p keylen=%d cx=%p\n",
				alg->ixt_name, key, keylen, cx);
	return (__u8 *) cx;
}
/*
 * 	core encryption function: will use cx->ci to call actual cipher's
 * 	cbc function
 */
static int 
_capi_cbc_encrypt(struct ipsec_alg_enc *alg, __u8 * key_e, __u8 * in, int ilen, const __u8 * iv, int encrypt) {
	int error;
	struct cipher_context *cx=(struct cipher_context *)key_e;
	if (debug > 0)
		printk(KERN_DEBUG "klips_debug:_capi_cbc_encrypt:"
				"cx=%p key_e=%p "
				"in=%p out=%p ilen=%d iv=%p encrypt=%d\n"
				, cx , key_e
				, in, in, ilen, iv, encrypt);
	/* this _iv() functions seems to appear AFTER 2.4.18.1 */
	if (encrypt)
		error = cx->ci->encrypt_atomic_iv (cx, in, in, ilen, iv);
	else
		error = cx->ci->decrypt_atomic_iv (cx, in, in, ilen, iv);
	if (debug > 0)
		printk(KERN_DEBUG "klips_debug:_capi_cbc_encrypt:"
				"error=%d\n"
				, error);
	return (error<0)? error : ilen;
}
/*
 * 	main initialization loop: for each cipher in list, do
 * 	1) setup cryptoapi cipher else continue
 * 	2) register ipsec_alg object
 * 	   dont bind (lock) cipher if latebind specified, it will
 * 	   get re-searched and locked at SA creation time (_new_key())
 */
static int
setup_cipher_list (struct ipsec_alg_capi_cipher* clist) 
{
	struct ipsec_alg_capi_cipher *cptr;
	struct cipher_implementation *ci;
	/* foreach cipher in list ... */
	for (cptr=clist;cptr->ciphername;cptr++) {
		/* 
		 * see if cipher has been disabled (0) or
		 * if noauto set and not enabled (1)
		 */
		if (cptr->parm[0] == 0 || (noauto && cptr->parm[0] < 0)) {
			if (debug>0)
				printk(KERN_INFO "setup_cipher_list(): "
					"ciphername=%s skipped at user request: "
					"noauto=%d parm[0]=%d parm[1]=%d\n"
					, cptr->ciphername
					, noauto
					, cptr->parm[0]
					, cptr->parm[1]);
			continue;
		}
		/* 
		 * 	use a temp ci to avoid touching cptr->ci,
		 * 	if register ipsec_alg success then bind cipher
		 */
		ci = setup_cipher(cptr->ciphername);
		if (ci) {
			ci->lock();
			if (debug > 0)
				printk(KERN_DEBUG "klips_debug:"
						"setup_cipher_list():"
						"ciphername=%s found\n"
				, cptr->ciphername);
			if (setup_ipsec_alg_capi_cipher(cptr, ci) == 0) {
				if (latebind==0)
					_capi_ci_bind(cptr, ci);
			} else {
				printk(KERN_ERR "klips_debug:"
						"setup_cipher_list():"
						"ciphername=%s failed ipsec_alg_register\n"
				, cptr->ciphername);
			}
			ci->unlock();
		}
	}
	return 0;
}
/*
 * 	deregister ipsec_alg objects and unbind ciphers
 */
static int
unsetup_cipher_list (struct ipsec_alg_capi_cipher* clist) 
{
	struct ipsec_alg_capi_cipher *cptr;
	/* foreach cipher in list ... */
	for (cptr=clist;cptr->ciphername;cptr++) {
		if (cptr->alg.ixt_state & IPSEC_ALG_ST_REGISTERED) {
			unregister_ipsec_alg_enc(&cptr->alg);
			if (latebind==0)
				_capi_ci_unbind(cptr);
		}
	}
	return 0;
}
/*
 * 	test loop for registered algos
 */
static int
test_cipher_list (struct ipsec_alg_capi_cipher* clist) 
{
	int test_ret;
	struct ipsec_alg_capi_cipher *cptr;
	/* foreach cipher in list ... */
	for (cptr=clist;cptr->ciphername;cptr++) {
		if (cptr->alg.ixt_state & IPSEC_ALG_ST_REGISTERED) {
			test_ret=ipsec_alg_test(
					cptr->alg.ixt_alg_type,
					cptr->alg.ixt_alg_id, 
					test);
			printk("test_cipher_list(alg_type=%d alg_id=%d): test_ret=%d\n", 
					cptr->alg.ixt_alg_type, 
					cptr->alg.ixt_alg_id, 
					test_ret);
		}
	}
	return 0;
}

IPSEC_ALG_MODULE_INIT( ipsec_cryptoapi_init )
{
	int ret, test_ret;
	if ((ret=setup_cipher_list(alg_capi_carray)) < 0)
		return  -EPROTONOSUPPORT;
	if (ret==0 && test) {
		test_ret=test_cipher_list(alg_capi_carray);
	}
	return ret;
}
IPSEC_ALG_MODULE_EXIT( ipsec_cryptoapi_fini )
{
	unsetup_cipher_list(alg_capi_carray);
	return;
}
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

EXPORT_NO_SYMBOLS;
