#include "sf_include.h"

#define  NAME_POLICY "policy"
#define  NAME_PCRE     "pcre"
#define  NAME_SCAP     "cap"
#define  NAME_NEWSPIDER     "new_spider"
#define  NAME_TRIGGER     "trigger"
#define  NAME_ACT      "action"
#define  NAME_URL_WHITE "url_white"
#define  NAME_RANGE   "range"
#define  NAME_POLICY_RANGE "policy_range"
#define  NAME_VARS     "vars"
#define  NAME_VAR      "var"
#define  NAME_HEADER   "header"
#define  NAME_HEADERS  "headers"



#define  compare_name(_A_, _B_) (strcmp(_A_, _B_))
#define  sf_conf_insert_tail(s, h)\
{\
	s->next = (h);  /* 记录头 */\
	if (!(h))\
	{\
		(h) = s;\
	}\
	else {\
		while (h)\
		{\
			if (!(h)->next)\
			{\
				(h)->next = s;\
				break;\
			}\
			(h) = (h)->next;\
		}\
		(h) = s->next;\
		s->next = NULL;\
	}\
}

int sf_conf_vars(xmlNodePtr node, sf_spider_t *s);


int sf_conf_cap(xmlNodePtr node, sf_pcre_capture_t **capptr)
{
    char *name;
    sf_pcre_capture_t *cap;
    sf_pcre_capture_t *precap = NULL;
    xmlChar *szAttr;          //临时字符串变量

    for(node; node != NULL; node=node->next)
    {
        name=(char*)(node->name);

        if (!compare_name(name, NAME_SCAP))
        {
            sf_log(LOGMOD_PROC, LOG_DEBUG, "cap name %s\n", name);
            cap = sf_malloc(sizeof(sf_pcre_capture_t));


            if (precap)
            {
                precap->next = cap;
                precap = cap;
            }
            else
            {
                *capptr =cap;
                precap = cap;
            }

            sf_load_attr_string(node, "capstr" , cap->capstr);
            sf_load_attr_string_default(node, "out_pattern", cap->out_pattern, NULL);
            sf_load_attr_string_default(node, "json_out", cap->json_out, NULL);
            sf_load_attr_string(node, "store_as", cap->store_as);
            sf_load_attr_string(node, "content_as", cap->content_as);

        }
    }

}

int sf_conf_newspider(xmlNodePtr node, sf_newspider_t **newspider)
{
    char *name;
    sf_newspider_t *ns;
    sf_newspider_t *prens = NULL;
    xmlChar *szAttr;          //临时字符串变量

    for(node; node != NULL; node=node->next)
    {
        name=(char*)(node->name);

        if (!compare_name(name, NAME_NEWSPIDER))
        {
            sf_log(LOGMOD_PROC, LOG_DEBUG, "newspider name %s\n", name);
            ns = sf_malloc(sizeof(sf_newspider_t));

            if (prens)
            {
                prens->next = ns;
                prens = ns;
            }
            else
            {
                *newspider =ns;
                prens = ns;
            }

            sf_load_attr_string(node, "config_file" , ns->conf_fn);
            sf_load_attr_string_default(node, "url", ns->url_exp, NULL);
            sf_load_attr_string_default(node, "input_file", ns->fin_fn, NULL);
        }

    }

}

int sf_conf_trigger(xmlNodePtr node, sf_pcre_trigger_t **trigptr)
{
    char *name;
    sf_pcre_trigger_t *trig;
    sf_pcre_trigger_t *pretrig = NULL;
    xmlChar *szAttr;          //临时字符串变量

    for(node; node != NULL; node=node->next)
    {
        name=(char*)(node->name);

        if (!compare_name(name, NAME_TRIGGER))
        {
            sf_log(LOGMOD_PROC, LOG_DEBUG, "trigger name %s\n", name);
            trig = sf_malloc(sizeof(sf_pcre_trigger_t));


            if (pretrig)
            {
                pretrig->next = trig;
                pretrig = trig;
            }
            else
            {
                *trigptr =trig;
                pretrig = trig;
            }

            sf_load_attr_string_default(node, "program", trig->program, NULL);
            sf_load_attr_string_default(node, "args", trig->args, NULL);
	     sf_load_attr_string_default(node, "store_as", trig->store_as, NULL);

        }
    }

}


