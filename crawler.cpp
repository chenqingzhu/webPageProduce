#include "crawler.h"

bool crawler::m_crawlerIsExited = false;
string crawler::m_destDirectory = "";

crawler::crawler(  unsigned int maxThreadNum,unsigned int maxUrlQueueLenght)
    :m_crawlerThreadPool(crawlerMainLoop,maxThreadNum,maxUrlQueueLenght)
{
}

crawler::~crawler()
{
   m_crawlerThreadPool.newThreadPoolDestroy();
}

//解析url
int crawler::parseUrl(string url, string &protocol, string &domain, string &path)
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

//添加url
int crawler::crawlerAddUrl(const string url)
{
    m_crawlerThreadPool.newThreadPool_addUrl(url);
    return 1;
}

//获取url对应网页的内容
int crawler::crawlerGetUrlPage(string url, string &utf_8_content)
{
   // cout<<"in mainLoop 1"<<endl;
    string protocol,domain,path;
    if(parseUrl(url,protocol,domain,path) != 0)
    {
        //cout<< "parseUrl error...\n";
        utf_8_content =  string("parseUrl error...");
        return -1;
    }
    // cout<<"in mainLoop 2"<<endl;
    //连接http服务器
    int sockfd;
    struct sockaddr_in addr;
    struct hostent* ht;
    //struct in_addr inaddr;
   // cout<<"domain:  " <<domain<<endl;
    ht = gethostbyname(domain.c_str());
    if(ht == NULL)
    {
        //cout<<"gethostbyname error: "<<strerror(errno)<<endl;
        utf_8_content = string("gethostbyname error: ") +strerror(errno);
        return -1;
    }
    // cout<<"in mainLoop 3"<<endl;
    //cout<<ht->h_name<<endl;
    /*
    char** pp;
    for(pp = ht->h_aliases;*pp != NULL; pp++)
        cout<<"ht_h_aliases: "<<*pp<<endl;
    for(pp = ht->h_addr_list;*pp != NULL;pp++)
    {
        inaddr.s_addr = ((struct in_addr*)*pp)->s_addr;
        cout<<"address: "<<inet_ntoa(inaddr)<<endl;
    }
    */
    memcpy((char*)&addr.sin_addr.s_addr,(char*)ht->h_addr_list[0],ht->h_length);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(struct sockaddr)) != 0)
    {
        //cout<<"connect error:  "<<strerror(errno)<<endl;
        utf_8_content = string("connect error: ") +strerror(errno);
        return -1;
    }
    // cout<<"in mainLoop 4"<<endl;
    //发送HTTP请求头部
    string request("");
    request += "GET "+path +" HTTP/1.0\r\n";        //使用GET方式请求该域名的path路径，采用HTTP1.0版本
    request += "Accept: */*\r\n";                           //Accept中是客户端能够处理的文档类型
    request += "Accept-Language: zh-CN\r\n";        //支持的语言类型
    //客户端软件的名称及版本说明
    request += "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:33.0) Gecko/20100101 Firefox/33.0\r\n";
    request += "Host: "+domain+" \r\n";
    //request +="Accept-Encoding: gzip,deflate\r\n";
    request += "Connection: close\r\n\r\n";

    //发送请求报文
    int nw = write(sockfd,request.c_str(),request.size());
    //cout<<"write len: "<<nw<<endl;
    if(nw <= 0)
    {
       // cout<<"send request error: "<<strerror(errno)<<endl;
        utf_8_content = string("send http request error: ") +strerror(errno);
        return -1;
    }
    // cout<<"in mainLoop 5"<<endl;
    //接收消息
    string content("");
    char buf[100000];
    int totallen = 0;
    //nw = read(sockfd,buf,1000000);
    //string headContent = buf;
    //cout<<"\n"<<headContent<<endl;

    while((nw = read(sockfd,buf,100000)))
    {
        buf[nw] = '\0';
        content += buf;
    }
    // cout<<"in mainLoop 6"<<endl;
    //获取报文反馈消息是否成功  成功为200 字段 //如果不是200 则请求错误
    string requestType = content.substr(9,3);
    if(requestType != "200")
    {
        //cout<< "request error with type: "<<requestType<<endl;
        utf_8_content = string("request error with type: ") +requestType;
        return -1;
    }
    // cout<<"in mainLoop 7"<<endl;
    //cout<<"request successful with type:"<<requestType<<endl;

      int requestheadIndex =  content.find("<!");
    if(requestheadIndex != string::npos)
    {
        content.erase(content.begin(),content.begin()+requestheadIndex);
        cout<<"erase the requestHead ok \n";
    }
    totallen = content.size();
    cout<<"page content totallen: "<< totallen <<endl;

    //寻找网页编码格式：content="text/html; charset=gb2312"
      int charsetIndex = content.find("charset");
    string codingType("");
    if(charsetIndex != string::npos)
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
        cout<<"codingType:"<<codingType<<endl;
        if(codingType[0] == '"')
            codingType.erase(codingType.begin());
        codingType.erase(codingType.begin() + codingType.size() -1);
    }
    cout<<"in mainLoop 8"<<endl;
   /*当抓取网页的编码格式不是utf8，content内容编码格式为网页编码格式，
        此时将网页的编码格式转化为系统默认格式UTF-8,
        获取保存为utf_8格式的utf_8_content内容，
        这样在数据分析的时候和本地显示的时候才不会出错。
        网页编码格式为gb2312： url("http://sports.sina.com.cn/nba/2015-04-24/12277585850.shtml");
        网页编码格式为utf-8:  url1("http://sports.sina.com.cn/");
    */
    //string utf_8_content;
    if(codingType == "UTF-8" || codingType == "utf-8")
    {
        cout<<"codingType is utf-8\n";
        utf_8_content = content;
    }
    else if(codingType == "gb2312")
    {
        cout<<"codingType is gb2312\n";
        char utf_8_buf[10000000];
        char tmpbuf[10000000];
        memcpy(tmpbuf,content.c_str(),content.size());

        //gb2312码转为unicode码
        g22u(tmpbuf,strlen(tmpbuf),utf_8_buf,10000000);
        utf_8_content = utf_8_buf;
    }
    else
    {
        //cout<<"codingType is NULL or unknown\n";
        utf_8_content = string("codingType is NULL or unknown");
        return -1;
    }
    // cout<<"in mainLoop 9"<<endl;
    return 0;
}


