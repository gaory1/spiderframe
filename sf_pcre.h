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
    char *capstr; /* ������ִ� */
    char *out_pattern;   /* ������ʱʹ�õ����� */
    char *json_out;
    char *store_as;
    char *content_as;
    struct sf_pcre_capture_s *next;
} sf_pcre_capture_t;

typedef struct sf_pcre_s {
    int urlmatch:1;  /* ƥ��HTTP�����URL */
    int hdrmatch:1;  /* ƥ��HTTP��Ӧͷ�� */
    int movetoend:1; /* ƥ�����bufferָ�����������ƥ���ִ�β */
    int repeat;    /* ��ʾ�������ִ�м��β��䣬��1����ʾִ��һ�� */
    char* regex;  /* ������ʽ�ִ� */
    pcre *re;        /* ������������ʽ */
    char* uniq;
    char *json_new;    /* �½�һ��JSONԪ�� */
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
