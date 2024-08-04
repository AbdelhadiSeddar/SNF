#include <SNF/thpool.h>
#include "thpool.h"

time_t SNF_THPOOL_STOPWAIT = 1;

/// @brief Frees a worker
/// @param work  The Worker to be freed
void thpool_work_free(SNF_thpool_work *work)
{
    free(work);
}
/// @brief Pops a work from the "works" Pile
/// @param pool  The Thread pool to be operated on.
/// @return Poped Work or `NULL` if  thpool_works_sem has been `sem_post`'d immaturely
/// @warning If there is no Work it will block till a new `work` is added by snf_thpool_addwork
SNF_thpool_work *thpool_pop_work(SNF_thpool *pool)
{
    sem_wait(&(pool->thpool_works_sem));
    pthread_mutex_lock(&(pool->thpool_works_MUTEX));
    SNF_thpool_work *work = pool->thpool_works;
    if(work != NULL)
    {
        pool->thpool_works = pool->thpool_works->next;
    }
    pthread_mutex_unlock(&(pool->thpool_works_MUTEX));
    return work;
}
/// @brief Serves as a Wrapper to the "work"
/// @param arg Must be of Type [ SNF_thpool * ]
/// @return
void *thpool_work_wrapper(void *arg)
{
    SNF_thpool *tp = (SNF_thpool *)arg;
    while(tp->thpool_status == SNF_THPOOL_RUN)
    {
        SNF_thpool_work * wrk = thpool_pop_work(tp);
        if(wrk == NULL)
            continue;
        wrk->func(wrk->arg);
    }
    return NULL;
}
void *thpool_handler(void *arg)
{
    SNF_thpool *pool = arg;
thpool_handler_start:;
    pthread_mutex_lock(&(pool->thpool_status_MUTEX));
    sem_post(&(pool->thpool_status_sem));
    pthread_cond_wait(&(pool->thpool_status_COND), &(pool->thpool_status_MUTEX));
    switch(pool->thpool_status)
    {
        case SNF_THPOOL_STOP:
            goto thpool_handler_status_stop;
        case SNF_THPOOL_RUN:
            goto thpool_handler_status_run;
        case SNF_THPOOL_DESTROY:
            goto thpool_handler_status_end;
    }

thpool_handler_status_stop:;
    struct timespec *timewait = calloc(1, sizeof(struct timespec));
    timewait->tv_nsec = 0;
    timewait->tv_sec = SNF_THPOOL_STOPWAIT;
    for(int i = 0; i < (pool->thpool_n_workers) ; i++ )
    {
        int re = pthread_timedjoin_np(
            pool->thpool_workers[i].worker,
            NULL,
            timewait
        );
        switch (re) {
            case EBUSY:
            case ETIMEDOUT:
                pthread_cancel(pool->thpool_workers[i].worker);
                pthread_join(pool->thpool_workers[i].worker, NULL);
            default:
                break;    
        }

    }
    free(timewait);
    free(pool->thpool_workers);
    goto thpool_handler_status_end;
thpool_handler_status_run:;
    pool->thpool_workers = calloc((pool->thpool_n_workers), sizeof(SNF_thpool_worker));
    for(int i = 0; i < (pool->thpool_n_workers); i++)
    {
        pthread_create(
            &((pool->thpool_workers)[i]).worker,
            NULL,
            thpool_work_wrapper,
            pool
        );
    }
    goto thpool_handler_status_end;
thpool_handler_status_end:;
    pthread_mutex_unlock(&(pool->thpool_status_MUTEX));
    goto thpool_handler_start;

thpool_handler_status_return:;
    return NULL;
}

