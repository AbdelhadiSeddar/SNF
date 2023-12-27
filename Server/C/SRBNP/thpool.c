#include "_Imports.h"
#include "thpool.h"
/*
    Must be 3+ 
    - 1 for Thread pool Handler
    - 1 for Controlling thread ( one that adds works into the pool thread )
    - 1+ for Worker Threads
*/ 
int _MAX_NETWORK_THREAD_COUNT = 4; 

void *thpool_work_wrapper(void *arg);
thpool_work* thpool_pop_work(thpool* pool);
thpool_worker *thpool_insertnew_worker(thpool *pool);
void *thpool_handler(void *arg);



thpool *inis_thpool(int Max_Threads, void *(*Main_Worker)(), void *arg)
{
    thpool *new = calloc(1, sizeof(thpool));
    pthread_mutex_init(&(new->thpool_works_MUTEX), NULL);
    pthread_mutex_init(&(new->thpool_workers_MUTEX), NULL);
    pthread_cond_init(&(new->thpool_works_cond), NULL);
    sem_init(&(new->thpool_sem), 0, Max_Threads);
    (new->thpool_handler) = malloc(sizeof(pthread_t));
    (new->thpool_main_worker) = malloc(sizeof(pthread_t));
    (new->thpool_works) = NULL;
    (new->max_workers_count) = Max_Threads;
    (new->stop) = 0;

    pthread_create(
        (new->thpool_handler),
        NULL,
        thpool_handler,
        new
    );
    if(Main_Worker != NULL)
        pthread_create(
            (new->thpool_main_worker),
            NULL,
            Main_Worker,
            arg
        );
        return new;
}

void *thpool_handler(void *arg)
{
    thpool *pool = arg;
    pthread_mutex_lock(&(pool->thpool_works_MUTEX));
    while (!(pool->stop))
    {
        if (pool->thpool_works == NULL)
            pthread_cond_wait(&(pool->thpool_works_cond), &(pool->thpool_works_MUTEX));
        if (pool ->thpool_works == NULL)
            continue;
        sem_wait(&(pool->thpool_sem));

        pthread_create(
            thpool_insertnew_worker(pool) ->worker,
            NULL,
            thpool_work_wrapper,
            thpool_pop_work(pool)
        );
    }
    thpool_wait(pool);    
    pthread_mutex_unlock(&(pool->thpool_works_MUTEX));
    pthread_mutex_destroy(&(pool->thpool_works_MUTEX));
    pthread_mutex_destroy(&(pool->thpool_workers_MUTEX));
    pthread_cond_destroy(&(pool->thpool_works_cond));
    sem_destroy(&(pool->thpool_sem));
    bzero(pool, sizeof(thpool));
    return NULL;
}

thpool_worker *thpool_insertnew_worker(thpool *pool)
{
    thpool_worker *worker = calloc(1, sizeof(thpool_worker));
    worker->worker = calloc(1, sizeof(pthread_t));
    pthread_mutex_lock(&(pool->thpool_workers_MUTEX));
    thpool_worker *tmp = pool->thpool_workers;
    if (tmp == NULL)
    {
        pool -> thpool_workers = worker;
    }
    else
    {
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp ->next = worker;
    }
    pthread_mutex_unlock(&(pool->thpool_workers_MUTEX));
    return worker;
}

thpool_work* thpool_pop_work(thpool* pool)
{
    thpool_work* work = pool->thpool_works;
    pool -> thpool_works = pool -> thpool_works ->next;
    return work;
}
void *thpool_work_wrapper(void *arg)
{
    thpool_work* work = (thpool_work*)arg;
    void * re = (work->func)(work->arg);
    sem_post(&(work->pool->thpool_sem));
    return re;
}

void thpool_addwork(thpool *pool, void *(*func)(), void *arg)
{
    thpool_work* work = calloc(1, sizeof(thpool_work));
    work->arg = arg;
    work->func = func;
    work->pool = pool;

    pthread_mutex_lock(&(pool->thpool_works_MUTEX));
    thpool_work *tmp = pool->thpool_works;
    if (tmp == NULL)
    {
        pool -> thpool_works = work;
        pthread_cond_broadcast(&(pool->thpool_works_cond));
    }
    else
    {
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp ->next = work;
    }
    pthread_mutex_unlock(&(pool->thpool_works_MUTEX));
}

void thpool_wait(thpool* pool)
{
    for(int i = 0; i < pool->max_workers_count ; i++)
        sem_wait(&(pool->thpool_sem));
}

void thpool_join(thpool* pool)
{
    pthread_join(*(pool->thpool_main_worker), NULL);
}
void thpool_stop(thpool* pool)
{
    pool->stop = 1;
}

