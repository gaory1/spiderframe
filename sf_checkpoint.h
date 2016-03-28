#ifndef _SF_CHECKPOINT_H_
#define _SF_CHECKPOINT_H_


#define SF_CHKPOINT_PATTERN  "%s|%d\n"

#define SF_CHKPOINT_FN       "check_point"

void sf_load_chkpoint();
void sf_save_chkpoint(char *conf_fn, int line);
int sf_do_checkpoint(int line);
int sf_get_checkpoint(char *conf_fn);



#endif /* #ifndef _SF_CHECKPOINT_H_ */
