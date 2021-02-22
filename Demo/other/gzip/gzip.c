#include "gzip.h"
#include "lzw.h"

/* global buffers */
uch *inbuf;
uch *outbuf;
ush *d_buf;
uch *window;
ush *tab_prefix;
//DECLARE(uch, inbuf, INBUFSIZ + INBUF_EXTRA);
//DECLARE(uch, outbuf, OUTBUFSIZ + OUTBUF_EXTRA);
//DECLARE(ush, d_buf, DIST_BUFSIZE);
//DECLARE(uch, window, 2L * WSIZE); /* 声明 */
//DECLARE(ush, tab_prefix, 1L << BITS);
const ulg crc_32_tab[] = {
    /* Table of CRC-32's of all single-byte values */
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419,
    /* Table of CRC-32's of all single-byte values */
    0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
    /* Table of CRC-32's of all single-byte values */
    0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07,
    /* Table of CRC-32's of all single-byte values */
    0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    /* Table of CRC-32's of all single-byte values */
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856,
    /* Table of CRC-32's of all single-byte values */
    0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    /* Table of CRC-32's of all single-byte values */
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
    /* Table of CRC-32's of all single-byte values */
    0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    /* Table of CRC-32's of all single-byte values */
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3,
    /* Table of CRC-32's of all single-byte values */
    0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
    /* Table of CRC-32's of all single-byte values */
    0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599,
    /* Table of CRC-32's of all single-byte values */
    0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    /* Table of CRC-32's of all single-byte values */
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
    /* Table of CRC-32's of all single-byte values */
    0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
    /* Table of CRC-32's of all single-byte values */
    0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e,
    /* Table of CRC-32's of all single-byte values */
    0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    /* Table of CRC-32's of all single-byte values */
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed,
    /* Table of CRC-32's of all single-byte values */
    0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    /* Table of CRC-32's of all single-byte values */
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3,
    /* Table of CRC-32's of all single-byte values */
    0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    /* Table of CRC-32's of all single-byte values */
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a,
    /* Table of CRC-32's of all single-byte values */
    0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
    /* Table of CRC-32's of all single-byte values */
    0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010,
    /* Table of CRC-32's of all single-byte values */
    0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    /* Table of CRC-32's of all single-byte values */
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17,
    /* Table of CRC-32's of all single-byte values */
    0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
    /* Table of CRC-32's of all single-byte values */
    0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615,
    /* Table of CRC-32's of all single-byte values */
    0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    /* Table of CRC-32's of all single-byte values */
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344,
    /* Table of CRC-32's of all single-byte values */
    0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    /* Table of CRC-32's of all single-byte values */
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a,
    /* Table of CRC-32's of all single-byte values */
    0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    /* Table of CRC-32's of all single-byte values */
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
    /* Table of CRC-32's of all single-byte values */
    0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
    /* Table of CRC-32's of all single-byte values */
    0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef,
    /* Table of CRC-32's of all single-byte values */
    0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    /* Table of CRC-32's of all single-byte values */
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe,
    /* Table of CRC-32's of all single-byte values */
    0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
    /* Table of CRC-32's of all single-byte values */
    0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
    /* Table of CRC-32's of all single-byte values */
    0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    /* Table of CRC-32's of all single-byte values */
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b,
    /* Table of CRC-32's of all single-byte values */
    0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    /* Table of CRC-32's of all single-byte values */
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1,
    /* Table of CRC-32's of all single-byte values */
    0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    /* Table of CRC-32's of all single-byte values */
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278,
    /* Table of CRC-32's of all single-byte values */
    0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
    /* Table of CRC-32's of all single-byte values */
    0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66,
    /* Table of CRC-32's of all single-byte values */
    0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    /* Table of CRC-32's of all single-byte values */
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605,
    /* Table of CRC-32's of all single-byte values */
    0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
    /* Table of CRC-32's of all single-byte values */
    0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b,
    /* Table of CRC-32's of all single-byte values */
    0x2d02ef8d};
/* local variables */
int quiet = 0;            /* be very quiet (-q) */
int maxbits = BITS;       /* max bits per code for LZW */
int method = MAX_METHODS; /* compression method */
int level = 6;            /* compression level */
int exit_code = 0;        /* program exit code */
int time_stamp;           /* original time stamp (modification time) */

int bytes_in = 0;        /* number of input bytes */
int bytes_out = 0;       /* number of output bytes */
int total_in = 0;        /* input bytes for all files */
int total_out = 0;       /* output bytes for all files */
unsigned int insize = 0; /* valid bytes in inbuf */
unsigned int inptr = 0;  /* index of next byte to be processed in inbuf */
unsigned int outcnt = 0; /* bytes in output buffer */
local int get_method OF((void));
int(*work) OF(()) = zip; /* function to call */
int zipmem(char *mem_inptr, int mem_insize, char *mem_outptr)
{
    /*printf("执行压缩\r\n");*/
    time_stamp = 0;
    /*printf("inbuf申请内存\r\n");*/
    ALLOC(uch, inbuf, mem_insize + INBUF_EXTRA);
    /*printf("outbuf申请内存\r\n");*/
    // ALLOC(uch, outbuf, OUTBUFSIZ + OUTBUF_EXTRA);
    ALLOC(uch, outbuf, mem_insize * 2 + 200);
    /*printf("d_buf申请内存\r\n");*/
    // if ((mem_insize * 4) > DIST_BUFSIZE)
    // {
    // 	ALLOC(ush, d_buf, DIST_BUFSIZE);
    // }
    // else
    // {
    ALLOC(ush, d_buf, mem_insize * 4 + 200);
    // }

    /*printf("window申请内存\r\n");*/
    ALLOC(uch, window, 2L * WSIZE); /* 分配 */
    /*printf("tab_prefix申请内存\r\n");*/
    ALLOC(ush, tab_prefix, 1L << BITS);

    clear_bufs();                /* clear input and output buffers */
    zip_mem_outptr = mem_outptr; /* 输出缓存 */
    zip_mem_outlen = 0;          /* 输出缓存大小 */
    zip_mem_inptr = mem_inptr;   /* 输入指针 */
    zip_mem_insize = mem_insize; /* 输入缓存长度 */
    zip_mem_inpos = 0;           /* 输入缓存当前位置 */
    zip();
    FREE(inbuf);
    FREE(outbuf);
    FREE(d_buf);
    FREE(window);
    FREE(tab_prefix);
    /*printf("压缩结束\r\n");*/
    return zip_mem_outlen;
}
int unzipmem(char *mem_inptr, int mem_insize, char *mem_outptr)
{
    /*printf("执行解压\r\n");*/
    time_stamp = 0;
    // printf("inbuf申请内存\r\n");
    // ALLOC(uch, inbuf, INBUFSIZ + INBUF_EXTRA);
    // printf("outbuf申请内存\r\n");
    // ALLOC(uch, outbuf, OUTBUFSIZ + OUTBUF_EXTRA);
    // printf("d_buf申请内存\r\n");
    // ALLOC(ush, d_buf, DIST_BUFSIZE);
    // printf("window申请内存\r\n");
    // ALLOC(uch, window, 2L * WSIZE); /* 分配 */
    // printf("tab_prefix申请内存\r\n");
    // ALLOC(ush, tab_prefix, 1L << BITS);
    /*printf("inbuf申请内存\r\n");*/
    ALLOC(uch, inbuf, mem_insize + INBUF_EXTRA);
    /*printf("outbuf申请内存\r\n");*/
    // ALLOC(uch, outbuf, OUTBUFSIZ + OUTBUF_EXTRA);
    ALLOC(uch, outbuf, mem_insize * 2 + 200);
    /*printf("d_buf申请内存\r\n");*/
    // if ((mem_insize * 4) > DIST_BUFSIZE)
    // {
    // 	ALLOC(ush, d_buf, DIST_BUFSIZE);
    // }
    // else
    // {
    ALLOC(ush, d_buf, mem_insize * 4 + 200);
    // }

    /*printf("window申请内存\r\n");*/
    ALLOC(uch, window, 2L * WSIZE); /* 分配 */
    /*printf("tab_prefix申请内存\r\n");*/
    ALLOC(ush, tab_prefix, 1L << BITS);

    clear_bufs();                  /* clear input and output buffers */
    zip_mem_outptr = mem_outptr;   /* 输出缓存 */
    zip_mem_outlen = 0;            /* 输出缓存大小 */
                                   /* 解压缩 */
    unzip_mem_inptr = mem_inptr;   /* 输入指针 */
    unzip_mem_insize = mem_insize; /* 输入缓存长度 */
    unzip_mem_inpos = 0;           /* 输入缓存当前位置 */
    get_method();
    unzip();
    FREE(inbuf);
    FREE(outbuf);
    FREE(d_buf);
    FREE(window);
    FREE(tab_prefix);
    /*printf("解压结束\r\n");*/
    return zip_mem_outlen;
}
local int get_method()
{
    /* If --force and --stdout, zcat == cat, so do not complain about
    * premature end of file: use try_byte instead of get_byte.
    */
    (char)get_byte();
    (char)get_byte();
    method = -1; /* unknown yet */
    /* assume multiple members in gzip file except for record oriented I/O */

    method = (int)get_byte();
    work = unzip;
    (uch) get_byte();
    (ulg) get_byte();
    ((ulg)get_byte());
    ((ulg)get_byte());
    ((ulg)get_byte());
    (void)get_byte(); /* Ignore extra flags for the moment */
    (void)get_byte(); /* Ignore OS type for the moment */

    return method;
}

ulg updcrc(uch *s, unsigned n)
{
    register ulg c;

    static ulg crc = (ulg)0xffffffff;

    if (s == NULL)
    {
        c = 0xffffffff;
    }
    else
    {
        c = crc;
        if (n)
            do
            {
                c = crc_32_tab[((int)c ^ (*s++)) & 0xff] ^ (c >> 8);
            } while (--n);
    }
    crc = c;
    return c ^ 0xffffffff;
}

void clear_bufs()
{
    outcnt = 0;
    insize = inptr = 0;
    bytes_in = bytes_out = 0L;
}

