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
#include <iconv.h> //用于编码转换
#include "newthreadpool.h"
#include "crawler.h"
#include "kmeansCluster.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "publicPart.h"
#include "getWebPageByURLClass.h"
#include "hierarchialCluster.h"
#include "contentProcessClass.h"
#include "AGENSCluster.h"

using namespace std;
                    
void printMsg(struct lableMsg msg)
{
    cout<<"lableName: "<<msg.lableName<<"  " ;
    cout<<"lableFlag: " <<( (msg.lableFlag == 0) ? "error" : (msg.lableFlag == LEFTLABLE?  "leftLable" : "rightLable") )<<"  ";
    cout<<"beginIndex: "<<msg.beginIndex <<"  endIndex: " << msg.endIndex<<endl;
}

//标签名统一转化为小写字符
string string_uptolower(string name)
{
    for(int i=0;i<name.size();++i)
        name[i] = tolower(name[i]);
    return name;
}

lableMsg getLableMsg(string &content,int findPostIndex)
{
    int rightIndex, spaceIndex;
    int leftIndex = content.find("<",findPostIndex);
    if(leftIndex == string::npos)
    {
        struct lableMsg msg;
        //return lableMsg("");
        return msg;
    }
    if(content[leftIndex+1] == '/')  //右标签
    {
        rightIndex = content.find(">",leftIndex+1);
        if(rightIndex == string::npos)
        {
            /*
             cout<<"------------------------------------\n";
             cout<<content.size()<<endl;
             cout<<leftIndex<<endl;
             cout<<content.substr(leftIndex-10)<<endl;
             cout<<"------------------------------------\n";
             */
            content.erase(content.begin() + leftIndex,content.end());
            struct lableMsg msg;
            return msg;
        }
        
        //  return lableMsg
        
        string lableName = content.substr(leftIndex+2,rightIndex-leftIndex-2);
        struct lableMsg msg(string_uptolower(lableName),leftIndex,rightIndex,RIGHTLABLE);
        return msg;
    }
    else
    {
        rightIndex = content.find(">",leftIndex +1);
        if(rightIndex == string::npos)
        {
            /*
             cout<<"------------------------------------\n";
             cout<<content.size()<<endl;
             cout<<leftIndex<<endl;
             cout<<content.substr(leftIndex-10)<<endl;
             cout<<"------------------------------------\n";
             */
            content.erase(content.begin() + leftIndex,content.end());
            struct lableMsg msg;
            return msg;
        }
        if(content[rightIndex-1] == '/')   //单个不配对标签  eg：<input .....  />
        {
            content.erase(content.begin() + leftIndex,content.begin() + rightIndex + 1);
            return getLableMsg(content, findPostIndex);
        }
        string tmpcontent = content.substr(leftIndex+1,rightIndex - leftIndex-1);
        spaceIndex = tmpcontent.find(" ");
        string lableName;
        if(spaceIndex != string::npos)
            lableName = tmpcontent.substr(0, spaceIndex);
        else
            lableName = tmpcontent;
        struct lableMsg msg(string_uptolower(lableName),leftIndex,rightIndex,LEFTLABLE);
        return msg;
    }
}


//判断标签内容是否为空
bool  inner_content_is_empty(string str)
{
    int len = str.size();
    if(len <= 1)
    {
        return true;
    }
    
    for(int i=0; i<len ; i++)
    {
        //if(str[i] == '\r')
        //  cout<<"change line\n";
        if(str[i] ==' ' || str[i] == '\t' || str[i] == '\r' || str[i] =='\n' || str[i] =='-' || str[i] == '|'|| str[i] == '['|| str[i] == ']' || str[i] == '.' )
            continue;
        else
            return false;
    }
    return true;
}

string  get_destDirectory_by_time1()
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


int main123()
{
    crawler *myCrawler = new crawler(3,100);
    cout<<"begi３３n...\n";
    myCrawler->crawlerAddUrl("www.zhihu.com/question/36531933#answer-22595076");
    cout<<"end...\n";
    sleep(12);
    //string test("http://sports.sina.com.cn/nba/2015-04-24/12277585850.shtml");
    //int t = test.rfind(".");
    //cout<<test.substr(t+1)<<endl;
    delete myCrawler;
    return 0;
}
int xmlParser(string files){
    xmlDocPtr doc;
    xmlNodePtr curNode;
    xmlChar *key;
    doc = xmlReadFile(files.c_str(), "utf-8", 256);
    if(doc == NULL)
    {
        cout<<"xml open error!"<<endl;
        cout<<strerror(errno)<<endl;
    }
    else
        cout<<"xml open success!"<<endl;
    
    curNode = xmlDocGetRootElement(doc);
    if(curNode == NULL)
        cout<<"doc is empty"<<endl;
    else
    {
        cout<< curNode->name<<endl;
    }
    
    curNode = curNode->children;
    while(curNode != NULL)
    {
        key = xmlNodeGetContent(curNode);
        cout<<key<<endl;
        curNode = curNode->next;
        cout<<"---------------------------"<<endl;
        xmlFree(key);
    }
    cout<<"endl"<<endl;
    return 0;
}




//将标签特性转化为特征向量   <id,文本长度，左标签长度，右标签长度，标点符号个数，层次数>
vector<Feature> get_feature_vector(vector<struct lableFeature> lf)
{
    vector<Feature> ret;
    for(int i=0;i<lf.size();i++)
    {
        if(lf[i].lableName == "a" || lf[i].lableName == "input"|| lf[i].lableName == "li"|| lf[i].lableName == "ul" ||lf[i].lableContentLength <= 9)
            continue;
        Feature tmpf;
        tmpf.push_back(lf[i].lableId);
        tmpf.push_back(lf[i].lableContentLength);
        tmpf.push_back(lf[i].lableLeftPartLength);
        tmpf.push_back(lf[i].lableRightPartLength);
        tmpf.push_back(lf[i].lablePunctNumber);
        tmpf.push_back(lf[i].lableLevelNumber);
        //tmpf.push_back(lf[i].lableId/4);
        //tmpf.push_back(1);
        ret.push_back(tmpf);
    }
    return ret;
}

//打印标签特征
void print_lable_feature(struct lableFeature lf)
{
    cout<<"lableId:" << lf.lableId << "\n";
    cout<<"lableLevelNumber:" << lf.lableLevelNumber << "\n";
    cout<<"lablePunctNumber:"<<lf.lablePunctNumber<<endl;
    cout<<"lableName:"<<lf.lableName <<"\nlableContent:" << lf.lableContent ;
    cout<<"\nlableBeginIndex:"<< lf.lableBeginIndex;
    cout<<"\nlableEndIndex:"<< lf.lableEndIndex;
    cout << "\nlableLeftPartContent:" <<lf.lableLeftPartContent<<endl;
    cout<<"lableAttributeVector:";
    for(int i=0;i<lf.lableAttributeVector.size();i++)
    {
        cout<<lf.lableAttributeVector[i]<<"  ";
    }
    
    cout<<"\n ---------------------------------------\n";
}

int stringFindPunct(string str,string pat)
{
    int ret=0;
    int findPostIndex = 0;
    while(1)
    {
        int tmp = str.find(pat,findPostIndex);
        if(tmp == string::npos)
            return ret;
        ret++;
        findPostIndex = tmp+1;
    }
    return ret;
}

//获取标点符号数量
int get_punct_number(string content)
{
    if(content.size() <= 0)
        return 0;
    int ret = 0;
    //cout<<content<<endl;
    for(int i=0; i<content.size(); i++)
    {
        if(ispunct(content[i]))
        //if( (content[i][0] & 0xff) ==0xA1   &&   (content[i][1] & 0xff)==0xA3 )
        {
            //cout<<"content:"<<content[i]<< "====is punch"<<endl;
            ret++;
        }
        else{
            //cout<<"content:"<<content[i]<< "====not punch"<<endl;
        }
    }
    ret += stringFindPunct(content,"，");
    ret += stringFindPunct(content,"。");
    ret += stringFindPunct(content,"；");
    ret += stringFindPunct(content,"“");
    ret += stringFindPunct(content,"”");
    ret += stringFindPunct(content,"！");
    ret += stringFindPunct(content,"‘");
    ret += stringFindPunct(content,"’");
    ret += stringFindPunct(content,"（");
    ret += stringFindPunct(content,"）");
    ret += stringFindPunct(content,"《");
    ret += stringFindPunct(content,"》");
    ret += stringFindPunct(content,"、");
    //cout<< ret<<endl;
    return ret;
}


//获取左标签的属性值
vector<string> get_lable_attribute_vector(string content)
{
    vector<string> ret;
    int findPostIndex = 0;
    while(1)
    {
        int spaceIndex = content.find(' ',findPostIndex);
        if(spaceIndex != string::npos)
        {
            while(spaceIndex < content.size() &&  content[spaceIndex] == ' ')
                spaceIndex ++;
            if(spaceIndex >= content.size())
                return ret;
            int equalIndex = content.find('=',spaceIndex);
            if(equalIndex == string::npos)
            {
                return ret;
            }
            else
            {
                findPostIndex = equalIndex;
                ret.push_back(content.substr(spaceIndex,equalIndex - spaceIndex));
            }
        }
        else
        {
            return ret;
        }
 
    }

    return ret;
}

//获取标签特征
vector<struct lableFeature> get_content_lable_feature(string content){
    vector<struct lableFeature> ret;
    stack<struct lableMsg> lableStack;
    int findPostIndex = 0;
    int lableId = 0;
    lableMsg msg = getLableMsg(content, findPostIndex);
    while(msg.lableFlag == 2)
    {
        findPostIndex = msg.endIndex +1;
        msg = getLableMsg(content, findPostIndex);
    }
    lableStack.push(msg);
    findPostIndex = msg.endIndex+1;
    
    while(1)
    {
        msg = getLableMsg(content, findPostIndex);
        if(msg.lableFlag == 0)
        {
            cout<<"get content lable feature successful\n";
            break;
        }
        if(msg.lableFlag == LEFTLABLE) //左标签
        {
            if(lableStack.empty())
            {
                lableStack.push(msg);
            }
            else
            {
                lableStack.top().leafLableFlag = false;
                lableStack.push(msg);
            }
            findPostIndex = msg.endIndex +1;
        }
        else   //右标签
        {
            if(lableStack.empty())
            {
                findPostIndex = msg.endIndex +1;
                continue;
            }
            struct lableMsg topMsg = lableStack.top();
            if(topMsg.lableName == msg.lableName && topMsg.lableFlag == LEFTLABLE )
            {
                if(topMsg.leafLableFlag == false)
                {
                    findPostIndex = msg.endIndex + 1;
                    lableStack.pop();
                    continue;
                }
                string lableName = msg.lableName;
                /*
                if(lableName == "div")
                {
                    findPostIndex = msg.endIndex + 1;
                    lableStack.pop();
                    continue;
                }
                 */
                string lableContent = content.substr(topMsg.endIndex+1, msg.beginIndex -  topMsg.endIndex -1);
                if(inner_content_is_empty(lableContent))
                {
                    findPostIndex = msg.endIndex + 1;
                    lableStack.pop();
                }
                else
                {
                    string lableLeftPartContent = content.substr(topMsg.beginIndex,topMsg.endIndex - topMsg.beginIndex +1);
                    int lableBeginIndex = topMsg.endIndex + 1;
                    int lableEndIndex = msg.beginIndex -1;
                    // lableId ++;
                    int lableContentLength = lableContent.size();
                    int lableLeftPartLength = lableLeftPartContent.size();
                    int lableRightPartLength = msg.lableName.size() + 3;
                    int lablePunctNumber = get_punct_number(lableContent);
                    vector<string> lableAttributeVector = get_lable_attribute_vector(lableLeftPartContent);
                    int lableLevelNumber = lableStack.size();
                    struct lableFeature lf(lableName,lableContent,lableLeftPartContent,lableBeginIndex,lableEndIndex,
                                           lableId++,lableContentLength,lableLeftPartLength,lableRightPartLength,lablePunctNumber,lableAttributeVector,lableLevelNumber);
                    
                    
                    ret.push_back(lf);
                    findPostIndex = msg.endIndex + 1;
                    lableStack.pop();
                }
            }
            else
            {
                findPostIndex = msg.endIndex +1;
            }
        }
    }
    
    //for(int i=0;i<ret.size();i++)
        //print_lable_feature(ret[i]);
    
    return ret;
}