//获取网页标题
int crawler::crawlerGetPageTitle(string content,string& pageTitle)
{
      int titlestart = content.find("<title>");
      int titleend = content.find("</title>");
    if(titlestart == string::npos || titleend == string::npos)
    {
        pageTitle = string("crawler get page title error.");
        return -1;
    }
    pageTitle =  content.substr(titlestart+7,titleend- titlestart-7);
    return 0;
}

//线程执行函数，crawler将crawlerMainLoop传递给newThreadPool。。。
void crawler::crawlerMainLoop(string url, newThreadPool *pool)
{
    string content;
    if(crawlerGetUrlPage(url,content) != 0)
    {
        cout<<"crawler get url page error: "<<content<<endl;
        return;
    }
    string pageTitle;
    if(crawlerGetPageTitle(content,pageTitle) == -1)
    {
        cout<<pageTitle<<endl;
        pageTitle = "Can not find the title.";
    }
    cout<<"title: "<<pageTitle<<endl;

    //获取page中的url
    list<string> urlList;
    int urlCount = crawlerGetUrlFromPageContent(url,content,urlList);
    if(urlCount == -1)
    {
        return ;
    }

    cout<<url<<"::  has  url count: "<<urlCount<<endl;
    list<string>::iterator iter = urlList.begin();
    int i=0;
    for(;iter!= urlList.end();iter++)
    {
        cout<<"第 "<<i<<" 个:  "<<*iter<<endl;
        i++;
    }

    fstream  ifile;
    ifile.open("/home/xian/get_page1.html",ios_base::out|ios::out );
    /*
    if(ifile == NULL)
    {
        cout<< "open file error: "<< strerror(errno)<<endl;
        return ;
    }
     */
    ifile << content;
    ifile.close();

    //处理文本  删除不相关的节点
    if(parse_content_by_delete_some_html_tag(content) == -1)
    {
        return;
    }

    //获取文本内容
    if(parse_content_to_get_txt_content(url,content) == -1)
    {
        return ;
    }

    if(crawler_save_page(url,pageTitle,content) == -1)
    {
        return;
    }
    //cout<< content<<endl;
    cout<<" 保存文本成功\n";

    //添加提取的url到线程池quit
    crawlerAddUrl(pool,urlList);

   /*
    ifile.open("/home/xian/get_clear_page1.html",ios_base::out|ios::out );
    if(ifile == NULL)
    {
        cout<< "open file error: "<< strerror(errno)<<endl;
        return ;
    }
    else
        cout<< "open file ok"<<endl;
    ifile << content;
    ifile.close();
    //getchar();
    */
    return ;
}

