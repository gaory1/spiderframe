/* 此文件负责做pcre的内容提取*/
/* Written by jic 2013-03 */
#include "sf_include.h"

sf_pcre_t sf_pcre;
//#define PATTERN_URL  "(?im)(?<url>(http|https):\\/\\/[\\w\\.-]+(:\\d+)?[-;=\\/\\w\\.\\?&%#]*)|(href\\s*=\\s*[\'\\\"](?<hrefurl>(\\.+|\\?|\\/)?[\\w\\/\\.=\\-;\\?&%#]*))"
//#define PATTERN_URL    "(?im)(?<url>(http|https)://[\\w\\.\\-]+(:\\d+)?(/[\\w\\-\\.,%]*)*\\??([\\w\\-\\.,%#&=]*)?)|(?:href\\s*=\\s*['\"](?<hrefurl>([^'\"\\s]*)))"
#define PATTERN_URL    "(?im)(?<url>(http|https)://[\\w\\.\\-]+(:\\d+)?(/[\\w\\-\\.,%]*)*(\\??[\\w\\-\\.,%#&+=]*))|(?:href\\s*=\\s*['\"](?<hrefurl>([^'\"\\s]*)))"
#define PATTERN_HOST "(?im)(?<host>(http|https):\\/\\/[\\w\\.-]+(:\\d+)?)"
#define PATTERN_HOSTPATH "(?im)(?<hostpath>(http|https):\\/\\/[\\w\\.-]+(:\\d+)?\\/?.*\\/)"

#define const_strlen(s) (sizeof(s) - 1)
int spider_level;




pcre *sf_pcre_compile(char* pattern)
{
    pcre *re;
    int erroffset;
    const char *error;

    re = pcre_compile( pattern, 0, &error, &erroffset, NULL);
    if (NULL == re)
    {
        sf_log(LOGMOD_PCRE, LOG_ERR, "pcre_compile error: %s \n", error);
        exit(0);
        return NULL;
    }
    return re;
}

void sf_pcre_free_outbuf(sf_pcre_buflink_t **head)
{
    return ;
    sf_pcre_buflink_t *node;
    while (*head)
    {
        node = *head;
        DualLinkRemove((void**)head, (void*)node);
        //sf_free(node->buf);
        //sf_free(node);
    }
}

void sf_pcre_print_outbuf(FILE *fout, sf_pcre_buflink_t **head)
{
    return;
    sf_pcre_buflink_t *node = *head;
    if (!node)
        return;
    do {
        fprintf(fout, node->buf);

        node = (sf_pcre_buflink_t *)node->tDualLink.next;
    }
    while (node != *head);

    fflush(fout);
}

void sf_pcre_append_outbuf(sf_pcre_buflink_t **head, char *buf, ngx_pool_t *pool)
{
    sf_pcre_buflink_t *bufnode = ngx_pcalloc(pool, sizeof(sf_pcre_buflink_t));
    bufnode->buf = buf;
    DualLinkAppend((void**)head, (void*)bufnode);
}

char* sf_pcre_out_pattern(sf_spider_t *s, char *pattern)
{
    char tmpstr[SF_MAX_VAR_SIZE];
    char *tmpexp;

    tmpexp = sf_var_exp(s->var_handle, pattern, s->pool);
    snprintf(tmpstr, SF_MAX_VAR_SIZE, "%s", tmpexp);
    return ngx_pstrdup(s->pool, tmpstr);
}