//获取标签特征-----加入包含标签
/*
 加入左标签时候，如果上一个标签和该标签之间有文本，则把该文本取出
*/
vector<struct lableFeature> get_content_lable_feature_new(string content){
    vector<struct lableFeature> ret;
    stack<struct lableMsg> lableStack;
    int findPostIndex = 0;
    int lableId = 0;
    lableMsg msg = getLableMsg(content, findPostIndex);
    while(msg.lableFlag == 2)
    {
        findPostIndex = msg.endIndex +1;
        msg = getLableMsg(content, findPostIndex);
    }
    lableStack.push(msg);
    findPostIndex = msg.endIndex+1;
    
    while(1)
    {
        int contentsize = content.size();
        msg = getLableMsg(content, findPostIndex);
        if(msg.lableFlag == 0)
        {
            cout<<"get content lable feature successful\n";
            break;
        }
        if(msg.lableFlag == LEFTLABLE) //左标签
        {
            if(lableStack.empty())
            {
                lableStack.push(msg);
            }
            else
            {
                lableStack.top().leafLableFlag = false;
                lableStack.push(msg);
            }
            findPostIndex = msg.endIndex +1;
        }
        else   //右标签
        {
            if(lableStack.empty())
            {
                content.erase(0,msg.endIndex+1);
                findPostIndex = 0;
                continue;
            }
            struct lableMsg topMsg = lableStack.top();
            if(topMsg.lableName == msg.lableName && topMsg.lableFlag == LEFTLABLE )
            {
                if(topMsg.leafLableFlag == false)
                {
                    content.erase(topMsg.beginIndex +1, msg.endIndex - topMsg.beginIndex + 1);
                    findPostIndex = topMsg.beginIndex;
                    lableStack.pop();
                    continue;
                }
                string lableName = msg.lableName;
                string lableContent = content.substr(topMsg.endIndex+1, msg.beginIndex -  topMsg.endIndex -1);
                if(inner_content_is_empty(lableContent))
                {
                    content.erase(topMsg.beginIndex +1, msg.endIndex - topMsg.beginIndex + 1);
                    findPostIndex = topMsg.beginIndex + 1;
                    lableStack.pop();
                }
                else
                {
                    string lableLeftPartContent = content.substr(topMsg.beginIndex,topMsg.endIndex - topMsg.beginIndex +1);
                    int lableBeginIndex = topMsg.endIndex + 1;
                    int lableEndIndex = msg.beginIndex -1;
                    // lableId ++;
                    int lableContentLength = lableContent.size();
                    int lableLeftPartLength = lableLeftPartContent.size();
                    int lableRightPartLength = msg.lableName.size() + 3;
                    int lablePunctNumber = get_punct_number(lableContent);
                    vector<string> lableAttributeVector = get_lable_attribute_vector(lableLeftPartContent);
                    int lableLevelNumber = lableStack.size();
                    struct lableFeature lf(lableName,lableContent,lableLeftPartContent,lableBeginIndex,lableEndIndex,
                                           lableId,lableContentLength,lableLeftPartLength,lableRightPartLength,lablePunctNumber,lableAttributeVector,lableLevelNumber);
                    //ret.push_back(lf);
                    findPostIndex = topMsg.beginIndex;
                    lableStack.pop();
                    
                    
                    if(findPostIndex - lableStack.top().endIndex >= 2 )
                    {
                        struct lableMsg tmp_topMsg = lableStack.top();
                        string lableName = tmp_topMsg.lableName;
                        string lableContent = content.substr(tmp_topMsg.endIndex+1, findPostIndex  -  tmp_topMsg.endIndex -1);
                        if(inner_content_is_empty(lableContent))
                        {
                            ret.push_back(lf);
                            
                            //lableStack.pop();
                            
                            cout<<"-------11111-----"<<endl;
                            cout<<"lable: "<< lf.lableContent<<endl;
                            //cout<<"content: "<< content.substr(0,findPostIndex)<<endl;
                            cout<<"-------11111----"<<endl;
                            content.erase(topMsg.beginIndex,msg.endIndex - topMsg.beginIndex +1);
                            findPostIndex = topMsg.beginIndex;
                            lableId++;
                            //continue;
                            if(!lableStack.empty())
                            {
                                int tmp_end = content.find("<",findPostIndex);
                                if(tmp_end == string::npos)
                                    continue;
                                string  leftpart_string = content.substr(findPostIndex, tmp_end - findPostIndex);
                                if(!inner_content_is_empty(leftpart_string))
                                {
                                    lableStack.top().leafLableFlag = true;
                                }
                            }
                        }
                        else
                        {
                            string lableLeftPartContent = content.substr(tmp_topMsg.beginIndex,tmp_topMsg.endIndex - tmp_topMsg.beginIndex +1);
                            int lableBeginIndex = topMsg.endIndex + 1;
                            int lableEndIndex = findPostIndex - 1;
                            // lableId ++;
                            int lableContentLength = lableContent.size();
                            int lableLeftPartLength = lableLeftPartContent.size();
                            int lableRightPartLength = lableName.size() + 3;
                            int lablePunctNumber = get_punct_number(lableContent);
                            vector<string> lableAttributeVector = get_lable_attribute_vector(lableLeftPartContent);
                            int lableLevelNumber = lableStack.size();
                            struct lableFeature lf1(lableName,lableContent,lableLeftPartContent,lableBeginIndex,lableEndIndex,
                                                   lableId,lableContentLength,lableLeftPartLength,lableRightPartLength,lablePunctNumber,lableAttributeVector,lableLevelNumber);
                            ret.push_back(lf1);
                            lf.lableLevelNumber += 1;
                            lableId += 2;
                            ret.push_back(lf);
                            content.erase(tmp_topMsg.endIndex + 1 , msg.endIndex - (tmp_topMsg.endIndex +1) +1);
                            tmp_topMsg.leafLableFlag = true;
                            
                            findPostIndex = tmp_topMsg.endIndex;
                            
                            cout<<"-------22222-----"<<endl;
                            cout<<"lable1: "<< lf1.lableContent<<endl;
                            cout<<"lable2: "<< lf.lableContent<<endl;
                            //cout<<"content: "<< content.substr(0,200)<<endl;
                            cout<<"-------22222----"<<endl;
                            cout<<endl;
                        }
                    }
                    else
                    {
                        ret.push_back(lf);
                        content.erase(topMsg.beginIndex,msg.endIndex - topMsg.beginIndex +1);
                        findPostIndex = topMsg.beginIndex;
                        lableId++;
                        
                        if(!lableStack.empty())
                        {
                            int tmp_end = content.find("<",findPostIndex);
                            if(tmp_end == string::npos)
                                continue;
                            string  leftpart_string = content.substr(findPostIndex, tmp_end - findPostIndex);
                            if(!inner_content_is_empty(leftpart_string))
                            {
                                lableStack.top().leafLableFlag = true;
                            }
                        }
                    }
                }
            }
            else
            {
                findPostIndex = msg.endIndex +1;
            }
        }
    }
    return ret;
}


//获取文本实际长度
int get_content_real_length(string str)
{
    int ret = 0;
    for(int i=0;i<str.size();i++)
    {
        if(str[i] ==' '|| str[i] == '\t' || str[i] == '\r' || str[i] =='\n' || str[i] == '|'|| str[i] == '['|| str[i] == ']')
            continue;
        else
            ret ++;
    }
    return ret;
}
//去除前后的标点符
string get_real_content(string str){
    int i=0;
    int len = str.size();
    for(;i<= len-1; i ++)
    {
        if(str[i] ==' ' || str[i] == '\t' || str[i] == '\r' || str[i] =='\n' || str[i] == '|'|| str[i] == '['|| str[i] == ']' || str[i] == '-')
            continue;
        else
            break;
    }
    int j=len -1;
    for(;j>i;j--)
    {
        if(str[j] ==' ' || str[j] == '\t' || str[j] == '\r' || str[j] =='\n' || str[j] == '|'|| str[j] == '['|| str[j] == ']'|| str[j] == '-')
            continue;
        else
            break;
    }
    return str.substr(i,j-i+1);
}

//获取标签特征-----加入包含标签
/*
 1、如果是左标签，当stack不为空，且将左标签和top标签之间内容不为空，则将其内提出，左标签坐标前移
 */
vector<struct lableFeature> get_content_lable_feature_1124(string content){
    vector<struct lableFeature> ret;
    stack<struct lableMsg> lableStack;
    string init_content = content;
    int findPostIndex = 0;
    int lableId = 0;
    lableMsg msg = getLableMsg(content, findPostIndex);
    while(msg.lableFlag == 2)
    {
        findPostIndex = msg.endIndex +1;
        msg = getLableMsg(content, findPostIndex);
    }
    lableStack.push(msg);
    findPostIndex = msg.endIndex+1;
    
    while(1)
    {
        int contentsize = content.size();
        msg = getLableMsg(content, findPostIndex);
        if(msg.lableFlag == 0)
        {
            cout<<"get content lable feature successful\n";
            break;
        }
        if(msg.lableFlag == LEFTLABLE) //左标签
        {
            if(lableStack.empty())
            {
                lableStack.push(msg);
            }
            else
            {
                
                string content_between_topLable_and_leftLable = content.substr(lableStack.top().endIndex+1, msg.beginIndex -lableStack.top().endIndex-1 );
                if(inner_content_is_empty(content_between_topLable_and_leftLable))
                {
                    //lableStack.top().leafLableFlag = false;
                    lableStack.push(msg);
                    findPostIndex = msg.endIndex +1;
                }
                else
                {
                    struct lableMsg topMsg = lableStack.top();
                    string lableName = topMsg.lableName;
                    //string lableContent = content_between_topLable_and_leftLable;
                    string lableContent = get_real_content(content_between_topLable_and_leftLable);
                    string lableLeftPartContent = content.substr(topMsg.beginIndex,topMsg.endIndex - topMsg.beginIndex +1);
                    int lableContentLength =get_content_real_length(lableContent);
                    int lableBeginIndex = init_content.find(lableContent);
                    int lableEndIndex = lableBeginIndex + lableContent.size() - 1;
                    int lableLeftPartLength = lableLeftPartContent.size();
                    int lableRightPartLength = msg.lableName.size() + 3;
                    int lablePunctNumber = get_punct_number(lableContent);
                    vector<string> lableAttributeVector = get_lable_attribute_vector(lableLeftPartContent);
                    int lableLevelNumber = lableStack.size();
                    struct lableFeature lf(lableName,lableContent,lableLeftPartContent,lableBeginIndex,lableEndIndex,
                                           lableId++,lableContentLength,lableLeftPartLength,lableRightPartLength,lablePunctNumber,lableAttributeVector,lableLevelNumber);
                    //ret.push_back(lf);
                    if(lableName == "i")
                    {
                        if( lableContent.find("http") == 0 && ( (lableContent.find("jpg") == lableContent.size() - 3) ||(lableContent.find("JPG") == lableContent.size() - 3 ) ) )
                            lableId --;
                        else
                            ret.push_back(lf);
                    }
                    else
                        ret.push_back(lf);
                    content.erase(content.begin() + topMsg.endIndex+1,content.begin() + msg.beginIndex);
                    msg.beginIndex -= content_between_topLable_and_leftLable.size();
                    msg.endIndex -= content_between_topLable_and_leftLable.size();
                    lableStack.push(msg);
                    findPostIndex = msg.endIndex +1;
                }
            }
        }
        else   //右标签
        {
            if(lableStack.empty())
            {
                content.erase(0,msg.endIndex+1);
                findPostIndex = 0;
                continue;
            }
            struct lableMsg topMsg = lableStack.top();
            if(topMsg.lableName == msg.lableName && topMsg.lableFlag == LEFTLABLE )
            {
                string lableName = msg.lableName;
                string lableContent = content.substr(topMsg.endIndex+1, msg.beginIndex -  topMsg.endIndex -1);
                if(inner_content_is_empty(lableContent))
                {
                    content.erase(topMsg.beginIndex, msg.endIndex - topMsg.beginIndex + 1);
                    findPostIndex = topMsg.beginIndex;
                    lableStack.pop();
                }
                else
                {
                    lableContent = get_real_content(lableContent);
                    string lableLeftPartContent = content.substr(topMsg.beginIndex,topMsg.endIndex - topMsg.beginIndex +1);
                    int lableContentLength = get_content_real_length(lableContent);
                    int lableBeginIndex = init_content.find(lableContent);
                    int lableEndIndex = lableBeginIndex + lableContent.size() - 1;
                    int lableLeftPartLength = lableLeftPartContent.size();
                    int lableRightPartLength = msg.lableName.size() + 3;
                    int lablePunctNumber = get_punct_number(lableContent);
                    vector<string> lableAttributeVector = get_lable_attribute_vector(lableLeftPartContent);
                    int lableLevelNumber = lableStack.size();
                    struct lableFeature lf(lableName,lableContent,lableLeftPartContent,lableBeginIndex,lableEndIndex,
                                           lableId++,lableContentLength,lableLeftPartLength,lableRightPartLength,lablePunctNumber,lableAttributeVector,lableLevelNumber);
                    if(lableName == "i")
                    {
                        if( lableContent.find("http") == 0 && ( (lableContent.find("jpg") == lableContent.size() - 3) ||(lableContent.find("JPG") == lableContent.size() - 3 ) ) )
                            lableId --;
                        else
                            ret.push_back(lf);
                    }
                    else
                        ret.push_back(lf);
                    //ret.push_back(lf);
                    content.erase(content.begin() + topMsg.beginIndex,content.begin() + msg.endIndex +1);
                    findPostIndex = topMsg.beginIndex;
                    lableStack.pop();
                }
            }
            else
            {
                findPostIndex = msg.endIndex +1;
            }
        }
    }
    
    //for(int i=0;i<ret.size();i++)
    //print_lable_feature(ret[i]);
    
    return ret;
}


