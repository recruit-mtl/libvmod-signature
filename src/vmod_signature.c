#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
	return (0);
}

size_t calc_decode_length(const char* b64input) { //Calculates the length of a decoded string
	size_t len = strlen(b64input),
		padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len-1] == '=') //last char is =
		padding = 1;

	return (len*3)/4 - padding;
}

int base64_decode(VRT_CTX, const char* b64message, unsigned char** buffer, size_t* length) { //Decodes a base64 encoded string
	BIO *bio, *b64;

	int decode_len = calc_decode_length(b64message);
	WS_Reserve(ctx->ws, 0);
	*buffer = (unsigned char*)ctx->ws->f;
	*buffer = (unsigned char*)malloc(decode_len + 1);
	(*buffer)[decode_len] = '\0';

	bio = BIO_new_mem_buf((void*)b64message, -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
	*length = BIO_read(bio, *buffer, strlen(b64message));
	BIO_free_all(bio);

	return (0); //success
}

VCL_BOOL
vmod_valid_signature(VRT_CTX, VCL_STRING msg_b64, VCL_STRING sig_b64, VCL_STRING pub_key)
{
	/* Returned to caller */
	int result = 0;
	int rc = 0;
	EVP_MD_CTX* evp_ctx = NULL;
	EVP_PKEY* vkey = NULL;
	BIO* bio = NULL;
	RSA* rsa = NULL;
	unsigned char* sig = NULL;
	size_t sig_len;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(ctx->ws, WS_MAGIC);

	if(!msg_b64 || !sig_b64 || !pub_key) {
			return 0;
	}

	evp_ctx = EVP_MD_CTX_create();
	AN(evp_ctx);

	OpenSSL_add_all_digests();
	const EVP_MD* md = EVP_get_digestbyname("SHA1");
	AN(md);

	rc = EVP_DigestInit_ex(evp_ctx, md, NULL);
	assert(rc == 1);

	bio = BIO_new_mem_buf((void *)pub_key, strlen(pub_key));
	AN(bio);
	rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, 0, NULL);
	vkey = EVP_PKEY_new();
	AN(vkey);
	rc = EVP_PKEY_assign_RSA(vkey, RSAPublicKey_dup(rsa));
	assert(rc == 1);

	rc = EVP_DigestVerifyInit(evp_ctx, NULL, md, NULL, vkey);
	assert(rc == 1);

	rc = EVP_DigestVerifyUpdate(evp_ctx, msg_b64, strlen(msg_b64));
	assert(rc == 1);

	base64_decode(ctx, sig_b64, &sig, &sig_len);
	result = EVP_DigestVerifyFinal(evp_ctx, sig, sig_len);
	WS_Release(ctx->ws, sig_len);

	if(ctx) {
			EVP_MD_CTX_destroy(evp_ctx);
			ctx = NULL;
	}

	if(vkey) {
		EVP_PKEY_free(vkey);
		vkey = NULL;
	}

	return result == 1;
}
