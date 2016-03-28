#ifndef _SF_ICONV_H_
#define _SF_ICONV_H_

int sf_iconv_u2g(char *inbuf,size_t *inlen,char *outbuf,size_t *outlen);
int sf_iconv_g2u(char *inbuf,size_t *inlen,char *outbuf,size_t *outlen);

#endif
