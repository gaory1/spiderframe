#ifndef _SF_LOG_H_
#define _SF_LOG_H_

extern FILE *pattern_file;
extern int   log_lvl_cfg;

#define log_file stdout
#define sf_log(mod, lvl, ...) if(lvl >= log_lvl_cfg){if (lvl== LOG_FILE) {fprintf(pattern_file? pattern_file: (pattern_file=fopen("pattern.txt", "w")),__VA_ARGS__ ); fflush(pattern_file);}else fprintf(log_file,__VA_ARGS__);}


enum log_mod_e {
    LOGMOD_PCRE,
    LOGMOD_MEM,
    LOGMOD_CURL,
    LOGMOD_CONF,
    LOGMOD_PROC,
    LOGMOD_JSON,
    LOGMOD_CHK
};

enum log_level_e {
    LOG_DEBUG,
    LOG_INFO,
    LOG_ERR,
    LOG_WARNING,
    LOG_FILE
};

#endif /*  define _SF_LOG_H_ */