char *unzip_mem_inptr = 0;
int unzip_mem_insize = 0;
int unzip_mem_inpos = 0;

int fill_inbuf(void)
{
    int len;
#undef min
#define min(a, b) (((int)(a) < (int)(b)) ? (a) : (b))

    insize = 0;

    do
    {
        len = min(unzip_mem_insize - unzip_mem_inpos, INBUFSIZ - insize);
        if (len > 0)
        {
            memcpy((char *)inbuf + insize, &unzip_mem_inptr[unzip_mem_inpos], len);
            insize += len;
            unzip_mem_inpos += len;
        }
        else
            break;
    } while (insize < INBUFSIZ);

    if (insize == 0)
    {
        read_error();
    }
    bytes_in += (ulg)insize;
    inptr = 1;
    return inbuf[0];
}

void flush_outbuf()
{
    if (outcnt == 0)
        return;

    write_buf((char *)outbuf, outcnt);
    bytes_out += (ulg)outcnt;
    outcnt = 0;
}

void flush_window()
{
    if (outcnt == 0)
        return;
    updcrc(window, outcnt);
    write_buf((char *)window, outcnt);
    bytes_out += (ulg)outcnt;
    outcnt = 0;
}

char *zip_mem_outptr;
int zip_mem_outlen = 0;

void write_buf(voidp buf, unsigned cnt)
{
    memcpy(&zip_mem_outptr[zip_mem_outlen], buf, cnt);
    zip_mem_outlen += cnt;
}

void error(char *m)
{
}

void warn(char *a, char *b)
{
}

void read_error()
{
}

void write_error()
{
}

voidp xmalloc(unsigned size)
{
    voidp cp = (voidp)mymalloc(size);

    if (cp == NULL)
        error("out of memory");
    return cp;
}

local ulg crc;     /* crc on uncompressed file data */
local ulg crc = 0; /* crc on uncompressed file data */
#ifndef HASH_BITS
#define HASH_BITS (BITS - 1) /* hash */
/* For portability to 16 bit machines, do not use values above 15. */
#endif
#if (WSIZE << 1) > (1 << BITS) /* 编译判断 */
//error: cannot overlay window with tab_suffix and prev with tab_prefix0
#endif
#if HASH_BITS > (BITS - 1)
//error: cannot overlay head with tab_prefix1
#endif

#define HASH_SIZE (unsigned)(1 << HASH_BITS)
#define HASH_MASK (HASH_SIZE - 1)
#define WMASK (WSIZE - 1)
/* HASH_SIZE and WSIZE must be powers of two */
/* Tail of hash chains */
#define NIL 0

/* speed options for the general purpose bit flag */
#define FAST 4
/* speed options for the general purpose bit flag */
#define SLOW 2

#ifndef TOO_FAR
#define TOO_FAR 4096 /* TOO FAR */
#endif
/* Matches of length 3 are discarded if their distance exceeds TOO_FAR */

/* ===========================================================================
       * Local data used by the "longest match" routines.
       */

typedef ush Pos;
typedef unsigned IPos;
ulg window_size = (int)2 * WSIZE;
int block_start = 0;
local unsigned ins_h = 0; /* hash index of string to be inserted */

#define H_SHIFT ((HASH_BITS + MIN_MATCH - 1) / MIN_MATCH)
unsigned int near prev_length = 0;
unsigned near strstart = 0;    /* start of string to insert */
unsigned near match_start = 0; /* start of matching string */
local int eofile = 0;          /* flag set at end of input file */
local unsigned lookahead = 0;  /* number of valid bytes ahead in window */

unsigned near max_chain_length = 0;

local unsigned int max_lazy_match = 0;

#define max_insert_length max_lazy_match

unsigned near good_match = 0;

typedef struct config
{
    ush good_length; /* reduce lazy search above this match length */
    ush max_lazy;    /* do not perform lazy search above this match length */
    ush nice_length; /* quit search above this match length */
    ush max_chain;
} config;

int near nice_match = 0; /* Stop searching when current match exceeds this */

/* good lazy nice chain */
const local config configuration_table[10] = {
    {0, 0, 0, 0}, /* store only */ /* 0 */
    {4, 4, 8, 4},
    /* maximum speed, no lazy matches */ /* 1 */
    {4, 5, 16, 8},                       /* 2 */
    {4, 6, 32, 32},                      /* 3 */

    {4, 4, 16, 16},
    /* lazy matches */    /* 4 */
    {8, 16, 32, 32},      /* 5 */
    {8, 16, 128, 128},    /* 6 */
    {8, 32, 128, 256},    /* 7 */
    {32, 128, 258, 1024}, /* 8 */
    {32, 258, 258, 4096}};
/* maximum compression */ /* 9 */

#define EQUAL 0
/* result of memcmp for equal strings */

/* ===========================================================================
           * Prototypes for local functions.
           */
local void fill_window OF((void));

int longest_match OF((IPos cur_match));

#define UPDATE_HASH(h, c) (h = (((h) << H_SHIFT) ^ (c)) & HASH_MASK)

#define INSERT_STRING(s, match_head)                  \
    (UPDATE_HASH(ins_h, window[(s) + MIN_MATCH - 1]), \
     prev[(s)&WMASK] = match_head = head[ins_h],      \
     head[ins_h] = (s))
local unsigned short bi_buf = 0;
#define Buf_size (8 * 2 * sizeof(char))
local int bi_valid = 0;
int(*read_buf) OF((char *buf, unsigned size));

void bi_init(void)
{
    bi_buf = 0;
    bi_valid = 0;
    read_buf = mem_read;
}
void send_bits(value, length) int value; /* value to send */
int length;                              /* number of bits */
{
    if (bi_valid > (int)Buf_size - length)
    {
        bi_buf |= (value << bi_valid);
        put_short(bi_buf);
        bi_buf = (ush)value >> (Buf_size - bi_valid);
        bi_valid += length - Buf_size;
    }
    else
    {
        bi_buf |= value << bi_valid;
        bi_valid += length;
    }
}
unsigned bi_reverse(unsigned code, int len)
{
    register unsigned res = 0;
    do
    {
        res |= code & 1;
        code >>= 1, res <<= 1;
    } while (--len > 0);
    return res >> 1;
}
void bi_windup()
{
    if (bi_valid > 8) /* 是否大于short长度 */
    {
        put_short(bi_buf);
    }
    else if (bi_valid > 0)
    {
        put_byte(bi_buf);
    }
    bi_buf = 0;
    bi_valid = 0;
}
void copy_block(char *buf, unsigned len, int header)
{
    bi_windup(); /* align on byte boundary */

    if (header)
    {
        put_short((ush)len);
        put_short((ush)~len);
    }
    while (len--)
    {
        put_byte(*buf++);
    }
}

void lm_init(int pack_level, ush *flags)
{
    register unsigned j;

    /* Initialize the hash table. */
    memzero((char *)head, HASH_SIZE * sizeof(*head));
    /* prev will be initialized on the fly */

    /* Set the default configuration parameters:
               */
    max_lazy_match = configuration_table[pack_level].max_lazy;
    good_match = configuration_table[pack_level].good_length;

    nice_match = configuration_table[pack_level].nice_length;

    max_chain_length = configuration_table[pack_level].max_chain;
    /* ??? reduce max_chain_length for binary files */

    strstart = 0;
    block_start = 0L;

    lookahead = read_buf((char *)window,
                         sizeof(int) <= 2 ? (unsigned)WSIZE : 2 * WSIZE); /* 读入数据 */

    if (lookahead == 0 || lookahead == (unsigned)-1)
    {
        eofile = 1, lookahead = 0;
        return;
    }
    eofile = 0;
    /* Make sure that we always have enough lookahead. This is important
               * if input comes from a device such as a tty.
               */
    while (lookahead < MIN_LOOKAHEAD && !eofile)
        fill_window();

    ins_h = 0;
    for (j = 0; j < MIN_MATCH - 1; j++)
        UPDATE_HASH(ins_h, window[j]);
    /* If lookahead < MIN_MATCH, ins_h is garbage, but this is
               * not important since only literal bytes will be emitted.
               */
}

int longest_match(IPos cur_match)
{
    unsigned chain_length = max_chain_length; /* max hash chain length */
    register uch *scan = window + strstart;   /* current string */
    register uch *match;                      /* matched string */
    register int len;                         /* length of current match */
    int best_len = prev_length;               /* best match length so far */
    IPos limit = strstart > (IPos)MAX_DIST ? strstart - (IPos)MAX_DIST : NIL;
    /* Stop when cur_match becomes <= limit. To simplify the code,
               * we prevent matches with the string of window index 0.
               */

    /* The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2 multiple of 16.
               * It is easy to get rid of this optimization if necessary.
               */
    register uch *strend = window + strstart + MAX_MATCH;
    register uch scan_end1 = scan[best_len - 1];
    register uch scan_end = scan[best_len];

    if (prev_length >= good_match)
    {
        /* Do not waste too much time if we already have a good match: */
        chain_length >>= 2;
    }

    do
    {
        Assert(cur_match < strstart, "no future");
        match = window + cur_match;

        /* Skip to next match if the match length cannot increase
                   * or if the match length is less than 2:
                   */

        if (match[best_len] != scan_end ||
            match[best_len - 1] != scan_end1 ||
            *match != *scan ||
            *++match != scan[1])
            continue;
        scan += 2, match++;

        /* We check for insufficient lookahead only every 8th comparison;
                   * the 256th check will be made at strstart+258.
                   */
        do
        {
        } while (*++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 scan < strend);

        len = MAX_MATCH - (int)(strend - scan);
        scan = strend - MAX_MATCH;

        if (len > best_len)
        {
            match_start = cur_match;
            best_len = len;
            if (len >= nice_match)
                break;
            scan_end1 = scan[best_len - 1];
            scan_end = scan[best_len];
        }
    } while ((cur_match = prev[cur_match & WMASK]) > limit && --chain_length != 0);

    return best_len;
}

#define check_match(start, match, length)

