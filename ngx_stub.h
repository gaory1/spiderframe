#ifndef _NGX_STUB_H_
#define _NGX_STUB_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <errno.h>

#define  NGX_OK          0
#define  NGX_ERROR      -1
#define  NGX_AGAIN      -2
#define  NGX_BUSY       -3
#define  NGX_DONE       -4
#define  NGX_DECLINED   -5
#define  NGX_ABORT      -6

#define NGX_FILE_ERROR           -1

#define NGX_ENOENT        ENOENT

#define NGX_HAVE_MD5 0
#define NGX_HAVE_LITTLE_ENDIAN 1
#define NGX_HAVE_NONALIGNED  1
#define ngx_memcpy memcpy
#define ngx_memzero bzero
#define ngx_pagesize 4096
#define ngx_errno                  errno
#define ngx_memalign(alignment, size, log)  memalign(alignment, size)
#define ngx_free          free
#define ngx_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define ngx_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))
#define NGX_ALIGNMENT   NGX_POOL_ALIGNMENT
#define ngx_log_debug1(...)
#define ngx_log_debug2(...)
#define ngx_log_error(...)
#define ngx_delete_file(name)    unlink((const char *) name)
#define ngx_close_file           close

typedef unsigned char u_char;
typedef int                      ngx_fd_t;
typedef struct ngx_pool_s        ngx_pool_t;
typedef struct ngx_chain_s       ngx_chain_t;
typedef void ngx_log_t;
typedef unsigned int ngx_uint_t;
typedef int ngx_int_t;
typedef int               ngx_err_t;

#endif

