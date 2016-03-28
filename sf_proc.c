/* Written by jic 2013-03 */
#include "sf_include.h"

int sf_temp_cnt;
pthread_mutex_t url_mutex = PTHREAD_MUTEX_INITIALIZER;

char *sf_proc_temp_filename()
{
    char *fn = calloc(30, 1 );
    if (fn)
        sprintf(fn, "tempurl.%d", sf_temp_cnt++);
    return fn;
}

int compare_string(const T_HashData* hashData, const void *UserData)
{
    unsigned long long key;
    sf_hash_url_t *hd = (sf_hash_url_t*)hashData;

   key = BKDRHash((const char*)UserData);

    return (key == hd->key);
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

int sf_proc_url_match_range(char *url, sf_conf_range_t* range)
{
    int rc;

    if (!range)
        return 1;

    while (range)
    {
        rc = pcre_exec(range->url_white_re, NULL, url, strlen(url), 0, 0, 0, 0);
        if (rc >= 0)
        {
            sf_log(LOGMOD_PROC, LOG_DEBUG, "sf_proc_url_match_range match in url range for [%s]!\n", url);
            return 1;
        }
        range =range->next;
    }
    return 0;
}

char* sf_rewrite_exp(char *varexp, pcre *re, char *buf, int *vector, int rc, ngx_pool_t *pool)
{
    //char *dststr = (char*)sf_calloc(1024);
    char *dststr = (char*)ngx_pcalloc(pool, 1024);
    char *c = varexp;
    char *d = dststr;
    int rc2;
    char *outstr = NULL;

    if (!varexp)
    {
        strcpy(dststr, buf);
        return dststr;
    }

    while(*c != '\0')
    {
        if (*c=='(' && *(c+1)=='#')
        {
            char *p = c+1;
            while(*p && *p!=')')
            {
                p++;
            }
            if (*p == ')')
            {
                char *name;

                name = ngx_pstrndup(pool, c+1, p-c-1);
                rc2 = pcre_get_named_substring(re, buf, vector, rc, name+1, (const char **)&outstr);
                ngx_pfree(pool, name);

                if (rc2 < 0)
                {
                    strncpy(d, c, p-c+1);
                    d = d + (p-c+1);
                    c = p + 1;
                }
                else
                {
                    size_t len = strlen(outstr);
                    strncpy(d, outstr, len);
                    d = d + len;
                    c= p + 1;
                }
                pcre_free_substring(outstr);

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
}

char* sf_proc_url_rewrite_range(char *url, sf_conf_range_t* range, ngx_pool_t *pool)
{
    int rc;
    int vector[SF_MAX_URL_VECTOR];
    sf_conf_range_t* first_range = range;
    
    if (!range)
        return url;

    while (range)
    {
        if (range->url_black_re)
        {
            rc = pcre_exec(range->url_black_re, NULL, url, strlen(url), 0, 0, vector, SF_MAX_URL_VECTOR);
            if (rc >= 0)
            {
                sf_log(LOGMOD_PROC, LOG_DEBUG, "sf_proc_url_match_range black match in url range for [%s]!\n", url);
                return NULL;
            }
        }
        range =range->next;
    }

    range = first_range;
    while (range)
    {
        if (range->url_white_re)
        {
            rc = pcre_exec(range->url_white_re, NULL, url, strlen(url), 0, 0, vector, SF_MAX_URL_VECTOR);
            if (rc >= 0)
            {
                sf_log(LOGMOD_PROC, LOG_DEBUG, "sf_proc_url_match_range match in url range for [%s]!\n", url);

                return sf_rewrite_exp(range->url_rewrite, range->url_white_re, url, vector, rc, pool);
            }
        }
        range =range->next;
    }
    return NULL;
}

int sf_proc_insert_url_withrange(sf_spider_t*s, char *url, int level)
{
    char *rewrite = sf_proc_url_rewrite_range(url, s->conf.range, s->pool);
    if (!rewrite)
        return 0;
    else
    {
        sf_proc_insert_url(rewrite, level, s->url_fdw, s->url_handle);
        if (rewrite != url)
        {
            //free(rewrite);
        }
        return 0;
    }
}



int sf_proc_insert_url(char *url, int level, FILE* outfile, HANDLE hUrl)
{
    WORD32 key;
    HANDLE hItem;
    size_t urllen;

    urllen = strlen(url);
    //strlow(url, url, urllen);
    key = sf_crc(url, urllen);

    pthread_mutex_lock(&url_mutex);
    hItem = HashModuleSearch(hUrl, key, compare_string, url, sizeof(sf_hash_url_t) -  sizeof(T_HashData));
    if (hItem == NULL)
    {
        pthread_mutex_lock(&global_pool_mutex);
        sf_hash_url_t  *pData = (sf_hash_url_t*)AllocHashData(global_pool, sizeof(sf_hash_url_t));
        pthread_mutex_unlock(&global_pool_mutex);
        if(!pData)
        {
            pthread_mutex_unlock(&url_mutex);
            sf_log(LOGMOD_PROC, LOG_ERR, "error for malloc hashdata\n");
            return -1;
        }
        pData->key = BKDRHash(url);
        /* 如果没有查找到，则添加HASH表，并输出到文件 */
        HashModuleInsert(hUrl, key, (T_HashData *)pData);
        pthread_mutex_unlock(&url_mutex);
        pthread_mutex_lock(&file_mutex);
        fprintf(outfile, SF_URL_OUT_PATTERN, level, url);
        fflush(outfile);
        pthread_mutex_unlock(&file_mutex);
    }
    else
    {
        pthread_mutex_unlock(&url_mutex);
    }
}


HANDLE* sf_proc_init_url_entry(HANDLE hdl, char *urlfile)
{
    HANDLE* h;
    sf_hash_url_t  *pData;
    FILE *fp;
    char url[2048];
    int level;

    if (hdl)
    {
        h = hdl;
    } else
    {
        pthread_mutex_lock(&global_pool_mutex);
        h = HashModuleInit(global_pool, 16);
        pthread_mutex_unlock(&global_pool_mutex);
    }

    if (0 == h)
    {
        sf_log(LOGMOD_PROC, LOG_ERR, "init hash for domain error\n");
        return NULL;
    }

    fp = fopen(urlfile, "a+");
    if(NULL == fp)
    {
        sf_log(LOGMOD_PROC, LOG_ERR, "open file %s error!\n", urlfile);
        return NULL;
    }

    /* 循环取DOMAIN，建立HASH表 */
    while(EOF!= fscanf(fp,SF_URL_OUT_PATTERN, &level, url))
    {
        WORD32 key;
        size_t  urllen = strlen(url);

        sf_log(LOGMOD_PROC, LOG_DEBUG, "load from file %s: l=%d url=%s\n", urlfile, level,  url);
        //pData = (sf_hash_url_t*)AllocHashData(sizeof(sf_hash_url_t));
        //pData->key= BKDRHash(url);
        key = sf_crc(url,urllen);

        pthread_mutex_lock(&url_mutex);
        if (HashModuleSearch(h, key, compare_string, url, sizeof(sf_hash_url_t) -  sizeof(T_HashData) ))
        {
            //sf_log(LOGMOD_PROC, LOG_DEBUG, "    Match key %d   ignore insert\n", key);
            pthread_mutex_unlock(&url_mutex);
            continue;
        };
        pthread_mutex_lock(&global_pool_mutex);
        pData = (sf_hash_url_t*)AllocHashData(global_pool, sizeof(sf_hash_url_t));
        pthread_mutex_unlock(&global_pool_mutex);
        pData->key= BKDRHash(url);

        HashModuleInsert(h, key, (T_HashData*)pData);
        pthread_mutex_unlock(&url_mutex);

    }

    HashModuleDump(h);

    fclose(fp);

    return h;

}

void sf_proc_init_varhandle(sf_spider_t *s)
{
    static int isdone = 0;
    if (isdone++)
        return;

    sf_vartable_t *v;
    s->var_handle = NULL;
    v = sf_var_create("", "", s->lifetime_pool);
    sf_var_insert(&s->var_handle, v);
}

sf_spider_t* sf_spider_init(char *conf_fn)
{
    sf_spider_t *s;
    ngx_pool_t  *pool = ngx_create_pool(4096, NULL);

    s = ngx_pcalloc(pool, sizeof(sf_spider_t));
    if (!s)
        return NULL;
    s->lifetime_pool = pool;
    s->conf_fn = conf_fn;
    sf_proc_init_varhandle(s);

    if (0> sf_conf_init(s, s->conf_fn))
        return NULL;



    return s;
}