char* sf_pcre_getcontent(ngx_pool_t *pool, char *outstr)
{
    size_t len  = strlen(outstr);
    //char *newstr = calloc(len+1,1);
    char *newstr = ngx_pcalloc(pool, len+1);
    char *p = outstr;
    char *d = newstr;
    int  isopen = 0;
    char *end = outstr + len;
    
    do {
        if (*p == '<')
        {
            if (end - p >= const_strlen("<blockquote")
                && strncmp(p, "<blockquote", const_strlen("<blockquote")) == 0)
            {
                p += const_strlen("<blockquote");
                while (*p != '\0')
                {
                    if (*p == '<'
                        && end - p >= const_strlen("</blockquote>")
                        && strncmp(p, "</blockquote>", const_strlen("</blockquote>")) == 0)
                    {
                        p += const_strlen("</blockquote>");
                        break;
                    }
                    p++;
                }
            }
            if (end - p >= 7 //"<script"
                && strncmp(p, "<script", 7) == 0)
            {
                p += 7;
                while (*p != '\0')
                {
                    if (*p == '<'
                        && end - p >= 9 //"</script>"
                        && strncmp(p, "</script>", 9) == 0)
                    {
                        p += 9;
                        break;
                    }
                    p++;
                }
            }
            else if (end - p >= 6 //"<style"
                && strncmp(p, "<style", 6) == 0)
            {
                p += 6;
                while (*p != '\0')
                {
                    if (*p == '<'
                        && end - p >= 8 //"</style>"
                        && strncmp(p, "</style>", 8) == 0)
                    {
                        p += 8;
                        break;
                    }
                    p++;
                }
            }
            #if 0
            else if (end - p >= 3 //"<em"
                && strncmp(p, "<em", 3) == 0)
            {
                p += 3;
                while (*p != '\0')
                {
                    if (*p == '<'
                        && end - p >= 5 //"</em>"
                        && strncmp(p, "</em>", 5) == 0)
                    {
                        p += 5;
                        break;
                    }
                    p++;
                }
            }
            else if (end - p >= 3 //"<a "
                && strncmp(p, "<a ", 3) == 0)
            {
                p += 3;
                while (*p != '\0')
                {
                    if (*p == '<'
                        && end - p >= 4 //"</a>"
                        && strncmp(p, "</a>", 4) == 0)
                    {
                        p += 4;
                        break;
                    }
                    p++;
                }
            }
            #endif
            else
            {
                char *pr = p;
                while(*pr != '>' && *pr != '\0')
                    pr++;

                if (*pr == '>')
                    p = pr+1;
                else
                {
                    strcpy(d, p);
                    p = pr;
                }
            }
        }
        else
        {
            *d++ = *p++;
        }
    } while(*p);

    return newstr;
}