local void fill_window()
{
    register unsigned n, m;
    unsigned more = (unsigned)(window_size - (ulg)lookahead - (ulg)strstart);
    if (more == (unsigned)-1)
    {
        more--;
    }
    else if (strstart >= WSIZE + MAX_DIST)
    {
        /* By the IN assertion, the window is not empty so we can't confuse
                   * more == 0 with more == 64K on a 16 bit machine.
                   */
        Assert(window_size == (ulg)2 * WSIZE, "no sliding with BIG_MEM");

        memcpy((char *)window, (char *)window + WSIZE, (unsigned)WSIZE);
        match_start -= WSIZE;
        strstart -= WSIZE; /* we now have strstart >= MAX_DIST: */

        block_start -= (int)WSIZE;

        for (n = 0; n < HASH_SIZE; n++)
        {
            m = head[n];
            head[n] = (Pos)(m >= WSIZE ? m - WSIZE : NIL);
        }
        for (n = 0; n < WSIZE; n++)
        {
            m = prev[n];
            prev[n] = (Pos)(m >= WSIZE ? m - WSIZE : NIL);
            /* If n is not on any hash chain, prev[n] is garbage but
                       * its value will never be used.
                       */
        }
        more += WSIZE;
    }
    /* At this point, more >= 2 */
    if (!eofile)
    {
        n = read_buf((char *)window + strstart + lookahead, more);
        if (n == 0 || n == (unsigned)-1)
        {
            eofile = 1;
        }
        else
        {
            lookahead += n;
        }
    }
}

/* ===========================================================================
           * Flush the current block, with given end-of-file flag.
           * IN assertion: strstart is set to the end of the current match.
           */
#define FLUSH_BLOCK(eof) \
    flush_block(block_start >= 0L ? (char *)&window[(unsigned)block_start] : (char *)NULL, (long)strstart - block_start, (eof))
ulg deflate()
{
    IPos hash_head;                                 /* head of hash chain */
    IPos prev_match;                                /* previous match */
    int flush;                                      /* set if current block must be flushed */
    int match_available = 0;                        /* set if previous match exists */
    register unsigned match_length = MIN_MATCH - 1; /* length of best match */

    /* Process the input block. */
    while (lookahead != 0)
    {
        /* Insert the string window[strstart .. strstart+2] in the
                   * dictionary, and set hash_head to the head of the hash chain:
                   */
        INSERT_STRING(strstart, hash_head);

        /* Find the longest match, discarding those <= prev_length.
                   */
        prev_length = match_length, prev_match = match_start;
        match_length = MIN_MATCH - 1;

        if (hash_head != NIL && prev_length < max_lazy_match &&
            strstart - hash_head <= MAX_DIST)
        {
            match_length = longest_match(hash_head);
            /* longest_match() sets match_start */
            if (match_length > lookahead)
                match_length = lookahead;

            /* Ignore a length 3 match if it is too distant: */
            if (match_length == MIN_MATCH && strstart - match_start > TOO_FAR)
            {
                match_length--;
            }
        }
        if (prev_length >= MIN_MATCH && match_length <= prev_length)
        {

            check_match(strstart - 1, prev_match, prev_length);

            flush = ct_tally(strstart - 1 - prev_match, prev_length - MIN_MATCH);
            lookahead -= prev_length - 1;
            prev_length -= 2; /* -2 */
            do
            {
                strstart++;
                INSERT_STRING(strstart, hash_head);
            } while (--prev_length != 0);
            match_available = 0;
            match_length = MIN_MATCH - 1;
            strstart++;
            if (flush)
                FLUSH_BLOCK(0), block_start = strstart;
        }
        else if (match_available)
        {
            Tracevv((stderr, "%c", window[strstart - 1]));
            if (ct_tally(0, window[strstart - 1]))
            {
                FLUSH_BLOCK(0), block_start = strstart;
            }
            strstart++;
            lookahead--;
        }
        else
        {
            /* There is no previous match to compare with, wait for
                           * the next step to decide.
                           */
            match_available = 1;
            strstart++;
            lookahead--;
        }
        Assert(strstart <= isize && lookahead <= isize, "a bit too far");
        while (lookahead < MIN_LOOKAHEAD && !eofile)
            fill_window();
    }
    if (match_available)
        ct_tally(0, window[strstart - 1]);

    return FLUSH_BLOCK(1); /* eof */
}
int zip(void)
{
    uch flags = 0; //通用位标致
    ush attr = 0;  //二进制标致
    ush deflate_flags = 0;
    outcnt = 0; /* Write the header to the gzip file. See algorithm.doc for the format */
    method = DEFLATED;
    put_byte(GZIP_MAGIC[0]); /* magic header */
    put_byte(GZIP_MAGIC[1]);
    put_byte(DEFLATED);   /* compression method */
    put_byte(flags);      /* general flags */
    put_long(time_stamp); /* Write deflated file to zip file */
    crc = updcrc(0, 0);
    bi_init();
    ct_init(&attr, &method);
    lm_init(level, &deflate_flags);
    put_byte((uch)deflate_flags); /* extra flags */
    put_byte(0);                  /* OS identifier */
    (void)deflate();              /* Write the crc and uncompressed size */
    put_long(crc);
    put_long(isize);
    flush_outbuf();
    return 0;
}
char *zip_mem_inptr = NULL; /* 输入缓存 */
int zip_mem_insize = 0;     /* 输入缓存长度 */
int zip_mem_inpos = 0;      /* 已经使用的位置 */
int mem_read(char *buf, unsigned size)
{
    int len;
#define min(a, b) (((int)(a) < (int)(b)) ? (a) : (b))
    len = min(zip_mem_insize - zip_mem_inpos, size);
    if (len > 0)
    {
        memcpy(buf, &zip_mem_inptr[zip_mem_inpos], len);
        crc = updcrc((uch *)buf, len);
        isize += (ulg)len;
        zip_mem_inpos += len;
    }
    else
        len = -1;
    return (int)len;
}

#define slide window
struct huft
{
    uch e; /* number of extra bits or operation */
    uch b; /* number of bits in this code or subcode */
    union
    {
        ush n;          /* literal, length base, or distance base */
        struct huft *t; /* pointer to next level of table */
    } v;
};

/* Function prototypes */
int huft_build OF((unsigned *, unsigned, unsigned, ush *, ush *,
                   struct huft **, int *));
int huft_free OF((struct huft *));
int inflate_codes OF((struct huft *, struct huft *, int, int));
int inflate_stored OF((void));
int inflate_fixed OF((void));
int inflate_dynamic OF((void));
int inflate_block OF((int *));
int inflate OF((void));
#define wp outcnt
#define flush_output(w) (wp = (w), flush_window())