int sf_conf_pcre(xmlNodePtr node, sf_pcre_t **pcreptr)
{
    sf_pcre_t *pcre;

    pcre = sf_malloc(sizeof(sf_pcre_t));
    sf_load_attr_string(node, "regex", pcre->regex);
    sf_load_attr_int_default(node, "urlmatch", pcre->urlmatch, 0);
    sf_load_attr_int_default(node, "hdrmatch", pcre->hdrmatch, 0);
    sf_load_attr_int_default(node, "movetoend", pcre->movetoend, 1);
    sf_load_attr_int_default(node, "repeat", pcre->repeat, 1);
    sf_load_attr_string_default(node, "uniq" ,pcre->uniq, NULL);
    sf_load_attr_string_default(node, "json_new", pcre->json_new, NULL);
    pcre->re = sf_pcre_compile(pcre->regex);
    if (!pcre->re)
    {
        sf_log(LOGMOD_CONF, LOG_ERR, "pcre error for name:%s\n", node->name);
        return -1;
    }

    sf_conf_insert_tail(pcre, *pcreptr);

    sf_conf_cap(node->children, &pcre->sfcap);
    sf_conf_newspider(node->children, &pcre->ns);
    sf_conf_trigger(node->children, &pcre->trigger);

}

int sf_conf_policy_range(xmlNodePtr node, sf_conf_range_t **rangeptr)
{
    sf_conf_range_t *porang;

    porang = sf_malloc(sizeof(sf_conf_range_t));
    sf_load_attr_string(node, "url_white", porang->url_white);
    sf_load_attr_string_default(node, "url_black", porang->url_black, NULL);
    porang->url_white_re = sf_pcre_compile(porang->url_white);
    if (!porang->url_white_re)
    {
        sf_log(LOGMOD_CONF, LOG_ERR, "pcre error for name %s:%s\n", node->name, porang->url_white);
        return -1;
    }

    if (porang->url_black)
    {
        porang->url_black_re = sf_pcre_compile(porang->url_black);
        if (!porang->url_black_re)
        {
            sf_log(LOGMOD_CONF, LOG_ERR, "pcre error for name %s:%s\n", node->name, porang->url_black);
            return -1;
        }
    }
    
    sf_conf_insert_tail(porang, *rangeptr);

}

int sf_conf_policy(xmlNodePtr node, sf_conf_policy_t **policyptr)
{
    char *name;
    sf_pcre_t *pcre;
    sf_pcre_t *prepcre = NULL;
    sf_pcre_t **pcreptr = NULL;

    sf_conf_policy_t *p = sf_malloc(sizeof(sf_conf_policy_t));
    p->pcre_root = NULL;
    pcreptr = &(p->pcre_root);

    p->next = (*policyptr);
    *policyptr = p;

    for(node; node != NULL; node=node->next)
    {
        name=(char*)(node->name);
        sf_log(LOGMOD_CONF, LOG_DEBUG, "policy name %s\n", name);

        if (!compare_name(name, NAME_PCRE))
        {
            sf_conf_pcre(node, pcreptr);
        } else if (!compare_name(name, NAME_POLICY_RANGE))
        {
            sf_conf_policy_range(node,  &((*policyptr)->policy_range));
        }
    }

    return 0;
}


int sf_conf_range(xmlNodePtr node, sf_conf_range_t **rangeptr)
{
    char *name;
    sf_conf_range_t *range;

    range = sf_malloc(sizeof(sf_conf_range_t));

    sf_load_attr_string_default(node, "url_white", range->url_white, NULL);
    sf_load_attr_string_default(node, "url_black", range->url_black, NULL);
    sf_load_attr_string_default(node, "url_rewrite", range->url_rewrite, NULL);
    if (range->url_white)
    {
        range->url_white_re = sf_pcre_compile(range->url_white);

        if (!range->url_white_re)
        {
            sf_log(LOGMOD_CONF, LOG_ERR, "Cannot compile range: %s", range->url_white);
            sf_free(range);
            return -1;
        }
    }
    if (range->url_black)
    {
        range->url_black_re = sf_pcre_compile(range->url_black);

        if (!range->url_black_re)
        {
            sf_log(LOGMOD_CONF, LOG_ERR, "Cannot compile range: %s", range->url_black);
            sf_free(range);
            return -1;
        }
    }
    
    sf_conf_insert_tail(range, *rangeptr);

}

int sf_conf_headers(xmlNodePtr node, sf_conf_headers_t **headersptr)
{
    char *name;
    sf_conf_headers_t *headers;

    node = node->children;
    for(node; node != NULL; node=node->next)
    {
        name=(char*)(node->name);
        sf_log(LOGMOD_CONF, LOG_DEBUG, "var name %s\n", name);

        if (!compare_name(name, NAME_HEADER))
        {
            char *hdrv=NULL;
            sf_load_attr_string_default(node, "value", hdrv, NULL);
            if ( hdrv)
            {
                sf_conf_headers_t *hdr= sf_calloc(sizeof(sf_conf_headers_t));
                hdr->hdr_value = hdrv;
                DualLinkAppend((void**)headersptr,(void *)hdr);
            }
        }
    }

}

