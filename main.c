/* Written by jic 2013-03 */

#include <sys/stat.h>
#include <sys/types.h>
#include "sf_include.h"
#include "ngx_md5.h"

void sf_proc_init_varhandle(sf_spider_t *s);

FILE *out_fd;
FILE *url_fdr;
FILE *url_fdw;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
int line;

HANDLE    *url_handle;

sf_spider_t **spider_list;
int           spider_cnt;

ngx_pool_t   *global_pool;
pthread_mutex_t global_pool_mutex = PTHREAD_MUTEX_INITIALIZER;
    
extern FILE *fail_fp;
extern char *proxy;

void sf_global_init(int skipline)
{
    int level;
    char url[2048];
    int i;

    //mkdir("data", 0777);

    global_pool = ngx_create_pool(1024 * 1024 * 16, NULL);
    
    out_fd = fopen("result.txt", "a");
    url_fdr  = fopen("url.txt", "r");
    url_fdw = fopen("url.txt", "a");
    fail_fp = fopen("fail_list", "a+");
    url_handle = sf_proc_init_url_entry(NULL, "url.txt");

    for (i = 0; i < skipline; i++)
    {
        fscanf(url_fdr,SF_URL_OUT_PATTERN, &level, url);
    }
    line = skipline;
}

int sf_global_scan_url(int *level, char *url)
{
    int rc;
    pthread_mutex_lock(&file_mutex);
    rc = fscanf(url_fdr, SF_URL_OUT_PATTERN, level, url);
    if (rc > 0)
    {
        line++;
    }
    pthread_mutex_unlock(&file_mutex);
    return rc;
}

void sf_proc_output_unparse(char *url_fn, FILE* out_fd, int line)
{
    FILE *fp = fopen(url_fn, "r");
    int i;
    int level;
    static char url[2048];

    for (i=0; i<line; i++)
    {
        fscanf(fp,SF_URL_OUT_PATTERN, &level,  url);
    }


    sf_json_print_field(JSON_OBJECT_BEGIN, NULL, 0, __LINE__);
    sf_json_print_field(JSON_KEY, "unparse_url", strlen("unparse_url"), __LINE__);
    sf_json_print_field(JSON_ARRAY_BEGIN, NULL, 0, __LINE__);

    while (EOF != fscanf(fp, SF_URL_OUT_PATTERN, &level, url))
    {
        sf_json_print_field(JSON_OBJECT_BEGIN, NULL, 0, __LINE__);
        sf_json_print_field(JSON_KEY, "url", sizeof("url")-1, __LINE__);
        sf_json_print_field(JSON_STRING, url, strlen(url), __LINE__);
        sf_json_print_field(JSON_OBJECT_END, NULL, 0, __LINE__);
        sf_json_flush(out_fd);
        sf_log(LOGMOD_PROC, LOG_DEBUG, "Unparsed url --- %s\n", url);
    }
    sf_json_print_field(JSON_ARRAY_END, NULL, 0, __LINE__);
    sf_json_print_field(JSON_OBJECT_END, NULL, 0, __LINE__);
    sf_json_flush(out_fd);
}

int sf_all_spiders_eof()
{
    int i;
    
    for (i = 0; i < spider_cnt; i++)
    {
        if (!spider_list[i]->eof)
        {
            return 0;
        }
    }
    return 1;
}

void sf_save_body(char *url, char *data, size_t len)
{
    char path[200];
    ngx_md5_t ngx_ctx;
    u_char md5[16];
    int i;
    FILE *fp;
    
    ngx_md5_init(&ngx_ctx);
    ngx_md5_update(&ngx_ctx, url, strlen(url));
    ngx_md5_final(md5, &ngx_ctx);

    sprintf(path, "data/%02x", md5[0]);
    mkdir(path, 0777);
    sprintf(path, "data/%02x/%02x", md5[0], md5[1]);
    mkdir(path, 0777);
    
    sprintf(path, "data/%02x/%02x/%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
        md5[0], md5[1], 
        md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7],
        md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);
    fp = fopen(path, "w");
    if (!fp)
    {
        fprintf(stderr, "unabled to open %s for write\n", path);
        return;
    }
    fprintf(fp, "%s\n\n", url);
    fwrite(data, len, 1, fp);
    fclose(fp);
}