/* Tables for deflate from PKZIP's appnote.txt. */
static unsigned border[] = {
    /* Order of the bit length code lengths */
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
static ush cplens[] = {
    /* Copy lengths for literal codes 257..285 */
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
    /* Copy lengths for literal codes 257..285 */
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
/* note: see note #13 above about the 258 in this list. */
static ush cplext[] = {
    /* Extra bits for literal codes 257..285 */
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    /* Extra bits for literal codes 257..285 */
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99}; /* 99==invalid */
static ush cpdist[] = {
    /* Copy offsets for distance codes 0..29 */
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
    /* Copy offsets for distance codes 0..29 */
    257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
    /* Copy offsets for distance codes 0..29 */
    8193, 12289, 16385, 24577};
static ush cpdext[] = {
    /* Extra bits for distance codes */
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, /* Extra bits for distance codes */
    7, 7, 8, 8, 9, 9, 10, 10, 11, 11,               /* Extra bits for distance codes */
    12, 12, 13, 13                                  /* Extra bits for distance codes */
};

ulg bb = 0;      /* bit buffer */
unsigned bk = 0; /* bits in bit buffer */

const ush mask_bits[] = {
    /* bits in mask bit buffer */
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff, /* bits in mask bit buffer */
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff  /* bits in mask bit buffer */
};

#define NEXTBYTE() (uch) get_byte()

#define NEEDBITS(n)                      \
    {                                    \
        while (k < (n))                  \
        {                                \
            b |= ((ulg)NEXTBYTE()) << k; \
            k += 8;                      \
        }                                \
    }
#define DUMPBITS(n) \
    {               \
        b >>= (n);  \
        k -= (n);   \
    }
int lbits = 9; /* bits in base literal/length lookup table */
int dbits = 6; /* bits in base distance lookup table */

/* If BMAX needs to be larger than 16, then h and x[] should be ulg. */
#define BMAX 16   /* maximum bit length of any code (16 for explode) */
#define N_MAX 288 /* maximum number of codes in any set */

unsigned hufts; /* track memory usage */

int huft_build(unsigned *b, unsigned n, unsigned s, ush *d, ush *e, struct huft **t, int *m)
{
    unsigned a;              /* counter for codes of length k */
    unsigned c[BMAX + 1];    /* bit length count table */
    unsigned f;              /* i repeats in table every f entries */
    int g;                   /* maximum code length */
    int h;                   /* table level */
    register unsigned i;     /* counter, current code */
    register unsigned j;     /* counter */
    register int k;          /* number of bits in current code */
    int l;                   /* bits per table (returned in m) */
    register unsigned *p;    /* pointer into c[], b[], or v[] */
    register struct huft *q; /* points to current table */
    struct huft r;           /* table entry for structure assignment */
    struct huft *u[BMAX];    /* table stack */
    unsigned v[N_MAX];       /* values in order of bit length */
    register int w;          /* bits before this table == (l * h) */
    unsigned x[BMAX + 1];    /* bit offsets, then code stack */
    unsigned *xp;            /* pointer into x */
    int y;                   /* number of dummy codes added */
    unsigned z;              /* number of entries in current table */

    /* Generate counts for each bit length */
    memzero(c, sizeof(c));
    p = b;
    i = n;
    do
    {
        Tracecv(*p, (stderr, (n - i >= ' ' && n - i <= '~' ? "%c %d\n" : "0x%x %d\n"),
                     n - i, *p));
        c[*p]++; /* assume all entries <= BMAX */
        p++;     /* Can't combine with above line (Solaris bug) */
    } while (--i);
    if (c[0] == n) /* null input--all zero length codes */
    {
        *t = (struct huft *)NULL;
        *m = 0;
        return 0;
    }

    /* Find minimum and maximum length, bound *m by those */
    l = *m;
    for (j = 1; j <= BMAX; j++)
        if (c[j])
            break;
    k = j; /* minimum code length */
    if ((unsigned)l < j)
        l = j;
    for (i = BMAX; i; i--)
        if (c[i])
            break;
    g = i; /* maximum code length */
    if ((unsigned)l > i)
        l = i;
    *m = l;

    /* Adjust last length count to fill out codes, if needed */
    for (y = 1 << j; j < i; j++, y <<= 1)
        if ((y -= c[j]) < 0)
            return 2; /* bad input: more codes than bits */
    if ((y -= c[i]) < 0)
        return 2; /* comment */
    c[i] += y;

    /* Generate starting offsets into the value table for each length */
    x[1] = j = 0;
    p = c + 1;
    xp = x + 2; /* comment */
    while (--i)
    { /* note that i == g from above */
        *xp++ = (j += *p++);
    }

    /* Make a table of values in order of bit lengths */
    p = b;
    i = 0;
    do
    {
        if ((j = *p++) != 0)
            v[x[j]++] = i;
    } while (++i < n);

    /* Generate the Huffman codes and for each, make the table entries */
    x[0] = i = 0;               /* first Huffman code is zero */
    p = v;                      /* grab values in bit order */
    h = -1;                     /* no tables yet--level -1 */
    w = -l;                     /* bits decoded == (l * h) */
    u[0] = (struct huft *)NULL; /* just to keep compilers happy */
    q = (struct huft *)NULL;    /* ditto */
    z = 0;                      /* ditto */

    /* go through the bit lengths (k already is bits in shortest code) */
    for (; k <= g; k++)
    {
        a = c[k];
        while (a--)
        {
            /* here i is the Huffman code of length k bits for value *p */
            /* make tables up to required level */
            while (k > w + l)
            {
                h++;
                w += l; /* previous table always l bits */

                /* compute minimum size table less than or equal to l bits */
                z = (z = g - w) > (unsigned)l ? l : z; /* upper limit on table size */
                if ((f = 1 << (j = k - w)) > a + 1)    /* try a k-w bit table */
                {                                      /* too few codes for k-w bit table */
                    f -= a + 1;                        /* deduct codes from patterns left */
                    xp = c + k;
                    while (++j < z) /* try smaller tables up to z bits */
                    {
                        if ((f <<= 1) <= *++xp)
                            break; /* enough codes to use up j bits */
                        f -= *xp;  /* else deduct codes from patterns */
                    }
                }
                z = 1 << j; /* table entries for j-bit table */

                /* allocate and link in new table */
                if ((q = (struct huft *)mymalloc((z + 1) * sizeof(struct huft))) ==
                    (struct huft *)NULL)
                {
                    if (h)
                        huft_free(u[0]);
                    return 3; /* not enough memory */
                }
                hufts += z + 1; /* track memory usage */
                *t = q + 1;     /* link to list for huft_free() */
                *(t = &(q->v.t)) = (struct huft *)NULL;
                u[h] = ++q; /* table starts after link */

                /* connect to last table, if there is one */
                if (h)
                {
                    x[h] = i;            /* save pattern for backing up */
                    r.b = (uch)l;        /* bits to dump before this table */
                    r.e = (uch)(16 + j); /* bits in this table */
                    r.v.t = q;           /* pointer to this table */
                    j = i >> (w - l);    /* (get around Turbo C bug) */
                    u[h - 1][j] = r;     /* connect to last table */
                }
            }

            /* set up table entry in r */
            r.b = (uch)(k - w);
            if (p >= v + n)
                r.e = 99; /* out of values--invalid code */
            else if (*p < s)
            {
                r.e = (uch)(*p < 256 ? 16 : 15); /* 256 is end-of-block code */
                r.v.n = (ush)(*p);               /* simple code is just the value */
                p++;                             /* one compiler does not like *p++ */
            }
            else
            {
                r.e = (uch)e[*p - s]; /* non-simple--look up in lists */
                r.v.n = d[*p++ - s];
            }

            /* fill code-like entries with r */
            f = 1 << (k - w);
            for (j = i >> w; j < z; j += f)
                q[j] = r;

            /* backwards increment the k-bit code i */
            for (j = 1 << (k - 1); i & j; j >>= 1)
                i ^= j;
            i ^= j;

            /* backup over finished tables */
            while ((i & ((1 << w) - 1)) != x[h])
            {
                h--; /* don't need to update q */
                w -= l;
            }
        }
    }

    /* Return true (1) if we were given an incomplete table */
    return y != 0 && g != 1;
}

int huft_free(struct huft *t)
{
    register struct huft *p, *q;

    /* Go through linked list, freeing from the malloced (t[-1]) address. */
    p = t;
    while (p != (struct huft *)NULL)
    {
        q = (--p)->v.t;
        __myfree(p);
        p = q;
    }
    return 0;
}

int inflate_codes(struct huft *tl, struct huft *td, int bl, int bd)
{
    register unsigned e; /* table entry flag/number of extra bits */
    unsigned n, d;       /* length and index for copy */
    unsigned w;          /* current window position */
    struct huft *t;      /* pointer to table entry */
    unsigned ml, md;     /* masks for bl and bd bits */
    register ulg b;      /* bit buffer */
    register unsigned k; /* number of bits in bit buffer */

    /* make local copies of globals */
    b = bb; /* initialize bit buffer */
    k = bk;
    w = wp; /* initialize window position */

    /* inflate the coded data */
    ml = mask_bits[bl]; /* precompute masks for speed */
    md = mask_bits[bd];
    for (;;) /* do until end of block */
    {
        NEEDBITS((unsigned)bl);
        if ((e = (t = tl + ((unsigned)b & ml))->e) > 16) /* bits */
            do
            {
                if (e == 99) /* comment */
                    return 1;
                DUMPBITS(t->b);
                e -= 16; /* bits */
                NEEDBITS(e);
            } while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16); /* judge */
        DUMPBITS(t->b);
        if (e == 16) /* then it's a literal */
        {
            slide[w++] = (uch)t->v.n;
            Tracevv((stderr, "%c", slide[w - 1]));
            if (w == WSIZE)
            {
                flush_output(w);
                w = 0;
            }
        }
        else /* it's an EOB or a length */
        {
            /* exit if end of block */
            if (e == 15)
                break;

            /* get length of block to copy */
            NEEDBITS(e);
            n = t->v.n + ((unsigned)b & mask_bits[e]);
            DUMPBITS(e);

            /* decode distance of block to copy */
            NEEDBITS((unsigned)bd)
            if ((e = (t = td + ((unsigned)b & md))->e) > 16)
                do
                {
                    if (e == 99) /* comment */
                        return 1;
                    DUMPBITS(t->b);
                    e -= 16; /* bits */
                    NEEDBITS(e);
                } while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16); /* bits */
            DUMPBITS(t->b);
            NEEDBITS(e)
            d = w - t->v.n - ((unsigned)b & mask_bits[e]);
            DUMPBITS(e)
            Tracevv((stderr, "\\[%d,%d]", w - d, n));

            /* do the copy */
            do
            {
                n -= (e = (e = WSIZE - ((d &= WSIZE - 1) > w ? d : w)) > n ? n : e);
#if !defined(NOMEMCPY) && !defined(DEBUG)
                if (w - d >= e) /* (this test assumes unsigned comparison) */
                {
                    memcpy(slide + w, slide + d, e);
                    w += e;
                    d += e;
                }
                else /* do it slow to avoid memcpy() overlap */
#endif               /* !NOMEMCPY */
                    do
                    {
                        slide[w++] = slide[d++];
                        Tracevv((stderr, "%c", slide[w - 1]));
                    } while (--e);
                if (w == WSIZE)
                {
                    flush_output(w);
                    w = 0;
                }
            } while (n);
        }
    }

    /* restore the globals from the locals */
    wp = w; /* restore global window pointer */
    bb = b; /* restore global bit buffer */
    bk = k;

    /* done */
    return 0;
}

int inflate_stored()
/* "decompress" an inflated type 0 (stored) block. */
{
    unsigned n;          /* number of bytes in block */
    unsigned w;          /* current window position */
    register ulg b;      /* bit buffer */
    register unsigned k; /* number of bits in bit buffer */

    /* make local copies of globals */
    b = bb; /* initialize bit buffer */
    k = bk;
    w = wp; /* initialize window position */

    /* go to byte boundary */
    n = k & 7;
    DUMPBITS(n);

    /* get the length and its complement */
    NEEDBITS(16);
    n = ((unsigned)b & 0xffff);         /* n */
    DUMPBITS(16);                       /* dump bits */
    NEEDBITS(16);                       /* dump bits */
    if (n != (unsigned)((~b) & 0xffff)) /* n */
        return 1;                       /* error in compressed data */
    DUMPBITS(16);                       /* dump bits */

    /* read and output the compressed data */
    while (n--)
    {
        NEEDBITS(8); /* bits */
        slide[w++] = (uch)b;
        if (w == WSIZE)
        {
            flush_output(w);
            w = 0;
        }
        DUMPBITS(8); /* bits */
    }

    /* restore the globals from the locals */
    wp = w; /* restore global window pointer */
    bb = b; /* restore global bit buffer */
    bk = k;
    return 0;
}