static int sf_conf_vars_include(char *fn, sf_spider_t *s)
{
    xmlDocPtr doc;           //定义解析文档指针
    xmlNodePtr rootNode;      //定义结点指针(你需要它为了在各个结点间移动)
    xmlNodePtr node;
    xmlNodePtr curfieldNode;

    doc = xmlReadFile(fn,"UTF-8",XML_PARSE_RECOVER|XML_PARSE_NOBLANKS); //解析文件
    if (NULL == doc) {
        sf_log(LOGMOD_CONF, LOG_ERR, "var include file dose not parse successfully\n");
        return -1;
    }

    rootNode = xmlDocGetRootElement(doc); //确定文档根元素
    if (NULL == rootNode) {
        sf_log(LOGMOD_CONF, LOG_ERR, "var include file has empty document\n");
        xmlFreeDoc(doc);
        return 0;
    }

    return sf_conf_vars(rootNode, s);


}


int sf_conf_vars(xmlNodePtr node, sf_spider_t *s)
{
    sf_vartable_t *vh = s->var_handle;

    char *name;

    sf_load_attr_string_default(node, "include" , name, NULL);
    if (name) /* 存在外部变量 */
    {
        sf_conf_vars_include(name, s);
    }

    node = node->children;
    for(node; node != NULL; node=node->next)
    {
        name=(char*)(node->name);
        sf_log(LOGMOD_CONF, LOG_DEBUG, "var name %s\n", name);

        if (!compare_name(name, NAME_VAR))
        {
            char *varn=NULL, *varv=NULL;
            sf_load_attr_string_default(node, "name", varn, NULL);
            sf_load_attr_string_default(node, "value", varv, NULL);
            if (varn && varv)
            {
                sf_vartable_t *v= sf_var_create(varn, varv, s->lifetime_pool);
                sf_var_insert(&s->var_handle, v);
            }
            sf_free(varn);
            sf_free(varv);
        }

    }

    return 0;
}


int sf_conf_init(sf_spider_t *s, char *conf_fn)
{
    xmlDocPtr doc;           //定义解析文档指针
    xmlNodePtr rootNode;      //定义结点指针(你需要它为了在各个结点间移动)
    xmlNodePtr node;
    xmlNodePtr curfieldNode;
    xmlChar *szAttr;          //临时字符串变量
    sf_pcre_t *pcre;
    sf_conf_t *conf = &s->conf;

    doc = xmlReadFile(conf_fn,"UTF-8",XML_PARSE_RECOVER|XML_PARSE_NOBLANKS); //解析文件
    if (NULL == doc) {
        sf_log(LOGMOD_CONF, LOG_ERR, "spiderframe configuration dose not parse successfully\n");
        return -1;
    }

    rootNode = xmlDocGetRootElement(doc); //确定文档根元素
    if (NULL == rootNode) {
        sf_log(LOGMOD_CONF, LOG_ERR, "spiderframe configuration has empty document\n");
        xmlFreeDoc(doc);
        return -1;
    }

    sf_load_attr_int_default(rootNode,  "url_count", conf->url_count, 1);
    sf_load_attr_int_default(rootNode,  "url_level", conf->url_level, 1);
    sf_load_attr_int_default(rootNode,  "interval",  conf->interval, 0);
    sf_load_attr_string_default(rootNode, "method" , conf->method, "GET");
    sf_load_attr_string_default(rootNode, "post_data", conf->post_data, "");
    sf_load_attr_string_default(rootNode, "user_agent", conf->user_agent, IE_UA);
    sf_load_attr_string_default(rootNode, "url_suffix", conf->url_suffix, NULL);
    sf_load_attr_int_default(rootNode, "url_feedback", conf->url_feedback, 0);



    for(node = rootNode->children; node != NULL; node=node->next)
    {
        char *name, *value;
        name=(char*)(node->name);
        sf_log(LOGMOD_PROC, LOG_DEBUG, "name = %s\n", name);
        if (!compare_name(name, NAME_POLICY)) /* 如果是policy节点，则读入policy */
        {
            sf_conf_policy(node->children, &conf->policy);
        } else if (!compare_name(name, NAME_RANGE))
        {
            sf_conf_range(node, &conf->range);
        } else if (!compare_name(name, NAME_VARS))  /* 预置变量 */
        {
            sf_conf_vars(node, s);
        } else if (!compare_name(name, NAME_HEADERS))
        {
            sf_conf_headers(node, &conf->headers);
        }

    }

    sf_log(LOGMOD_CONF, LOG_DEBUG, "init conf finished\n");
    return 0;
}
