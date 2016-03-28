#ifndef _SF_PCRE_H_
#define _SF_PCRE_H_


typedef struct sf_newspider_s {
    char *url_exp;
    char *conf_fn;
    char *fin_fn;
    struct sf_newspider_s *next;
} sf_newspider_t;

typedef struct sf_pcre_trigger_s{
	char *program;
	char *args;
	char *store_as;
	struct sf_pcre_trigger_s *next;
}sf_pcre_trigger_t;

typedef struct sf_pcre_action_s {

} sf_pcre_action_t;

typedef struct sf_pcre_capture_s {
    char *capstr; /* 捕获的字串 */
    char *out_pattern;   /* 保存结果时使用的名称 */
    char *json_out;
    char *store_as;
    char *content_as;
    struct sf_pcre_capture_s *next;
} sf_pcre_capture_t;

typedef struct sf_pcre_s {
    int urlmatch:1;  /* 匹配HTTP请求的URL */
    int hdrmatch:1;  /* 匹配HTTP响应头部 */
    int movetoend:1; /* 匹配完后将buffer指针向后移至此匹配字串尾 */
    int repeat;    /* 表示这个规则执行几次不配，配1均表示执行一次 */
    char* regex;  /* 正则表达式字串 */
    pcre *re;        /* 编译后的正则表达式 */
    char* uniq;
    char *json_new;    /* 新建一个JSON元素 */
    sf_pcre_capture_t   *sfcap;
    sf_newspider_t  *ns;
    sf_pcre_trigger_t    *trigger;
    struct sf_pcre_s   *next;
} sf_pcre_t;

typedef struct sf_pcre_buflink_s {
    t_dual_link  tDualLink;
    char *buf;
} sf_pcre_buflink_t;


extern int spider_level;

sf_pcre_t* sf_pcre_test();
pcre *sf_pcre_compile(char* pattern);

#endif