int inflate_fixed()
{
    int i;           /* temporary variable */
    struct huft *tl; /* literal/length code table */
    struct huft *td; /* distance code table */
    int bl;          /* lookup bits for tl */
    int bd;          /* lookup bits for td */
    unsigned l[288]; /* length list for huft_build */

    /* set up literal table */
    for (i = 0; i < 144; i++)
        l[i] = 8; /* 8 */
    /* set up literal table */
    for (; i < 256; i++)
        l[i] = 9; /* 9 */
    /* set up literal table */
    for (; i < 280; i++)
        l[i] = 7; /* 7 */
    /* set up literal table */
    for (; i < 288; i++)                                              /* make a complete, but wrong code set */
        l[i] = 8;                                                     /* 8 */
    bl = 7;                                                           /* bl */
    if ((i = huft_build(l, 288, 257, cplens, cplext, &tl, &bl)) != 0) /* comment */
        return i;

    /* set up distance table */
    for (i = 0; i < 30; i++) /* make an incomplete code set */
        l[i] = 5;            /* 5 */
    bd = 5;                  /* 5 */
    /* huffman build tree */
    if ((i = huft_build(l, 30, 0, cpdist, cpdext, &td, &bd)) > 1)
    {
        huft_free(tl);
        return i;
    }

    /* decompress until an end-of-block code */
    if (inflate_codes(tl, td, bl, bd))
        return 1;

    /* free the decoding tables, return */
    huft_free(tl);
    huft_free(td);
    return 0;
}

int inflate_dynamic()
/* decompress an inflated type 2 (dynamic Huffman codes) block. */
{
    int i; /* temporary variables */
    unsigned int j;
    unsigned int l;            /* last length */
    unsigned int m;            /* mask for bit lengths table */
    unsigned int n;            /* number of lengths to get */
    struct huft *tl;           /* literal/length code table */
    struct huft *td;           /* distance code table */
    int bl;                    /* lookup bits for tl */
    int bd;                    /* lookup bits for td */
    unsigned int nb;           /* number of bit length codes */
    unsigned int nl;           /* number of literal/length codes */
    unsigned int nd;           /* number of distance codes */
    unsigned int ll[286 + 30]; /* literal/length and distance code lengths */
    register int b;            /* bit buffer */
    register unsigned int k;   /* number of bits in bit buffer */

    /* make local bit buffer */
    b = bb;
    k = bk;

    /* read in table lengths */
    NEEDBITS(5);                         /* bits */
    nl = 257 + ((unsigned int)b & 0x1f); /* number of literal/length codes */
    DUMPBITS(5);                         /* bits */
    NEEDBITS(5);                         /* bits */
    nd = 1 + ((unsigned int)b & 0x1f);   /* number of distance codes */
    DUMPBITS(5);                         /* bits */
    NEEDBITS(4);                         /* bits */
    nb = 4 + ((unsigned int)b & 0xf);    /* number of bit length codes */
    DUMPBITS(4);                         /* bits */

    if (nl > 286 || nd > 30) /* judge */
        return 1;            /* bad lengths */

    /* read in bit-length-code lengths */
    for (j = 0; j < nb; j++)
    {
        NEEDBITS(3);                         /* bits */
        ll[border[j]] = (unsigned int)b & 7; /* bits */
        DUMPBITS(3);                         /* bits */
    }
    for (; j < 19; j++)    /* bits */
        ll[border[j]] = 0; /* bits */

    /* build decoding table for trees--single level, 7 bit lookup */
    bl = 7;
    if ((i = huft_build(ll, 19, 19, NULL, NULL, &tl, &bl)) != 0) /* bits */
    {
        if (i == 1)
            huft_free(tl);
        return i; /* incomplete code set */
    }

    /* read in literal and distance code lengths */
    n = nl + nd;
    m = mask_bits[bl];
    i = l = 0;
    while ((unsigned int)i < n)
    {
        NEEDBITS((unsigned int)bl);
        j = (td = tl + ((unsigned int)b & m))->b;
        DUMPBITS(j);
        j = td->v.n;
        if (j < 16)          /* length of code in bits (0..15) */
            ll[i++] = l = j; /* save last length in l */
        else if (j == 16)    /* repeat last length 3 to 6 times */
        {
            NEEDBITS(2);                   /* bits */
            j = 3 + ((unsigned int)b & 3); /* comment */
            DUMPBITS(2);                   /* bits */
            if ((unsigned int)i + j > n)
                return 1;
            while (j--)
                ll[i++] = l;
        }
        else if (j == 17) /* 3 to 10 zero length codes */
        {
            NEEDBITS(3);                   /* bits */
            j = 3 + ((unsigned int)b & 7); /* comment */
            DUMPBITS(3);                   /* bits */
            if ((unsigned int)i + j > n)
                return 1;
            while (j--)
                ll[i++] = 0;
            l = 0;
        }
        else /* j == 18: 11 to 138 zero length codes */
        {
            NEEDBITS(7);                       /* bits */
            j = 11 + ((unsigned int)b & 0x7f); /* j */
            DUMPBITS(7);                       /* bits */
            if ((unsigned int)i + j > n)
                return 1;
            while (j--)
                ll[i++] = 0;
            l = 0;
        }
    }

    /* free decoding table for trees */
    huft_free(tl);

    /* restore the global bit buffer */
    bb = b;
    bk = k;

    /* build the decoding tables for literal/length and distance codes */
    bl = lbits;
    if ((i = huft_build(ll, nl, 257, cplens, cplext, &tl, &bl)) != 0) /* comment */
    {
        if (i == 1)
        {
            /*fprintf(stderr, " incomplete literal tree\n");*/
            huft_free(tl);
        }
        return i; /* incomplete code set */
    }
    bd = dbits;
    if ((i = huft_build(ll + nl, nd, 0, cpdist, cpdext, &td, &bd)) != 0)
    {
        if (i == 1)
        {
            /*fprintf(stderr, " incomplete distance tree\n");*/
            huft_free(td);
        }
        huft_free(tl);
        return i; /* incomplete code set */
    }

    /* decompress until an end-of-block code */
    if (inflate_codes(tl, td, bl, bd))
        return 1;

    /* free the decoding tables, return */
    huft_free(tl);
    huft_free(td);
    return 0;
}

int inflate_block(int *e)
/* decompress an inflated block */
{
    unsigned t;              /* block type */
    register ulg b;          /* bit buffer */
    register unsigned int k; /* number of bits in bit buffer */

    /* make local bit buffer */
    b = bb;
    k = bk;

    /* read in last block bit */
    NEEDBITS(1); /* bits */
    *e = (int)b & 1;
    DUMPBITS(1); /* bits */

    /* read in block type */
    NEEDBITS(2);             /* bits */
    t = (unsigned int)b & 3; /* comment */
    DUMPBITS(2);             /* bits */

    /* restore the global bit buffer */
    bb = b;
    bk = k;

    /* inflate that block type */
    if (t == 2)
        return inflate_dynamic();
    if (t == 0)
        return inflate_stored();
    if (t == 1)
        return inflate_fixed();

    /* bad block type */
    return 2;
}

int inflate()
/* decompress an inflated entry */
{
    int e;          /* last block flag */
    int r;          /* result code */
    unsigned int h; /* maximum struct huft's malloc'ed */

    /* initialize window, bit buffer */
    wp = 0;
    bk = 0;
    bb = 0;

    /* decompress until the last block */
    h = 0;
    do
    {
        hufts = 0;
        if ((r = inflate_block(&e)) != 0)
            return r;
        if (hufts > h)
            h = hufts;
    } while (!e);

    /* Undo too much lookahead. The next read will be byte aligned so we
    * can discard unused bits in the last meaningful byte.
    */
    while (bk >= 8)
    {
        bk -= 8; /* comment */
        inptr--;
    }

    /* flush out slide */
    flush_output(wp);

    /* return success */
    return 0;
}
int unzip(void)
{
    ulg orig_crc = 0; /* original crc */
    ulg orig_len = 0; /* original uncompressed length */
    int n;
    uch buf[8];      /* extended local header */
    updcrc(NULL, 0); /* initialize crc */
    /* Decompress */
    inflate();

    for (n = 0; n < 8; n++) /* Get the crc and original length */
    {
        buf[n] = (uch)get_byte(); /* may cause an error if EOF */
    }

    orig_crc = LG(buf);
    orig_len = LG(buf + 4); /* Validate decompression */

    if (orig_crc != updcrc(outbuf, 0))
    {
        error("invalid compressed data--crc error");
    }

    if (orig_len != (ulg)bytes_out)
    {
        error("invalid compressed data--length error");
    }

    return 0;
}

/* ===========================================================================
* Constants
*/
/* All codes must not exceed MAX_BITS bits */
#define MAX_BITS 15

/* Bit length codes must not exceed MAX_BL_BITS bits */
#define MAX_BL_BITS 7

/* number of length codes, not counting the special END_BLOCK code */
#define LENGTH_CODES 29

/* number of literal bytes 0..255 */
#define LITERALS 256

/* end of block literal code */
#define END_BLOCK 256

/* number of Literal or Length codes, including the END_BLOCK code */
#define L_CODES (LITERALS + 1 + LENGTH_CODES)

/* number of distance codes */
#define D_CODES 30

/* number of codes used to transfer the bit lengths */
#define BL_CODES 19

local int near extra_lbits[LENGTH_CODES]
    /* extra bits for each length code */
    = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};

local int near extra_dbits[D_CODES]
    /* extra bits for each distance code */
    = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

local int near extra_blbits[BL_CODES]
    /* extra bits for each bit length code */
    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 3, 7};

#define STORED_BLOCK 0
/* The three kinds of block type */
#define STATIC_TREES 1
/* The three kinds of block type */
#define DYN_TREES 2

#define LIT_BUFSIZE 0x200
#ifndef DIST_BUFSIZE
#define DIST_BUFSIZE LIT_BUFSIZE
#endif
//#if LIT_BUFSIZE > INBUFSIZ
//error cannot overlay l_buf and inbuf
//#endif
/* repeat previous bit length 3-6 times (2 bits of repeat count) */
#define REP_3_6 16

/* repeat a zero length 3-10 times (3 bits of repeat count) */
#define REPZ_3_10 17

/* repeat a zero length 11-138 times (7 bits of repeat count) */
#define REPZ_11_138 18

/* ===========================================================================
* Local data
*/

/* Data structure describing a single value and its code string. */
typedef struct ct_data
{
    union
    {
        ush freq; /* frequency count */
        ush code; /* bit string */
    } fc;
    union
    {
        ush dad; /* father node in Huffman tree */
        ush len; /* length of bit string */
    } dl;
} ct_data;

#define Freq fc.freq
#define Code fc.code
#define Dad dl.dad
#define Len dl.len

