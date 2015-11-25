#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <list>
#include <string>
#include<iostream>
#include <vector>
#include<set>
#include<queue>
#include<pthread.h>
#include<unistd.h>
using namespace std;
class threadPool
{
public:
    threadPool(void (*func)(string url,threadPool* pool),int  maxThreadNum,int maxUrlQueueLenght);
    ~threadPool();
    void threadPoolDestroy();
    int threadPoolAddUrl(const string &url_string);
    string threadPoolGetUrl();
private:
    //存放线程函数
    void(*m_userFuncCaller)(string url,threadPool * pool);
    static void* threadPoolCaller(void* arg);


    //记录线程数和最大等待爬取的url队列长度
    int m_maxThreadNum;
    int m_maxUrlQueueLength;
    int m_hadDealUrlNum;

    queue<string> m_UrlQueue;
    set<string> m_UrlSet;
    list<pthread_t> m_threadList;

    bool m_threadPoolIsClosed;
    //判断
    pthread_mutex_t  m_mutexForUrlQueue;
    pthread_mutex_t m_mutexForUrlSet;

    pthread_cond_t  m_urlQueueIsNotEmpty;
    pthread_cond_t m_urlQueueIsNotFull;




};

#endif // THREADPOOL_H