//打印第k簇的内容
void print_content_by_kmeansCluster(vector<struct lableFeature>lf,vector< vector<int> > cluster,vector<Feature> kCenter,int k)
{
    cout<<"－－－－－－第 " << k<<" 聚类簇中心：(";
    for(int i=0;i<kCenter[k-1].size();i++)
        cout<< kCenter[k-1][i]<<", ";
    cout<<")"<<endl;
    for(int i=0;i<cluster[k-1].size();i++)
    {
        cout<<"LableId:"<<cluster[k-1][i]<< "--lableLevel:"<<lf[cluster[k-1][i]].lableLevelNumber;
        cout<<"--lableName:"<<lf[cluster[k-1][i]].lableName<<"  --->"<< lf[cluster[k-1][i]].lableContent<<endl;
    }
    cout<<"－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－"<<endl;
}


//打印正文标签信息,并返回正文
string  print_page_content_by_id(vector<struct lableFeature>lf,vector<int> id,string selectLableName)
{
    string page_text("");
    for(int i=0;i<id.size(); i++)
    {
        cout<<"LableId:"<<id[i]<< "--lableLevel:"<<lf[id[i]].lableLevelNumber;
        cout<<"--lableName:"<<lf[id[i]].lableName<<"  --->"<< lf[id[i]].lableContent<<endl;
        
        //cout<<lf[id[i]].lableContent[0]<<"=="<<lf[id[i]].lableContent[1]<<endl;
        //cout<<"content2:"<<lf[id[i]].lableContent.substr(0,2)<<endl;
        //cout<<"content4:"<<lf[id[i]].lableContent.substr(0,4)<<endl;
        //cout<<"content6:"<<lf[id[i]].lableContent.substr(0,6)<<endl;
        /*
        if(lf[id[i]].lableContent.substr(0,2) == "\343\200")
        {
            //cout<<"yest"<<endl;
            page_text += "\n";
        }
         */
        if(lf[id[i]].lableContent.substr(0,2) == "\343\200" || (selectLableName == lf[id[i]].lableName && lf[id[i]].lableContentLength > 30))
            page_text+= "\n" + lf[id[i]].lableContent;
        else if(lf[id[i]].lableName == "h"||lf[id[i]].lableName == "h1"||lf[id[i]].lableName == "h2"||lf[id[i]].lableName == "h3"||lf[id[i]].lableName == "h4"||lf[id[i]].lableName == "h5"||lf[id[i]].lableName == "h6"||lf[id[i]].lableName == "h7")
        {
            page_text += "\n\t" +  lf[id[i]].lableContent;
        }
        else if(lf[id[i]].lableName == "li")
        {
            page_text += "\n\t\t" + lf[id[i]].lableContent;
        }
        else
            page_text += "  "+lf[id[i]].lableContent;
        
        //page_text += lf[id[i]].lableContent;
    }
    return page_text;
}

//获取聚类簇中的文本长度最长的N个特征标签ID
vector<int> get_top_N_vector(vector<struct lableFeature>lf,vector<int> ic)
{
    vector<int> ret;
    map< int,vector<int> > length_map;
    int topN = 5;
    /*
    if(ic.size() > 20)
        topN = 10;
    else if (ic.size() > 15)
        topN = 8;
    else if (ic.size() > 10)
        topN = 7;
    else
        topN = 6;
    */
    for(int i=0;i<ic.size();i++)
    {
        length_map[lf[ic[i]].lableContentLength].push_back(ic[i]);
    }
    map< int,vector<int> >::reverse_iterator iter = length_map.rbegin();
    while(1){
        if(ret.size() >= topN)
            break;
        if(length_map.size()== 0)
            break;
        for(int i=0;i<iter->second.size();i++)
        {
            ret.push_back(iter->second[i]);
        }
        length_map.erase(iter->first);
        iter = length_map.rbegin();
    }
    sort(ret.begin(), ret.end());
    return ret;
}

static bool cmp_labelFeature(lableFeature f1,lableFeature f2)
{
    return f1.lableContentLength > f2.lableContentLength;
}


