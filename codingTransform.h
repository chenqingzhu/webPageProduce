#ifndef CODINGTRANSFORM_H
#define CODINGTRANSFORM_H
#include <iconv.h> //用于编码转换
//代码转换:从一种编码转为另一种编码
static int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset,from_charset);
    if (cd==0) return -1;
    memset(outbuf,0,outlen);
    //if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
    iconv(cd,pin,(size_t*)&inlen,pout,(size_t*)&outlen);
    iconv_close(cd);
    return 0;
}
//UNICODE码转为GB2312码
static int u22g(char *inbuf,int inlen,char *outbuf,int outlen)
{
    char utf_8_str[10] = "utf-8";
    char gb2312_str[10] = "gb2312";
    return code_convert(utf_8_str,gb2312_str,inbuf,inlen,outbuf,outlen);
}
//GB2312码转为UNICODE码
static int g22u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
    char utf_8_str[10] = "utf-8";
    char gb2312_str[10] = "gb2312";
    return code_convert(gb2312_str,utf_8_str,inbuf,inlen,outbuf,outlen);
}

#endif // CODINGTRANSFORM_H
