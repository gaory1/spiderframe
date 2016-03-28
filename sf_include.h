#ifndef _SF_INCLUDE_H_
#define _SF_INCLUDE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <curl/curl.h>
#include <pcre.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <json.h>

#include "ngx_palloc.h"
#include "sf_types.h"
#include "sf_linkmodule.h"
#include "sf_hashmodule.h"
#include "sf_ua.h"
#include "sf_pcre.h"
#include "sf_mem.h"
#include "sf_log.h"
#include "sf_iconv.h"
#include "sf_conf.h"
#include "sf_proc.h"
#include "sf_var.h"
#include "sf_uniq.h"
#include "sf_confset.h"
#include "sf_json.h"
#include "sf_checkpoint.h"



#define SF_MAX_HTML_SIZE  (5*1024*1024) /* 5M */
#define SF_MAX_HDR_SIZE  (0.5*1024*1024) /* 0.5M */
#define SF_MAX_VAR_SIZE (128*1024)  /* 32K */
#define SF_MAX_URL_VECTOR (100)
#define SF_URL_OUT_PATTERN  "%d|%s\n"


#define BUF_OFFSET(buf)  (((char*)buf)+sizeof(size_t))
#define BUF_LEN(buf) (*((int*)buf))
#define BUF_CLEAR(buf) {(*((int*)buf) = 0); *BUF_OFFSET(buf) = '\0';}
#define BUF_TAIL(buf) (BUF_OFFSET(buf)+BUF_LEN(buf))

#define SAFE_FCLOSE(_fp_) {if (_fp_) fclose(_fp_); _fp_=NULL;}




typedef struct sf_spider_s {
    char *conf_fn;   /* 配置文件名 */
    sf_conf_t   conf;  /* 配置文件数据结构 */
    char *starturl;  /* 爬取的起始URL ，当此字段不为NULL时，从url_fn里取*/
    char *url_fn;   /* 爬取的起始URL文件 */
    FILE *url_fdr;  /* 文件的读句柄 */
    FILE *url_fdw;  /* 文件的写句柄 */
    char *out_fn;   /* 输出文件的文件句柄 */
    FILE *out_fd;    /* 输出文件的文件句柄 */
    HANDLE  *url_handle;  /* URL排重用的handle  */
    sf_vartable_t *var_handle;  /* var表的handle */
    HANDLE *uniq;     /* 用于排重的名柄 */
    int   line;
    int   skipline;       /* 最开始时从哪行开始续爬 */
    pcre *re_url;
    pcre *re_host;
    pcre *re_hostpath;
    FILE *pattern_fp;
    pthread_t thread; /* 爬虫线程 */
    int       id; /* 爬虫序号 */
    int       eof; /* 读取到URL文件尾 */
    int       outlen;
    ngx_pool_t *lifetime_pool; //整个生命周期使用的内存池
    ngx_pool_t *pool;          //每个URL使用的内存池
} sf_spider_t;

char *sf_curl_url(sf_spider_t *s, char *url, char*pBufPtr, char *pHdrPtr);
int sf_proc_start_spider(sf_spider_t *s);

#include "sf_extern.h"


#endif
