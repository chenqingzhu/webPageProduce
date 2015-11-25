#ifndef CRAWLER_H
#define CRAWLER_H
#include <iostream>
#include<unistd.h>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<string.h>
#include<netdb.h>
#include<stdio.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include<errno.h>
#include<stdlib.h>
#include <sstream>
#include <sys/stat.h>
#include <stack>
#include<list>
#include <iconv.h> //用于编码转换
#include "codingTransform.h"
#include "newthreadpool.h"
class crawler
{
public:
    crawler(unsigned int maxThreadNum,unsigned int maxUrlQueueLenght);
     ~crawler();
    static void crawlerMainLoop(string url,newThreadPool* pool);
    static int crawlerGetUrlPage(string url,string& content);
    static int crawlerGetPageTitle(string content,string& pageTitle);
    static int parseUrl(string url,string &protocol,string& domain,string& path);
    static int parse_content_by_delete_some_html_tag(string &content);
    static int crawlerGetUrlFromPageContent(string pageUrl,string content,list<string> &urlList);
    int crawlerAddUrl(string url);
    static int parse_content_to_get_txt_content(const string& url, string& pageContent);
    static int find_html_tag(const string & fileContent,string& tag,int position);
    static int crawlerAddUrl(newThreadPool *pool,const list<string>& urlList);
    static string get_domain_from_url(const string& url);
    static int translate_url_to_normal(string url,string tmpUrl,string& normalUrl);
    static  int crawler_save_page(string url,string title,string content);
    static string get_destDirectory_by_time();
    static string convert_url_to_file_name(const string url);
private:
    newThreadPool m_crawlerThreadPool;
    static bool m_crawlerIsExited;
    static string m_destDirectory;
};

#endif // CRAWLER_H