//标签是否是修饰文本或表格之类的修饰标签
bool if_lableName_for_statue(string lableName)
{
    return lableName =="strong" ||lableName == "tt" ||lableName == "i" ||lableName == "b" ||lableName == "big"||lableName == "small" ||lableName == "pre"||lableName == "li" ||lableName == "ul" ||lableName == "font" ||lableName == "th"||lableName == "tr"||lableName == "td";

}
//调整正文簇内lableID
//1、同层次最多  2、标签为strong或者最多lableName 3、标签lable大于平均lableLevel  4、标签不是h1/h2/h3/h4/h5/h6等
vector<int> get_page_text_cluster_id_after_produce(vector<struct lableFeature>lf,vector< vector<int> > cluster,int k,string &selectLableName)
{
    cout<<"K 簇： "<< k << endl;
    vector<int> ics = cluster[k-1];
    vector<int> ic = get_top_N_vector(lf, ics);
    for(int i=0;i<ic.size();i++)
        cout<<ic[i] <<",  ";
    cout<<endl;
    vector<int> ret;
    //if(ic.size() <= 1)
      //  return ret;
    //查询文本簇中出现次数最多的标签名,定次数最多的标签名为正文标签名
    map<string,vector<int> > lable_map;
    
    //每一个标签名中，最长content 长度
    map<string,int> lableName_with_max_contentLenght;
    //map<string,int> lable_level_map;
    string lableName = lf[ic[0]].lableName;
    for(int i=0;i<ic.size();i++)
    {
        string lableName_tmp = lf[ic[i]].lableName;
        if(lableName_tmp=="h" ||lableName_tmp=="h1" || lableName_tmp=="h2" ||lableName_tmp=="h3" ||lableName_tmp=="h4" ||lableName_tmp=="h5"||lableName_tmp=="h6" ||lableName_tmp=="h8" || lableName_tmp=="h9" || lableName_tmp=="a" )
            continue;
        lable_map[lableName_tmp].push_back(lf[ic[i]].lableLevelNumber);
        lableName_with_max_contentLenght[lableName_tmp] = max(lableName_with_max_contentLenght[lableName_tmp],lf[ic[i]].lableContentLength);
        if(lable_map[lableName].size() < lable_map[lableName_tmp].size())
            lableName = lableName_tmp;
        else if(lable_map[lableName].size() == lable_map[lableName_tmp].size())
        {
            if(lableName_with_max_contentLenght[lableName] < lableName_with_max_contentLenght[lableName_tmp])
                lableName = lableName_tmp;
        }
    }

    
    int min_id=-1,max_id =-1;
    bool flag_min = true;
    //用于确定正文在第几层
    vector<int> lableName_level_vect = lable_map[lableName];
    int text_lable_level = 0;
    int text_lable_level_num = 0;
    //正文标签每个层次对应的数量map  尽量不选择首尾
    map<int,int> lableName_level_time_map;
    bool firstFlag = true;
    for(int i=0;i<lableName_level_vect.size();i++)
    {
        lableName_level_time_map[lableName_level_vect[i]] ++;
        if(lableName_level_time_map[lableName_level_vect[i]]  > text_lable_level_num)
        {
            text_lable_level = lableName_level_vect[i];
            text_lable_level_num = lableName_level_time_map[lableName_level_vect[i]] ;
        }
        else if(lableName_level_time_map[lableName_level_vect[i]]  == text_lable_level_num && firstFlag == true &&lableName_level_vect.size() > 2)
        {
            text_lable_level = lableName_level_vect[i];
            text_lable_level_num = lableName_level_time_map[lableName_level_vect[i]] ;
            firstFlag = false;
        }
    }
    selectLableName = lableName;
    cout<<"lableName: "<< lableName<<endl;
    cout<<"text_lable_level: "<<text_lable_level<<endl;
    map<int,int>  lableLevelMap;
    cout<<"ic size: " << ic.size()<<endl;
    
    /*
    //选择收尾id作为基准，不符合预期
    for(int i=0;i<ic.size();i++)
    {
        string lable = lf[ic[i]].lableName;
        int lableLevel = lf[ic[i]].lableLevelNumber;
        if(lable == lableName && lableLevel == text_lable_level)
        {
            if(flag_min)
            {
                min_id = lf[ic[i]].lableId;
                flag_min = false;
            }
            max_id = lf[ic[i]].lableId;
        }
    }
    */
    /*
    vector<lableFeature> tmp_ic;
    for(int i=0;i<ic.size();i++)
    {
        string lable = lf[ic[i]].lableName;
        int lableLevel = lf[ic[i]].lableLevelNumber;
        int lableContentLength = lf[ic[i]].lableContentLength;
        if(lable == lableName && lableLevel == text_lable_level)
        {
            tmp_ic.push_back(lf[ic[i]]);
        }
        
        sort(tmp_ic.begin(),tmp_ic.end(),cmp_labelFeature);
    }
     */
    
    //定最长的文本的标签ID为左右起始id,排除收尾ID
    int max_length = 0;
    int middle_id = -1;
    /*
    for(int i=1;i<ic.size()-1;i++)
    {
        string lable = lf[ic[i]].lableName;
        int lableLevel = lf[ic[i]].lableLevelNumber;
        int lableContentLength = lf[ic[i]].lableContentLength;
        if(lable == lableName && lableLevel == text_lable_level && lableContentLength > max_length)
        {
            max_length = lableContentLength;
            middle_id = lf[ic[i]].lableId;
        }
    }
    */
    vector<int> tmp_ic;
    for(int i=0;i<ic.size();i++)
    {
        string lable = lf[ic[i]].lableName;
        int lableLevel = lf[ic[i]].lableLevelNumber;
        int lableContentLength = lf[ic[i]].lableContentLength;
        if(lable == lableName && lableLevel == text_lable_level)
        {
            tmp_ic.push_back(ic[i]);
        }
    }

    for(int i=0;i<ic.size(); i++)
        cout<<ic[i]<<"  ";
    cout<<endl;
    
    for(int i=0;i<tmp_ic.size(); i++)
        cout<<tmp_ic[i]<<"  ";
    cout<<endl;
 
    if(tmp_ic.size()<=2)
    {
        middle_id = lf[tmp_ic[0]].lableId;
    }
   /* else if(tmp_ic.size() == 3)
    {
        middle_id = lf[tmp_ic[1]].lableId;
    }
    */
    else
    {
        for(int i=0;i<tmp_ic.size()-1;i++)
        {
            string lable = lf[tmp_ic[i]].lableName;
            int lableLevel = lf[tmp_ic[i]].lableLevelNumber;
            int lableContentLength = lf[tmp_ic[i]].lableContentLength;
            if(lable == lableName && lableLevel == text_lable_level && lableContentLength > max_length)
            {
                max_length = lableContentLength;
                middle_id = lf[tmp_ic[i]].lableId;
            }
        }
    }
    min_id = middle_id;
    max_id = middle_id;
    
    cout << "init min_id: "<<min_id<< "    max_id:"<<max_id<<endl;

    
    int beginID = min_id;
    int endID = max_id;
    
    cout << "init beginID: "<<beginID<< "    endID:"<<endID<<endl;
    //正文标签层次
    //int lable_level = lable_level_map[lableName];
    
    //增加松弛度，最多允许未知标签加入文本簇的个数——max_num
    int max_num = 2;
    bool had_h_lable_flag = false;
    int p_lable_num = 0;
    int a_lable_num = 0;
    text_lable_level = 0;
    for(int i=beginID-1;i>=0; i--)
    {
        cout<<i<<" -->tmp_lableName: "<<lf[i].lableName<<endl;


        //cout<<"lfi name:"<<lf[i].lableName<<endl;
        if(lf[i].lableName == lableName   || if_lableName_for_statue(lf[i].lableName))
        {
            beginID = i;
            continue;
        }
        
        if(lf[i].lableLevelNumber < text_lable_level)
        {
            beginID = i + 1;
            break;
        }
        if(lf[i].lableContentLength > 100)
        {
            beginID =i;
            continue;
        }
        //add 有待验证
        if(lf[i].lableLevelNumber > text_lable_level +2 && lf[i].lableName == "p" )
        {
            //max_num --;
            beginID = i;
            continue;
        }
        if(lf[i].lableName == "h1" || lf[i].lableName == "h2" || lf[i].lableName == "h3"||
           lf[i].lableName == "h4" ||lf[i].lableName == "h5" || lf[i].lableName == "h6" ||
           lf[i].lableName == "h" )
        {
            //max_num --;
            beginID = i;
            had_h_lable_flag = true;
            continue;
        }
        
        if(lf[i].lableName == "a")
        {
            bool t_flag = false;
            for(int jj = i-1; jj >=0;jj--)
            {
                if((lf[jj].lableName == lableName && lf[jj].lableLevelNumber >= text_lable_level-1) ||  if_lableName_for_statue(lf[i].lableName))
                {
                    if( i - jj >2)
                        break;
                    t_flag = true;
                    break;
                }
            }
            if(t_flag == false)
            {
                beginID = i+1;
                break;
            }
            if(had_h_lable_flag && a_lable_num++ >=3)
            {
                beginID = i+1;
                break;
            }
            beginID = i;
            continue;
        }
        
        if(lf[i].lableName == "p")
        {
            if(p_lable_num++ >=4)
            {
                beginID = i+1;
                break;
            }
            beginID = i;
            continue;
        }
        if(lf[i].lableLevelNumber >= text_lable_level && max_num > 0)
        {
            bool t_flag = false;
            for(int jj = i-1; jj >=0;jj--)
            {
                if((lf[jj].lableName == lableName && lf[jj].lableLevelNumber >= text_lable_level-1) ||  if_lableName_for_statue(lf[i].lableName))
                {
                    if(i - jj  > 3)
                        break;
                    t_flag = true;
                    break;
                }
            }
            if(t_flag == false)
            {
                max_num--;
                continue;
            }
            
            beginID = i;
            continue;
        }
        beginID = i+1;
        break;
    }
    
    //增加松弛度，最多允许未知标签加入文本簇的个数——max_num
    max_num = 2;
    int last_end = endID;
    a_lable_num = 0;
    for(int i= endID+1; i<lf.size();i++)
    {
        if(lf[i].lableName == lableName   || if_lableName_for_statue(lf[i].lableName) )
        {
            endID = i;
            last_end = endID;
            continue;
        }
        /*
        if(lf[i].lableContentLength > 100)
        {
            endID =i;
            continue;
        }
        */
        if(lf[i].lableName == "h1" || lf[i].lableName == "h2" || lf[i].lableName == "h3"||
           lf[i].lableName == "h4" ||lf[i].lableName == "h5" || lf[i].lableName == "h6" ||
           lf[i].lableName == "h"  )
        {
            endID =i;
            continue;
            
            //endID = i-1;
            //break;
        }
        //add 有待验证
        if(lf[i].lableLevelNumber > text_lable_level +2 && lf[i].lableName == "p" )
        {
            //endID = i-1;
            //continue;
        }
        
        
        
        if(lf[i].lableName =="a")
        {
            bool flag = false;
            int jj;
            for(jj =i+1; jj<lf.size();jj ++)
            {
                if((lf[jj].lableName == lableName && lf[jj].lableLevelNumber >= text_lable_level-1) || if_lableName_for_statue(lf[i].lableName))
                {
                    if(jj - i >=3)
                        break;
                    flag = true;
                    break;
                }
            }
            if(flag == false)
            {
                
                if(a_lable_num++ >= 3)
                {
                    endID = i-1;
                    break;
                }
                
                //endID = i-1;
                //break;
                continue;
            }

            endID = i;
            continue;
        }
        
        //增加松弛度，最多允许未知标签加入文本簇的个数——max_num
        
        if(lf[i].lableLevelNumber >= text_lable_level && max_num > 0)
        {
            bool flag = false;
            int jj;
            for(jj =i+1; jj<lf.size();jj ++)
            {
                if((lf[jj].lableName == lableName && lf[jj].lableLevelNumber >= text_lable_level-1) ||  if_lableName_for_statue(lf[i].lableName))
                {
                    if(jj - i >=3)
                        break;
                    flag = true;
                    break;
                }
            }
            if(flag == false)
            {
                max_num --;
                continue;
            }
            endID = i;
            continue;
        }

        if(lf[i].lableLevelNumber < text_lable_level || lf[i].lableLevelNumber >= text_lable_level+2)
        {
            endID = i-1;
            break;
        }

        if(lf[i].lableName != lableName && lf[i].lableName != "strong" && lf[i].lableName !="p" && lf[i].lableName !="img")
        {
            endID = i-1;
            break;
        }
        endID = i-1;
        break;
    }
    cout << "init beginID: "<<beginID<< "    endID:"<<endID<<endl;
    for(int i=beginID;i<=endID;i++)
        ret.push_back(i);
    for(int i=ret.size()-1; i>=0; i--)
    {
        cout<<"lableName:"<< lableName<<"    name:"<<lf[ret[i]].lableName<<endl;
        if(lf[ret[i]].lableName == lableName)
        {
            if(ret.size()-1 -i >=2)
            {
                ret.erase(ret.begin()+i+1,ret.end());
                break;
            }
            break;
        }
    }
    
    for(int i=0;i<ret.size();i++)
    {
        if(lf[ret[i]].lableName == lableName)
        {
            if(i >=3)
            {
                ret.erase(ret.begin(),ret.begin() + i-3);
                break;
            }
            break;
        }
    }
    
    return ret;
}


//从lf中一个标签开始，上下轮询取文本簇
vector<int> get_include_lableid_by_a_lableId(vector<struct lableFeature>lf,int id)
{
    int beginID = id;
    int endID = id;
    string lableName = lf[id].lableName;
    cout << "init beginID: "<<beginID<< "    endID:"<<endID<<endl;
    //正文标签层次
    //int lable_level = lable_level_map[lableName];
    
    //增加松弛度，最多允许未知标签加入文本簇的个数——max_num
    int max_num = 2;
    bool had_h_lable_flag = false;
    int p_lable_num = 0;
    int a_lable_num = 0;
    int text_lable_level = 0;
    for(int i=beginID-1;i>=0; i--)
    {
        cout<<i<<" -->tmp_lableName: "<<lf[i].lableName<<endl;
        
        
        //cout<<"lfi name:"<<lf[i].lableName<<endl;
        if(lf[i].lableName == lableName   || if_lableName_for_statue(lf[i].lableName))
        {
            beginID = i;
            continue;
        }
        
        if(lf[i].lableLevelNumber < text_lable_level)
        {
            beginID = i + 1;
            break;
        }
        if(lf[i].lableContentLength > 100)
        {
            beginID =i;
            continue;
        }
        //add 有待验证
        if(lf[i].lableLevelNumber > text_lable_level +2 && lf[i].lableName == "p" )
        {
            //max_num --;
            beginID = i;
            continue;
        }
        if(lf[i].lableName == "h1" || lf[i].lableName == "h2" || lf[i].lableName == "h3"||
           lf[i].lableName == "h4" ||lf[i].lableName == "h5" || lf[i].lableName == "h6" ||
           lf[i].lableName == "h" )
        {
            //max_num --;
            beginID = i;
            had_h_lable_flag = true;
            continue;
        }
        
        if(lf[i].lableName == "a")
        {
            bool t_flag = false;
            for(int jj = i-1; jj >=0;jj--)
            {
                if((lf[jj].lableName == lableName && lf[jj].lableLevelNumber >= text_lable_level-1) ||  if_lableName_for_statue(lf[i].lableName))
                {
                    if( i - jj >2)
                        break;
                    t_flag = true;
                    break;
                }
            }
            if(t_flag == false)
            {
                beginID = i+1;
                break;
            }
            if(had_h_lable_flag && a_lable_num++ >=3)
            {
                beginID = i+1;
                break;
            }
            beginID = i;
            continue;
        }
        
        if(lf[i].lableName == "p")
        {
            if(p_lable_num++ >=4)
            {
                beginID = i+1;
                break;
            }
            beginID = i;
            continue;
        }
        if(lf[i].lableLevelNumber >= text_lable_level && max_num > 0)
        {
            bool t_flag = false;
            for(int jj = i-1; jj >=0;jj--)
            {
                if((lf[jj].lableName == lableName && lf[jj].lableLevelNumber >= text_lable_level-1) ||  if_lableName_for_statue(lf[i].lableName))
                {
                    if(i - jj  > 3)
                        break;
                    t_flag = true;
                    break;
                }
            }
            if(t_flag == false)
            {
                max_num--;
                continue;
            }
            
            beginID = i;
            continue;
        }
        beginID = i+1;
        break;
    }
    
    //增加松弛度，最多允许未知标签加入文本簇的个数——max_num
    max_num = 2;
    int last_end = endID;
    a_lable_num = 0;
    for(int i= endID+1; i<lf.size();i++)
    {
        if(lf[i].lableName == lableName   || if_lableName_for_statue(lf[i].lableName) )
        {
            endID = i;
            last_end = endID;
            continue;
        }
        /*
         if(lf[i].lableContentLength > 100)
         {
         endID =i;
         continue;
         }
         */
        if(lf[i].lableName == "h1" || lf[i].lableName == "h2" || lf[i].lableName == "h3"||
           lf[i].lableName == "h4" ||lf[i].lableName == "h5" || lf[i].lableName == "h6" ||
           lf[i].lableName == "h"  )
        {
            endID =i;
            continue;
            
            //endID = i-1;
            //break;
        }
        //add 有待验证
        if(lf[i].lableLevelNumber > text_lable_level +2 && lf[i].lableName == "p" )
        {
            //endID = i-1;
            //continue;
        }
        
        if(lf[i].lableName =="a" && abs(lf[i].lableLevelNumber - lf[i-1].lableLevelNumber) >=4 )
            break;
        
        if(lf[i].lableName =="a")
        {
            bool flag = false;
            int jj;
            for(jj =i+1; jj<lf.size();jj ++)
            {
                if((lf[jj].lableName == lableName && lf[jj].lableLevelNumber >= text_lable_level-1) || if_lableName_for_statue(lf[i].lableName))
                {
                    if(jj - i >=3)
                        break;
                    flag = true;
                    break;
                }
            }
            if(flag == false)
            {
                
                if(a_lable_num++ >= 3)
                {
                    endID = i-1;
                    break;
                }
                
                //endID = i-1;
                //break;
                continue;
            }
            
            endID = i;
            continue;
        }
        
        //增加松弛度，最多允许未知标签加入文本簇的个数——max_num
        
        if(lf[i].lableLevelNumber >= text_lable_level && max_num > 0)
        {
            bool flag = false;
            int jj;
            for(jj =i+1; jj<lf.size();jj ++)
            {
                if((lf[jj].lableName == lableName && lf[jj].lableLevelNumber >= text_lable_level-1) ||  if_lableName_for_statue(lf[i].lableName))
                {
                    if(jj - i >=3)
                        break;
                    flag = true;
                    break;
                }
            }
            if(flag == false)
            {
                max_num --;
                continue;
            }
            endID = i;
            continue;
        }
        
        if(lf[i].lableLevelNumber < text_lable_level || lf[i].lableLevelNumber >= text_lable_level+2)
        {
            endID = i-1;
            break;
        }
        
        if(lf[i].lableName != lableName && lf[i].lableName != "strong" && lf[i].lableName !="p" && lf[i].lableName !="img")
        {
            endID = i-1;
            break;
        }
        endID = i-1;
        break;
    }
    vector<int> ret;
    cout << "init beginID: "<<beginID<< "    endID:"<<endID<<endl;
    for(int i=beginID;i<=endID;i++)
        ret.push_back(i);
    for(int i=ret.size()-1; i>=0; i--)
    {
        cout<<"lableName:"<< lableName<<"    name:"<<lf[ret[i]].lableName<<endl;
        if(lf[ret[i]].lableName == lableName)
        {
            if(ret.size()-1 -i >=2)
            {
                ret.erase(ret.begin()+i+1,ret.end());
                break;
            }
            break;
        }
    }
    
    for(int i=0;i<ret.size();i++)
    {
        if(lf[ret[i]].lableName == lableName)
        {
            if(i >=3)
            {
                ret.erase(ret.begin(),ret.begin() + i-3);
                break;
            }
            break;
        }
    }
    return ret;
}



