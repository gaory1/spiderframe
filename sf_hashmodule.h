#ifndef _HASH_MODULE_
#define _HASH_MODULE_


typedef struct {
    t_dual_link tDualLink;
    int iDataLen;
    int pad;
} T_HashData;
typedef int (*hashCmpFun)(const T_HashData*, const void *UserData);

extern HANDLE HashModuleInit(ngx_pool_t *pool, WORD32 iMaskBitsNum);
extern void HashModuleInsert(HANDLE hHashHandle, WORD32 key, T_HashData *HashData);
extern void HashModuleDump(HANDLE hHashHandle);
void* HashModuleSearch( HANDLE hHashHandle,
                        WORD32 key,
                        hashCmpFun CmpFun,
                        void *UserData,
                        int iDataLen
                      );
extern void *sf_malloc(size_t size);
extern void *sf_calloc(size_t size);
extern unsigned long long BKDRHash(const char *str);

#if 0
static inline WORD32
sf_crc(char *data, size_t len)
{
    WORD32  sum;

    for (sum = 0; len; len--) {

        /*
         * gcc 2.95.2 x86 and icc 7.1.006 compile
         * that operator into the single "rol" opcode,
         * msvc 6.0sp2 compiles it into four opcodes.
         */
        sum = sum >> 1 | sum << 31;

        sum += *data++;
    }

    return sum;
}
#endif

static inline uint32_t
sf_crc(char *p, size_t len)
{
    uint32_t  crc;
    extern uint32_t   ngx_crc32_table256[];
    
    crc = 0xffffffff;

    while (len--) {
        crc = ngx_crc32_table256[(crc ^ *p++) & 0xff] ^ (crc >> 8);
    }

    return crc ^ 0xffffffff;
}


static void * AllocHashData(ngx_pool_t *pool, size_t size)
{
    //T_HashData* pData = (T_HashData*)sf_calloc(size);

    T_HashData* pData = (T_HashData*)ngx_pcalloc(pool, size);

    pData->iDataLen = size - sizeof(T_HashData);

    return (void*)pData;
}

#endif

