#include "sf_include.h"


/* 对于检查点需要做的事情，就是对于每个输入的URL文件记录下已经处理的行数 */
void sf_load_chkpoint()
{
    FILE *fp;
    char conf_fn[255];
    int line;
    sf_hash_conf_t *h;
    sf_hash_conf_t *h_new;


    fp = fopen(SF_CHKPOINT_FN, "r");
    if (NULL == fp)
    {
        /* 如果没有文件或是无法读取，则结束 */
        sf_log(LOGMOD_CHK, LOG_ERR, "Cannot open checkpoint file for loading\n");
        return;
    }

    while (EOF != fscanf(fp, SF_CHKPOINT_FN, conf_fn, &line))
    {
        h = (sf_hash_conf_t*)sf_confset_search(conf_fn);

        if (h) /* 如果找到了节点，则修改里面的值 ，没有则创建*/
        {
            h->line = line;
        }
        else
        {
            pthread_mutex_lock(&global_pool_mutex);
            h_new = (sf_hash_conf_t*)AllocHashData(global_pool, sizeof(sf_hash_conf_t));
            pthread_mutex_unlock(&global_pool_mutex);
            h_new->s = NULL;
            h_new->conf_fn = sf_strdup(conf_fn);
            h_new->line = line;
            sf_confset_insert(h_new);
        }
    }

    fclose(fp);
}


void sf_save_chkpoint(char *conf_fn, int line)
{
    FILE *fp;

    fp =  fopen(SF_CHKPOINT_FN, "a+");
    if (fp == NULL)
    {
        sf_log(LOGMOD_CHK, LOG_ERR, "Cannot open checkpoint file for saving\n");
        return;
    }

    fprintf(fp, SF_CHKPOINT_PATTERN, conf_fn, line);

    fclose(fp);
}

int sf_do_checkpoint(int line)
{
    if (0 == line%10)
        return 1;
    else
        return 0;
}

int sf_get_checkpoint(char *conf_fn)
{
    sf_hash_conf_t *h;
    h = (sf_hash_conf_t*)sf_confset_search(conf_fn);

    if (h)
        return h->line;
    else
        return 0;
}