// 不分lableLevel  分别从提取出来的topN个标签ID开始调整文本簇ID，然后合并这些vector<int>

//调整正文簇内lableID
//1、同层次最多  2、标签为strong或者最多lableName 3、标签lable大于平均lableLevel  4、标签不是h1/h2/h3/h4/h5/h6等
vector<int> get_page_text_cluster_id_after_produce_1210(vector<struct lableFeature>lf,vector< vector<int> > cluster,int k,string &selectLableName)
{
    cout<<"K 簇： "<< k << endl;
    vector<int> ics = cluster[k-1];
    vector<int> ic = get_top_N_vector(lf, ics);
    for(int i=0;i<ic.size();i++)
        cout<<ic[i] <<",  ";
    cout<<endl;
    vector<int> ret;
    //if(ic.size() <= 1)
    //  return ret;
    //查询文本簇中出现次数最多的标签名,定次数最多的标签名为正文标签名
    map<string,vector<int> > lable_map;
    
    //每一个标签名中，最长content 长度
    map<string,int> lableName_with_max_contentLenght;
    //map<string,int> lable_level_map;
    string lableName = lf[ic[0]].lableName;
    for(int i=0;i<ic.size();i++)
    {
        string lableName_tmp = lf[ic[i]].lableName;
        if(lableName_tmp=="h" ||lableName_tmp=="h1" || lableName_tmp=="h2" ||lableName_tmp=="h3" ||lableName_tmp=="h4" ||lableName_tmp=="h5"||lableName_tmp=="h6" ||lableName_tmp=="h8" || lableName_tmp=="h9" || lableName_tmp=="a" )
            continue;
        lable_map[lableName_tmp].push_back(lf[ic[i]].lableLevelNumber);
        lableName_with_max_contentLenght[lableName_tmp] = max(lableName_with_max_contentLenght[lableName_tmp],lf[ic[i]].lableContentLength);
        if(lable_map[lableName].size() < lable_map[lableName_tmp].size())
            lableName = lableName_tmp;
        else if(lable_map[lableName].size() == lable_map[lableName_tmp].size())
        {
            if(lableName_with_max_contentLenght[lableName] < lableName_with_max_contentLenght[lableName_tmp])
                lableName = lableName_tmp;
        }
    }
    
    int min_id=-1,max_id =-1;
    bool flag_min = true;
    //用于确定正文在第几层
    vector<int> lableName_level_vect = lable_map[lableName];
    int text_lable_level = 0;
    int text_lable_level_num = 0;
    //正文标签每个层次对应的数量map  尽量不选择首尾
    map<int,int> lableName_level_time_map;
    bool firstFlag = true;
    for(int i=0;i<lableName_level_vect.size();i++)
    {
        lableName_level_time_map[lableName_level_vect[i]] ++;
        if(lableName_level_time_map[lableName_level_vect[i]]  > text_lable_level_num)
        {
            text_lable_level = lableName_level_vect[i];
            text_lable_level_num = lableName_level_time_map[lableName_level_vect[i]] ;
        }
        else if(lableName_level_time_map[lableName_level_vect[i]]  == text_lable_level_num && firstFlag == true &&lableName_level_vect.size() > 2)
        {
            text_lable_level = lableName_level_vect[i];
            text_lable_level_num = lableName_level_time_map[lableName_level_vect[i]] ;
            firstFlag = false;
        }
    }
    selectLableName = lableName;
    cout<<"lableName: "<< lableName<<endl;
    cout<<"text_lable_level: "<<text_lable_level<<endl;
    map<int,int>  lableLevelMap;
    cout<<"ic size: " << ic.size()<<endl;
    
    /*
     //选择收尾id作为基准，不符合预期
     for(int i=0;i<ic.size();i++)
     {
     string lable = lf[ic[i]].lableName;
     int lableLevel = lf[ic[i]].lableLevelNumber;
     if(lable == lableName && lableLevel == text_lable_level)
     {
     if(flag_min)
     {
     min_id = lf[ic[i]].lableId;
     flag_min = false;
     }
     max_id = lf[ic[i]].lableId;
     }
     }
     */
    /*
     vector<lableFeature> tmp_ic;
     for(int i=0;i<ic.size();i++)
     {
     string lable = lf[ic[i]].lableName;
     int lableLevel = lf[ic[i]].lableLevelNumber;
     int lableContentLength = lf[ic[i]].lableContentLength;
     if(lable == lableName && lableLevel == text_lable_level)
     {
     tmp_ic.push_back(lf[ic[i]]);
     }
     
     sort(tmp_ic.begin(),tmp_ic.end(),cmp_labelFeature);
     }
     */
    
    //定最长的文本的标签ID为左右起始id,排除收尾ID
    int max_length = 0;
    int middle_id = -1;
    /*
     for(int i=1;i<ic.size()-1;i++)
     {
     string lable = lf[ic[i]].lableName;
     int lableLevel = lf[ic[i]].lableLevelNumber;
     int lableContentLength = lf[ic[i]].lableContentLength;
     if(lable == lableName && lableLevel == text_lable_level && lableContentLength > max_length)
     {
     max_length = lableContentLength;
     middle_id = lf[ic[i]].lableId;
     }
     }
     */
    vector<int> tmp_ic;
    for(int i=0;i<ic.size();i++)
    {
        string lable = lf[ic[i]].lableName;
        int lableLevel = lf[ic[i]].lableLevelNumber;
        int lableContentLength = lf[ic[i]].lableContentLength;
        if(lable == lableName) //&& lableLevel == text_lable_level)
        {
            tmp_ic.push_back(ic[i]);
        }
    }
    
    for(int i=0;i<ic.size(); i++)
        cout<<ic[i]<<"  ";
    cout<<endl;
    
    for(int i=0;i<tmp_ic.size(); i++)
        cout<<tmp_ic[i]<<"  ";
    cout<<endl;
    /*
    if(tmp_ic.size()<=2)
    {
        middle_id = lf[tmp_ic[0]].lableId;
    }
    else
    {
        for(int i=0;i<tmp_ic.size()-1;i++)
        {
            string lable = lf[tmp_ic[i]].lableName;
            int lableLevel = lf[tmp_ic[i]].lableLevelNumber;
            int lableContentLength = lf[tmp_ic[i]].lableContentLength;
            if(lable == lableName && lableLevel == text_lable_level && lableContentLength > max_length)
            {
                max_length = lableContentLength;
                middle_id = lf[tmp_ic[i]].lableId;
            }
        }
    }
     
    min_id = middle_id;
    max_id = middle_id;
    
    cout << "init min_id: "<<min_id<< "    max_id:"<<max_id<<endl;
    */
    set<int> ret_set;
    
    vector<int> ret0 = get_include_lableid_by_a_lableId(lf, lf[tmp_ic[0]].lableId);
    //可用平均长度、id分布情况来适当筛选文本簇ID
    for(int i=0;i<ret0.size();i++)
    {
        ret_set.insert(ret0[i]);
    }
    
    vector< vector<int> > each_center_id_vect;
    vector<int> begin_end_id;
    begin_end_id.push_back(ret0[0]);
    begin_end_id.push_back(ret0[ret0.size()-1]);
    each_center_id_vect.push_back(begin_end_id);
    
    for(int i=1;i<tmp_ic.size() ;i++)
    {
        if(lf[tmp_ic[i]].lableId - lf[tmp_ic[i-1]].lableId == 1)
            continue;
        
        ret0 = get_include_lableid_by_a_lableId(lf, lf[tmp_ic[i]].lableId);
        int len_center = each_center_id_vect.size();
        if( ret0[0] -each_center_id_vect[len_center-1][1] > 1)
        {
            vector<int> begin_end_id;
            begin_end_id.push_back(ret0[0]);
            begin_end_id.push_back(ret0[ret0.size()-1]);
            each_center_id_vect.push_back(begin_end_id);
        }
        else
        {
            each_center_id_vect[len_center-1][1] = max (ret0[ret0.size() -1],each_center_id_vect[len_center-1][1]);
        }
        for(int i=0;i<ret0.size();i++)
        {
            ret_set.insert(ret0[i]);
        }
    }
    int len_center = each_center_id_vect.size();
    cout<<"-------center_len= "<<len_center<<endl;
    //当有多个文本中心时，尽量去判断非正文的中心
    //目前策略:靠近标签ID的最后10%，且和上一个正文中心相差很远的时候
    if(len_center > 1)
    {
        if(each_center_id_vect[len_center-1][0] >= lf.size()*92/100 && each_center_id_vect[len_center-1][0] - each_center_id_vect[len_center-2][1] > 12)
        {
            cout<<"删除最后一个正文中心，其ID范围为：";
            for(int i=each_center_id_vect[len_center-1][0]; i<=each_center_id_vect[len_center-1][1]; i++)
            {
                ret_set.erase(ret_set.find(i));
                cout<<i<<" ";
            }
            cout<<endl;
        }
    }
    else
    {
        if(each_center_id_vect[len_center-1][0] >= lf.size()-5 )
        {
            if( each_center_id_vect[len_center-1].size() <=3)
                ret_set.clear();
            else
            {
                int average_len =0;
                int begin = each_center_id_vect[0][0];
                int end = each_center_id_vect[0][1];
                for(int i=begin;i<= end;i++)
                {
                    average_len += lf[i].lableContentLength;
                }
                if(average_len/(end - begin + 1) < 20)
                    ret_set.clear();
            }
            if(ret_set.empty())
            {
                cout<<"清空了文本簇，其清空ID为：";
                int begin = each_center_id_vect[0][0];
                int end = each_center_id_vect[0][1];
                for(int i=begin;i<= end;i++)
                {
                    cout<<i<<" ";
                }
                cout<<"\n此网页可能不是新闻网页，网页内容为空"<<endl;
            }
            
        }
    }
    //if(each_center_id_vect[tmp_ic.size() -1][])
    //vector<int> ret;
    for(set<int>::iterator iter = ret_set.begin(); iter != ret_set.end(); iter++)
        ret.push_back(*iter);
    
    return ret;
}



