#include <stdio.h>
#include <stdlib.h>

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

VCL_BOOL
vmod_valid_signature(VRT_CTX, VCL_STRING msg, VCL_STRING signature, VCL_STRING pub_key)
{
	/* Returned to caller */
	int result = 0;
	int rc = 0;
	EVP_MD_CTX* evp_ctx = NULL;
	EVP_PKEY* vkey = NULL;
	BIO* bio = NULL;
	RSA* rsa = NULL;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(ctx->ws, WS_MAGIC);

	if(!msg || !signature || !pub_key) {
			return 0;
	}

	evp_ctx = EVP_MD_CTX_create();
	AN(evp_ctx);

	const EVP_MD* md = EVP_get_digestbyname("SHA1");
	AN(md);

	rc = EVP_DigestInit_ex(evp_ctx, md, NULL);
	assert(rc == 1);

	bio = BIO_new_mem_buf((void *)pub_key, strlen(pub_key));
	AN(bio);
	rsa = PEM_read_bio_RSAPublicKey(bio, &rsa, 0, NULL);
	vkey = EVP_PKEY_new();
	AN(vkey);
	rc = EVP_PKEY_assign_RSA(vkey, RSAPrivateKey_dup(rsa));
	assert(rc == 1);

	rc = EVP_DigestVerifyInit(evp_ctx, NULL, md, NULL, vkey);
	assert(rc == 1);

	rc = EVP_DigestVerifyUpdate(evp_ctx, msg, strlen(msg));
	assert(rc == 1);

	rc = EVP_DigestVerifyFinal(evp_ctx, signature, strlen(signature));
	assert(rc == 1);

	if(ctx) {
			EVP_MD_CTX_destroy(evp_ctx);
			ctx = NULL;
	}

	if(vkey) {
		EVP_PKEY_free(vkey);
		vkey = NULL;
	}

	return !!result;
}