//处理内容之前需要先将接收到的内容进行标准html格式处理，因为在接受数据的过程中可能会出现格式错误
int crawler::parse_content_by_delete_some_html_tag( string &content)
{
     cout<<"开始处理文本......"<<endl;
    //去除 <head ...> .........</head>内容
      int headBeginIndex = content.find("<head>");
    if(headBeginIndex == string::npos)
    {
        cout<<"The page has no header"<<endl;
        return -1;
    }
      int headEndIndex = content.find("</head>");
    if(headEndIndex == string::npos)
    {
        cout<<"The page has no header end."<<endl;
        return -1;
    }
      int nextHeadEndIndex = content.find("</head>",headEndIndex+5);
    if(nextHeadEndIndex == string::npos)
    {
        content.erase(content.begin() + headBeginIndex,content.begin() + headEndIndex+8);
    }
    else
    {
        content.erase(content.begin() + headBeginIndex,content.begin() + nextHeadEndIndex+8);
    }

    cout<<"<去除 <head ...> .........</head>内容 完成"<<endl;

      int bodyBeginIndex = content.find("<body");
    if(bodyBeginIndex == string::npos)
    {
        cout<<"The page has no body"<<endl;
        return -1;
    }
    //去除 <script ...> .........</script>内容
      int findPostIndex = bodyBeginIndex;
    while(1)
    {
          int scriptBeginIndex =  content.find("<script",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }

          int scriptEndIndex = content.find("</script>",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }

        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+9] == '\n')
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+10);
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+9);
        }
    }
    cout<<"<去除 <script ...> .........</script>内容 完成"<<endl;

    //去除 <style ...> .........</style>内容
    findPostIndex = bodyBeginIndex;
    while(1)
    {
          int scriptBeginIndex =  content.find("<style",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }
          int scriptEndIndex = content.find("</style>",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }
        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+8] == '\n')
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+9);
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
        }
    }
    cout<<"<去除 <style ...> .........</style>内容 完成"<<endl;

    //去除 <!-- .... -->内容
    findPostIndex = bodyBeginIndex;
    while(1)
    {
          int scriptBeginIndex =  content.find("<!--",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }

          int scriptEndIndex = content.find("-->",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }

        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+3] == '\n')
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+4);
        }
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+3);
        }
    }
    cout<<"<去除 <!-- .... -->内容 完成"<<endl;


    //去除 <li ...> .........</li>内容
    findPostIndex = bodyBeginIndex;
    while(1)
    {
          int scriptBeginIndex =  content.find("<li",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }

          int scriptEndIndex = content.find("</li>",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }

        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+5] == '\n')
        {
           // cout<<"li 5  等于换行符\n";
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+6);
        }
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+5);
        }
    }
    cout<<"<去除 <li ...> .........</li>内容 完成"<<endl;

    //去除 <a ...> .........</a>内容
    findPostIndex = bodyBeginIndex;
    while(1)
    {
          int scriptBeginIndex =  content.find("<a",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }

          int scriptEndIndex = content.find("</a>",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }

        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+4] == '\n')
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+5);
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+4);
        }
    }
    cout<<"<去除 <a ...> .........</a>内容 完成"<<endl;

    //去除 <ins ...> .........</ins>内容
    findPostIndex = bodyBeginIndex;
    while(1)
    {
          int scriptBeginIndex =  content.find("<ins",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }

          int scriptEndIndex = content.find("</ins>",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }

        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+6] == '\n')
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+6);
        }
    }
    cout<<"<去除 <ins ...> .........</ins>内容 完成"<<endl;

    //去除 <span ...> .........</span>内容
    // 百度的网页中有些正文是处于<span> ...</span>中，不能直接除去span标签
    /*
    findPostIndex = bodyBeginIndex;
    while(1)
    {
        int scriptBeginIndex =  content.find("<span",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }

        int scriptEndIndex = content.find("</span>",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }

        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+7] == '\n')
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
        }
    }
    cout<<"<去除 <span ...> .........</span>内容 完成"<<endl;
    */

    //去除 <form ...> .........</form>内容
    findPostIndex = bodyBeginIndex;
    while(1)
    {
          int scriptBeginIndex =  content.find("<form",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }
          int scriptEndIndex = content.find("</form>",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }
        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+7] == '\n')
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+8);
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+7);
        }
    }
    cout<<"<去除 <form ...> .........</form>内容 完成"<<endl;


    //去除 <noScript ...> .........</noScript>内容
    findPostIndex = bodyBeginIndex;
    while(1)
    {
          int scriptBeginIndex =  content.find("<noScript",findPostIndex);
        if(scriptBeginIndex == string::npos)
        {
            break;
        }
          int scriptEndIndex = content.find("</noScript>",scriptBeginIndex);
        if(scriptEndIndex == string::npos)
        {
            break;
        }
        findPostIndex= scriptBeginIndex;
        if(content[scriptEndIndex+11] == '\n')
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+12);
        else
        {
            content.erase(content.begin()+scriptBeginIndex,content.begin()+scriptEndIndex+11);
        }
    }
    cout<<"<去除 <noScript ...> .........</noScript>内容 完成"<<endl;

     //cout<<"\n\n"<<content<<"\n\n";
    /*  正文内容包含在<div ...> .........</div>中，不能直接删除div标签
     //去除 <div ...> .........</div>内容
    stack<string> divStringStack;
    stack<int> divIndexStack;
    findPostIndex = bodyBeginIndex;
    int divBeginIndex_tmp = content.find("<div",findPostIndex);
    if(divBeginIndex_tmp == string::npos)
    {
        cout<< "page has no div part\n";
        return 1;
    }

    divStringStack.push("<div");
    divIndexStack.push(divBeginIndex_tmp);
    findPostIndex = divBeginIndex_tmp+3;
    int i=0;
    while(1)
    {
        i++;
        int divBeginIndex_tmp = content.find("<div",findPostIndex);
        if(divBeginIndex_tmp == string::npos)
        {
            if(divStringStack.empty())
            {
                cout<<"last content has no div part\n";
                break;
            }
            else
            {

                int lastDivEndIndex = content.rfind("</div>");
                int tmp_div_index = divIndexStack.top();
                while(!divIndexStack.empty())
                {
                    tmp_div_index = divIndexStack.top();
                    divIndexStack.pop();
                    divStringStack.pop();
                }
                cout<<"\n清除最早一个:\n";
                cout<<content.substr(tmp_div_index,lastDivEndIndex+6)<<endl;
                cout<<"   和最后一个div标签：  "<<endl;
                cout<<"清除最早一个"<<tmp_div_index<<"   和最后一个div标签：  "<<lastDivEndIndex<<endl;

                content.erase(content.begin()+tmp_div_index,content.begin() +lastDivEndIndex+6);
                break;
            }
        }
        int divEndindex_tmp = content.find("</div>",findPostIndex);
        if(divEndindex_tmp == string::npos)
        {
            if(divStringStack.size() !=0)
            {
                cout<<"divStringStack is not empty but ";
            }
            cout<<"last content has no </div> part\n";
            break;
        }
        if(divBeginIndex_tmp < divEndindex_tmp)
        {
            divStringStack.push("<div");
            divIndexStack.push(divBeginIndex_tmp);
            findPostIndex = divBeginIndex_tmp+3;
            //continue;
        }
        else
        {
            int topindex = divIndexStack.top();
            divIndexStack.pop();
            divStringStack.pop();
            findPostIndex = topindex;
            content.erase(content.begin()+topindex,content.begin()+divEndindex_tmp + 6);
        }
        if(i%10== 0)
        {
            cout<<"处理div:  findPostIndex: "<<findPostIndex<<"   totallencontent :"<<content.size()<<endl;
        }
    }
    */
    return 0;
}


