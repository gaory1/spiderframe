#include "sf_include.h"


/* ���ڼ�����Ҫ�������飬���Ƕ���ÿ�������URL�ļ���¼���Ѿ���������� */
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
        /* ���û���ļ������޷���ȡ������� */
        sf_log(LOGMOD_CHK, LOG_ERR, "Cannot open checkpoint file for loading\n");
        return;
    }

    while (EOF != fscanf(fp, SF_CHKPOINT_FN, conf_fn, &line))
    {
        h = (sf_hash_conf_t*)sf_confset_search(conf_fn);

        if (h) /* ����ҵ��˽ڵ㣬���޸������ֵ ��û���򴴽�*/
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

