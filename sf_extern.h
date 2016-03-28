#ifndef _SF_EXTERN_H_
#define _SF_EXTERN_H_


sf_spider_t* sf_spider_init(char *conf_fn);
char* sf_pcre_perform(sf_spider_t *s, sf_conf_policy_t *p, char *url, char *buf, size_t buflen, char *hdrbuf, size_t hdrlen);
int sf_pcre_get_href(sf_spider_t *s, int level, char *buf, size_t buflen, char *origurl);
int sf_proc_insert_url_withrange(sf_spider_t*s, char *url, int level);
int sf_conf_init(sf_spider_t *s, char *conf_fn);

extern pthread_mutex_t file_mutex;
extern ngx_pool_t   *global_pool;
extern pthread_mutex_t global_pool_mutex;

#endif
