#ifndef _SF_UNIQ_H_
#define _SF_UNIQ_H_

typedef struct sf_uniq_s {
    T_HashData hashdata;
    char *string;
} sf_uniq_t;

HANDLE sf_uniq_init(ngx_pool_t *pool);
sf_uniq_t* sf_isdup(void *handle, char *name);
void sf_uniq_insert(HANDLE handle, sf_uniq_t *hItem);
sf_uniq_t* sf_uniq_create(ngx_pool_t *pool, size_t size);


#endif

