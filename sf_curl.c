/*  此文件就是用做下载使用*/
#include "sf_include.h"

#define FAIL_FILE_NAME  "fail_list"
FILE *fail_fp = NULL;
char *proxy = NULL;

static int writer_hdr(char *data, size_t size, size_t nmemb, void *pthrdBufPtr)
{
    unsigned long sizes = size * nmemb;
    size_t *length = (size_t *)*((char **)pthrdBufPtr);

    if (pthrdBufPtr == NULL)
        return 0;

    if (*length + sizes > SF_MAX_HDR_SIZE-sizeof(int)-1)
    {
        return 0;
    }

    //sf_log(LOGMOD_PROC, LOG_DEBUG, "write size %d\n", sizes);

    char *thrdBufPtr = BUF_OFFSET(*((char **)pthrdBufPtr));
    memcpy(thrdBufPtr+*length, data, sizes);
    // thrdBufPtr+=sizes;
    *length+=sizes;
    *(thrdBufPtr+*length+1) = 0;

    // *((char **)pthrdBufPtr) = thrdBufPtr;

    return sizes;
}

static int writer_html(char *data, size_t size, size_t nmemb, void *pthrdBufPtr)
{
    unsigned long sizes = size * nmemb;
    size_t *length = (size_t *)*((char **)pthrdBufPtr);

    if (pthrdBufPtr == NULL)
        return 0;

    if (*length + sizes > SF_MAX_HTML_SIZE-sizeof(int)-1)
    {
        return 0;
    }

    //sf_log(LOGMOD_PROC, LOG_DEBUG, "write size %d\n", sizes);

    char *thrdBufPtr = BUF_OFFSET(*((char **)pthrdBufPtr));
    memcpy(thrdBufPtr+*length, data, sizes);
    // thrdBufPtr+=sizes;
    *length+=sizes;
    *(thrdBufPtr+*length+1) = 0;

    // *((char **)pthrdBufPtr) = thrdBufPtr;

    return sizes;
}

char *sf_curl_url(sf_spider_t *s, char *url, char*pBufPtr, char *pHdrPtr)
{
    char errorBuffer[CURL_ERROR_SIZE];
    CURL *conn = NULL;
    CURLcode code;
    char *tmpstr;
    struct curl_slist *slist=NULL;
    sf_conf_t *conf = &s->conf;
    int retry = 3;

    #if 0
    if (!fail_fp)
    {
        fail_fp = fopen(FAIL_FILE_NAME, "a+");
    }
    #endif

    if (!conn)
        conn = curl_easy_init();

    if (conn == NULL)
    {
        sf_log(LOGMOD_PROC, LOG_DEBUG, "Failed to create CURL connection\n");
        return NULL;
    }

    if (!strcasecmp(s->conf.method, "POST"))
    {
        char *field;
        code = curl_easy_setopt(conn, CURLOPT_POST, 1);
        if (code != CURLE_OK)
        {
            fprintf(stderr, "Failed to set POST METHOD [%d]\n", code);
            curl_easy_cleanup(conn);
            return NULL;
        }
        field = sf_var_exp(s->var_handle, conf->post_data, s->pool);
        curl_easy_setopt(conn, CURLOPT_POSTFIELDS, field);
    }

    tmpstr = sf_var_exp(s->var_handle, s->conf.user_agent, s->pool);
    if (tmpstr)
    {
        code = curl_easy_setopt(conn, CURLOPT_USERAGENT, tmpstr);
        //sf_free(tmpstr);
        if (code != CURLE_OK)
        {
            fprintf(stderr, "Failed to set User-Agent [%d]\n", code);
            curl_easy_cleanup(conn);
            return NULL;
        }
    }

    code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set error buffer [%d]\n", code);
        curl_easy_cleanup(conn);
        return NULL;
    }
    code = curl_easy_setopt(conn, CURLOPT_URL, url);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_NOSIGNAL, 1L);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set NoSignal [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_LOW_SPEED_LIMIT, 10);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set low speed limit [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_LOW_SPEED_TIME, 20);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set low speed time [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn,  CURLOPT_ENCODING, "gzip");
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set gzip encoding [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    *(size_t*)pHdrPtr = 0;
    code = curl_easy_setopt(conn, CURLOPT_HEADERDATA, &pHdrPtr);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_HEADERFUNCTION, writer_hdr);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set Header Function [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_MAXREDIRS, 10);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set max redirect times [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer_html);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    *(size_t*)pBufPtr = 0;
    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &pBufPtr);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
        curl_easy_cleanup(conn);
        return NULL;
    }

    /* 插入自定义头部 */
    if(conf->headers)
    {
        sf_conf_headers_t *h = conf->headers;
        do {
            char *hdr;
            h = (sf_conf_headers_t*)h->tDualLink.next;
            hdr = sf_var_exp(s->var_handle, h->hdr_value, s->pool);
            if (hdr)
            {
                slist = curl_slist_append(slist, hdr);
                //sf_free(hdr);
            }
        } while(h != conf->headers);

        curl_easy_setopt(conn, CURLOPT_HTTPHEADER, slist);
    }

    code = curl_easy_setopt(conn, CURLOPT_TIMEOUT, 20);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set timeout [%s]\n", errorBuffer);
        if (slist) curl_slist_free_all(slist); 
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_CONNECTTIMEOUT, 20);
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set connect timeout [%s]\n", errorBuffer);
        if (slist) curl_slist_free_all(slist); 
        curl_easy_cleanup(conn);
        return NULL;
    }

    code = curl_easy_setopt(conn, CURLOPT_COOKIEFILE, "cookie.txt");
    if (code != CURLE_OK)
    {
        fprintf(stderr, "Failed to set curl cookie \n");
        if (slist) curl_slist_free_all(slist); 
        curl_easy_cleanup(conn);
        return NULL;
    }

    if (proxy)
    {
            code = curl_easy_setopt(conn, CURLOPT_PROXY, proxy);
            if (code != CURLE_OK)
            {
                fprintf(stderr, "Failed to set proxy \n");
            curl_easy_cleanup(conn);
                return NULL;
            }
    }
    
    do
    {
        code = curl_easy_perform(conn);

        if (code == CURLE_OK)
        {
            break;
        }
    } while (--retry);

    if (0 == retry)
    {
        fprintf(stderr, "Failed to get '%s' [%s]\n", url, errorBuffer);
        fprintf(fail_fp, url);
        fprintf(fail_fp, "\n");
        fflush(fail_fp);
        if (slist) curl_slist_free_all(slist);
        curl_easy_cleanup(conn);
        return NULL;
    }

    if (slist) curl_slist_free_all(slist); /* free the list again */
    curl_easy_cleanup(conn);

    return pBufPtr;
}