int save_content2file(string file_path_name,string content)
{
    fstream cfile;
    cfile.open(file_path_name.c_str(),ios_base::out|ios::out );
    if((char*)&cfile == NULL)
    {
        cout<< "open clear file error: "<< strerror(errno)<<endl;
        return 0;
    }
    cfile << content;
    cfile.close();
    return 1;
    
}


int main_loop_url_twoSplitKmeans(string url,int kcluster = 5)
{
    //请求爬去html网页
    getWebPageClass gwp;
    string utf_8_content = gwp.get_html_content_by_url(url);
    
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/qqfile.html");
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/sohufile.html");
    string web_page_title = gwp.get_web_page_title();
    
    if(utf_8_content.size() <= 0)
    {
        cout<<"get url content error\n";
        return 0;
    }
    
    //网页html预处理
    contentProcess cp;
    cp.parse_content_avoid_over_delete(utf_8_content);
    
    cout<<"content size after process : "<<utf_8_content.size()<<endl;
    
    int ret = save_content2file("/Users/pc/get_clear_page.html", utf_8_content);
    if(ret == 1)
        cout<< "save clear  file ok"<<endl;
    else
        cout<< "save clear  file error"<<endl;
    
    //vector<struct lableFeature> lable_feature = get_content_lable_feature(utf_8_content);
    
    
    vector<struct lableFeature> lable_feature = get_content_lable_feature_1124(utf_8_content);
    for(int i=0;i<lable_feature.size();i++)
        print_lable_feature(lable_feature[i]);
    
    vector<Feature> feature_vector = get_feature_vector(lable_feature);
    
    int KC =kcluster;
    try
    {
        //kmeansCluster kmeans(feature_vector);
        //kmeans.kmeans_with_select_k_function(KC);
        //kmeans.kmeans_plus_plus_function();
        
        
        two_split_K_Means  kmeans(feature_vector);
        kmeans.twoSplitKmeansFunction(8);
        KC = kmeans.get_K();
        for(int i=1;i<=KC;i++)
            kmeans.print_kmeans_cluster(i);
        
        vector< vector<int> > lableId_cluster = kmeans.get_lableId_from_cluster();
        vector<Feature> kCenter = kmeans.get_kCenter();
        vector< vector<Feature> > cluster = kmeans.get_cluster();
        
        for(int i=1;i<=KC; i++)
        {
            print_content_by_kmeansCluster(lable_feature, lableId_cluster, kCenter,i);
        }
        
        int page_text_cluster_k = kmeans.get_page_text_cluster_k();
        
        string selectLableName;
        vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k,selectLableName);
        
        if(page_text_id_vector.size() == 0)
        {
            cout<<" 这个网页非新闻文本"<<endl;
        }
        else
        {
            for(int i=0;i<page_text_id_vector.size();i++)
            {
                cout<<page_text_id_vector[i]<<" ";
            }
            cout<<endl;
            cout<<"\n－－－－－－－－－－－－正文标签信息开始－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"标题："<<web_page_title<<endl;
            string page_text = print_page_content_by_id(lable_feature, page_text_id_vector,selectLableName);
            cout<<"－－－－－－－－－－－－正文标签信息结束－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"              新闻标题： "<<web_page_title<<endl;
            cout<<"\n" << page_text<<endl;
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n\n";
            
            string tmp("              新闻标题： ");
            page_text = tmp + web_page_title + page_text;
            ret = save_content2file("/Users/pc/get_page_text.html", page_text);
            if(ret == 1)
                cout<< "save page text  file ok"<<endl;
            else
                cout<< "save page text  file error"<<endl;
        }
    }
    catch(int e)
    {
        if(e == 1)
            cout<<"K must big than 0  or  featureVector  is empty"<<endl;
        else if(e==2)
        {
            cout<<"No need to cluter,because K is small than featureCluster number "<<endl;
            
        }
        else if(e == 3)
            cout << "this page only has less two lables,maybe it's not a new page\n"<<endl;
    }
    
    return 1;
}

int main_loop_file_twoSplitKmeans(string filepath,int kcluster = 5)
{
    //请求爬去html网页
    getWebPageClass gwp;
    string utf_8_content = gwp.get_web_page_by_read_XML_file(filepath);
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/sohufile.html");
    string web_page_title = gwp.get_web_page_title();
    
    if(utf_8_content.size() <= 0)
    {
        cout<<"get url content error\n";
        return 0;
    }
    
    //网页html预处理
    contentProcess cp;
    cp.parse_content_avoid_over_delete(utf_8_content);
    
    cout<<"content size after process : "<<utf_8_content.size()<<endl;
    
    int ret = save_content2file("/Users/pc/get_clear_page.html", utf_8_content);
    if(ret == 1)
        cout<< "save clear  file ok"<<endl;
    else
        cout<< "save clear  file error"<<endl;
    
    //vector<struct lableFeature> lable_feature = get_content_lable_feature(utf_8_content);
    vector<struct lableFeature> lable_feature = get_content_lable_feature_1124(utf_8_content);
    for(int i=0;i<lable_feature.size();i++)
        print_lable_feature(lable_feature[i]);
    
    vector<Feature> feature_vector = get_feature_vector(lable_feature);
    
    int max_kc =kcluster;
    try
    {
        //kmeansCluster kmeans(feature_vector);
        //kmeans.kmeans_with_select_k_function(max_kc);
        
        two_split_K_Means  kmeans(feature_vector);
        kmeans.twoSplitKmeansFunction(8);
        
        int KC = kmeans.get_K();
        for(int i=1;i<=KC;i++)
            kmeans.print_kmeans_cluster(i);
        
        vector< vector<int> > lableId_cluster = kmeans.get_lableId_from_cluster();
        vector<Feature> kCenter = kmeans.get_kCenter();
        vector< vector<Feature> > cluster = kmeans.get_cluster();
        
        for(int i=1;i<=KC; i++)
        {
            print_content_by_kmeansCluster(lable_feature, lableId_cluster, kCenter,i);
        }
        
        int page_text_cluster_k = kmeans.get_page_text_cluster_k();
        
        //vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k);
        
        string selectLableName;
        vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k,selectLableName);
        
        if(page_text_id_vector.size() == 0)
        {
            cout<<" 这个网页非新闻文本"<<endl;
        }
        else
        {
            for(int i=0;i<page_text_id_vector.size();i++)
            {
                cout<<page_text_id_vector[i]<<" ";
            }
            cout<<endl;
            cout<<"\n－－－－－－－－－－－－正文标签信息开始－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"标题："<<web_page_title<<endl;
            string page_text = print_page_content_by_id(lable_feature, page_text_id_vector,selectLableName);
            cout<<"－－－－－－－－－－－－正文标签信息结束－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"              新闻标题： "<<web_page_title<<endl;
            cout<<"\n" << page_text<<endl;
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n\n";
            
            string tmp("              新闻标题： ");
            page_text = tmp + web_page_title +"\n\n" + page_text;
            ret = save_content2file("/Users/pc/get_page_text.html", page_text);
            if(ret == 1)
                cout<< "save page text  file ok"<<endl;
            else
                cout<< "save page text  file error"<<endl;
        }
    }
    catch(int e)
    {
        if(e == 1)
            cout<<"K must big than 0  or  featureVector  is empty"<<endl;
        else if(e==2)
        {
            cout<<"No need to cluter,because K is small than featureCluster number "<<endl;
            
        }
        else if(e == 3)
            cout << "this page only has less two lables,maybe it's not a new page\n"<<endl;
    }
    return 1;
}

int main_loop_url_kmeans(string url,int kcluster = 5)
{
    //请求爬去html网页
    getWebPageClass gwp;
    string utf_8_content = gwp.get_html_content_by_url(url);
    
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/qqfile.html");
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/sohufile.html");
    string web_page_title = gwp.get_web_page_title();
    
    if(utf_8_content.size() <= 0)
    {
        cout<<"get url content error\n";
        return 0;
    }
    
    //网页html预处理
    contentProcess cp;
    cp.parse_content_avoid_over_delete(utf_8_content);
    
    cout<<"content size after process : "<<utf_8_content.size()<<endl;
    
    int ret = save_content2file("/Users/pc/get_clear_page.html", utf_8_content);
    if(ret == 1)
        cout<< "save clear  file ok"<<endl;
    else
        cout<< "save clear  file error"<<endl;
    
    //vector<struct lableFeature> lable_feature = get_content_lable_feature(utf_8_content);
    
    
    vector<struct lableFeature> lable_feature = get_content_lable_feature_1124(utf_8_content);
    for(int i=0;i<lable_feature.size();i++)
        print_lable_feature(lable_feature[i]);
    
    vector<Feature> feature_vector = get_feature_vector(lable_feature);
    
    int KC =kcluster;
    try
    {
        kmeansCluster kmeans(feature_vector);
        kmeans.kmeans_with_select_k_function(KC);
        //kmeans.kmeans_plus_plus_function();
        
        
        //two_split_K_Means  kmeans(feature_vector);
        //kmeans.twoSplitKmeansFunction(8);
        KC = kmeans.get_K();
        for(int i=1;i<=KC;i++)
            kmeans.print_kmeans_cluster(i);
        
        vector< vector<int> > lableId_cluster = kmeans.get_lableId_from_cluster();
        vector<Feature> kCenter = kmeans.get_kCenter();
        vector< vector<Feature> > cluster = kmeans.get_cluster();
        
        for(int i=1;i<=KC; i++)
        {
            print_content_by_kmeansCluster(lable_feature, lableId_cluster, kCenter,i);
        }
        
        int page_text_cluster_k = kmeans.get_page_text_cluster_k();
        
        //vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k);
        string selectLableName;
        vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k,selectLableName);
        
        if(page_text_id_vector.size() == 0)
        {
            cout<<" 这个网页非新闻文本"<<endl;
        }
        else
        {
            for(int i=0;i<page_text_id_vector.size();i++)
            {
                cout<<page_text_id_vector[i]<<" ";
            }
            cout<<endl;
            cout<<"\n－－－－－－－－－－－－正文标签信息开始－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"标题："<<web_page_title<<endl;
            string page_text = print_page_content_by_id(lable_feature, page_text_id_vector,selectLableName);
            cout<<"－－－－－－－－－－－－正文标签信息结束－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"              新闻标题： "<<web_page_title<<endl;
            cout<<"\n" << page_text<<endl;
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n\n";
            
            string tmp("              新闻标题： ");
            page_text = tmp + web_page_title + page_text;
            ret = save_content2file("/Users/pc/get_page_text.html", page_text);
            if(ret == 1)
                cout<< "save page text  file ok"<<endl;
            else
                cout<< "save page text  file error"<<endl;
        }
    }
    catch(int e)
    {
        if(e == 1)
            cout<<"K must big than 0  or  featureVector  is empty"<<endl;
        else if(e==2)
        {
            cout<<"No need to cluter,because K is small than featureCluster number "<<endl;
            
        }
        else if(e == 3)
            cout << "this page only has less two lables,maybe it's not a new page\n"<<endl;
    }
    
    return 1;
}

