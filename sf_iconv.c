#include <iconv.h>


/*代码转换:从一种编码转为另一种编码*/
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t *inlen,char *outbuf,size_t *outlen)
{
    iconv_t cd;
    int rc;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset,from_charset);
    if (cd==0)
        return -1;

    rc = iconv(cd,pin,inlen,pout,outlen);
    if (rc ==-1)
        return -1;

    *pout='\0';

    iconv_close(cd);
    return 0;
}
/*UNICODE码转为GB2312码*/
int sf_iconv_u2g(char *inbuf,size_t *inlen,char *outbuf,size_t *outlen)
{
    return code_convert("UTF-8","GB2312",inbuf,inlen,outbuf,outlen);
}
/*GB2312码转为UNICODE码*/
int sf_iconv_g2u(char *inbuf,size_t *inlen,char *outbuf,size_t *outlen)
{
    return code_convert("GB2312","UTF-8",inbuf,inlen,outbuf,outlen);
}