char* sf_pcre_perform(sf_spider_t *s, sf_conf_policy_t *p, char *url, char *buf, size_t buflen, char *hdrbuf, size_t hdrlen)
{
    int erroffset;
    const char *error;
    int rc, rc2;
    int vector[SF_MAX_URL_VECTOR];
    size_t offset_out;
    char *outstr;
    sf_pcre_t *pcre = p->pcre_root;
    FILE* outfile = s->out_fd;
    sf_pcre_buflink_t *outbuf_head = NULL;
    char *tmpstr;

    size_t urllencur= strlen(url);
    sf_log(LOGMOD_PCRE, LOG_DEBUG, "-------------pcre perform -------------\n");

    if (NULL == pcre)
    {
        sf_log(LOGMOD_PCRE, LOG_ERR, "pcre arg is null!\n");
        return NULL;
    }


    /* 执行策略 */
    while (pcre)
    {
        char **pbuf;
        size_t   *pbuflen;
        int   repeat = 0;

        sf_pcre_capture_t *cap = NULL;
        sf_newspider_t  *ns =NULL;
        sf_pcre_trigger_t  *trig = NULL;


        if (NULL == pcre->re)
        {
            sf_log(LOGMOD_PCRE, LOG_ERR, "pcre_compile is null\n");
            return NULL;
        }

        sf_log(LOGMOD_PCRE, LOG_DEBUG, "begin pcre: %s\n", pcre->regex);


        if (pcre->urlmatch)
        {
            pbuf = &url;
            pbuflen = &urllencur;
        }
        else if (pcre->hdrmatch)
        {
            pbuf = &hdrbuf;
            pbuflen = &hdrlen;
        }
        else
        {
            pbuf = &buf;
            pbuflen = &buflen;
        }

        while (repeat++ < pcre->repeat)
        {
            int uniqflg = 0;

            rc = pcre_exec(pcre->re, NULL, *pbuf, *pbuflen, 0, 0, vector, SF_MAX_URL_VECTOR); /* 效率高*/
            if (rc <0)
            {
                /* 如果无法匹配，需要把变量清空*/
                if (rc == PCRE_ERROR_NOMATCH)
                {
                    sf_log(LOGMOD_PCRE, LOG_DEBUG, "pcre_exec error in buffer!error code=[%d]\n", rc);
                }
                else
                {
                    sf_log(LOGMOD_PCRE, LOG_ERR, "pcre_exec error in buffer!error code=[%d]\n", rc);
                }
                cap = pcre->sfcap;
                while (cap)
                {
                    if (cap->store_as)
                        sf_var_delete(&s->var_handle, cap->store_as);

                    if (cap->content_as)
                        sf_var_delete(&s->var_handle, cap->content_as);

                    cap = cap->next;
                }

                break;
            }

            if (pcre->json_new)
            {
                sf_json_print_field(JSON_OBJECT_BEGIN, NULL, 0, __LINE__);
                sf_json_print_field(JSON_KEY, pcre->json_new, strlen(pcre->json_new), __LINE__);
                sf_json_print_field(JSON_ARRAY_BEGIN, NULL, 0, __LINE__);
                sf_json_flush(s->out_fd);
            }

            /* 第一步: 遍历所有的捕获字串，并调用捕获后的一组行为 */
            cap = pcre->sfcap;
            while (cap)
            {
                rc2 = pcre_get_named_substring(pcre->re, *pbuf, vector, rc, cap->capstr, (const char **)&outstr);
                if (rc2 < 0)
                {
                    sf_log(LOGMOD_PCRE, LOG_ERR, "cap %s failed\n", cap->capstr);
                    outstr = NULL;
                }
                else
                {
                    sf_log(LOGMOD_PCRE, LOG_DEBUG, "cap %s success:%s\n", cap->capstr, outstr);
                }

                if (cap->store_as)
                {
                    sf_vartable_t *v;
                    v = sf_var_create(cap->store_as, outstr, s->lifetime_pool);
                    sf_var_insert(&s->var_handle, v);
                    sf_log(LOGMOD_PCRE, LOG_DEBUG, "insert store var %s = %s\n", v->name, v->value);
                }

                if (cap->content_as)
                {
                    sf_vartable_t *v;
                    char *content = NULL;
                    if (outstr) {
                        content = sf_pcre_getcontent(s->pool, outstr);
                    }

                    v = sf_var_create(cap->content_as, content, s->lifetime_pool);
                    sf_var_insert(&s->var_handle, v);
                    ngx_pfree(s->pool, content);
                    sf_log(LOGMOD_PCRE, LOG_DEBUG, "insert content var %s = %s\n", v->name, v->value);
                }
                sf_log(LOGMOD_PCRE, LOG_DEBUG, "1111111111111111parttern %s\n",cap->capstr);
                if (cap->out_pattern)
                {
                    char *varbuf = sf_pcre_out_pattern(s, cap->out_pattern);
                    sf_log(LOGMOD_PCRE, LOG_DEBUG, "%s", varbuf);
                    s->outlen += fprintf(s->pattern_fp, "%s", varbuf);
                    fflush(s->pattern_fp);
                    sf_pcre_append_outbuf(&outbuf_head, varbuf, s->pool);
                }

                if (cap->json_out)
                {
                    sf_json_print_buf(s->var_handle, cap->json_out);
                }

                if (outstr)
                    pcre_free_substring(outstr);

                cap = cap->next;
            }

            /* 第二步: 字段排重后，打印输出到文件 */
            if (pcre->uniq)
            {
                tmpstr = sf_var_exp(s->var_handle, pcre->uniq, s->lifetime_pool);

                if (sf_isdup(s->uniq, tmpstr))
                {
                    sf_log(LOGMOD_PCRE, LOG_DEBUG, "uniq -- duplicated! %s:\n", tmpstr);
                    //sf_pcre_free_outbuf(&outbuf_head);
                    ngx_pfree(s->pool, tmpstr);
                    sf_json_free_print_buf();
                    goto move_to_end;
                }
                else
                {
                    sf_log(LOGMOD_PCRE, LOG_DEBUG, "uniq -- insert! %s:\n", tmpstr);
                    sf_uniq_t *u = sf_uniq_create(s->lifetime_pool, sizeof(sf_uniq_t));
                    u->string = tmpstr;
                    sf_uniq_insert(s->uniq, u);
                }
            }

            sf_pcre_print_outbuf(s->out_fd, &outbuf_head);
            //sf_pcre_free_outbuf(&outbuf_head);

            sf_json_flush(s->out_fd);


            /* 第三步:调用外部处理函数 */
            trig = pcre->trigger;
            while (trig)
            {
                char *cmd;
                size_t   cmdlen;
                FILE *pfp;
                char result[1000+1];
                char *args;
		  
                /* 构造命令行 */
                args = sf_var_exp(s->var_handle, trig->args, s->pool);
                cmdlen =  strlen(trig->program) + strlen(args) + 4; /* 4包括"./", 一个空格，一个\0 */
                //cmd = malloc (cmdlen);
                cmd = ngx_palloc(s->pool, cmdlen);
                sprintf(cmd, "./%s %s", trig->program, args);
                //free (args);
		  
                if((pfp=popen(cmd,"r"))==NULL)
                {

                }
                else if (trig->store_as)
                {
                    char *value;
                    sf_vartable_t *v;
                    memset(result, 0, sizeof(result));
                    fread(result,  1, 1000, pfp);

                    v = sf_var_create(trig->store_as, result, s->lifetime_pool);  /* result传进去后会自动复制 */
                    sf_var_insert(&s->var_handle, v);
                    sf_log(LOGMOD_PCRE, LOG_DEBUG, "insert store var %s = %s\n", v->name, v->value);

                    pclose(pfp);
                }
                else
                {
                    pclose(pfp);
                }


                trig = trig->next;
            }

            /* 第四步: 处理需要开启新的爬虫的功能 */
            ns = pcre->ns;
            while (ns)
            {
                sf_spider_t *s_new;
                sf_hash_conf_t *h_new;

                HANDLE h = sf_confset_search(ns->conf_fn);

                s_new = h ? (sf_spider_t*)((sf_hash_conf_t* )h)->s : sf_spider_init(ns->conf_fn);
                if (!s_new)
                {
                    s_new = sf_spider_init(ns->conf_fn);
                }

                if (ns->url_exp)
                {
                    s_new->starturl = sf_var_exp(s->var_handle, ns->url_exp, s->pool);
                }

                if (ns->fin_fn)
                {
                    s_new->url_fn = ngx_pstrdup(s->pool, ns->fin_fn);
                    s_new->url_handle = sf_proc_init_url_entry(s->url_handle, ns->fin_fn);
                }
                else
                {
                    s_new->url_handle = s->url_handle;
                }

                s_new->out_fd = s->out_fd;
                s_new->var_handle = s->var_handle;
                s_new->uniq = s->uniq;

                if (!h)
                {
                    pthread_mutex_lock(&global_pool_mutex);
                    h_new = (sf_hash_conf_t*)AllocHashData(global_pool, sizeof(sf_hash_conf_t));
                    pthread_mutex_unlock(&global_pool_mutex);
                    h_new->s = (void *)s_new;
                    h_new->conf_fn = ngx_pstrdup(s->pool, ns->conf_fn);
                    sf_confset_insert(h_new);
                }

                spider_level++;
                sf_proc_start_spider(s_new);
                spider_level--;

                ngx_pfree(s->pool, s_new->starturl);

                ns = ns->next;
            }
move_to_end:
            if(pcre->movetoend)
            {
                *pbuf += vector[1];
                *pbuflen = buflen - vector[1];
            }

            if (pcre->json_new)
            {
                sf_json_print_field(JSON_ARRAY_END, NULL, 0, __LINE__);
                sf_json_print_field(JSON_OBJECT_END, NULL, 0, __LINE__);
                sf_json_flush(s->out_fd);
            }

        }


        pcre= pcre->next;
    }

    return buf+offset_out;

}


