#include "sf_include.h"

static HANDLE  hConfset = NULL;  /* 配置文件的集合排重句柄 */
pthread_mutex_t hConfset_mutex = PTHREAD_MUTEX_INITIALIZER;

int compare_conf_fn(const T_HashData* hashData, const void *UserData)
{
    sf_hash_conf_t *hd = (sf_hash_conf_t*)hashData;

    return !strcmp(hd->conf_fn, UserData);
}

HANDLE sf_confset_init()
{
    hConfset = HashModuleInit(global_pool, 8);
    return hConfset;
}

HANDLE sf_confset_search(char *conf_fn)
{
    WORD32 key;
    HANDLE hItem;

    key = sf_crc(conf_fn, strlen(conf_fn));
    pthread_mutex_lock(&hConfset_mutex);
    hItem = HashModuleSearch(hConfset, key, compare_conf_fn, conf_fn, sizeof(sf_hash_conf_t) -  sizeof(T_HashData));
    pthread_mutex_unlock(&hConfset_mutex);
    return hItem;
}

HANDLE sf_confset_insert(sf_hash_conf_t *pData)
{
    WORD32 key;
    HANDLE hItem;

    key = sf_crc(pData->conf_fn, strlen(pData->conf_fn));
    pthread_mutex_lock(&hConfset_mutex);
    hItem = HashModuleSearch(hConfset, key, compare_conf_fn, pData->conf_fn, sizeof(sf_hash_conf_t) -  sizeof(T_HashData));
    if (!hItem)
    {
        HashModuleInsert(hConfset, key, (T_HashData *)pData);
    }
    pthread_mutex_unlock(&hConfset_mutex);
}