int main_loop_file_kmeans(string filepath,int kcluster = 5)
{
    //请求爬去html网页
    getWebPageClass gwp;
    string utf_8_content = gwp.get_web_page_by_read_XML_file(filepath);
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/sohufile.html");
    string web_page_title = gwp.get_web_page_title();
    
    if(utf_8_content.size() <= 0)
    {
        cout<<"get url content error\n";
        return 0;
    }
    
    //网页html预处理
    contentProcess cp;
    cp.parse_content_avoid_over_delete(utf_8_content);
    
    cout<<"content size after process : "<<utf_8_content.size()<<endl;
    
    int ret = save_content2file("/Users/pc/get_clear_page.html", utf_8_content);
    if(ret == 1)
        cout<< "save clear  file ok"<<endl;
    else
        cout<< "save clear  file error"<<endl;
    
    //vector<struct lableFeature> lable_feature = get_content_lable_feature(utf_8_content);
    vector<struct lableFeature> lable_feature = get_content_lable_feature_1124(utf_8_content);
    for(int i=0;i<lable_feature.size();i++)
        print_lable_feature(lable_feature[i]);
    
    vector<Feature> feature_vector = get_feature_vector(lable_feature);
    
    int max_kc =kcluster;
    try
    {
        kmeansCluster kmeans(feature_vector);
        //kmeans.kmeans_function();
        kmeans.kmeans_with_select_k_function(max_kc);
        int KC = kmeans.get_K();
        for(int i=1;i<=KC;i++)
            kmeans.print_kmeans_cluster(i);
        
        vector< vector<int> > lableId_cluster = kmeans.get_lableId_from_cluster();
        vector<Feature> kCenter = kmeans.get_kCenter();
        vector< vector<Feature> > cluster = kmeans.get_cluster();
        
        for(int i=1;i<=KC; i++)
        {
            print_content_by_kmeansCluster(lable_feature, lableId_cluster, kCenter,i);
        }
        
        int page_text_cluster_k = kmeans.get_page_text_cluster_k();
        
        //vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k);
        string selectLableName;
        vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k,selectLableName);
        
        if(page_text_id_vector.size() == 0)
        {
            cout<<" 这个网页非新闻文本"<<endl;
        }
        else
        {
            for(int i=0;i<page_text_id_vector.size();i++)
            {
                cout<<page_text_id_vector[i]<<" ";
            }
            cout<<endl;
            cout<<"\n－－－－－－－－－－－－正文标签信息开始－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"标题："<<web_page_title<<endl;
            string page_text = print_page_content_by_id(lable_feature, page_text_id_vector,selectLableName);
            cout<<"－－－－－－－－－－－－正文标签信息结束－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"              新闻标题： "<<web_page_title<<endl;
            cout<<"\n" << page_text<<endl;
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n\n";
            
            string tmp("              新闻标题： ");
            page_text = tmp + web_page_title +"\n\n" + page_text;
            ret = save_content2file("/Users/pc/get_page_text.html", page_text);
            if(ret == 1)
                cout<< "save page text  file ok"<<endl;
            else
                cout<< "save page text  file error"<<endl;
        }
    }
    catch(int e)
    {
        if(e == 1)
            cout<<"K must big than 0  or  featureVector  is empty"<<endl;
        else if(e==2)
        {
            cout<<"No need to cluter,because K is small than featureCluster number "<<endl;
            
        }
        else if(e == 3)
            cout << "this page only has less two lables,maybe it's not a new page\n"<<endl;
    }
    return 1;
}

//由于新浪新闻网页的推荐阅读链接的文本长度太长，当新闻文本正文很短的时，分类时会把推荐阅读文本选为文本簇，造成错误，顾将网页尾部连续的p、a、a、p、a、a、删去
vector<struct lableFeature>  news_sina_page_produce(vector<struct lableFeature> lf)
{
    int len = lf.size() -1;
    int i;
    
    //vector<int> vect_p_lable_index;
    for(i=len-1; i>=2;)
    {
        if(lf[i].lableName=="p" && lf[i-1].lableName=="a" && lf[i-2].lableName=="a")
        {
            int j=i-3;
            while(j>=2 && lf[j].lableName=="p" && lf[j-1].lableName=="a" && lf[j-2].lableName=="a")
            {
                j -= 3;
            }
            if((i-j)/3 >= 3)
            {
                if(j<= len*2/5)
                {
                    //break;
                    return lf;
                }
                else
                {
                    vector<struct lableFeature> ret = lf;
                    ret.erase(ret.begin() + j+1,ret.begin() +i +1);
                    return ret;
                    break;
                }
            }
            i =j-1;
        }
        else
            i--;
    }
    return lf;
}

int main_loop_url_AGENS(string url,int kcluster = 5)
{
    //请求爬去html网页
    getWebPageClass gwp;
    string utf_8_content = gwp.get_html_content_by_url(url);
    
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/qqfile.html");
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/sohufile.html");
    string web_page_title = gwp.get_web_page_title();
    
    if(utf_8_content.size() <= 0)
    {
        cout<<"get url content error\n";
        return 0;
    }
    
    //网页html预处理
    contentProcess cp;
    cp.parse_content_avoid_over_delete(utf_8_content);
    
    cout<<"content size after process : "<<utf_8_content.size()<<endl;
    
    int ret = save_content2file("/Users/pc/get_clear_page.html", utf_8_content);
    if(ret == 1)
        cout<< "save clear  file ok"<<endl;
    else
        cout<< "save clear  file error"<<endl;
    
    //vector<struct lableFeature> lable_feature = get_content_lable_feature(utf_8_content);
    
    
    vector<struct lableFeature> lable_feature = get_content_lable_feature_1124(utf_8_content);
    for(int i=0;i<lable_feature.size();i++)
        print_lable_feature(lable_feature[i]);
    
    vector<Feature> feature_vector ;
    //对新浪新闻（http:/ /news.sina）做特殊处理
    if(url.find("news.sina") != string::npos)
    {
        vector<struct lableFeature> tmp_label_feature_for_news_sina = news_sina_page_produce(lable_feature);
        feature_vector = get_feature_vector(tmp_label_feature_for_news_sina);
    }
    else{
        feature_vector = get_feature_vector(lable_feature);
    }

    
    int KC =kcluster;
    try
    {
        //kmeansCluster kmeans(feature_vector);
        //kmeans.kmeans_with_select_k_function(KC);
        //kmeans.kmeans_plus_plus_function();
        
        AGENSCluster kmeans(feature_vector);
        kmeans.AGENS(KC);
        //two_split_K_Means  kmeans(feature_vector);
        //kmeans.twoSplitKmeansFunction(8);
        KC = kmeans.get_K();
        for(int i=1;i<=KC;i++)
            kmeans.print_kmeans_cluster(i);
        
        vector< vector<int> > lableId_cluster = kmeans.get_lableId_from_cluster();
        vector<Feature> kCenter = kmeans.get_kCenter();
        vector< vector<Feature> > cluster = kmeans.get_cluster();
        
        for(int i=1;i<=KC; i++)
        {
            print_content_by_kmeansCluster(lable_feature, lableId_cluster, kCenter,i);
        }
        
        int page_text_cluster_k = kmeans.get_page_text_cluster_k();
        
        //vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k);
        string selectLableName;
        //vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k,selectLableName);
         vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce_1210(lable_feature, lableId_cluster, page_text_cluster_k,selectLableName);
        if(page_text_id_vector.size() == 0)
        {
            cout<<" 这个网页非新闻文本"<<endl;
        }
        else
        {
            for(int i=0;i<page_text_id_vector.size();i++)
            {
                cout<<page_text_id_vector[i]<<" ";
            }
            cout<<endl;
            cout<<"\n－－－－－－－－－－－－正文标签信息开始－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"标题："<<web_page_title<<endl;
            string page_text = print_page_content_by_id(lable_feature, page_text_id_vector,selectLableName);
            cout<<"－－－－－－－－－－－－正文标签信息结束－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"              新闻标题： "<<web_page_title<<endl;
            cout<<"\n" << page_text<<endl;
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n\n";
            
            string tmp("              新闻标题： ");
            page_text = tmp + web_page_title + page_text;
            ret = save_content2file("/Users/pc/get_page_text.html", page_text);
            if(ret == 1)
                cout<< "save page text  file ok"<<endl;
            else
                cout<< "save page text  file error"<<endl;
        }
    }
    catch(int e)
    {
        if(e == 1)
            cout<<"K must big than 0  or  featureVector  is empty"<<endl;
        else if(e==2)
        {
            cout<<"No need to cluter,because K is small than featureCluster number "<<endl;
            
        }
        else if(e == 3)
            cout << "this page only has less two lables,maybe it's not a new page\n"<<endl;
    }
    
    return 1;
}


int main_loop_file_AGENS(string url,int kcluster = 5)
{
    //请求爬去html网页
    getWebPageClass gwp;
    string utf_8_content = gwp.get_web_page_by_read_XML_file(url);
    
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/qqfile.html");
    //string utf_8_content = gwp.get_web_page_by_read_XML_file("/Users/pc/sohufile.html");
    string web_page_title = gwp.get_web_page_title();
    
    if(utf_8_content.size() <= 0)
    {
        cout<<"get url content error\n";
        return 0;
    }
    
    //网页html预处理
    contentProcess cp;
    cp.parse_content_avoid_over_delete(utf_8_content);
    
    cout<<"content size after process : "<<utf_8_content.size()<<endl;
    
    int ret = save_content2file("/Users/pc/get_clear_page.html", utf_8_content);
    if(ret == 1)
        cout<< "save clear  file ok"<<endl;
    else
        cout<< "save clear  file error"<<endl;
    
    //vector<struct lableFeature> lable_feature = get_content_lable_feature(utf_8_content);
    
    
    vector<struct lableFeature> lable_feature = get_content_lable_feature_1124(utf_8_content);
    for(int i=0;i<lable_feature.size();i++)
        print_lable_feature(lable_feature[i]);
    
    vector<Feature> feature_vector ;
    //对新浪新闻（http:/ /news.sina）做特殊处理
    if(url.find("news.sina") != string::npos)
    {
        vector<struct lableFeature> tmp_label_feature_for_news_sina = news_sina_page_produce(lable_feature);
        feature_vector = get_feature_vector(tmp_label_feature_for_news_sina);
    }
    else{
        feature_vector = get_feature_vector(lable_feature);
    }
    
    
    int KC =kcluster;
    try
    {
        //kmeansCluster kmeans(feature_vector);
        //kmeans.kmeans_with_select_k_function(KC);
        //kmeans.kmeans_plus_plus_function();
        
        AGENSCluster kmeans(feature_vector);
        kmeans.AGENS(KC);
        //two_split_K_Means  kmeans(feature_vector);
        //kmeans.twoSplitKmeansFunction(8);
        KC = kmeans.get_K();
        for(int i=1;i<=KC;i++)
            kmeans.print_kmeans_cluster(i);
        
        vector< vector<int> > lableId_cluster = kmeans.get_lableId_from_cluster();
        vector<Feature> kCenter = kmeans.get_kCenter();
        vector< vector<Feature> > cluster = kmeans.get_cluster();
        
        for(int i=1;i<=KC; i++)
        {
            print_content_by_kmeansCluster(lable_feature, lableId_cluster, kCenter,i);
        }
        
        int page_text_cluster_k = kmeans.get_page_text_cluster_k();
        
        //vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k);
        string selectLableName;
        //vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce(lable_feature, lableId_cluster, page_text_cluster_k,selectLableName);
        vector<int> page_text_id_vector =  get_page_text_cluster_id_after_produce_1210(lable_feature, lableId_cluster, page_text_cluster_k,selectLableName);
        if(page_text_id_vector.size() == 0)
        {
            cout<<" 这个网页非新闻文本"<<endl;
        }
        else
        {
            for(int i=0;i<page_text_id_vector.size();i++)
            {
                cout<<page_text_id_vector[i]<<" ";
            }
            cout<<endl;
            cout<<"\n－－－－－－－－－－－－正文标签信息开始－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"标题："<<web_page_title<<endl;
            string page_text = print_page_content_by_id(lable_feature, page_text_id_vector,selectLableName);
            cout<<"－－－－－－－－－－－－正文标签信息结束－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n";
            cout<<"              新闻标题： "<<web_page_title<<endl;
            cout<<"\n" << page_text<<endl;
            cout<<"\n\n－－－－－－－－－－－－网页正文－－－－－－－－－－－－－－－－－－－－－－－－－－－\n\n";
            
            string tmp("              新闻标题： ");
            page_text = tmp + web_page_title + page_text;
            ret = save_content2file("/Users/pc/get_page_text.html", page_text);
            if(ret == 1)
                cout<< "save page text  file ok"<<endl;
            else
                cout<< "save page text  file error"<<endl;
        }
    }
    catch(int e)
    {
        if(e == 1)
            cout<<"K must big than 0  or  featureVector  is empty"<<endl;
        else if(e==2)
        {
            cout<<"No need to cluter,because K is small than featureCluster number "<<endl;
            
        }
        else if(e == 3)
            cout << "this page only has less two lables,maybe it's not a new page\n"<<endl;
    }
    
    return 1;
}


