#include "threadpool.h"




threadPool::threadPool(void (*func)(string, threadPool *), int maxThreadNum, int maxUrlQueueLenght)
    :m_userFuncCaller(func),m_maxThreadNum(maxThreadNum),m_maxUrlQueueLength(maxUrlQueueLenght),
      m_threadPoolIsClosed(false),m_hadDealUrlNum(0)
{
    pthread_mutex_init(&m_mutexForUrlQueue,NULL);
    pthread_mutex_init(&m_mutexForUrlSet,NULL);

    pthread_cond_init(&m_urlQueueIsNotEmpty,NULL);
    pthread_cond_init(&m_urlQueueIsNotFull,NULL);
    pthread_t tid;
    threadPool* arg = this;
    for(int i=0;i<m_maxThreadNum;i++)
    {
        pthread_create(&tid,NULL,threadPoolCaller,(void*) arg);

        m_threadList.push_back(tid);
    }

}

threadPool::~threadPool()
{
    if(m_threadPoolIsClosed == false)
        threadPoolDestroy();
}

void threadPool::threadPoolDestroy()
{
    if(m_threadPoolIsClosed == true)
    {
        return;
    }
    m_threadPoolIsClosed = true;
    pthread_cond_broadcast(&m_urlQueueIsNotEmpty);
    pthread_cond_broadcast((&m_urlQueueIsNotFull));
    list<pthread_t>::iterator iter = m_threadList.begin();
    for(;iter!= m_threadList.end();iter++)
    {
        pthread_join(*iter,NULL);
    }

    pthread_mutex_destroy(&m_mutexForUrlQueue);
    pthread_mutex_destroy(&m_mutexForUrlSet);
    pthread_cond_destroy(&m_urlQueueIsNotEmpty);
    pthread_cond_destroy(&m_urlQueueIsNotFull);
}

//old
/*
void* threadPool::threadPoolCaller(void *arg)
{
    threadPool *ptp = (threadPool*)arg;
    while(1)
    {
        pthread_mutex_lock(&(ptp->m_mutexForUrlQueue));
        while((ptp->m_UrlQueue.empty() == true) && (ptp->m_threadPoolIsClosed == false))
        {
            pthread_cond_wait(&(ptp->m_urlQueueIsNotEmpty),&(ptp->m_mutexForUrlQueue));
        }
        if(ptp->m_threadPoolIsClosed == true)
        {
            pthread_mutex_unlock(&(ptp->m_mutexForUrlQueue));
            //pthread_cancel(getpid());
            pthread_exit(NULL);
        }
        string tmpUrl = ptp->threadPoolGetUrl();
        (ptp->m_hadDealUrlNum) ++;

        if((ptp->m_UrlQueue).size() == ptp->m_maxUrlQueueLength -1)
        {
            pthread_cond_broadcast(&(ptp->m_urlQueueIsNotFull));
        }
        pthread_mutex_unlock(&(ptp->m_mutexForUrlQueue));
        if(ptp->m_userFuncCaller != NULL)
        {
            (*(ptp->m_userFuncCaller))(tmpUrl,ptp);
        }
    }
}
*/

void* threadPool::threadPoolCaller(void *arg)
{
    threadPool *ptp = (threadPool*)arg;
    while(1)
    {
        string tmpUrl = ptp->threadPoolGetUrl();
        if(ptp->m_userFuncCaller != NULL)
        {
            (*(ptp->m_userFuncCaller))(tmpUrl,ptp);
        }
    }
}
string threadPool::threadPoolGetUrl()
{
    pthread_mutex_lock(&(m_mutexForUrlQueue));
    while((m_UrlQueue.empty() == true) && (m_threadPoolIsClosed == false))
    {
        pthread_cond_wait(&(m_urlQueueIsNotEmpty),&(m_mutexForUrlQueue));
    }
    if(m_threadPoolIsClosed == true)
    {
        pthread_mutex_unlock(&(m_mutexForUrlQueue));
        //pthread_cancel(getpid());
        pthread_exit(NULL);
    }
    string tmpUrl = m_UrlQueue.front();
    m_UrlQueue.pop();
    (m_hadDealUrlNum) ++;

    if((m_UrlQueue).size() == m_maxUrlQueueLength -1)
    {
        pthread_cond_broadcast(&(m_urlQueueIsNotFull));
    }
    pthread_mutex_unlock(&(m_mutexForUrlQueue));
    return tmpUrl;
}


int threadPool::threadPoolAddUrl(const string &url_string)
{
    pthread_mutex_lock(&m_mutexForUrlSet);
    if(m_UrlSet.count(url_string) != 0)
    {
        pthread_mutex_unlock(&m_mutexForUrlSet);
        return 0;
    }
    m_UrlSet.insert(url_string);
    pthread_mutex_unlock(&m_mutexForUrlSet);

    pthread_mutex_lock(&m_mutexForUrlQueue);
    while((m_UrlQueue.size() == m_maxUrlQueueLength) && (m_threadPoolIsClosed == false))
    {
        pthread_cond_wait(&m_urlQueueIsNotFull,&m_mutexForUrlQueue);
    }
    if(m_threadPoolIsClosed == true)
    {
        pthread_mutex_unlock(&m_mutexForUrlQueue);
        return 0;
    }

    m_UrlQueue.push(url_string);

    if(m_UrlQueue.size() == 1)
    {
        pthread_cond_broadcast(&m_urlQueueIsNotEmpty);
    }
    pthread_mutex_unlock(&m_mutexForUrlQueue);
    return 1;
}