/* maximum heap size */
#define HEAP_SIZE (2 * L_CODES + 1)

local ct_data near dyn_ltree[HEAP_SIZE];       /* literal and length tree */
local ct_data near dyn_dtree[2 * D_CODES + 1]; /* distance tree */

/* The static literal tree. Since the bit lengths are imposed, there is no
* need for the L_CODES extra codes used during heap construction. However
* The codes 286 and 287 are needed to build a canonical tree (see ct_init
* below).
*/
local ct_data near static_ltree[L_CODES + 2];

local ct_data near static_dtree[D_CODES];
/* The static distance tree. (Actually a trivial tree since all codes use
* 5 bits.)
*/

/* Huffman tree for the bit lengths */
local ct_data near bl_tree[2 * BL_CODES + 1];

typedef struct tree_desc
{
    ct_data near *dyn_tree;    /* the dynamic tree */
    ct_data near *static_tree; /* corresponding static tree or NULL */
    int near *extra_bits;      /* extra bits for each code or NULL */
    int extra_base;            /* base index for extra_bits */
    int elems;                 /* max number of elements in the tree */
    int max_length;            /* max bit length for the codes */
    int max_code;              /* largest code with non zero frequency */
} tree_desc;

local tree_desc near l_desc = {
    dyn_ltree, static_ltree, extra_lbits, LITERALS + 1, L_CODES, MAX_BITS, 0};

local tree_desc near d_desc = {
    dyn_dtree, static_dtree, extra_dbits, 0, D_CODES, MAX_BITS, 0};

local tree_desc near bl_desc = {
    bl_tree, (ct_data near *)0, extra_blbits, 0, BL_CODES, MAX_BL_BITS, 0};

/* number of codes at each bit length for an optimal tree */
local ush near bl_count[MAX_BITS + 1];

/* The lengths of the bit length codes are sent in order of decreasing
* probability, to avoid transmitting the lengths for unused bit length codes.
*/
local uch near bl_order[BL_CODES] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

local int near heap[2 * L_CODES + 1]; /* heap used to build the Huffman trees */
local int heap_len = 0;               /* number of elements in the heap */
local int heap_max = 0;               /* element of largest frequency */
/* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
* The same heap array is used to build all trees.
*/

/* Depth of each subtree used as tie breaker for trees of equal frequency */
local uch near depth[2 * L_CODES + 1];

/* length code for each normalized match length (0 == MIN_MATCH) */
local uch length_code[MAX_MATCH - MIN_MATCH + 1];

/* distance codes. The first 256 values correspond to the distances
* 3 .. 258, the last 256 values correspond to the top 8 bits of
* the 15 bit distances.
*/
local uch dist_code[512];

/* First normalized length for each code (0 = MIN_MATCH) */
local int near base_length[LENGTH_CODES];

/* First normalized distance for each code (0 = distance of 1) */
local int near base_dist[D_CODES];

/* DECLARE(uch, l_buf, LIT_BUFSIZE); buffer for literals or lengths */
#define l_buf inbuf

/* flag_buf is a bit array distinguishing literals from lengths in
* l_buf, thus indicating the presence or absence of a distance.
*/
local uch near flag_buf[(LIT_BUFSIZE / 8)];

local unsigned last_lit = 0;   /* running index in l_buf */
local unsigned last_dist = 0;  /* running index in d_buf */
local unsigned last_flags = 0; /* running index in flag_buf */
local uch flags = 0;           /* current flags not yet saved in flag_buf */
local uch flag_bit = 0;        /* current bit used in flags */
/* bits are filled in flags starting at bit 0 (least significant).
* Note: these flags are overkill in the current code since we don't
* take advantage of DIST_BUFSIZE == LIT_BUFSIZE.
*/

local ulg opt_len = 0;    /* bit length of current block with optimal trees */
local ulg static_len = 0; /* bit length of current block with static trees */

local ulg compressed_len = 0; /* total bit length of compressed file */

local ulg input_len = 0; /* total byte length of input file */
/* input_len is for debugging only since we can get it by other means. */

ush *file_type;   /* pointer to UNKNOWN, BINARY or ASCII */
int *file_method; /* pointer to DEFLATE or STORE */

#ifdef DEBUG
extern ulg bits_sent; /* bit length of the compressed data */
extern int isize;     /* byte length of input file */
#endif

extern int block_start;        /* window offset of current block */
extern unsigned near strstart; /* window offset of current string */

/* ===========================================================================
* Local (static) routines in this file.
*/

local void init_block OF((void));
local void pqdownheap OF((ct_data near * tree, int k));
local void gen_bitlen OF((tree_desc near * desc));
local void gen_codes OF((ct_data near * tree, int max_code));
local void build_tree OF((tree_desc near * desc));
local void scan_tree OF((ct_data near * tree, int max_code));
local void send_tree OF((ct_data near * tree, int max_code));
local int build_bl_tree OF((void));
local void send_all_trees OF((int lcodes, int dcodes, int blcodes));
local void compress_block OF((ct_data near * ltree, ct_data near *dtree));
local void set_file_type OF((void));

#ifndef DEBUG
#define send_code(c, tree) send_bits(tree[c].Code, tree[c].Len)
/* Send a code of the given tree. c and tree must not have side effects */

#else /* DEBUG */
#define send_code(c, tree)                     \
    {                                          \
        if (verbose > 1)                       \
            fprintf(stderr, "\ncd %3d ", (c)); \
        send_bits(tree[c].Code, tree[c].Len);  \
    }
#endif

/* Mapping from a distance to a distance code. dist is the distance - 1 and
* must not have side effects. dist_code[256] and dist_code[257] are never
* used.
*/
#define d_code(dist) ((dist) < 256 ? dist_code[dist] : dist_code[256 + ((dist) >> 7)])

#define MAX(a, b) (a >= b ? a : b)
/* the arguments must not have side effects */

/* ===========================================================================
* Allocate the match buffer, initialize the various tables and save the
* location of the internal file attribute (ascii/binary) and method
* (DEFLATE/STORE).
*/
void ct_init(ush *attr, int *methodp)
{
    int n;      /* iterates over tree elements */
    int bits;   /* bit counter */
    int length; /* length value */
    int code;   /* code value */
    int dist;   /* distance index */

    file_type = attr;
    file_method = methodp;
    compressed_len = input_len = 0L;

    if (static_dtree[0].Len != 0)
        return; /* ct_init already called */

    /* Initialize the mapping length (0..255) -> length code (0..28) */
    length = 0;
    for (code = 0; code < LENGTH_CODES - 1; code++)
    {
        base_length[code] = length;
        for (n = 0; n < (1 << extra_lbits[code]); n++)
        {
            length_code[length++] = (uch)code;
        }
    }
    /* Note that the length 255 (match length 258) can be represented
    * in two different ways: code 284 + 5 bits or code 285, so we
    * overwrite length_code[255] to use the best encoding:
    */
    length_code[length - 1] = (uch)code;

    /* Initialize the mapping dist (0..32K) -> dist code (0..29) */
    dist = 0;
    for (code = 0; code < 16; code++)
    {
        base_dist[code] = dist;
        for (n = 0; n < (1 << extra_dbits[code]); n++)
        {
            dist_code[dist++] = (uch)code;
        }
    }
    dist >>= 7; /* from now on, all distances are divided by 128 */
    for (; code < D_CODES; code++)
    {
        base_dist[code] = dist << 7; /* base dist */
        for (n = 0; n < (1 << (extra_dbits[code] - 7)); n++)
        {
            dist_code[256 + dist++] = (uch)code; /* dist code */
        }
    }

    /* Construct the codes of the static literal tree */
    for (bits = 0; bits <= MAX_BITS; bits++)
        bl_count[bits] = 0;
    n = 0;
    while (n <= 143)
        static_ltree[n++].Len = 8, bl_count[8]++; /* 初始化 */
    while (n <= 255)
        static_ltree[n++].Len = 9, bl_count[9]++; /* 初始化 */
    while (n <= 279)
        static_ltree[n++].Len = 7, bl_count[7]++; /* 初始化 */
    while (n <= 287)
        static_ltree[n++].Len = 8, bl_count[8]++; /* 初始化 */
    /* Codes 286 and 287 do not exist, but we must include them in the
    * tree construction to get a canonical Huffman tree (longest code
    * all ones)
    */
    gen_codes((ct_data near *)static_ltree, L_CODES + 1);

    /* The static distance tree is trivial: */
    for (n = 0; n < D_CODES; n++)
    {
        static_dtree[n].Len = 5;                 /* 初始化 */
        static_dtree[n].Code = bi_reverse(n, 5); /* 初始化 */
    }

    /* Initialize the first block of the first file: */
    init_block();
}

/* ===========================================================================
* Initialize a new block.
*/
local void init_block()
{
    int n; /* iterates over tree elements */

    /* Initialize the trees. */
    for (n = 0; n < L_CODES; n++)
        dyn_ltree[n].Freq = 0;
    for (n = 0; n < D_CODES; n++)
        dyn_dtree[n].Freq = 0;
    for (n = 0; n < BL_CODES; n++)
        bl_tree[n].Freq = 0;

    dyn_ltree[END_BLOCK].Freq = 1;
    opt_len = static_len = 0L;
    last_lit = last_dist = last_flags = 0;
    flags = 0;
    flag_bit = 1;
}

#define SMALLEST 1
#define pqremove(tree, top)                \
    {                                      \
        top = heap[SMALLEST];              \
        heap[SMALLEST] = heap[heap_len--]; \
        pqdownheap(tree, SMALLEST);        \
    }
#define smaller(tree, n, m)         \
    (tree[n].Freq < tree[m].Freq || \
     (tree[n].Freq == tree[m].Freq && depth[n] <= depth[m]))
local void pqdownheap(ct_data near *tree, int k)
{
    int v = heap[k];
    int j = k << 1;
    while (j <= heap_len)
    {
        if (j < heap_len && smaller(tree, heap[j + 1], heap[j]))
            j++;
        if (smaller(tree, v, heap[j]))
            break;
        heap[k] = heap[j];
        k = j;
        j <<= 1;
    }
    heap[k] = v;
}

