#ifndef _SF_CONF_H_
#define _SF_CONF_H_


typedef struct sf_conf_range_s {
    char* url_white;              /* URL可爬取范围 */
    char* url_black;
    pcre *url_white_re;        /* 编译后的正则表达式 */
    pcre *url_black_re;
    char *url_rewrite;
    struct sf_conf_range_s *next;
} sf_conf_range_t;

typedef struct sf_conf_policy_s {
    sf_pcre_t   *pcre_root;
    sf_conf_range_t *policy_range;
    struct sf_conf_policy_s *next;
} sf_conf_policy_t;

typedef struct sf_conf_headers_s {
    t_dual_link tDualLink;
    char *hdr_value;
} sf_conf_headers_t;

typedef struct sf_conf_s {
    int  url_level;
    int  url_count;
    int  interval;
    int url_feedback;
    char *method;     /* method用什么GET, POST, HEAD */
    char *post_data;   /* 如果采用post，发送什么 数据 */
    char *user_agent;
    char *url_suffix;
    sf_conf_policy_t *policy;
    sf_conf_range_t *range;
    sf_conf_headers_t *headers;
} sf_conf_t;


#define sf_load_attr_int(n, a, var) \
{\
	xmlChar *_s_;\
       _s_ = xmlGetProp(n,BAD_CAST a);\
       if (_s_){	var = atoi(_s_);	xmlFree(_s_);}else var=0;\
}

#define sf_load_attr_int_default(n, a, var, def) \
{\
	xmlChar *_s_;\
       _s_ = xmlGetProp(n,BAD_CAST a);\
       if (_s_){	var = atoi(_s_);	xmlFree(_s_);}else var=def;\
}


#define sf_load_attr_string(n, a, var) \
{\
	xmlChar *_s_;\
       _s_ = xmlGetProp(n,BAD_CAST a);\
	 if (_s_){var = strdup(_s_);	xmlFree(_s_);}\
}

#define sf_load_attr_string_default(n, a, var, def) \
{\
	xmlChar *_s_;\
       _s_ = xmlGetProp(n,BAD_CAST a);\
	 if (_s_){var = strdup(_s_);	xmlFree(_s_);}else var=def?sf_strdup(def):def;\
}





extern sf_conf_t sf_conf;

#endif