int sf_proc_start_spider(sf_spider_t *s)
{
    char *url_file = s->url_fn;
    char *out_file = s->out_fn;
    sf_conf_t *conf = &s->conf;
    HANDLE hUrl = s->url_handle;
    char *pbuf = ngx_palloc(s->lifetime_pool, SF_MAX_HTML_SIZE);
    //char *pbuf2 =malloc(SF_MAX_HTML_SIZE);
    char *phdr = ngx_palloc(s->lifetime_pool, SF_MAX_HDR_SIZE);
    size_t inlen, outlen;
    //char *tmp1, *tmp2;
    int level, count;
    char url[2048];
    //int line = s->skipline;
    //int line = sf_get_checkpoint(s->conf_fn);
    int last_line =0;
    sf_conf_policy_t *p;
    char backup_url[2048];
    
    sf_json_print_field(JSON_ARRAY_BEGIN, NULL, 0, __LINE__);
    sf_json_flush(s->out_fd);

    s->pool = NULL;
    
    while (1)
    {
        while (1)
        {
            if (s->starturl)
            {
                strcpy(url, s->starturl);
            }
            else if (EOF == sf_global_scan_url(&level, url))
            {
                sf_log(LOGMOD_PROC, LOG_INFO, "Read url from file End(thr id %d)\n", s->id);
                s->eof = 1;
                sleep(1);
                break;
            }

            if (s->pool)
            {
                ngx_destroy_pool(s->pool);
            }
            s->pool = ngx_create_pool(1024*1024, NULL);
            if (!s->pool)
            {
                sf_log(LOGMOD_PROC, LOG_ERR, "ngx_create_pool failed(thr id %d)\n", s->id);
                sleep(1);
                break;
            }
            
            s->outlen = 0;
            s->eof = 0;
            //backup_url = sf_strdup(url);
            strcpy(backup_url, url);

            if (conf->url_suffix)
            {
                char *suffix  = sf_var_exp(s->var_handle, conf->url_suffix, s->pool);
                strcat(url, suffix);
                //sf_free(suffix);
            }

            sf_log(LOGMOD_PROC, LOG_INFO, "line=%d, level=%d, and url is %s\n", line, level, url);
            //line ++;


            BUF_CLEAR(pbuf);
            BUF_CLEAR(phdr);


            if (!sf_curl_url(s, url, pbuf,phdr) && conf->url_feedback)
            {
                sf_json_print_field(JSON_OBJECT_BEGIN, NULL, 0, __LINE__);
                sf_json_print_field(JSON_KEY, "fail_url", strlen("fail_url"), __LINE__);
                sf_json_print_field(JSON_ARRAY_BEGIN, NULL, 0, __LINE__);
                sf_log(LOGMOD_PROC, LOG_DEBUG, "curl URL error [%s]\n", url);
                sf_json_print_field(JSON_OBJECT_BEGIN, NULL, 0, __LINE__);
                sf_json_print_field(JSON_KEY, "url", sizeof("fail_url")-1, __LINE__);
                sf_json_print_field(JSON_STRING, backup_url, strlen(backup_url), __LINE__);
                sf_json_print_field(JSON_OBJECT_END, NULL, 0, __LINE__);
                sf_json_print_field(JSON_ARRAY_END, NULL, 0, __LINE__);
                sf_json_print_field(JSON_OBJECT_END, NULL, 0, __LINE__);
                sf_json_flush(s->out_fd);
                //sf_free(backup_url);
                break;
            }
            else if (conf->url_feedback)
            {
                sf_json_print_field(JSON_OBJECT_BEGIN, NULL, 0, __LINE__);
                sf_json_print_field(JSON_KEY, "success_url", strlen("success_url"), __LINE__);
                sf_json_print_field(JSON_ARRAY_BEGIN, NULL, 0, __LINE__);
                sf_json_print_field(JSON_OBJECT_BEGIN, NULL, 0, __LINE__);
                sf_json_print_field(JSON_KEY, "url", sizeof("url")-1, __LINE__);
                sf_json_print_field(JSON_STRING, backup_url, strlen(backup_url), __LINE__);
                sf_json_print_field(JSON_OBJECT_END, NULL, 0, __LINE__);
                sf_json_print_field(JSON_ARRAY_END, NULL, 0, __LINE__);
                sf_json_print_field(JSON_OBJECT_END, NULL, 0, __LINE__);
                sf_json_flush(s->out_fd);
                //sf_free(backup_url);
            }

            if (conf->interval)
                sleep(conf->interval);

            inlen = BUF_LEN(pbuf);
            outlen = SF_MAX_HTML_SIZE;
            sf_log(LOGMOD_PROC, LOG_DEBUG, "result :\n\tbuffer:%d\theader:%d\n", inlen, BUF_LEN(phdr));

            //tmp1 = BUF_OFFSET(pbuf);
            //tmp2 = BUF_OFFSET(pbuf2);
            //sf_iconv_u2g(tmp1, &inlen, tmp2, &outlen);
            //BUF_LEN(pbuf2) = SF_MAX_HTML_SIZE - outlen;

            sf_pcre_get_href(s, level+1, BUF_OFFSET(pbuf), BUF_LEN(pbuf), url);

            /* 遍历Policy配置 */
            p = conf->policy;
            while (p)
            {
                if (sf_proc_url_match_range(url, p->policy_range))
                {
                    sf_pcre_perform(s, p, url, BUF_OFFSET(pbuf), BUF_LEN(pbuf), BUF_OFFSET(phdr), BUF_LEN(phdr));
                }
                p = p->next;
            }

            //if (s->outlen < 1000)
            //{
                //sf_save_body(url, BUF_OFFSET(pbuf), BUF_LEN(pbuf));
            //}
            
            BUF_CLEAR(pbuf);
            //BUF_CLEAR(pbuf2);

            if (level >conf->url_level||line >=conf->url_count)
            {
                s->eof = 1;
                break;
            }

            if (sf_do_checkpoint(line))
            {
                sf_save_chkpoint(s->conf_fn, line);
                sf_log(LOGMOD_PROC, LOG_INFO, 
                    "---------- pool global %luk spider %luk request %luk (thr %d) ----------\n",
                    global_pool->acc/1024, s->lifetime_pool->acc/1024, s->pool->acc/1024, s->id);
            }
        }
        //SAFE_FCLOSE(s->url_fdr);
        //SAFE_FCLOSE(s->url_fdw);

        if (level >conf->url_level||line >=conf->url_count)
        {
            sf_log(LOGMOD_PROC, LOG_DEBUG, "Reach the exit condition level %d>%d or count %d>=%d\n",
                   level, conf->url_level, line, conf->url_count);
            if (!s->starturl && conf->url_feedback)
            {
                sf_proc_output_unparse(s->url_fn, s->out_fd, line);
            }
            break;
        }

        #if 0 //各线程不应该根据自己的情况就简单决定退出, 应该...
        if (last_line == 0)
            last_line = line;
        else if (last_line == line)
            break;
        #endif
        if (sf_all_spiders_eof())
        {
            break;
        }
    }

    sf_json_print_field(JSON_ARRAY_END, NULL, 0, __LINE__);
    sf_json_flush(s->out_fd);

    if (s->out_fn)
    {
        //SAFE_FCLOSE(s->out_fd);
    }
    SAFE_FCLOSE(s->pattern_fp);
    
    return 0;
}


