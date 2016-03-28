#ifndef _SF_CONFSET_H_
#define _SF_CONFSET_H_

typedef struct sf_hash_conf_s
{
    T_HashData hashdata;
    char *conf_fn;
    void *s;   /* spider 的指针 */
    int line;  /* checkpoint检查后行数 */
} sf_hash_conf_t;

HANDLE sf_confset_init();
HANDLE sf_confset_search(char *conf_fn);
HANDLE sf_confset_insert(sf_hash_conf_t *pData);


#endif /* #ifndef _SF_CONFSET_H_ */