//获取url后，提取其html内容，并进行编码转换
int crawler::crawlerGetUrlFromPageContent(string pageUrl,string content, list<string> &urlList)
{
    //href = 与“”之间允许的最大空格数
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
        if(r != -1)
        {
            urlList.push_back(normalUrl);
            urlCount ++;
        }

        posStart = rightQuotationIndex+1;

    }
    return urlCount;

}

//将从网页中提取的url规范化
int crawler::translate_url_to_normal(string url, string tmpUrl, string &normalUrl)
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

//从url中获得域名
string crawler::get_domain_from_url(const string& url)
{
    string protocol,domain,path;
    parseUrl(url,protocol,domain,path);
    return domain;
}

//将从网页文本中提取的url列表加入到线程池
int crawler::crawlerAddUrl(newThreadPool *pool, const list<string> &urlList)
{
    list<string>::const_iterator iter = urlList.begin();
    int addCount=0;
    for(;iter!= urlList.end();iter++)
    {
        addCount = pool->newThreadPool_addUrl(*iter);
    }
    return addCount;
}


 //寻找网页标签1、fileContent：网页内容  2、tag：存储找到的标签 3、position：查找的起始位置
//返回1、int：标签的起始位置，即"<"的位置
int crawler::find_html_tag(const string & fileContent,string& tag,int position)
{
    int tagStart,tagEnd;
    if((tagStart = fileContent.find("<",position)) == string::npos)
    {
        return string::npos;
    }
    if((tagEnd = fileContent.find(">",tagStart+1)) == string::npos)
    {
        return string::npos;
    }

    tag = fileContent.substr(tagStart,tagEnd-tagStart+1);
    return tagStart;
}