char *gethost(sf_spider_t *s, char *url)
{
    int rc;
    int vector[SF_MAX_URL_VECTOR];
    char *outstr=NULL;

    if (!s->re_host)
    {
        s->re_host = sf_pcre_compile(PATTERN_HOST);
    }

    rc = pcre_exec(s->re_host, NULL, url, strlen(url), 0, 0, vector, SF_MAX_URL_VECTOR);
    if (rc <0)
    {
        sf_log(LOGMOD_PCRE, LOG_ERR, "Cannot find host\n");
        return 0;
    }

    pcre_get_named_substring(s->re_host, url, vector, rc, "host", (const char **)&outstr);

    return outstr;

}

char *gethostpath(sf_spider_t *s, char *url)
{
    int rc;
    int vector[SF_MAX_URL_VECTOR];
    char *outstr=NULL;

    if (!s->re_hostpath)
    {
        s->re_hostpath = sf_pcre_compile(PATTERN_HOSTPATH);
    }

    rc = pcre_exec(s->re_hostpath, NULL, url, strlen(url), 0, 0, vector, SF_MAX_URL_VECTOR);
    if (rc <0)
    {
        sf_log(LOGMOD_PCRE, LOG_ERR, "Cannot find hostpath\n");
        return 0;
    }

    pcre_get_named_substring(s->re_hostpath, url, vector, rc, "hostpath", (const char **)&outstr);

    return outstr;

}

