#include "sf_include.h"


#define JSON_MAX_BUF_LEN (SF_MAX_VAR_SIZE+sizeof(size_t))



int sf_json_print_buf_callback(void *userdata, const char *s, uint32_t length)
{
    char *buf = (char*)userdata;
    if (BUF_LEN(buf) + length > JSON_MAX_BUF_LEN)
    {
        return -1;
    }

    strncpy(BUF_TAIL(buf), s, (size_t)length);
    BUF_LEN(buf)+=length;
    *BUF_TAIL(buf) = '\0';

    return 0;
}

static char json_buf[JSON_MAX_BUF_LEN+1];
static json_printer *json_print = NULL;


char* sf_json_get_print_buf()
{
    return BUF_OFFSET(json_buf);
}

void sf_json_free_print_buf()
{
    BUF_CLEAR(json_buf);
}



int sf_json_print_field(int type, const char *data, uint32_t length, int line)
{
    if (!json_print)
    {
        json_print = sf_calloc(sizeof(json_printer));
        json_print_init(json_print, sf_json_print_buf_callback, json_buf);
        BUF_CLEAR(json_buf);
    }

    sf_log(LOGMOD_JSON, LOG_DEBUG, "sf_json_print_field(%d, \"%s\", %d, line %d)\n", type, data, length, line);
    return json_print_raw(json_print, type, data, length);
}


int sf_json_print_buf(sf_vartable_t* handle, char *varexp)
{

    char *c = varexp;


    if (!json_print)
    {
        json_print = sf_calloc(sizeof(json_printer));
        json_print_init(json_print, sf_json_print_buf_callback, json_buf);
        BUF_CLEAR(json_buf);
    }

    sf_json_print_field(JSON_OBJECT_BEGIN, NULL, 0, __LINE__);

    while(*c != '\0')
    {
        if (*c=='(' && *(c+1)=='$')
        {
            char *p = c+1;
            while(*p && *p!=')')
            {
                p++;
            }
            if (*p == ')')
            {
                char *name;
                sf_vartable_t *v;

                name = strndup(c+1, p-c-1);
                v = sf_var_search(handle, name);
                free(name);
                if (v)
                {
                    size_t len = strlen(v->value);

                    sf_json_print_field(JSON_KEY, v->name+1, strlen(v->name)-1, __LINE__);
                    sf_json_print_field(JSON_STRING, v->value, strlen(v->value), __LINE__);

                    c= p + 1;
                }
                else
                {
                    c = p + 1;
                }
            }
            else
            {
                c = p;
            }
        }
        else
            c++;
    }
    sf_json_print_field(JSON_OBJECT_END, NULL, 0, __LINE__);

    return 0;

}



int sf_json_free(json_printer *p)
{
    if (!p)
        return 0;

    json_print_free(p);
    free(p);
}