//在去除标签后的content中去出网页文本内容
int crawler::parse_content_to_get_txt_content(const string& url, string& pageContent)
{
    string text; //存储剔除标签后的网页文本内容
    string url_ss = url;
    url_ss = "adsf";
    string fileContent = pageContent;

    string tagBefore;   //存储前一个标签
    string tagAfter;    //存储后一个标签
    size_t posTagBefore;  //存储前一个标签的起始位置，即“<”的位置
    size_t posTagAfter;   //存储后一个标签的起始位置，即“<”的位置
    size_t posStart = 0;  //每次查询标签的起始位置

    //查找第一个标签

       //若无第一个标签或只有标签则出错
    if((posTagBefore = find_html_tag(fileContent,tagBefore,posStart)) == string::npos)
    {
        return -1;
    }

    posStart = posTagBefore + tagBefore.size(); //修改查找标签的起始位置，此时正好位于第一个标签的">"之后的第一个字符

    //依次查找后一个标签，直到文件尾部的</html>
    while((posTagAfter = find_html_tag(fileContent,tagAfter,posStart))!=string::npos)
    {
        //如果两个标签之间的正文字数大于1，并且不是script或style的执行程序，则提取标签之间的正文内容
        //其中script标签之间是js的执行代码，而style之间则是文档的样式信息
        if((posTagAfter-posTagBefore-tagBefore.size() >= 1) && (tagBefore.find("<script") == string::npos)
                && (tagBefore.find("<style") == string::npos))
        {
            string textSegment = fileContent.substr(posTagBefore+tagBefore.size(),
                                                    posTagAfter - posTagBefore - tagBefore.size());

            int len =0;
            string newStr_without_space("");
            for(  int i=0;i<textSegment.size() ;i++)
            {
                if((textSegment[i]) == ' ')
                {
                    len++;
                }
                else //if(textSegment[i] != '\n')
                {
                    newStr_without_space += textSegment[i];
                }
            }
            stringstream ss;
            ss << "totalLen: "<<textSegment.size();
            ss<<"可打印："<<len<<"\n";
            if(textSegment.size() - len > 20)
                text.append(newStr_without_space).append(" \n");

            /*
            if(textSegment[i] - len >20)
            {
                if(ispunct(textSegment[textSegment.size()-1]))
                    text.append(textSegment).append(" \n");     //每段正文之间以空格分隔
                else
                   text.append(textSegment).append(" ");
            }
            */

        }
        posTagBefore = posTagAfter;
        tagBefore = tagAfter;
         //修改查找标签的起始位置，此时正好位于第一个标签的">"之后的第一个字符
        posStart = posTagBefore + tagBefore.size();
/*
        if((posTagBefore = find_html_tag(fileContent,tagBefore,posStart)) == string::npos)
        {
            return -1;
        }

        posStart = posTagBefore + tagBefore.size();
        */

    }

    if(text.size() == 0)
    {
        cout<<"网页内容剔除标签错误"<<endl;
        return -1;
    }

    pageContent = text;

    return 0;
}