void sf_help(char *command)
{
    printf("%s:create by jic\n", command);
    printf("\t%s [options]         : open the default \"sf_conf.xml\" config xml file and run the spider\n", command);
    printf("\t%s [options] [config] : open a config xml file and run the spider\n", command);
    printf("\t%s [options] [config] [skipline] : open a config xml file and run the spider and from the No.skipline\n", command);
    printf("\t%s [options] [config] [skipline] [thread count]: using multi-thread\n", command);
    printf("\t[options]:\n");
    printf("\t\t-q        : no debug log\n");
    printf("\t\t-m <n>    : same as [thread count], for convinence\n");
    printf("\t\t-l <n>    : same as [skipline], for convinence\n");
}

sf_spider_t *sf_spider_create(int id, char *conf_fn)
{
    sf_spider_t *s;
    char buf[500];
    
    s = sf_spider_init(conf_fn);
    if (!s)
    {
        sf_log(LOGMOD_PROC, LOG_DEBUG, "sf_spider_init failed\n");
        exit(1);
    }

    s->id = id;
    s->url_fdr = url_fdr;
    s->url_fdw = url_fdw;
    s->url_fn = "url.txt";
    s->out_fn = "result.txt";
    s->url_handle= url_handle; //sf_proc_init_url_entry(NULL, s->url_fn);
    s->uniq = sf_uniq_init(s->lifetime_pool);
    //s->skipline = skipline;

    sprintf(buf, "pattern_%d.txt", id);
    s->pattern_fp = fopen(buf, "a");
    if (s->pattern_fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    if (-1 == pthread_create(&s->thread, NULL, (void *(*)(void *))sf_proc_start_spider, s))
    {
        perror("pthread_create");
        exit(1);
    }

    return s;
}

int main(int argc, char* argv[])
{
    HANDLE hUrl;
    char *conf_fn;
    int skipline= 0;
    int i;
    void *thr_ret;

    //self destroy after 2014-6-30
    #if 0
    if (time(NULL) >= 1404144000)
    {
        char exe_sym[500];
        char exe_real[500];
        ssize_t n;
        
        sprintf(exe_sym, "/proc/%u/exe", (unsigned)getpid());
        n = readlink(exe_sym, exe_real, sizeof(exe_real));
        exe_real[n] = '\0';
        unlink(exe_real);
        return 0;
    }
    #endif

    spider_cnt = 1;
    conf_fn = "sf_conf.xml";
    
    #if 0
    if (argc > 4)
    {
        sf_help(argv[0]);
        return 0;
    }
    #endif
    
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-q") == 0)
        {
            log_lvl_cfg = LOG_INFO;
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            sf_help(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-m") == 0)
        {
            if (i + 1 < argc)
            {
                i++;
                spider_cnt = atoi(argv[i]);
            }
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            if (i + 1 < argc)
            {
                i++;
                skipline = atoi(argv[i]);
            }
        }
        else if (strcmp(argv[i], "-x") == 0)
        {
            if (i + 1 < argc)
            {
                i++;
                proxy = argv[i];
            }
        }
        else
        {
            conf_fn = argv[i];
            if (i + 1 < argc)
            {
                i++;
                skipline = atoi(argv[i]);
                if (i + 1 < argc)
                {
                    i++;
                    spider_cnt = atoi(argv[i]);
                }
            }
        }
    }

    #if 0
    if (argc == 1)
        conf_fn = "sf_conf.xml";
    else if (!strcmp("-h", argv[1]))
    {
        sf_help(argv[0]);
        return 0;
    }
    else if (argc == 2)
    {
        conf_fn = argv[1];
    }
    else if (argc == 3)
    {
	  conf_fn = argv[1];
	  skipline = atoi(argv[2]);
    }
    else if (argc == 4)
    {
	  conf_fn = argv[1];
	  skipline = atoi(argv[2]);
      spider_cnt = atoi(argv[3]);
    }
    #endif
    
    //system("tar xvf example*");

    sf_log(LOGMOD_PROC, LOG_DEBUG, "test begin......\n");

    //sf_load_chkpoint();
    
    sf_global_init(skipline);
    sf_confset_init();

    spider_list = ngx_palloc(global_pool, spider_cnt * sizeof(sf_spider_t*));
    
    for (i = 0; i < spider_cnt; i++)
    {
        spider_list[i] = sf_spider_create(i, conf_fn);
    }
    for (i = 0; i < spider_cnt; i++)
    {
        pthread_join(spider_list[i]->thread, NULL);
    }

    sf_log(LOGMOD_PROC, LOG_INFO, "all spiders finished.\n");
    //sf_proc_start_spider(s);

}
