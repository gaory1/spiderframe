#ifndef _SF_PROC_H_
#define _SF_PROC_H_


typedef struct sf_hash_url_s
{
    T_HashData hashdata;
    unsigned long long key;
} sf_hash_url_t;

int sf_proc_url_match_range(char *url, sf_conf_range_t* range);
HANDLE* sf_proc_init_url_entry(HANDLE hdl, char *urlfile);
int sf_proc_insert_url(char *url, int level, FILE* outfile, HANDLE hUrl);
extern int sf_temp_cnt;
char *sf_proc_temp_filename();


#endif

