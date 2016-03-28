#ifndef _SF_JSON_H_
#define _SF_JSON_H_


int sf_json_free(json_printer *p);
json_printer* sf_json_init(FILE *fp);
char* sf_json_get_print_buf();
void sf_json_free_print_buf();
int sf_json_print_buf(sf_vartable_t* handle, char *varexp);
int sf_json_print_field(int type, const char *data, uint32_t length, int line);

#define sf_json_flush(_fd_)
//#define sf_json_flush(_fd_) {fprintf((_fd_), "%s", sf_json_get_print_buf());fflush((_fd_));sf_json_free_print_buf();}






#endif