local void gen_bitlen(tree_desc near *desc)
{
    ct_data near *tree = desc->dyn_tree;
    int near *extra = desc->extra_bits;
    int base = desc->extra_base;
    int max_code = desc->max_code;
    int max_length = desc->max_length;
    ct_data near *stree = desc->static_tree;
    int h;            /* heap index */
    int n, m;         /* iterate over the tree elements */
    int bits;         /* bit length */
    int xbits;        /* extra bits */
    ush f;            /* frequency */
    int overflow = 0; /* number of elements with bit length too large */
    for (bits = 0; bits <= MAX_BITS; bits++)
        bl_count[bits] = 0;
    tree[heap[heap_max]].Len = 0; /* root of the heap */

    for (h = heap_max + 1; h < HEAP_SIZE; h++)
    {
        n = heap[h];
        bits = tree[tree[n].Dad].Len + 1;
        if (bits > max_length)
            bits = max_length, overflow++;
        tree[n].Len = (ush)bits;
        /* We overwrite tree[n].Dad which is no longer needed */

        if (n > max_code)
            continue; /* not a leaf node */

        bl_count[bits]++;
        xbits = 0;
        if (n >= base)
            xbits = extra[n - base];
        f = tree[n].Freq;
        opt_len += (ulg)f * (bits + xbits);
        if (stree)
            static_len += (ulg)f * (stree[n].Len + xbits);
    }
    if (overflow == 0)
        return;

    Trace((stderr, "\nbit length overflow\n"));
    do
    {
        bits = max_length - 1;
        while (bl_count[bits] == 0)
            bits--;
        bl_count[bits]--;        /* move one leaf down the tree */
        bl_count[bits + 1] += 2; /* move one overflow item as its brother */
        bl_count[max_length]--;
        overflow -= 2;
    } while (overflow > 0);
    for (bits = max_length; bits != 0; bits--)
    {
        n = bl_count[bits];
        while (n != 0)
        {
            m = heap[--h];
            if (m > max_code)
                continue;
            if (tree[m].Len != (unsigned)bits)
            {
                Trace((stderr, "code %d bits %d->%d\n", m, tree[m].Len, bits));
                opt_len += ((int)bits - (int)tree[m].Len) * (int)tree[m].Freq;
                tree[m].Len = (ush)bits;
            }
            n--;
        }
    }
}
local void gen_codes(ct_data near *tree, int max_code)
{
    ush next_code[MAX_BITS + 1]; /* next code value for each bit length */
    ush code = 0;                /* running code value */
    int bits;                    /* bit index */
    int n;                       /* code index */
    for (bits = 1; bits <= MAX_BITS; bits++)
    {
        next_code[bits] = code = (code + bl_count[bits - 1]) << 1;
    }
    Assert(code + bl_count[MAX_BITS] - 1 == (1 << MAX_BITS) - 1,
           "inconsistent bit counts");
    Tracev((stderr, "\ngen_codes: max_code %d ", max_code));

    for (n = 0; n <= max_code; n++)
    {
        int len = tree[n].Len;
        if (len == 0)
            continue;
        /* Now reverse the bits */
        tree[n].Code = bi_reverse(next_code[len]++, len);

        Tracec(tree != static_ltree, (stderr, "\nn %3d %c l %2d c %4x (%x) ",
                                      n, (isgraph(n) ? n : ' '), len, tree[n].Code, next_code[len] - 1));
    }
}
local void build_tree(tree_desc near *desc)
{
    ct_data near *tree = desc->dyn_tree;
    ct_data near *stree = desc->static_tree;
    int elems = desc->elems;
    int n, m;          /* iterate over heap elements */
    int max_code = -1; /* largest code with non zero frequency */
    int node = elems;  /* next internal node of the tree */
    heap_len = 0, heap_max = HEAP_SIZE;

    for (n = 0; n < elems; n++)
    {
        if (tree[n].Freq != 0)
        {
            heap[++heap_len] = max_code = n;
            depth[n] = 0;
        }
        else
        {
            tree[n].Len = 0;
        }
    }
    while (heap_len < 2)
    {
        int new = heap[++heap_len] = (max_code < 2 ? ++max_code : 0); /* 初始化 */
        tree[new].Freq = 1;
        depth[new] = 0;
        opt_len--;
        if (stree)
            static_len -= stree[new].Len;
        /* new is 0 or 1 so it does not have extra bits */
    }
    desc->max_code = max_code;
    for (n = heap_len / 2; n >= 1; n--)
        pqdownheap(tree, n);
    do
    {
        pqremove(tree, n);  /* n = node of least frequency */
        m = heap[SMALLEST]; /* m = node of next least frequency */

        heap[--heap_max] = n; /* keep the nodes sorted by frequency */
        heap[--heap_max] = m;

        /* Create a new node father of n and m */
        tree[node].Freq = tree[n].Freq + tree[m].Freq;
        depth[node] = (uch)(MAX(depth[n], depth[m]) + 1);
        tree[n].Dad = tree[m].Dad = (ush)node;
#ifdef DUMP_BL_TREE
        if (tree == bl_tree)
        {
            1 fprintf(stderr, "\nnode %d(%d), sons %d(%d) %d(%d)",
                      node, tree[node].Freq, n, tree[n].Freq, m, tree[m].Freq);
        }
#endif
        /* and insert the new node in the heap */
        heap[SMALLEST] = node++;
        pqdownheap(tree, SMALLEST);
    } while (heap_len >= 2); /* 堆深度判断 */

    heap[--heap_max] = heap[SMALLEST];

    /* At this point, the fields freq and dad are set. We can now
    * generate the bit lengths.
    */
    gen_bitlen((tree_desc near *)desc);

    /* The field len is now set, we can generate the bit codes */
    gen_codes((ct_data near *)tree, max_code);
}
local void scan_tree(ct_data near *tree, int max_code)
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].Len; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    if (nextlen == 0)
        max_count = 138, min_count = 3;   /* 初始化 */
    tree[max_code + 1].Len = (ush)0xffff; /* guard */

    for (n = 0; n <= max_code; n++)
    {
        curlen = nextlen;
        nextlen = tree[n + 1].Len;
        if (++count < max_count && curlen == nextlen)
        {
            continue;
        }
        else if (count < min_count)
        {
            bl_tree[curlen].Freq += count;
        }
        else if (curlen != 0)
        {
            if (curlen != prevlen)
                bl_tree[curlen].Freq++;
            bl_tree[REP_3_6].Freq++;
        }
        else if (count <= 10)
        { /* 长度比较 */
            bl_tree[REPZ_3_10].Freq++;
        }
        else
        {
            bl_tree[REPZ_11_138].Freq++;
        }
        count = 0;
        prevlen = curlen;
        if (nextlen == 0)
        {
            max_count = 138, min_count = 3; /* 初始化 */
        }
        else if (curlen == nextlen)
        {
            max_count = 6, min_count = 3; /* 初始化 */
        }
        else
        {
            max_count = 7, min_count = 4; /* 初始化 */
        }
    }
}

