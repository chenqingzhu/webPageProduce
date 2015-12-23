//
//  getWebPageClass.h
//  spiter
//  获取网页信息，两个接口：从url获取，从html文件获取
//  Created by chenqingzhu on 15/11/25.
//  Copyright (c) 2015年 graduate. All rights reserved.
//

#ifndef spiter_getWebPageClass_h
#define spiter_getWebPageClass_h
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stack>
#include <map>
#include <vector>
#include <cctype>
#include <iconv.h>

#include "codingTransform.h"

using namespace std;
class getWebPageClass{
public:
    getWebPageClass()
    {
        web_page_title = "";
        protocol = "";
        domain = "";
        path = "";
        page_url = "";
    }
    ~getWebPageClass(){}
    
    string page_url;
    string protocol;
    string domain;
    string path;
    string web_page_title;
    
    int parse_url(string url)
    {
        size_t posFound,posStart;
        //获取协议类型
        posFound = url.find("://");
        if(posFound == string::npos || posFound == 0)
        {
            cout<<"协议解析错误"<<endl;
            return -1;
        }
        
        protocol = url.substr(0,posFound);
        
        //获取服务器域名地址
        
        posStart = posFound+3;
        
        if((protocol.size()+3) >= url.size())
        {
            cout<<"服务器域名解析错误"<<endl;
            return -1;
        }
        
        posFound = url.find('/',posStart);
        
        //若出现http:///的形式则返回
        if(posFound == posStart)
        {
            cout<<"服务器域名解析错误"<<endl;
            return -1;
        }
        //若地址中只包含域名，则直接取其后的所有字符串为域名地址
        else if(posFound == string::npos)
        {
            domain = url.substr(posStart,url.size()-posStart);
        }
        else
        {
            domain = url.substr(posStart,posFound - posStart);
        }
        
        //获取资源路径
        
        posStart = posFound;
        
        //若无资源路径，则默认使用根路径“/”
        if( (protocol.size()+3+domain.size()) >= url.size())
        {
            path = string("/");
        }
        else
        {
            size_t pathlen = protocol.size()+3+domain.size(); //此处正是“/”的位置
            path = url.substr(pathlen,url.size()-pathlen);
        }
        
        cout<<"the url is: "<<url<<" \nthe protocol is: "<<protocol<<endl;
        cout<<"the domain is: "<<domain<<" \nthe path is "<<path<<endl;
        return 0;
    }
    
    static void signal_alm(int sig)
    {
        cout<<"get_html_content_by_url 超时"<<endl;
        return ;
    }
    
