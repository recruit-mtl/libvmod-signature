#include <stdio.h>
#include <stdlib.h>

#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"

#include <openssl/evp.h>

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
	return (0);
}

VCL_BOOL
vmod_valid_signature(VRT_CTX, VCL_STRING msg, VCL_STRING signature, VCL_STRING pub_key)
{
	/* Returned to caller */
	int result = 0;
	int rc = 0;
	EVP_MD_CTX* ctx = NULL;
	EVP_PKEY* vkey = NULL;
	BIO* bio = NULL;
	RSA* rsa = NULL;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(ctx->ws, WS_MAGIC);

	if(!msg || !signature || !sig || !pub_key) {
			return 0;
	}

	ctx = EVP_MD_CTX_create();
	AN(ctx);

	const EVP_MD* md = EVP_get_digestbyname("SHA1");
	AN(md);

	rc = EVP_DigestInit_ex(ctx, md, NULL);
	assert(rc == 1);

	bio = BIO_new_mem_buf(pub_key, strlen(pub_key));
	AN(bio);
	rsa = PEM_read_bio_RSAPublicKey(bio, &rsa, 0, NULL);
	vkey = EVP_PKEY_new();
	AN(vkey);
	rc = EVP_PKEY_assign_RSA(vkey, RSAPrivateKey_dup(rsa));
	assert(rc == 1);

	rc = EVP_DigestVerifyInit(ctx, NULL, md, NULL, pkey);
	assert(rc == 1);

	rc = EVP_DigestVerifyUpdate(ctx, msg, strlen(msg));
	assert(rc == 1);

	/* Clear any errors for the call below */
	ERR_clear_error();

	rc = EVP_DigestVerifyFinal(ctx, sig, strlen(sig));
	assert(rc == 1);

	if(ctx) {
			EVP_MD_CTX_destroy(ctx);
			ctx = NULL;
	}

	if(vkey) {
		EVP_PKEY_free(vkey);
		vkey = NULL;
	}

	return !!result;
}
