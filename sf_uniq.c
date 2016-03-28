/* Written by jic 2013-03 */
/* 本文件用于变量或其它的内容进行排重 */

#include "sf_include.h"

static int compare_string(const T_HashData* hashData, const void *UserData)
{
    sf_uniq_t *hd = (sf_uniq_t*)hashData;

    return !strcmp(hd->string, UserData);
}

#define tolower(c)      (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)

static void strlow(char *dst, char *src, size_t n)
{
    while (n)
    {
        *dst = tolower(*src);
        dst++;
        src++;
        n--;
    }
}


sf_uniq_t* sf_isdup(void *handle, char *name)
{
    WORD32 key;
    sf_uniq_t* hItem;

    key = sf_crc(name, strlen(name));

    hItem = (sf_uniq_t*)HashModuleSearch(handle, key, compare_string, name, sizeof(sf_uniq_t) -  sizeof(T_HashData));

    return hItem;
}

void sf_uniq_insert(HANDLE handle, sf_uniq_t *hItem)
{
    WORD32 key;

    key = sf_crc(hItem->string, strlen(hItem->string));

    HashModuleInsert(handle, key, (T_HashData *)hItem);
}

HANDLE sf_uniq_init(ngx_pool_t *pool)
{
    return HashModuleInit(pool, 16);
}

sf_uniq_t* sf_uniq_create(ngx_pool_t *pool, size_t size)
{
    return (sf_uniq_t*)AllocHashData(pool, size);
}