    string get_html_content_by_url(string url)
    {
        //分解URL，获取协议，域名和资源路径
        cout<<"1\n";
        page_url = url;
        
        parse_url(url);
        
        int sockfd;
        struct sockaddr_in addr;
        struct hostent* ht;
        struct in_addr inaddr;
        ht = gethostbyname(domain.c_str());
        
        cout<<ht->h_name<<endl;
        char** pp;
        for(pp = ht->h_aliases;*pp != NULL; pp++)
            cout<<"ht_h_aliases: "<<*pp<<endl;
        for(pp = ht->h_addr_list;*pp != NULL;pp++)
        {
            inaddr.s_addr = ((struct in_addr*)*pp)->s_addr;
            cout<<"address: "<<inet_ntoa(inaddr)<<endl;
        }
        memcpy((char*)&addr.sin_addr.s_addr,(char*)ht->h_addr_list[0],ht->h_length);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(80);
        sockfd = socket(AF_INET,SOCK_STREAM,0);
        
        struct timeval timeout = {3,0};
        //设置发送超时
        setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout, sizeof(struct timeval));
        //设置接收超时
        setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout, sizeof(struct timeval));

        
        if(connect(sockfd,(struct sockaddr*)&addr,sizeof(struct sockaddr)) == 0)
        {
            cout<<"Connect OK..."<<endl;
        }
        string request("");
        //发送HTTP请求头部
        request += "GET "+path +" HTTP/1.0\r\n";        //使用GET方式请求该域名的path路径，采用HTTP1.0版本
        request += "Accept: */*\r\n";                           //Accept中是客户端能够处理的文档类型
        request += "Accept-Language: zh-CN\r\n";        //支持的语言类型
        //客户端软件的名称及版本说明
        request += "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:33.0) Gecko/20100101 Firefox/33.0\r\n";
        request += "Host: "+domain+" \r\n";
        //request +="Accept-Encoding: gzip,deflate\r\n";
        request += "Connection: close \r\n\r\n";
        
        //发送请求报文
        int nw = write(sockfd,request.c_str(),request.size());
        cout<<"write len: "<<nw<<endl;
        
        //接收消息
        string content("");
        char buf[1000000];
        int totallen = 0;
        int loop = 2;
        bool requestMsg = true;
        signal(SIGALRM, signal_alm);
       // alarm(15);
        while(1)
        {
            nw = read(sockfd,buf,1000000);
            if(nw == -1)
            {
                cout<<"socket read error:"<<strerror(errno)<<endl;
                break;
            }
            if(nw == 0)
                break;
            buf[nw] = '\0';
            //cout<<buf<<endl;
            cout<<"------------------------------------------------------------------------\n";
            content += buf;
            if(requestMsg)
            {
                content += "split_request_and_content\n";
                requestMsg =false;
            }
            totallen += nw;
            cout<<"totalLen:"<<totallen<<endl;
            //sleep(1);
        }
       // alarm(0);
        close(sockfd);
        //获取报文反馈消息是否成功  成功为200 字段 //如果不是200 则请求错误
        if(content.size() <= 0)
        {
            return "";
        }
        string requestType = content.substr(9,3);
        if(requestType != "200")
        {
            cout<< "request error with type: "<<requestType<<endl;
            return "";
        }
        cout<<"request successful with type:"<<requestType<<endl;
        cout<<"page content totallen: "<< totallen <<endl;
        
        //寻找网页编码格式：content="text/html; charset=gb2312"
        int charsetIndex = content.find("charset");
        string codingType("");
        if(charsetIndex == string::npos)
        {
            cout<<"cannot  get the charset(编码格式)\n";
        }
        else
        {
            int i=charsetIndex+7;
            while(content[i] != '=')
                i++;
            i++;
            while(content[i]!='>' && content[i]!='/' && content[i]!=' ' &&content[i]!='\n')
            {
                codingType += content[i];
                i++;
            }
            if(codingType[0] == '"')
                codingType.erase(codingType.begin());
            codingType.erase(codingType.begin() + codingType.size() -1);
            cout<<"codingType:"<<codingType<<endl;
        }
        /*当抓取网页的编码格式不是utf8，content内容编码格式为网页编码格式，
         此时将网页的编码格式转化为系统默认格式UTF-8,
         获取保存为utf_8格式的utf_8_content内容，
         这样在数据分析的时候和本地显示的时候才不会出错。
         网页编码格式为gb2312： url("http://sports.sina.com.cn/nba/2015-04-24/12277585850.shtml");
         网页编码格式为utf-8:  url1("http://sports.sina.com.cn/");
         */
        string utf_8_content;
        if(codingType == "UTF-8" || codingType == "utf-8")
        {
            cout<<"codingType is utf-8\n";
            utf_8_content = content;
        }
        else if(codingType == "gb2312" || codingType == "GBK" ||  codingType == "GB2312" || codingType == "gbk" )
        {
            cout<<"codingType is gb2312\n";
            char utf_8_buf[1000000];
            char tmpbuf[1000000];
            memcpy(tmpbuf,content.c_str(),content.size());
            //gb2312码转为unicode码
            g22u(tmpbuf,strlen(tmpbuf),utf_8_buf,1000000);
            utf_8_content = utf_8_buf;
        }
        else
        {
            cout<<"codingType is NULL or unknown\n";
            utf_8_content = content;
            //int titlestart = utf_8_content.find("<title>");
            //int titleend = utf_8_content.find("</title>");
            //string title =  utf_8_content.substr(titlestart+7,titleend- titlestart-7);
            //web_page_title = title;
            //cout<<"totallen: "<<totallen<<endl;
            //cout<< "title : "<<title<<endl;
            //cout<<"codingType:  "<<codingType<<endl;
            
            //int  htmlIndex_t = utf_8_content.find("<html");
            //utf_8_content.erase(utf_8_content.begin(),utf_8_content.begin() + htmlIndex_t );
            
            fstream  ifile;
            ifile.open("/Users/pc/get_html_page.html",ios_base::out|ios::out );
            
            if((char*) &ifile == NULL)
            {
                cout<< "open html file error: "<< strerror(errno)<<endl;
                return "";
            }
            
            ifile << utf_8_content;
            ifile.close();
            cout<< "save  html file ok"<<endl;
            return "";
        }
        //cout<<"--------------------pp-------\n"<<utf_8_content.substr(1,1000);
        
        int findPostIndex = 0;
        int tmpIndex;
        while(1)
        {
            tmpIndex = utf_8_content.find("TITLE");
            if(tmpIndex == string::npos)
            {
                break;
            }
            utf_8_content.replace(tmpIndex, 5, "title");
            findPostIndex = tmpIndex +3;
        }
        
        int titlestart = utf_8_content.find("<title>");
        int titleend = utf_8_content.find("</title>");
        cout<< "titlestart: "<< titlestart<<endl;
        cout<<" titleend: "<< titleend<<endl;
        string title =  utf_8_content.substr(titlestart+7,titleend- titlestart-7);
        cout<<"totallen: "<<totallen<<endl;
        cout<< "title : "<<title<<endl;
        cout<<"codingType:  "<<codingType<<endl;
        web_page_title = title;
        
        //cout<<"++++++++++++++++++++++++++++++++\n"<<utf_8_content<<endl;
        /*
        int  htmlIndex_t = utf_8_content.find("html");
        utf_8_content.erase(utf_8_content.begin(),utf_8_content.begin() + htmlIndex_t );
        utf_8_content = "<" + utf_8_content;
        */
        int htmlIndex = utf_8_content.find("split_request_and_content");
        //cout<<"contentsize: "<<content.size()<<endl;
        //cout<<"htmlIndex:"<< htmlIndex<<endl;
        utf_8_content.erase(utf_8_content.begin(),utf_8_content.begin() + htmlIndex + 26);
        
        fstream  ifile;
        ifile.open("/Users/pc/get_html_page.html",ios_base::out|ios::out );
        if((char*)&ifile == NULL)
        {
            cout<< "open html file error: "<< strerror(errno)<<endl;
            return "";
        }
        
        ifile << utf_8_content;
        ifile.close();
        cout<< "save  html file ok"<<endl;
        
        return utf_8_content;
    }
    
    
    //获取网页标题
    int findPageTitle(string content)
    {
        int titlestart = content.find("<title>");
        int titleend = content.find("</title>");
        if(titlestart == string::npos || titleend == string::npos)
        {
            web_page_title = string("crawler get page title error.");
            return -1;
        }
        web_page_title =  content.substr(titlestart+7,titleend- titlestart-7);
        return 0;
    }

    
    string get_web_page_by_read_XML_file(string filename)
    {
        if(filename.size() == 0)
        {
            return "";
        }
        ifstream in(filename);
        string content("");
        string lines;
        if(!in)
        {
            cout<<"open file error:"<<strerror(errno)<<endl;
            return content;
        }
        while(getline(in, lines))
        {
            content += lines + '\n';
        }
        //提取标题
        findPageTitle(content);
        return content;
        
    }
    
    
    string get_web_page_title()
    {
        return web_page_title;
    }
    
    
    //从url中获得域名
    string get_domain_from_url(const string& url)
    {
        string protocol,domain,path;
        parseUrl(url,protocol,domain,path);
        return domain;
    }
    //将从网页中提取的url规范化
    int translate_url_to_normal(string url, string tmpUrl, string &normalUrl)
    {
        if(tmpUrl.find("mailto:") != string::npos  || tmpUrl.find("javascript:") != string::npos || tmpUrl.find("#") != string::npos)
        {
            return -1;
        }
        //获得一个完整的网站
        if(tmpUrl.find("://") != string::npos)
        {
            if(get_domain_from_url(url) != get_domain_from_url(tmpUrl))
            {
                //获得站外链接
                //normalUrl = "";
                normalUrl = tmpUrl;
                return 1;
            }
            else
            {
                //获得内站链接
                normalUrl = tmpUrl;
                return 0;
            }
        }
        //查找该网站的本地链接
        else
        {
            int posTmp = 0;
            //该链接是从根目录开始的绝对路径
            if(tmpUrl[0] == '/')
            {
                //如果在原url中未找到“http://”则出错
                if((posTmp = url.find("://")) == string::npos)
                {
                    return -1;
                }
                //如果原url中无根目录标示“/”，则在原url的末尾加入该路径
                if((posTmp = url.find("/",posTmp+3)) == string::npos)
                {
                    normalUrl = url + tmpUrl;
                    return 0;
                }
                //如果原url带有根标示“/”,则在原url的第一个根标示“/”的前一个位置加入路径
                else
                {
                    normalUrl = url.substr(0,posTmp) + tmpUrl;
                    return 0;
                }
            }
            //该路径为链接至上级目录的相对路径，如href="../../intx.html"
            else if(tmpUrl[0] == '.')
            {
                //搜索路径中../的数量
                int posTmp = 0,posStart = 0,backTraceDepth = 0;
                while((posTmp = tmpUrl.find("../",posStart)) != string::npos)
                {
                    backTraceDepth++;
                    posStart = posTmp +3;
                }
                //进入原url的当前目录
                int posEnd = 0;
                posTmp = url.rfind("/");
                posEnd = posTmp-1;
                while(backTraceDepth>0)
                {
                    posTmp = url.rfind("/",posEnd);
                    if(posTmp == string::npos || url[posTmp-1] == '/' || url[posTmp+1] == '/')
                    {
                        return -1;
                    }
                    posEnd = posTmp-1;
                    backTraceDepth--;
                }
                normalUrl = url.substr(0,posEnd+2) + tmpUrl.substr(posStart);
                return 0;
            }
            //该链接为不带路径信息的当前目录中的文件，如href="link1.html"
            else
            {
                int posTmp = 0;
                //该链接为不带路径信息的当前目录中的文件，如href="link1.html"
                if((posTmp = url.rfind('/')) == string::npos)
                {
                    return -1;
                }
                //如果搜索到http://中的“/”，则在url末尾添加根目录“/”以及该文件
                if(url[posTmp-1] == '/')
                {
                    normalUrl = url +"/"+tmpUrl;
                    return 0;
                }
                else
                {
                    normalUrl = url+tmpUrl;
                    return 0;
                }
            }
        }
    }
    
    string get_url_from_web_page_by_url(string url)
    {
        string content = get_html_content_by_url(url);
        string urlStringTofile("");
        set<string> url_set;
        getUrlFromPageContent(url, content, url_set);
        for(set<string>::iterator iter = url_set.begin();iter!= url_set.end(); iter++)
        {
            if(*iter != "")
            {
                urlStringTofile += *iter +"\n";
            }
        }
        return urlStringTofile;
    }
    
    //获取url后，提取其html内容，并进行编码转换
    int getUrlFromPageContent(string pageUrl,string content,set<string> &url_set)
    {
        //href = 与“”之间允许的最大空格数
        //string urlStringTofile("");
        const   int numAllowedSpace = 2;
        string strTmp;
        int postTmp = pageUrl.find("://");
        if(postTmp == string::npos)
        {
            cout<<"网址错误"<<endl;
            return -1;
        }
        //如果url中包含资源文件，则检查是不是常见的资源文件
        if(pageUrl[pageUrl.size() -1] !='/' && (postTmp = pageUrl.substr(postTmp+3).rfind('/'))!=string::npos)
        {
            if((postTmp = pageUrl.substr(postTmp+1).rfind(".")) !=string::npos)
            {
                strTmp = pageUrl.substr(postTmp+1);
                if(strTmp.size() <=4)
                {
                    if(strTmp!=string("html") && strTmp != string("htm") && strTmp!=string("php") && strTmp!=string("jsp")
                       && strTmp!=string("aspx") && strTmp!=string("asp") && strTmp!=string("cgi"))
                    {
                        //cout<<"网页文件格式错误"<<endl;
                        return -1;
                    }
                }
            }
        }
        
        int hrefIndex,leftQuotationIndex,rightQuotationIndex;
        int posStart = 0;
        int urlCount = 0;
        while(1)
        {
            if((hrefIndex = content.find("href=",posStart)) == string::npos)
                break;
            leftQuotationIndex = content.find('\"',hrefIndex+5);
            if(leftQuotationIndex == string::npos)
            {
                break;
            }
            
            //href与“”之间的空格不能超过numAllowedSpace
            if(leftQuotationIndex - (hrefIndex+5) > numAllowedSpace)
            {
                posStart = hrefIndex +5;
                continue;
            }
            
            rightQuotationIndex = content.find('\"',leftQuotationIndex+1);
            if(rightQuotationIndex == string::npos)
                break;
            
            //href=""的情况
            if(rightQuotationIndex - leftQuotationIndex == 1)
            {
                posStart = rightQuotationIndex+1;
                continue;
            }
            
            string tmpurl;
            int urllen = rightQuotationIndex - leftQuotationIndex - 1;
            tmpurl = content.substr(leftQuotationIndex+1,urllen);
            
            //向列表中添加url
            string normalUrl;
            //处理tmpurl，将其转化为正规的http://domain/path的形式
            int r = translate_url_to_normal(pageUrl,tmpurl,normalUrl);
            if(r != -1 && normalUrl != "")
            {
                //urlStringTofile += normalUrl + "\n";
                url_set.insert(normalUrl);
                //urlList.push_back(normalUrl);
                urlCount ++;
            }
            
            posStart = rightQuotationIndex+1;
            
        }
        
        return urlCount;
        
    }

    //解析url
    int parseUrl(string url, string &protocol, string &domain, string &path)
    {
        //获取协议类型
        int httpIndex = url.find("://");
        if(httpIndex == string::npos || httpIndex == 0)
        {
            cout<<"协议解析错误\n";
            return -1;
        }
        protocol = url.substr(0,httpIndex);
        //获取服务器域名地址
        
        if(httpIndex +3 >= url.size())
        {
            cout<<"服务器域名解析错误"<<endl;
            return -1;
        }
        int domainIndex = url.find('/',httpIndex+3);
        
        //若出现http:///的形式则返回
        if(domainIndex == httpIndex+3)
        {
            cout<<"服务器域名解析错误"<<endl;
            return -1;
        }
        //若地址中只包含域名，则直接取其后的所有字符串为域名地址
        else if(domainIndex == string::npos)
        {
            domain = url.substr(httpIndex+3,url.size()-httpIndex-3);
        }
        else
        {
            domain = url.substr(httpIndex+3,domainIndex - (httpIndex+3));
        }
        
        //获取资源路径
        //若无资源路径，则默认使用根路径“/”
        if( (protocol.size()+3+domain.size()) >= url.size())
        {
            path = string("/");
            return -1;
        }
        else
        {
            size_t pathlen = protocol.size()+3+domain.size(); //此处正是“/”的位置
            path = url.substr(pathlen,url.size()-pathlen);
            
            int posIndex = path.rfind(".");
            if(posIndex == string::npos)
            {
                return -1;
            }
            else
            {
                string urlType = path.substr(posIndex+1);
                if(urlType !="html"  && urlType !="shtml" && urlType !="htm")
                {
                    return -1;
                }
            }
            
        }
        //cout<<"the url is: "<<url<<" \nthe protocol is: "<<protocol<<endl;
        //cout<<"the domain is: "<<domain<<" \nthe path is "<<path<<endl;
        return 0;
    }

    
};
#endif