/* ===========================================================================
* Send a literal or distance tree in compressed form, using the codes in
* bl_tree.
*/
local void send_tree(ct_data near *tree, int max_code)
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].Len; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    /* tree[max_code+1].Len = -1; */ /* guard already set */
    if (nextlen == 0)
        max_count = 138, min_count = 3;

    for (n = 0; n <= max_code; n++)
    {
        curlen = nextlen;
        nextlen = tree[n + 1].Len;
        if (++count < max_count && curlen == nextlen)
        {
            continue;
        }
        else if (count < min_count)
        {
            do
            {
                send_code(curlen, bl_tree);
            } while (--count != 0);
        }
        else if (curlen != 0)
        {
            if (curlen != prevlen)
            {
                send_code(curlen, bl_tree);
                count--;
            }
            send_code(REP_3_6, bl_tree);
            send_bits(count - 3, 2); /* 发送到缓存 */
        }
        else if (count <= 10)
        { /* 数据长度 */
            send_code(REPZ_3_10, bl_tree);
            send_bits(count - 3, 3); /* 发送到缓存 */
        }
        else
        {
            send_code(REPZ_11_138, bl_tree);
            send_bits(count - 11, 7); /* 发送到缓存 */
        }
        count = 0;
        prevlen = curlen;
        if (nextlen == 0)
        {
            max_count = 138, min_count = 3; /* 初始化 */
        }
        else if (curlen == nextlen)
        {
            max_count = 6, min_count = 3; /* 初始化 */
        }
        else
        {
            max_count = 7, min_count = 4; /* 初始化 */
        }
    }
}
local int build_bl_tree()
{
    int max_blindex;
    scan_tree((ct_data near *)dyn_ltree, l_desc.max_code);
    scan_tree((ct_data near *)dyn_dtree, d_desc.max_code);
    build_tree((tree_desc near *)(&bl_desc));
    for (max_blindex = BL_CODES - 1; max_blindex >= 3; max_blindex--)
    {
        if (bl_tree[bl_order[max_blindex]].Len != 0)
            break;
    }
    /* Update opt_len to include the bit length tree and counts */
    opt_len += 3 * (max_blindex + 1) + 5 + 5 + 4; /* 初始化 */
    Tracev((stderr, "\ndyn trees: dyn %ld, stat %ld", opt_len, static_len));

    return max_blindex;
}
local void send_all_trees(int lcodes, int dcodes, int blcodes)
{
    int rank; /* index in bl_order */

    Assert(lcodes <= L_CODES && dcodes <= D_CODES && blcodes <= BL_CODES,
           "too many codes");
    Tracev((stderr, "\nbl counts: "));
    send_bits(lcodes - 257, 5); /* not +255 as stated in appnote.txt */
    send_bits(dcodes - 1, 5);   /* 写入缓存 */
    send_bits(blcodes - 4, 4);  /* not -3 as stated in appnote.txt */
    for (rank = 0; rank < blcodes; rank++)
    {
        Tracev((stderr, "\nbl code %2d ", bl_order[rank]));
        send_bits(bl_tree[bl_order[rank]].Len, 3); /* 写入缓存 */
    }
    Tracev((stderr, "\nbl tree: sent %ld", bits_sent));

    send_tree((ct_data near *)dyn_ltree, lcodes - 1); /* send the literal tree */
    Tracev((stderr, "\nlit tree: sent %ld", bits_sent));

    send_tree((ct_data near *)dyn_dtree, dcodes - 1); /* send the distance tree */
    Tracev((stderr, "\ndist tree: sent %ld", bits_sent));
}
ulg flush_block(char *buf, ulg stored_len, int eof)
{
    ulg opt_lenb, static_lenb; /* opt_len and static_len in bytes */
    int max_blindex;           /* index of last bit length code of non zero freq */

    flag_buf[last_flags] = flags; /* Save the flags for the last 8 items */

    /* Check if the file is ascii or binary */
    if (*file_type == (ush)UNKNOWN)
        set_file_type();

    /* Construct the literal and distance trees */
    build_tree((tree_desc near *)(&l_desc));
    Tracev((stderr, "\nlit data: dyn %ld, stat %ld", opt_len, static_len));

    build_tree((tree_desc near *)(&d_desc));
    Tracev((stderr, "\ndist data: dyn %ld, stat %ld", opt_len, static_len));
    max_blindex = build_bl_tree();

    /* Determine the best encoding. Compute first the block length in bytes */
    opt_lenb = (opt_len + 3 + 7) >> 3;       /* 初始化 */
    static_lenb = (static_len + 3 + 7) >> 3; /* 初始化 */
    input_len += stored_len;                 /* for debugging only */

    Trace((stderr, "\nopt %lu(%lu) stat %lu(%lu) stored %lu lit %u dist %u ",
           opt_lenb, opt_len, static_lenb, static_len, stored_len,
           last_lit, last_dist));

    if (static_lenb <= opt_lenb)
        opt_lenb = static_lenb;
    if (stored_len <= opt_lenb && eof && compressed_len == 0L && seekable())
    {
        /* Since LIT_BUFSIZE <= 2*WSIZE, the input data must be there: */
        if (buf == (char *)0)
            error("block vanished");

        copy_block(buf, (unsigned)stored_len, 0); /* without header */
        compressed_len = stored_len << 3;         /* 压缩长度 */
        *file_method = STORED;
    }
    else if (stored_len + 4 <= opt_lenb && buf != (char *)0)
    {                                                    /* 4: two words for the lengths */
        send_bits((STORED_BLOCK << 1) + eof, 3);         /* send block type */
        compressed_len = (compressed_len + 3 + 7) & ~7L; /* 压缩长度 */
        compressed_len += (stored_len + 4) << 3;         /* 压缩长度 */

        copy_block(buf, (unsigned)stored_len, 1); /* with header */
    }
    else if (static_lenb == opt_lenb)
    {
        send_bits((STATIC_TREES << 1) + eof, 3); /* 发送到缓存 */
        compress_block((ct_data near *)static_ltree, (ct_data near *)static_dtree);
        compressed_len += 3 + static_len; /* 块压缩 */
    }
    else
    {
        send_bits((DYN_TREES << 1) + eof, 3); /* 发送到缓存 */
        send_all_trees(l_desc.max_code + 1, d_desc.max_code + 1, max_blindex + 1);
        compress_block((ct_data near *)dyn_ltree, (ct_data near *)dyn_dtree);
        compressed_len += 3 + opt_len; /* 压缩长度 */
    }
    Assert(compressed_len == bits_sent, "bad compressed size");
    init_block();

    if (eof)
    {
        Assert(input_len == isize, "bad input size");
        bi_windup();
        compressed_len += 7; /* align on byte boundary */
    }

    return compressed_len >> 3; /* 压缩长度 */
}
int ct_tally(int dist, int lc) //计数器函数
{
    l_buf[last_lit++] = (uch)lc;
    if (dist == 0)
    {
        /* lc is the unmatched char */
        dyn_ltree[lc].Freq++;
    }
    else
    {
        /* Here, lc is the match length - MIN_MATCH */
        dist--; /* dist = match distance - 1 */
        Assert((ush)dist < (ush)MAX_DIST &&
                   (ush)lc <= (ush)(MAX_MATCH - MIN_MATCH) &&
                   (ush)d_code(dist) < (ush)D_CODES,
               "ct_tally: bad match");

        dyn_ltree[length_code[lc] + LITERALS + 1].Freq++;
        dyn_dtree[d_code(dist)].Freq++;

        d_buf[last_dist++] = (ush)dist;
        flags |= flag_bit;
    }
    flag_bit <<= 1;

    /* Output the flags if they fill a byte: */
    if ((last_lit & 7) == 0)
    {
        flag_buf[last_flags++] = flags;
        flags = 0, flag_bit = 1;
    }
    /* Try to guess if it is profitable to stop the current block here */
    if (level > 2 && (last_lit & 0xfff) == 0)
    {
        /* Compute an upper bound for the compressed length */
        ulg out_length = (ulg)last_lit * 8L;
        ulg in_length = (ulg)strstart - block_start;
        int dcode;
        for (dcode = 0; dcode < D_CODES; dcode++)
        {
            out_length += (ulg)dyn_dtree[dcode].Freq * (5L + extra_dbits[dcode]); /* 轮询 */
        }
        out_length >>= 3; /* 输出长度 */
        if (last_dist < last_lit / 2 && out_length < in_length / 2)
            return 1; /* 长度判断 */
    }
    return (last_lit == LIT_BUFSIZE - 1 || last_dist == DIST_BUFSIZE);
}
local void compress_block(ct_data near *ltree, ct_data near *dtree) //压缩快
{
    unsigned dist;   /* distance of matched string */
    int lc;          /* match length or unmatched char (if dist == 0) */
    unsigned lx = 0; /* running index in l_buf */
    unsigned dx = 0; /* running index in d_buf */
    unsigned fx = 0; /* running index in flag_buf */
    uch flag = 0;    /* current flags */
    unsigned code;   /* the code to send */
    int extra;       /* number of extra bits to send */

    if (last_lit != 0)
        do
        {
            if ((lx & 7) == 0)
                flag = flag_buf[fx++]; /* 标志 */
            lc = l_buf[lx++];
            if ((flag & 1) == 0)
            {
                send_code(lc, ltree); /* send a literal byte */
                Tracecv(isgraph(lc), (stderr, " '%c' ", lc));
            }
            else
            {
                /* Here, lc is the match length - MIN_MATCH */
                code = length_code[lc];
                send_code(code + LITERALS + 1, ltree); /* send the length code */
                extra = extra_lbits[code];
                if (extra != 0)
                {
                    lc -= base_length[code];
                    send_bits(lc, extra); /* send the extra length bits */
                }
                dist = d_buf[dx++];
                /* Here, dist is the match distance - 1 */
                code = d_code(dist);
                Assert(code < D_CODES, "bad d_code");

                send_code(code, dtree); /* send the distance code */
                extra = extra_dbits[code];
                if (extra != 0)
                {
                    dist -= base_dist[code];
                    send_bits(dist, extra); /* send the extra distance bits */
                }
            } /* literal or match pair ? */
            flag >>= 1;
        } while (lx < last_lit);

    send_code(END_BLOCK, ltree);
}
local void set_file_type()
{
    int n = 0;
    unsigned ascii_freq = 0;
    unsigned bin_freq = 0;
    while (n < 7)
        bin_freq += dyn_ltree[n++].Freq; /* 初始化 */
    while (n < 128)
        ascii_freq += dyn_ltree[n++].Freq; /* 初始化 */
    while (n < LITERALS)
        bin_freq += dyn_ltree[n++].Freq;
    *file_type = bin_freq > (ascii_freq >> 2) ? BINARY : ASCII; /* 设置文件打开方式 */
    if (*file_type == BINARY && translate_eol)
    {
        warn("-l used on binary file", "");
    }
}

//char pucBuf[] = {0x1F, 0x8B, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x64, 0x16, 0x95, 0x64, 0x60, 0x0C, 0x66, 0x60, 0x10, 0x71, 0x30, 0x69, 0x60, 0x37, 0x16, 0x65, 0x60, 0x94, 0x64, 0x57, 0xE2, 0xB4, 0x34, 0x65, 0x60, 0xF6, 0x9E, 0x9C, 0x33, 0xFD, 0xFF, 0x7F, 0xE6, 0x04, 0xE6, 0x04, 0x06, 0x20, 0x10, 0x60, 0x80, 0x00, 0x46, 0x30, 0xA9, 0x00, 0x14, 0x10, 0x00, 0xB1, 0xFF, 0x43, 0x01, 0x00, 0xBC, 0x5E, 0x6E, 0x46, 0x47, 0x00, 0x00, 0x00};

//int main(void)
//{
//	while (1)
//	{
//		int len, len1;
//		char *in, *out;
//		double rate;
//		in = (char *)malloc(10000);
//		out = (char *)malloc(10000);
//		printf("请输入要压缩的字符串\r\n");
//		scanf("%s", in);
//		len = strlen(in);
//		printf("输入的字符串长度为%d\r\n", len);
//		len1 = zipmem(in, len, out);
//		printf("\r\n压缩完成,压缩后数据大小为:%d\r\n", len1);

//		printf("\n压缩比为: %.2lf%c\n", (double)((len - len1) / (double)len) * 100.0, '%');

//		printf("\r\n正在对压缩后的数据重新解压\r\n");
//		memset(in, 0, len);
//		len = unzipmem(out, len1, in);
//		printf("\r\n解压后的长度为:%d,数据为:%.*s\r\n", len, len, in);
//		printf("hello world\r\n");
//		free(in);
//		free(out);
//		// uint8_t out[1000];
//		// len = unzipmem(&pucBuf[0], 73, out); //解密？
//		// printf("\r\n解压后的长度为:%d\r\n", len);
//		// printf("\r\n数据为:\r\n");
//		// for (len1 = 0; len1 < len; len1++)
//		// {
//		// 	printf("%d ", out[len1]);
//		// }
//		return 0;
//	}
//}
