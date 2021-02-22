#if !defined(OF) && defined(lint)
#include "gzip.h"
#endif
#ifndef BITS
#define BITS 12 /* bits len */
#endif
#define INIT_BITS 9          /* Initial number of bits per code */
#define LZW_MAGIC "\037\235" /* Magic header for lzw files, 1F 9D */
#define BIT_MASK 0x1f        /* Mask for 'number of compression bits' */
#define BLOCK_MODE 0x80
#define LZW_RESERVED 0x60 /* reserved bits */
#define CLEAR 256         /* flush the dictionary */
#define FIRST (CLEAR + 1) /* first free entry */
extern int maxbits;       /* max bits per code for LZW */
extern int block_mode;    /* block compress mode -C compatible with 2.0 */
extern int lzw OF((int in, int out));
extern int unlzw OF((int in, int out));
