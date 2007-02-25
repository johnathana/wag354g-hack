/* $Id: ipsecrets2pem.c,v 1.1.1.2 2005/03/28 06:57:36 sparq Exp $ */
/*
 * Author: JuanJo Ciarlante <jjo-ipsec@mendoza.gov.ar>
 * Usage:  ./ipsecrets2pem < /etc/ipsec.secrets > hostkey.pem
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
 */


#include <stdio.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>

struct n2bn_t {
	const char *name;
	BIGNUM **bn;
};

static struct n2bn_t *
n2bn_find(struct n2bn_t *n2bn, const char *str, const char **off_p)
{
	for (; n2bn->name; n2bn++) {
		if (strncmp(n2bn->name, str, strlen(n2bn->name))==0) {
			if (off_p) *off_p=str+strlen(n2bn->name);
			return n2bn;
		}
	}
	return NULL;
	
}
static void n2bn_init(struct n2bn_t *n2bn) {
	for (; n2bn->name; n2bn++) { *n2bn->bn=NULL; }
}
static int
n2bn_check(struct n2bn_t *n2bn) 
{
	for (; n2bn->name; n2bn++) {
		if (*(n2bn->bn)==NULL) {
			fprintf(stderr, "Missing \"%s...\"\n", n2bn->name);
			return 0;
		}
	}
	return 1;
}

static struct n2bn_t *
n2bn_line(struct n2bn_t *n2bn, const char *buf)
{
	const char *offstr;
	struct n2bn_t *n2bn_tmp;
	n2bn_tmp=n2bn_find(n2bn, buf, &offstr);	
	if (n2bn_tmp) {
		*(n2bn_tmp->bn)=NULL;
		BN_hex2bn(n2bn_tmp->bn, offstr);
		return n2bn_tmp;
	}
	return NULL;
}

RSA rsa;
struct n2bn_t n2bn_RSA[] = {
	{ "\tModulus: 0x",         &rsa.n },
	{ "\tPublicExponent: 0x",  &rsa.e },
	{ "\tPrivateExponent: 0x", &rsa.d },
	{ "\tPrime1: 0x",          &rsa.p },
	{ "\tPrime2: 0x",          &rsa.q },
	{ "\tExponent1: 0x",       &rsa.dmp1 },
	{ "\tExponent2: 0x",       &rsa.dmq1 },
	{ "\tCoefficient: 0x",     &rsa.iqmp },
	{ NULL, NULL }
};

int main(void)
{
	char buf[1024];
	BIO *bp;

	n2bn_init(n2bn_RSA);
	while(fgets(buf, sizeof buf, stdin)) {
		n2bn_line(n2bn_RSA, buf);
	}
	if (!n2bn_check(n2bn_RSA)) {
		fprintf(stderr, "RSA build failed\n");
		return 1;
	}
	/* RSA_print_fp(stdout, &rsa, 1); */
	if (RSA_check_key(&rsa)!=1) {
		fprintf(stderr, "RSA_check_key() failed: %s\n", 
				ERR_error_string(ERR_get_error(), NULL));
		return 2;
	}
	if (!(bp=BIO_new_fp(stdout, 0))) {
		fprintf(stderr, "BIO_new_fp(stdout, 0) failed: %s\n", 
				ERR_error_string(ERR_get_error(), NULL));
		return 2;
	}
	/* PEM_write_bio_RSAPublicKey(bp, &rsa); */
	if (!PEM_write_bio_RSAPrivateKey(bp, &rsa,NULL,NULL,0,NULL,NULL)) {
		fprintf(stderr, "PEM_write_bio_RSAPublicKey() failed: %s\n", 
				ERR_error_string(ERR_get_error(), NULL));
		return 2;
	}
	return 0;
}
