#ifndef __Sha256_H__
#define __Sha256_H__

typedef unsigned char BYTE; // 8-bit byte
typedef unsigned long WORD; // 32-bit word, change to "long" for 16-bit machines

typedef struct
{
	BYTE data[64];			   // current 512-bit chunk of message data, just like a buffer
	WORD datalen;			   // sign the data length of current chunk
	unsigned long long bitlen; // the bit length of the total message
	WORD state[8];			   // store the middle state of hash abstract
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
void sha256_final(SHA256_CTX *ctx, BYTE hash[]);
void sha256_deal(const BYTE *data, BYTE *hash, size_t len);

#endif