//保存网页
int crawler::crawler_save_page(string url, string title,string content)
{

    if(m_destDirectory == "")
    {
        m_destDirectory = get_destDirectory_by_time();
        m_destDirectory= "/home/xian/"+m_destDirectory;

        if(mkdir(m_destDirectory.c_str(),S_IRWXG|S_IRWXO|S_IRWXU) == -1)
        {
            cout<<"目录创建失败\n";
            return -1;
        }
        cout<<m_destDirectory<<" :目录创建成功\n";
    }

    string fileName = convert_url_to_file_name(url);
    string filePath = m_destDirectory+"/"+fileName;
    fstream ifile;
    //ifile.open(filePath.c_str(),ios_base::out|ios:out);
    ifile.open(filePath.c_str(),ios_base::out|ios::out );
    /*
    if(ifile == NULL)
    {
        cout<< "write file "<<filePath<<" error: "<< strerror(errno)<<endl;
        return -1;
    }
     */
    ifile<<"URL:   "<<url<<"\n\n";
    ifile<<content;
    ifile.close();
    return 0;
}

//用时间生成文件夹名字
string  crawler::get_destDirectory_by_time()
{
    time_t  tm;
    time(&tm);
    string tmpstr = ctime(&tm);
    for(  int i=0;i<tmpstr.size();i++)
    {
        if(tmpstr[i] == ' ' || tmpstr[i] == ':')
        {
            tmpstr[i] = '_';
        }
        if(tmpstr[i] == '\n')
        {
            tmpstr.erase(tmpstr.begin()+i);
        }
    }
    return tmpstr;
}

//将url转换为文件名可用的字符串  作为文件名
 string crawler::convert_url_to_file_name(const string url)
{
    string fileName = url;

    for(size_t index = 0; index < fileName.size(); ++index)
    {
        //文件名中不能包含代表根目录的“/”，也尽量避免冒号
        if((fileName[index] == '/')||(fileName[index] == ':'))
            fileName[index] = '-';
        //文件名中应避免使用转义字符和空格
        else if((fileName[index]=='?')||(fileName[index] == '*')||(fileName[index] == ' ')||(fileName[index] == '&'))
            fileName[index] = '_';
    }
    return fileName;
}