int snf_thpool_inis(SNF_thpool **ThreadPool, int Max_Threads, void *(*Main_Worker)(), void *arg)
{
    if (ThreadPool == NULL || Max_Threads < (Main_Worker == NULL ? 2 : 3))
        return -1;
    SNF_thpool *new = calloc(1, sizeof(SNF_thpool));
    pthread_mutex_init(&(new->thpool_works_MUTEX), NULL);
    pthread_mutex_init(&(new->thpool_workers_MUTEX), NULL);
    pthread_mutex_init(&(new->thpool_noworks_MUTEX), NULL);
    pthread_mutex_init(&(new->thpool_status_MUTEX), NULL);
    pthread_cond_init(&(new->thpool_status_COND), NULL);
    sem_init(&(new->thpool_workers_sem), 0, 0);
    sem_init(&(new->thpool_works_sem), 0, 0);
    sem_init(&(new->thpool_status_sem), 0, 0);
    (new->thpool_handler) = malloc(sizeof(pthread_t));
    (new->thpool_works) = NULL;
    (new->thpool_n_workers) = Max_Threads - 1 - (Main_Worker == NULL ? 0 : 1);
    (new->thpool_workers) = calloc((new->thpool_n_workers), sizeof(struct SNF_ThreadPool_worker_t));
    (new->thpool_status) = SNF_THPOOL_STOP;

    (*ThreadPool) = new;

    pthread_create(
            new->thpool_handler,
            NULL,
            thpool_handler,
            (void*)new
        );

    snf_thpool_run(new);
    snf_thpool_wait(new);
    if (Main_Worker != NULL)
    {
        (new->thpool_main_worker) = malloc(sizeof(pthread_t));
        pthread_create(
            (new->thpool_main_worker),
            NULL,
            Main_Worker,
            arg);
    }
    return 0;
}
void snf_thpool_addwork(SNF_thpool *pool, void *(*func)(), void *arg)
{
    SNF_thpool_work *work = calloc(1, sizeof(SNF_thpool_work));
    work->arg = arg;
    work->func = func;
    work->pool = pool;

    pthread_mutex_lock(&(pool->thpool_works_MUTEX));
    SNF_thpool_work *tmp = pool->thpool_works;
    if (tmp == (SNF_thpool_work *)0x0)
    {
        pool->thpool_works = work;
    }
    else
    {
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = work;
    }
    pthread_mutex_unlock(&(pool->thpool_works_MUTEX));
    sem_post(&(pool->thpool_works_sem));
}
void snf_thpool_wait(SNF_thpool *pool)
{
    if (pool == NULL || pool->thpool_handler == NULL)
        return;
    sem_wait(&(pool->thpool_status_sem));
    sem_post(&(pool->thpool_status_sem));
    pthread_mutex_lock(&(pool->thpool_status_MUTEX));
    pthread_mutex_unlock(&(pool->thpool_status_MUTEX));
}
void snf_thpool_join(SNF_thpool *pool)
{
    if (pool == NULL || pool->thpool_handler == NULL)
        return;
    sem_wait(&(pool->thpool_status_sem));
    sem_post(&(pool->thpool_status_sem));
    pthread_join(*(pool->thpool_handler), NULL);
}
void snf_thpool_stop(SNF_thpool *pool)
{
  if (pool == NULL || pool->thpool_handler == NULL)
    return;
    int re = 0;
    sem_wait(&(pool->thpool_status_sem));
    sem_post(&(pool->thpool_status_sem));
  pthread_mutex_lock(&(pool->thpool_status_MUTEX));
    if(pool->thpool_status != SNF_THPOOL_RUN)
    {
        re = EINPROGRESS;
    }
    else
    {
        pool->thpool_status = SNF_THPOOL_STOP;
        pthread_cond_broadcast(&(pool->thpool_status_COND));
    }
  pthread_mutex_unlock(&(pool->thpool_status_MUTEX));
}

void snf_thpool_run(SNF_thpool *pool)
{
  if (pool == NULL || pool->thpool_handler == NULL)
    return;
    int re = 0;
    sem_wait(&(pool->thpool_status_sem));
    sem_post(&(pool->thpool_status_sem));
  pthread_mutex_lock(&(pool->thpool_status_MUTEX));
    if(pool->thpool_status != SNF_THPOOL_STOP)
    {
        re = ESHUTDOWN;
    }
    else
    {
        pool->thpool_status = SNF_THPOOL_RUN;
        pthread_cond_broadcast(&(pool->thpool_status_COND));
    }
  pthread_mutex_unlock(&(pool->thpool_status_MUTEX));
}

