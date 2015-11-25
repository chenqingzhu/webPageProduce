#ifndef NEWTHREADPOOL_H
#define NEWTHREADPOOL_H
#include <list>
#include <string>
#include<iostream>
#include <vector>
#include<set>
#include<queue>
#include<pthread.h>
#include<unistd.h>
using namespace std;
class newThreadPool
{
public:
    newThreadPool(void(*func)(string url,newThreadPool* pool),unsigned int maxThreadNum,unsigned int maxUrlQueueLength);
    ~newThreadPool();
    string newThreadPool_getUrl();
    int newThreadPool_addUrl(const string url);
    void (*userFunctionCaller)(string url,newThreadPool* pool);
    void newThreadPoolDestroy();
private:
    static void* newThreadPool_caller(void* arg);

    list<pthread_t> m_threadList;
    queue<string> m_urlQueue;
    set<string> m_urlSet;

    pthread_mutex_t m_mutexForUrlSet,m_mutexForUrlQueue;

    pthread_cond_t m_condForUrlQueueIsNotEmpty,m_condForUrlQueueIsNotFull;

    unsigned int m_maxThreadNum,m_maxUrlQueueLength;

    unsigned int m_hadCompleteUrlNum;

    bool m_threadPoolIsClosed;
};

#endif // NEWTHREADPOOL_H
