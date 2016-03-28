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
    char *conf_fn;   /* �����ļ��� */
    sf_conf_t   conf;  /* �����ļ����ݽṹ */
    char *starturl;  /* ��ȡ����ʼURL �������ֶβ�ΪNULLʱ����url_fn��ȡ*/
    char *url_fn;   /* ��ȡ����ʼURL�ļ� */
    FILE *url_fdr;  /* �ļ��Ķ���� */
    FILE *url_fdw;  /* �ļ���д��� */
    char *out_fn;   /* ����ļ����ļ���� */
    FILE *out_fd;    /* ����ļ����ļ���� */
    HANDLE  *url_handle;  /* URL�����õ�handle  */
    sf_vartable_t *var_handle;  /* var���handle */
    HANDLE *uniq;     /* �������ص����� */
    int   line;
    int   skipline;       /* �ʼʱ�����п�ʼ���� */
    pcre *re_url;
    pcre *re_host;
    pcre *re_hostpath;
    FILE *pattern_fp;
    pthread_t thread; /* �����߳� */
    int       id; /* ������� */
    int       eof; /* ��ȡ��URL�ļ�β */
    int       outlen;
    ngx_pool_t *lifetime_pool; //������������ʹ�õ��ڴ��
    ngx_pool_t *pool;          //ÿ��URLʹ�õ��ڴ��
} sf_spider_t;

char *sf_curl_url(sf_spider_t *s, char *url, char*pBufPtr, char *pHdrPtr);
int sf_proc_start_spider(sf_spider_t *s);

#include "sf_extern.h"


#endif
