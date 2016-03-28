#include "sf_include.h"


#undef  _Debug_M        /* 是否调试hash */
#define _Debug_M	1

#define offset_t(t, m)			((void*)(&((t*)0)->m))
#define BodyHeadGet(addr, t, m)	((void*)(addr) - offset_t(t, m))

#define SemCreate()
#define SemGive()
#define SemTake()  1

typedef struct
{
    HANDLE         iHashRoot;
#if _Debug_M
    int			iCount;
#endif
} t_hash_que;

typedef struct
{
    WORD32		iMaskBitsNum;
    t_hash_que taHashItem[0];
} t_hash_handle;

#define BITS_TO_NUM(bits)   ((0xffffffffu >> (32 - (bits))) + 1)
#if 0



/***********************************************************************
* 函数名称： HashRootFirst()
* 功能描述： 获得Hash表的第一个入口地址
* 访问的表：
* 修改的表：
* 输入参数:     hHashHandle：Hash表的句柄
* 输出参数：
* 返 回 值：    Hash表的第一个入口地址
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void *HashRootFirst(int hHashHandle)
{
    t_hash_handle   *h  = (t_hash_handle *)hHashHandle;

    if (! h) return 0;

    return &h->taHashItem[0].iHashRoot;
}

/***********************************************************************
* 函数名称： HashRootNext()
* 功能描述： 获得Hash表的下一个入口地址
* 访问的表：
* 修改的表：
* 输入参数:     hHashHandle：Hash表的句柄
                pCurrentRoot：当前Hash表的入口地址
* 输出参数：
* 返 回 值：    Hash表的下一个入口地址
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void *HashRootNext(int hHashHandle, void *pCurrentRoot)
{
    t_hash_handle   *h  = (t_hash_handle *)hHashHandle;
    WORD32 num;
    WORD32 start, end;

    if (! h || ! pCurrentRoot) return 0;

    num = h->iMaskBitsNum;
    num = BITS_TO_NUM(num);

    start = (WORD32)&h->taHashItem[0].iHashRoot;
    end   = start + num * sizeof(t_hash_que);

    if (start > (WORD32)pCurrentRoot || end < (WORD32)pCurrentRoot) return 0;   /* 地址范围不对 */
    if ((((WORD32)pCurrentRoot - start) % sizeof(t_hash_que)) != 0) return 0;   /* 不是Root */
    if (end - (WORD32)pCurrentRoot == sizeof(t_hash_que)) return 0;             /* 已经是最后一个 */

    return (void*)((WORD32)pCurrentRoot + sizeof(t_hash_que));
}

