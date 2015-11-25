#include "newthreadpool.h"

newThreadPool::newThreadPool(void (*func)(string, newThreadPool *), unsigned int maxThreadNum, unsigned int maxUrlQueueLength)
    :userFunctionCaller(func),m_maxThreadNum(maxThreadNum),m_maxUrlQueueLength(maxUrlQueueLength),
      m_hadCompleteUrlNum(0),m_threadPoolIsClosed(false)
{
    pthread_mutex_init(&m_mutexForUrlQueue,NULL);
    pthread_mutex_init(&m_mutexForUrlSet,NULL);

    pthread_cond_init(&m_condForUrlQueueIsNotEmpty,NULL);
    pthread_cond_init(&m_condForUrlQueueIsNotFull,NULL);

    pthread_t tid;
    newThreadPool * ptp = this;
    for(int i=0;i<m_maxThreadNum;i++)
    {
        pthread_create(&tid,NULL,newThreadPool_caller,(void*) ptp);
        //cout<<"create thread : "<<i<<"   with tid: "<<tid<<endl;
        m_threadList.push_back(tid);
    }
     //cout<<"new thread pool create ok\n";
}

newThreadPool::~newThreadPool()
{
    if(m_threadPoolIsClosed == false)
        newThreadPoolDestroy();
}

void newThreadPool::newThreadPoolDestroy()
{
    if(m_threadPoolIsClosed == true)
        return ;
    m_threadPoolIsClosed = true;
    pthread_cond_broadcast(&m_condForUrlQueueIsNotEmpty);
    pthread_cond_broadcast(&m_condForUrlQueueIsNotFull);

    list<pthread_t>::iterator iter = m_threadList.begin();
    for(;iter!= m_threadList.end();iter++)
    {
        pthread_join(*iter,NULL);
    }
    pthread_cond_destroy(&m_condForUrlQueueIsNotEmpty);
    pthread_cond_destroy(&m_condForUrlQueueIsNotFull);

    pthread_mutex_destroy(&m_mutexForUrlQueue);
    pthread_mutex_destroy(&m_mutexForUrlSet);
}


string newThreadPool::newThreadPool_getUrl()
{
    pthread_mutex_lock(&m_mutexForUrlQueue);
    while(m_urlQueue.size() == 0 && m_threadPoolIsClosed == false)
    {
        pthread_cond_wait(&m_condForUrlQueueIsNotEmpty,&m_mutexForUrlQueue);
    }
    if(m_threadPoolIsClosed == true)
    {
        pthread_mutex_unlock(&m_mutexForUrlQueue);
        pthread_exit(NULL);
    }

    string url = m_urlQueue.front();
    m_urlQueue.pop();
    m_hadCompleteUrlNum ++;

    if(m_urlQueue.size() == m_maxUrlQueueLength-1)
    {
        pthread_cond_broadcast(&m_condForUrlQueueIsNotFull);
    }
    pthread_mutex_unlock(&m_mutexForUrlQueue);

    return url;
}

int newThreadPool::newThreadPool_addUrl(const string url)
{

    pthread_mutex_lock(&m_mutexForUrlSet);
    if(m_urlSet.count(url) != 0)
    {
        pthread_mutex_unlock(&m_mutexForUrlSet);
        return 0;
    }
    m_urlSet.insert(url);
    pthread_mutex_unlock(&m_mutexForUrlSet);

    pthread_mutex_lock(&m_mutexForUrlQueue);
    while(m_urlQueue.size() == m_maxUrlQueueLength && m_threadPoolIsClosed == false)
    {
        pthread_cond_wait(&m_condForUrlQueueIsNotFull,&m_mutexForUrlQueue);
    }
    if(m_threadPoolIsClosed ==true)
    {
        pthread_mutex_unlock(&m_mutexForUrlQueue);
        return 0;
    }
    m_urlQueue.push(url);
    if(m_urlQueue.size() == 1)
    {
        pthread_cond_broadcast(&m_condForUrlQueueIsNotEmpty);
    }
    pthread_mutex_unlock(&m_mutexForUrlQueue);
    return 1;
}

void* newThreadPool::newThreadPool_caller(void *arg)
{
    newThreadPool * ptp = (newThreadPool* )arg;

    while(1)
    {
        string url = ptp->newThreadPool_getUrl();
        cout<<"thread pid:  "<<getpid()<<endl;
        if(ptp->userFunctionCaller != NULL)
        {
            (*(ptp->userFunctionCaller))(url,ptp);
        }
    }
}
