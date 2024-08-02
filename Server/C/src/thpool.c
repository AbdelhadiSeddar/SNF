#include <SNF/thpool.h>

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
    while(!tp->stop)
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
    sem_wait(&(pool->thpool_workers_sem));
    sem_post(&(pool->thpool_workers_sem));
        
    sem_wait(&(pool->stop_sem));
    //TODO: Destroy properly
    pthread_mutex_destroy(&(pool->thpool_works_MUTEX));

    pthread_mutex_destroy(&(pool->thpool_workers_MUTEX));
    sem_destroy(&(pool->thpool_workers_sem));
    bzero(pool, sizeof(SNF_thpool));
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
    sem_init(&(new->thpool_workers_sem), 0, 0);
    sem_init(&(new->thpool_works_sem), 0, 0);
    sem_init(&(new->stop_sem), 0, 0);
    (new->thpool_handler) = malloc(sizeof(pthread_t));
    (new->thpool_main_worker) = malloc(sizeof(pthread_t));
    (new->thpool_works) = NULL;
    (new->thpool_n_workers) = Max_Threads - 1;
    (new->thpool_workers) = calloc((new->thpool_n_workers), sizeof(struct SNF_ThreadPool_worker_t));
    (new->stop) = 0;

    (*ThreadPool) = new;

    for(int i = 0; i < (new->thpool_n_workers); i++)
    {
        pthread_create(
            &((new->thpool_workers)[i]).worker,
            NULL,
            thpool_work_wrapper,
            new
        );
        sem_post(&(new->thpool_workers_sem));
    }

    pthread_create(
        (new->thpool_handler),
        NULL,
        thpool_handler,
        new);
    if (Main_Worker != NULL)
    {
        sem_wait(&(new->thpool_workers_sem));
        sem_post(&(new->thpool_workers_sem));
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
    sem_post(&(pool)->thpool_works_sem);
}
void snf_thpool_wait(SNF_thpool *pool)
{
    if (pool == NULL || pool->thpool_handler == NULL)
        return;
}
void snf_thpool_join(SNF_thpool *pool)
{
    if (pool == NULL || pool->thpool_handler == NULL)
        return;
    pthread_join(*(pool->thpool_handler), NULL);
}
void snf_thpool_stop(SNF_thpool *pool)
{
  if (pool == NULL || pool->thpool_handler == NULL)
    return;
  pool->stop = 1;
  
  sem_post(&(pool->stop_sem));
}