/***********************************************************************
* 函数名称： HashModuleReset()
* 功能描述： 对一个已初始化的Hash索引表进行复位，删除所有入口信息。
* 访问的表：
* 修改的表：
* 输入参数:     hHashHandle：Hash表的句柄
* 输出参数：
* 返 回 值：    0成功，-1不成功
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
int HashModuleReset(int hHashHandle)
{
    t_hash_handle   *h  = (t_hash_handle *)hHashHandle;

    WORD32 num;

    if (! h) return -1;

    num = h->iMaskBitsNum;
    num = BITS_TO_NUM(num);

    if (! SemTake()) return -1;

    memset (&h->taHashItem[0], 0, num * sizeof(t_hash_que));

    SemGive();

    return 0;
}
#endif



uint32_t  ngx_crc32_table256[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


 unsigned long long BKDRHash(const char *str)
{
     unsigned int seed = 13131313; // 31 131 1313 13131 131313 etc..
    unsigned long long hash = 0;
  
     while (*str)
     {
         hash = hash * seed + (*str++);
     }
     return (hash & 0x7FFFFFFFFFFFFFFFL);
 }

/***********************************************************************
* 函数名称： HashModuleInit()
* 功能描述： 对Hash表队列进行初始化
* 访问的表：
* 修改的表：
* 输入参数:     iMaskBitsNum：掩码位数，用于计算Hash表的长度
* 输出参数：
* 返 回 值：    0不成功，非0成功，返回Hash表队列句柄
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
HANDLE HashModuleInit(ngx_pool_t *pool, WORD32 iMaskBitsNum)
{
    t_hash_handle   *h;
    t_hash_que  *ptHashItem;
    size_t size;
    WORD32 iHashNum, idx;

    if (iMaskBitsNum > 24 || iMaskBitsNum < 4)
    {
        return 0;
    }

    SemCreate();

    iHashNum    = BITS_TO_NUM(iMaskBitsNum);
    size        = sizeof (t_hash_handle) + sizeof (t_hash_que) * iHashNum;
    sf_log(LOGMOD_PROC, LOG_DEBUG, "alloc table size=%d\n", size);

    //h = (t_hash_handle *)malloc (size);
    h = (t_hash_handle *)ngx_palloc (pool, size);
    if (! h)
    {
        return 0;
    }

    h->iMaskBitsNum = iMaskBitsNum;
    ptHashItem = &h->taHashItem[0];
    for (idx = 0; idx < iHashNum; idx ++)
    {
        ptHashItem->iHashRoot = 0;

#if _Debug_M
        ptHashItem->iCount    = 0;
#endif
        ptHashItem ++;
    }

    return (HANDLE)h;
}




/***********************************************************************
* 函数名称： Search()
* 功能描述： 根据给定的关键字在双向链表中查找匹配的Hash映射节点
* 访问的表：
* 修改的表：
* 输入参数:     que：Hash表队列的一个节点，存储着双向链表的首地址
                CmpFun：用于比较的函数指针
                UserData：用于查找的关键字信息
                iDataLen：关键字的长度
* 输出参数：
* 返 回 值：    0不成功，非0成功，返回查找到地的数据存储区的地址
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
static void* Search(t_hash_que    *que,
                    hashCmpFun     CmpFun,
                    void           *UserData,
                    int            iDataLen
                   )
{
    T_HashData *hashHead;
    T_HashData *hashTemp;

    if (que->iHashRoot == 0) return 0;

    if (! SemTake()) return 0;

    hashHead = (T_HashData *)BodyHeadGet(que->iHashRoot, T_HashData, tDualLink);
    hashTemp = hashHead;

    do
    {
        if (hashTemp->iDataLen == iDataLen && CmpFun (hashTemp, UserData))
        {
            SemGive();
            return hashTemp;
        }
        else
        {
            hashTemp = (T_HashData *)BodyHeadGet(hashTemp->tDualLink.next, T_HashData, tDualLink);
        }
    } while (hashTemp != hashHead);

    SemGive();

    return 0;
}


/***********************************************************************
* 函数名称： KeyToItem()
* 功能描述： 根据给定的key转化成Hash表队列的一个节点
* 访问的表：
* 修改的表：
* 输入参数:     hHashHandle：Hash表的句柄
                key：根据关键字转化而来的32位无符号值
* 输出参数：
* 返 回 值：    转化后的Hash表队列中的一个节点的地址
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
static t_hash_que* KeyToItem(HANDLE hHashHandle, WORD32 key)
{
    t_hash_que  *que;
    t_hash_handle   *h  = (t_hash_handle *)hHashHandle;

    WORD32 Shift, Mask, key1;

    key *= 7591;

    Shift 	= h->iMaskBitsNum;
    Mask  	= 0xffffffffu << (32 - Shift);
    key1    = key << (32 - Shift);

    key = (key >> Shift) << Shift;

    while(key)
    {
        key1 ^= Mask & key;
        key <<= Shift;
    }

    que  = &h->taHashItem[key1 >> (32 - Shift)];

    return que;
}

/***********************************************************************
* 函数名称： HashModuleInsert()
* 功能描述： 将节点插入到Hash表及其管理的双向链表中
* 访问的表：
* 修改的表：
* 输入参数:     hHashHandle：Hash表的句柄
                key：根据关键字转化而来的32位无符号值
                HashData：要插入的节点的地址
* 输出参数：
* 返 回 值：    转化后的Hash表队列中的一个节点的地址
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void HashModuleInsert(HANDLE hHashHandle, WORD32 key, T_HashData *HashData)
{
    t_hash_que  *que = KeyToItem(hHashHandle, key);

    if (! SemTake()) return;

    DualLinkAppend((void **)&que->iHashRoot, &HashData->tDualLink);
#if _Debug_M
    que->iCount ++;
#endif

    SemGive();
}

/***********************************************************************
* 函数名称： HashModuleSearch()
* 功能描述： 将节点从Hash表及其管理的双向链表中快速移去
* 访问的表：
* 修改的表：
* 输入参数:     hHashHandle：Hash表的句柄
                key：根据关键字转化而来的32位无符号值
                HashData：要插入的节点的地址
                CmpFun：用于比较的函数指针
                UserData：关键字信息
                iDataLen：关键字的长度
* 输出参数：
* 返 回 值：    要查找的映射节点的地址
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void* HashModuleSearch( HANDLE hHashHandle,
                        WORD32 key,
                        hashCmpFun CmpFun,
                        void *UserData,
                        int iDataLen
                      )
{
    void   *HashData;
    t_hash_que  *que = KeyToItem(hHashHandle, key);

    HashData = Search(que, CmpFun, UserData, iDataLen);

    return HashData;
}
#if 0

void HashModuleRemoveFast(int hHashHandle, WORD32 key, T_GsnHashData *HashData)
{
    t_hash_que  *que = KeyToItem(hHashHandle, key);

    if (!SemTake()) return;

    DualLinkRemove((void **)&que->iHashRoot, &HashData->tDualLink);
#if _Debug_M
    que->iCount --;
#endif

    SemGive();
}
#endif
/***********************************************************************
* 函数名称： HashModuleRemove()
* 功能描述： 将节点从Hash表及其管理的双向链表中移去
* 访问的表：
* 修改的表：
* 输入参数:     hHashHandle：Hash表的句柄
                key：根据关键字转化而来的32位无符号值
                CmpFun：用于比较的函数指针
                UserData：关键字信息
                iDataLen：关键字的长度
* 输出参数：
* 返 回 值：    0不成功，非零不成功，返回移去节点的地址
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
HANDLE HashModuleRemove(HANDLE hHashHandle,
                        WORD32 key,
                        hashCmpFun CmpFun,
                        void *UserData,
                        int iDataLen
                       )
{
    T_HashData   *ptHashDataTemp;
    t_hash_que  *que = KeyToItem(hHashHandle, key);
    HANDLE handle;

    if (! SemTake()) return 0;

    if (que->iHashRoot && CmpFun &&
            (ptHashDataTemp = Search(que, CmpFun, UserData, iDataLen)))
    {
        handle = DualLinkRemove((void **)&que->iHashRoot, &ptHashDataTemp->tDualLink);
#if _Debug_M
        que->iCount --;
#endif
        handle = (HANDLE)BodyHeadGet(handle, T_HashData, tDualLink);

        SemGive();

        return handle;
    }

    SemGive();

    return 0;
}


/***********************************************************************
* 函数名称： HashModuleDump()
* 功能描述： 打印Hash相关信息
* 访问的表：
* 修改的表：
* 输入参数:     hHashHandle：Hash表的句柄
* 输出参数：
* 返 回 值：    无
* 其它说明：
* 修改日期    版本号     修改人      修改内容
*************************************************************************/
void HashModuleDump(HANDLE hHashHandle)
{
#if _Debug_M
    int iCount[11]		= {0};
    int iMin			= 999999;
    int iMax			= 0;
    t_hash_handle *h	= (t_hash_handle *)hHashHandle;
    int num				= (0xffffffffu >> (32 - h->iMaskBitsNum)) + 1;
    int idx, count, all;
    t_hash_que *que = &h->taHashItem[0];

    all = 0;
    for (idx = 0; idx < num; idx ++)
    {
        count = que->iCount;
        all  += count;

        if (count < iMin)
        {
            iMin = count;
        }

        if (count > iMax)
        {
            iMax = count;
        }

        if (count >= 10)
        {
            iCount[10] ++;
        }
        else
        {
            iCount[count] ++;
        }

        que ++;
    }

    sf_log(LOGMOD_PROC, LOG_DEBUG, "\n ----------------------------------");
    sf_log(LOGMOD_PROC, LOG_DEBUG, "\n Dump hash %#x", hHashHandle);
    sf_log(LOGMOD_PROC, LOG_DEBUG, "\n all item    %d", all);
    sf_log(LOGMOD_PROC, LOG_DEBUG, "\n num min     %d", iMin);
    sf_log(LOGMOD_PROC, LOG_DEBUG, "\n num max     %d", iMax);
    sf_log(LOGMOD_PROC, LOG_DEBUG, "\n num >= 10   %d", iCount[10]);

    for (idx = 0; idx < 10; idx ++)
    {
        if (iCount[idx])
        {
            sf_log(LOGMOD_PROC, LOG_DEBUG, "\n num = %d     %d", idx, iCount[idx]);
        }
    }
    sf_log(LOGMOD_PROC, LOG_DEBUG, "\n ----------------------------------");

#else
    sf_log(LOGMOD_PROC, LOG_DEBUG, "\n This is Release Version, unsupport dump command");
#endif

}