vector<string> get_test_url_vect()
{
    vector<string> urlVect;
    //新浪网
    string url0("http://sports.sina.com.cn/nba/2015-04-24/12277585850.shtml");
    //string url00("http://sports.sina.com.cn/basketball/nba/2015-11-23/doc-ifxkxfvn8964499.shtml");
    string url1("http://sports.sina.com.cn/basketball/nba/2015-11-13/doc-ifxksqis4781087.shtml");
    string url2("http://finance.sina.com.cn/chanjing/gsnews/20151030/010123623334.shtml?cre=financepagepc&mod=f&loc=3&r=h&rfunc=6");  //两桶油
    urlVect.push_back(url0);
    urlVect.push_back(url1);
    urlVect.push_back(url2);
    //新浪博客  特殊网页：大小写标签混乱一起
    string url2_0("http://blog.sina.com.cn/s/blog_aa875eee0102wftg.html?tj=1");
    urlVect.push_back(url2_0);
    //百度新闻
    string url3("http://www.baidu.com/s?ie=utf-8&f=8&rsv_bp=1&tn=ubuntuu_cb&wd=%E7%BD%91%E7%BB%9C%E7%88%AC%E8%99%AB%20%20%E6%96%87%E6%9C%AC%E4%B9%B1%E7%A0%81%20C%2B%2B&rsv_pq=f7b9acd800000ed2&rsv_t=e7254jg%2BHKV2oGILj6QU1qUUY8RmgxC18gdzcLQ5%2F%2F4MrxgSJYq5FZqJzFVCrTTvOA&rsv_enter=1&rsv_sug3=27&rsv_sug1=5&rsv_sug2=0&inputT=9319&rsv_sug4=10773");
    //urlVect.push_back(url3);
    //新华网
    string url4("http://news.xinhuanet.com/politics/2015-11/09/c_1117086503.htm");
    urlVect.push_back(url4);
    //cn博客
    string url5("http://www.cnblogs.com/hoys/archive/2011/08/11/2134653.html");
    urlVect.push_back(url5);
    //博客网博客网页
    string url6("view-source:http://www.cnblogs.com/daniagger/archive/2012/06/08/2541506.html");
    urlVect.push_back(url6);
    //直播吧新闻
    string url7("http://news.zhibo8.cc/zuqiu/2015-11-12/564489ba44a33.htm");
    urlVect.push_back(url7);
    //搜狐新闻  完全正确
    string url8("http://mt.sohu.com/20151111/n426103878.shtml");
    string url9("http://mil.sohu.com/20151113/n426330845_1.shtml");
    urlVect.push_back(url8);
    urlVect.push_back(url9);
    //网易新闻
    string url10("http://news.163.com/15/1113/09/B89TC4RP0001124J.html");
    string url11("http://news.163.com/15/1117/14/B8KNNLR8000146BE.html");
    urlVect.push_back(url10);
    urlVect.push_back(url11);
    //电子科大校网
    string url12("http://www.new1.uestc.edu.cn/?n=UestcNews.Front.Document.ArticlePage&Id=51659");
    urlVect.push_back(url12);
    //腾讯
    string url13("http://news.qq.com/a/20151123/047720.htm");
    //urlVect.push_back(url13);
    //12
    //qqfile、
    //直播吧  正文长短句差距很明显，容易把短句子和长句子分开     ＝＝＝》 人为动态调控：连续
    //搜狐还行
    
    //微信
    string url14("http://mp.weixin.qq.com/s?__biz=MzI5MDA2MjgxNg==&mid=400896281&idx=1&sn=bffd0ea81bf14235e2e1c844a45c629c&scene=0&key=d72a47206eca0ea917efb4f6e73c3f35fc7f7a3c49448b58ce1d23298faf202cb35a7912d42b193c69e1ed2e00c795db&ascene=0&uin=MjE4NTYwODM2Mg%3D%3D&devicetype=iMac+MacBookAir5%2C2+OSX+OSX+10.10.5+build(14F27)&version=11020201&pass_ticket=810kuNxs9pVZVGTy5vAw6NnM2JhyeXrg6c%2BaMvnMIr0%2F9g68s9wrZmVhLvRIQ4XG");
    //七月算法网站
    string url15("http://ask.julyedu.com/question/787");
    urlVect.push_back(url15);
    //凤凰
    string url16("http://news.ifeng.com/a/20151123/46355849_0.shtml");
    urlVect.push_back(url16);
    //央视网
    string url17("http://news.cntv.cn/2015/11/23/ARTI1448281298076767.shtml");
    urlVect.push_back(url17);
    //网信网
    string url18("http://www.cac.gov.cn/2015-11/16/c_1117152499.htm");
    urlVect.push_back(url18);
    //环球网
    string url19("http://world.huanqiu.com/exclusive/2015-11/8022073.html");
    urlVect.push_back(url19);
    //中华军事
    string url20("http://military.china.com/important/11132797/20151123/20804396.html");
    urlVect.push_back(url20);
    //米尔军事
    string url21("http://club.miercn.com/201511/thread_588085_1.html");
    urlVect.push_back(url21);
    //铁血军事
    string url22("http://bbs.tiexue.net/post2_10361709_1.html");
    urlVect.push_back(url22);
    //强国网
    string url23("http://www.cnqiang.com/junshi/junqing/201511/01256156.html");
    string url24("http://www.cnqiang.com/junshi/zhanlue/201511/01256154.html");
    string url25("http://www.cnqiang.com/junshi/zhanlue/201512/01257001.html");
    string url26("http://www.cnqiang.com/junshi/zhanlue/201512/01256999.html");
    urlVect.push_back(url23);
    urlVect.push_back(url24);
    urlVect.push_back(url25);
    urlVect.push_back(url26);
    
    //谷歌新闻
    string url27("http://www.chicagotribune.com/news/local/politics/ct-rahm-emanuel-police-task-force-20151130-story.html");
    //东方财富网新闻
    string url28("http://finance.eastmoney.com/news/1344,20151205572804260.html");
    urlVect.push_back(url27);
    //urlVect.push_back(url28);
    return urlVect;
}
vector<string> get_url_vect_from_file(string fileName)
{
    ifstream infile(fileName);
    string urlstr;
    vector<string> ret;
    while(getline(infile, urlstr))
    {
        ret.push_back(urlstr);
    }
    infile.close();
    return ret;
}

//采集固定域名下的新闻链接
int main1()
{
    string url("http://news.ifeng.com/");
    vector<string> vecturl;
    vecturl.push_back("http://news.ifeng.com/");
    vecturl.push_back("http://finance.ifeng.com/");
    vecturl.push_back("http://ent.ifeng.com/");
    vecturl.push_back("http://sports.ifeng.com/");
    vecturl.push_back("http://fashion.ifeng.com/");
    vecturl.push_back("http://fashion.ifeng.com/");
    vecturl.push_back("http://auto.ifeng.com/");
    vecturl.push_back("http://cd.house.ifeng.com/");
    vecturl.push_back("http://tech.ifeng.com/");
    vecturl.push_back("http://blog.ifeng.com/");
    vecturl.push_back("http://games.ifeng.com/");
    vecturl.push_back("http://cp.ifeng.com/?aid=44");
    vecturl.push_back("http://yc.ifeng.com/?_bookch=top");
    vecturl.push_back("http://zhongyi.ifeng.com/");
    vecturl.push_back("http://audi-future.ifeng.com/");
    
    
    for(int i=0;i<vecturl.size(); i++)
    {
        getWebPageClass gwp;
        string urlStringToFile = gwp.get_url_from_web_page_by_url(vecturl[i]);
        //save_content2file(, <#string content#>)
        stringstream ss;
        ss <<"/Users/pc/UrlFile/temple_url"<<i<<".txt";
        save_content2file(ss.str(), urlStringToFile);
    }

    return 1;
}
int main()
{
    //vector<string> urlVect = get_test_url_vect();
    //vector<string> urlVect = get_url_vect_from_file("/Users/pc/sinaUrlFile.txt");
    //vector<string> urlVect = get_url_vect_from_file("/Users/pc/UrlFile/sinafileurl.txt");
    //vector<string> urlVect = get_url_vect_from_file("/Users/pc/UrlFile/baidufileurl.txt");
    //vector<string> urlVect = get_url_vect_from_file("/Users/pc/UrlFile/wangyifileurl.txt");
    //vector<string> urlVect = get_url_vect_from_file("/Users/pc/UrlFile/hfutfileurl.txt");
    //vector<string> urlVect = get_url_vect_from_file("/Users/pc/UrlFile/sohufileurl.txt");
    vector<string> urlVect = get_url_vect_from_file("/Users/pc/UrlFile/ifengfileurl.txt");
    cout<<urlVect.size()<<endl;
    string loop_url("url");
    //cout<<urlVect[0].size()<<endl;
    int loop_time = 9;
    cout<<"输入url开始坐标：";
    cin >> loop_time;
    while(loop_time < urlVect.size())
    {
        cout<<urlVect[loop_time]<<endl;
        //main_loop_url_kmeans(urlVect[loop_time],6);
        cout<<"----------------\n";
        //main_loop_url_twoSplitKmeans(urlVect[loop_time],6);
        cout<<"----------------\n";
        main_loop_url_AGENS(urlVect[loop_time],4);
        cout<<urlVect[loop_time]<<endl;
        cout<<"loop_time:"<<loop_time<<endl;
        int a;
        cout<<"输入下一个url坐标：";
        cin >> a;
        if( a== 0)
            loop_time ++;
        else
            loop_time = a;
    }
    main_loop_file_twoSplitKmeans("/Users/pc/cul_sohu_file.html",6);
    //main_loop_url_twoSplitKmeans("http://www.edf.uestc.edu.cn/index.php?m=content&c=index&a=lists&catid=144",6);
    
    main_loop_url_AGENS("http://news.163.com/15/1210/10/BAFGLBIA00011229.html",2);
    string urlGoogle("http://www.chicagotribune.com/news/local/politics/ct-rahm-emanuel-police-task-force-20151130-story.html");
    //main_loop_url("http://hackathon.ele.me/intro");
    //main_loop_file_twoSplitKmeans("http://www.iteye.com/topic/587673",4);
    //main_loop_file_kmeans("http://www.iteye.com/topic/587673",6);
    //xmlParser("/Users/pc/get_clear_page.html");
    return 1;
}
//question1: 页面抓取不完

#define OUTLEN 255

