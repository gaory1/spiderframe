#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/shm.h>

#include <pcre.h>

char            *pattern = NULL;
char            *file_start;
char            *file_end;

void get_pattern(char *file)
{
    FILE *fp;
    size_t n = 0;
    ssize_t len;
    
    fp = fopen(file, "r");
    if (!fp)
    {
        perror("fopen");
        exit(1);
    }

    len = getline(&pattern, &n, fp);
    if (len <= 0)
    {
        perror("getline");
        exit(1);
    }

    if (pattern[len - 1] == '\n')
    {
        pattern[len - 1] = '\0';
    }
    if (len > 2 && pattern[len - 2] == '\r')
    {
        pattern[n - 2] = '\0';
    }
    
    fclose(fp);
    return;
}


void load_text(char *filename)
{
    size_t file_size;
    struct stat st;
    int fd;
    
    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        perror("fstat");
        exit(1);
    }

    file_size = st.st_size;
    
    file_start = mmap(NULL, (file_size + 4095) / 4096 * 4096, PROT_READ, MAP_PRIVATE|MAP_NORESERVE, fd, 0);
    if (file_start == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    file_end = file_start + file_size;

    return;
}

int main(int argc, char *argv[])
{
    pcre *re;
    int erroffset;
    const char *error;
    int str_cnt;
    int vector[120];
    char *file_pos;
    char *outstr;
    int i;
    int str_len;
    char *str_end;
    int match;
    
    if (argc < 3)
    {
        fprintf(stderr, "usage: %s <pattern file> <text file>\n", argv[0]);
        return 1;
    }
    
    get_pattern(argv[1]);
    load_text(argv[2]);
    
    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (NULL == re)
    {
        fprintf(stderr, "pcre_compile failed at '%s', reason '%s'\n", &pattern[erroffset], error);
        return 1;
    }

    file_pos = file_start;
    match = 1;
    while (file_pos < file_end)
    {
        str_cnt = pcre_exec(re, NULL, file_pos, file_end - file_pos, 0, 0, vector, sizeof(vector)/sizeof(vector[0]));
        if (str_cnt == PCRE_ERROR_NOMATCH)
        {
            if (match == 1)
            {
                fprintf(stderr, "no match\n");
            }
            break;
        }
        if (str_cnt < 0)
        {
            fprintf(stderr, "pcre_exec returned %d\n", str_cnt);
            return 0;
        }

        printf("match %d\n", match);
        for (i = 0; i < str_cnt; i++)
        {
            str_len = pcre_get_substring(file_pos, vector, str_cnt, i, (const char **)&outstr);
            if (str_len < 0)
            {
                fprintf(stderr, "pcre_get_substring returned %d\n", str_len);
                continue;
            }
            printf("    group %d: %s\n", i, outstr);
            pcre_free_substring(outstr);
        }
        file_pos += vector[1]; //vector[1] is the end offset of the whole match
        match++;
    }
}

