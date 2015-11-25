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
    
    string get_html_content_by_url(string url)
    {
        //分解URL，获取协议，域名和资源路径
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
        while(1)
        {
            nw = read(sockfd,buf,1000000);
            //if((nw <= 0) && (loop--==0))
            //  break;
            if(nw == -1)
            {
                cout<<"socket read error:"<<strerror(errno)<<endl;
                break;
            }
            if(nw == 0)
                break;
            buf[nw] = '\0';
            cout<<"------------------------------------------------------------------------\n";
            content += buf;
            totallen += nw;
            cout<<"totalLen:"<<totallen<<endl;
            //sleep(1);
        }
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
            int titlestart = utf_8_content.find("<title>");
            int titleend = utf_8_content.find("</title>");
            string title =  utf_8_content.substr(titlestart+7,titleend- titlestart-7);
            web_page_title = title;
            cout<<"totallen: "<<totallen<<endl;
            //cout<< "title : "<<title<<endl;
            cout<<"codingType:  "<<codingType<<endl;
            
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
        int titlestart = utf_8_content.find("<title>");
        int titleend = utf_8_content.find("</title>");
        cout<< "titlestart: "<< titlestart<<endl;
        cout<<" titleend: "<< titleend<<endl;
        string title =  utf_8_content.substr(titlestart+7,titleend- titlestart-7);
        cout<<"totallen: "<<totallen<<endl;
        cout<< "title : "<<title<<endl;
        cout<<"codingType:  "<<codingType<<endl;
        web_page_title = title;
        
        int  htmlIndex_t = utf_8_content.find("<html");
        utf_8_content.erase(utf_8_content.begin(),utf_8_content.begin() + htmlIndex_t );
        
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
        
        int i;
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
};
#endif
