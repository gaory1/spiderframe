#ifndef _SF_VAR_H_
#define _SF_VAR_H_

typedef enum {
    VAR_INT,
    VAR_STRING,
    VAR_ARRAY
} sf_var_e;

typedef struct sf_vartable_s {
    t_dual_link tDualLink;
    char *name;
    char *value;
    sf_var_e type;
} sf_vartable_t;

sf_vartable_t* sf_var_create(char *name, char *value, ngx_pool_t *pool);
sf_vartable_t* sf_var_insert(sf_vartable_t** handle, sf_vartable_t *v);
sf_vartable_t* sf_var_search(sf_vartable_t* handle, char *name);
void sf_var_delete(sf_vartable_t** phandle, char *name);
char* sf_var_exp(sf_vartable_t* handle, char *varexp, ngx_pool_t *pool);



#endif