void redurceurl(char *url)
{
    char *p;
    char temp[255];

    while (p = strstr(url, "/../"))
    {
        char *c = p;
        while (c-- !=url)
        {
            if (*c == '/')
            {
                strcpy(temp, p+4);
                strcpy(c+1, temp);
                break;
            }
        }
    }


    while (p = strstr(url, "/./"))
    {
        strcpy(temp, p+2);
        strcpy(p,temp);
    }
}

int sf_pcre_get_href(sf_spider_t *s, int level, char *buf, size_t buflen, char *origurl)
{
    int rc;
    char *outstr=NULL;
    char *bufcur = buf;
    size_t buflencur = buflen;
    int vector[SF_MAX_URL_VECTOR];
    HANDLE hUrl = s->url_handle;
    FILE* outfile = s->out_fd;

    if (!s->re_url)
    {
        s->re_url = sf_pcre_compile(PATTERN_URL);
        if (s->re_url == NULL)
        {
            sf_log(LOGMOD_PCRE, LOG_ERR, "compile url pattern error!\n");
        }
    }

    while (1)
    {
        rc = pcre_exec(s->re_url, NULL, bufcur, buflencur, 0, 0, vector, SF_MAX_URL_VECTOR); /* 效率低 */
        if (rc <0)
        {
            sf_log(LOGMOD_PCRE, LOG_DEBUG, "Cannot found links any more!\n");
            return 0;
        }

        pcre_get_named_substring(s->re_url, bufcur, vector, rc, "url", (const char **)&outstr);

        if (!outstr||!outstr[0])
        {
            pcre_free_substring(outstr);
            pcre_get_named_substring(s->re_url, bufcur, vector, rc, "hrefurl", (const char **)&outstr);
            if (outstr)
            {
                char *newurl;
                char *host = NULL;

                if (strstr(outstr, "://") == NULL)
                {
                    if (*outstr == '/')
                        host = gethost(s, origurl);
                    else if (*outstr == '?')
                        host = origurl;
                    else 
                        host = gethostpath(s, origurl);

                    if (!host)
                    {
                        sf_log(LOGMOD_PCRE, LOG_ERR, "Get Host Error\n");
                        pcre_free_substring(outstr);
                        return 0;
                    }
                    //newurl = malloc(strlen(host)+strlen(outstr)+1);
                    newurl = ngx_pnalloc(s->pool, strlen(host)+strlen(outstr)+1);
                    strcpy(newurl, host);
                    strcat(newurl, outstr);
                    if (*outstr != '?')
                    {
                        pcre_free_substring(host);
                    }
                    redurceurl(newurl);
                    sf_proc_insert_url_withrange(s, newurl,level);
                    ngx_pfree(s->pool, newurl);
                }
                else
                {
                    sf_proc_insert_url_withrange(s, outstr, level);
                }
            }
        }
        else
        {
            sf_proc_insert_url_withrange(s, outstr, level);
        }
        pcre_free_substring(outstr);

        bufcur += vector[1];
        buflencur = buflencur - vector[1];

    }
    return 0;

}


