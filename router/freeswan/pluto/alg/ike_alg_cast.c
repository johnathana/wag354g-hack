#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <freeswan.h>

#include "constants.h"
#include "defs.h"
#include "log.h"
#include "libcast/cast.h"
#include "alg_info.h"
#include "ike_alg.h"

#define  CAST_KEY_LEN		128
#define  CAST_CBC_BLOCK_SIZE	8

static void
do_cast(u_int8_t *buf, size_t buf_size, u_int8_t *key, size_t key_size __attribute__((unused)), u_int8_t *iv, bool enc)
{
    CAST_KEY cast_ctx;
    passert (!key_size || (key_size==CAST_KEY_LEN/BITS_PER_BYTE))
    CAST_set_key(&cast_ctx, key_size, key);
    CAST_cbc_encrypt(buf, buf, buf_size, &cast_ctx, iv, enc);
}

struct encrypt_desc encrypt_desc_cast =
{
	algo_type: IKE_ALG_ENCRYPT,
	algo_id:   OAKLEY_CAST_CBC,
	algo_next: NULL, 
	enc_ctxsize: sizeof(CAST_KEY),
	enc_blocksize: CAST_CBC_BLOCK_SIZE,
	keyminlen: CAST_KEY_LEN,
	keydeflen: CAST_KEY_LEN,
	keymaxlen: CAST_KEY_LEN,
	do_crypt: do_cast,
};
int ike_alg_cast_init(void);
int
ike_alg_cast_init(void)
{
	int ret = ike_alg_register_enc(&encrypt_desc_cast);
	return ret;
}
/*
IKE_ALG_INIT_NAME: ike_alg_cast_init
*/
