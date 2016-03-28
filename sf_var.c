/* Written by jic 2013-03 */
#include "sf_include.h"

sf_vartable_t* sf_var_create(char *name, char *value, ngx_pool_t *pool)
{
#if 0
    sf_vartable_t *v = sf_calloc (sizeof(sf_vartable_t));
    v->name = sf_strdup(name);
    v->value = value?sf_strdup(value):strdup("");
#endif    
    sf_vartable_t *v = ngx_pcalloc(pool, sizeof(sf_vartable_t));
    v->name = ngx_pstrdup(pool, name);
    v->value = value?ngx_pstrdup(pool, value):"";
    return v;
}

void sf_var_free(sf_vartable_t *v)
{
    #if 0
    sf_free(v->name);
    sf_free(v->value);
    sf_free(v);
    #endif
}

sf_vartable_t* sf_var_search(sf_vartable_t* handle, char *name)
{
    sf_vartable_t* head = handle;
    while (head)
    {
        if(!strcmp(head->name, name))
        {
            return head;
        }
        head = (sf_vartable_t*)head->tDualLink.next;
        if (head == handle)
        {
            return NULL;
        }
    }
}

void sf_var_delete(sf_vartable_t** phandle, char *name)
{
	sf_vartable_t *v = sf_var_search(*phandle, name);
	if (v)
	{
		DualLinkRemove((void **)phandle, (void *)v);
		sf_var_free(v);
	}
}



char* sf_var_exp(sf_vartable_t* handle, char *varexp, ngx_pool_t *pool)
{
    //char *dststr = (char*)sf_calloc(SF_MAX_VAR_SIZE);
    char *dststr = (char*)ngx_pcalloc(pool, SF_MAX_VAR_SIZE);
    char *c = varexp;
    char *d = dststr;
    while(*c != '\0')
    {
        if (*c=='(' && *(c+1)=='$')
        {
            char *p = c+1;
            while(*p && *p!=')')
            {
                p++;
            }
            if (*p == ')')
            {
                char *name;
                sf_vartable_t *v;

                name = ngx_pstrndup(pool, c+1, p-c-1);
                v = sf_var_search(handle, name);
                ngx_pfree(pool, name);
                if (v)
                {
                    size_t len = strlen(v->value);
		      if (d+len <dststr +SF_MAX_VAR_SIZE)
		      {
                    		strncpy(d, v->value, len);
                    		d = d + len;
                     }
                    		c= p + 1;
                }
                else
                {
		      if ((d + (p -c + 1)) < (dststr +SF_MAX_VAR_SIZE))
		      {
	                    strncpy(d, c, p-c+1);
	                    d = d + (p-c+1);
		      }
                    c = p + 1;
                }
            } else
            {
                strcpy(d, c);
                d = d+(p-c);
                c = p;
            }
        }
        else
            *d++ = *c++;
    }
    return dststr;
#if 0
    /* ÁÙÊ± */
    sf_vartable_t *v =  sf_var_search(handle, varexp);
    if (v)
    {
        return sf_strdup(v->value);
    }
    else
    {
        return sf_strdup("");
    }
#endif
}


sf_vartable_t* sf_var_insert(sf_vartable_t** handle, sf_vartable_t *v)
{
    sf_vartable_t* head = *handle;
    while (head)
    {
        if(!strcmp(head->name, v->name))
        {
            DualLinkRemove((void **)handle, (void *)head);
            sf_var_free(head);
            return (sf_vartable_t*)DualLinkAppend((void **)handle, (void *)v);
        }
        head = (sf_vartable_t*)head->tDualLink.next;
        if (head == *handle)
        {
            break;
        }
    }
    return (sf_vartable_t*)DualLinkAppend((void **)handle, (void *)v);
}


