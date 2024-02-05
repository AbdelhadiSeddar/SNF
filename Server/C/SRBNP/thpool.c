#include "_Imports.h"

/// @brief Broadcasts thpool_noworks_cond if there is no appending "works" and no currently working "workers"
/// @param pool The Thread pool to be operated on.
void thpool_Check_nowork(thpool *pool)
{
    if (!(pool->thpool_n_works))
    {
        pthread_mutex_lock(&(pool->thpool_workers_MUTEX));
        if (pool->thpool_workers == NULL)
        {
            pthread_cond_broadcast(&(pool->thpool_noworks_cond));
        }
        pthread_mutex_unlock(&(pool->thpool_workers_MUTEX));
    }
}
/// @brief Inserts a new Worker into the Thead Pool's workers pile
/// @param pool The Thead Pool to be operated on.
/// @return returns the inserted worker.
thpool_worker *thpool_insertnew_worker(thpool *pool)
{
    if (pool == NULL)
        return NULL;
    thpool_worker *worker = calloc(1, sizeof(thpool_worker));
    worker->worker = calloc(1, sizeof(pthread_t));
    pthread_mutex_lock(&(pool->thpool_workers_MUTEX));
    thpool_worker *tmp = pool->thpool_workers;
    if (tmp == NULL)
    {
        pool->thpool_workers = worker;
    }
    else
    {
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = worker;
    }
    pthread_mutex_unlock(&(pool->thpool_workers_MUTEX));
    return worker;
}
/// @brief Removes a worker from the Thead Pool's workers pile.
/// @param pool The Thead Pool to be operated on.
/// @param worker  The Worker to be removed.
void thpool_remove_worker(thpool *pool, thpool_worker *worker)
{
    if (pool == NULL || worker == NULL)
        return;
    pthread_mutex_lock(&(pool->thpool_workers_MUTEX));
    thpool_worker *tmp = pool->thpool_workers;
    if (tmp != NULL)
    {
        if (tmp == worker)
        {
            pool->thpool_workers = worker->next;
            goto end_thpool_remove_worker;
        }
        while (tmp->next != NULL)
        {
            if (tmp == worker)
            {
                tmp->next = worker->next;
                goto end_thpool_remove_worker;
            }
            tmp = worker->next;
        }
    }
end_thpool_remove_worker:;
    pthread_mutex_unlock(&(pool->thpool_workers_MUTEX));
    free(worker);
    return;
}
/// @brief Frees a worker
/// @param work  The Worker to be freed
void thpool_work_free(thpool_work *work)
{
    free(work);
}
/// @brief Pops a work from the "works" Pile
/// @param pool  The Thread pool to be operated on.
/// @return Poped Work
thpool_work *thpool_pop_work(thpool *pool, thpool_worker *worker)
{
    thpool_work *work = pool->thpool_works;
    pool->thpool_n_works--;
    pool->thpool_works = pool->thpool_works->next;
    work->worker = worker;
    return work;
}
/// @brief Serves as a Wrapper to the "work"
/// @param arg Must be of Type [ thpool_work * ]
/// @return
void *thpool_work_wrapper(void *arg)
{
    thpool_work *work = (thpool_work *)arg;
    void *re = (work->func)(work->arg);
    thpool_remove_worker(work->pool, work->worker);
    thpool_Check_nowork(work->pool);
    sem_post(&(work->pool->thpool_workers_sem));
    thpool_work_free(work);
    return re;
}
void *thpool_handler(void *arg)
{
    thpool *pool = arg;
    while (!(pool->stop))
    {
        sem_wait(&(pool->thpool_workers_sem));
        thpool_Check_nowork(pool);
        pthread_mutex_lock(&(pool->thpool_works_MUTEX));
        if (!(pool->thpool_n_works))
            pthread_cond_wait(&(pool->thpool_works_cond), &(pool->thpool_works_MUTEX));
        thpool_worker *worker = thpool_insertnew_worker(pool);
        if (worker != NULL)
            pthread_create(
                (worker->worker),
                NULL,
                thpool_work_wrapper,
                thpool_pop_work(pool, worker));
        pthread_mutex_unlock(&(pool->thpool_works_MUTEX));
    }
    thpool_wait(pool);
    pthread_mutex_destroy(&(pool->thpool_works_MUTEX));

    pthread_mutex_destroy(&(pool->thpool_workers_MUTEX));
    pthread_cond_destroy(&(pool->thpool_works_cond));
    sem_destroy(&(pool->thpool_workers_sem));
    bzero(pool, sizeof(thpool));
    return NULL;
}
int inis_thpool(thpool **ThreadPool, int Max_Threads, void *(*Main_Worker)(), void *arg)
{
    if (ThreadPool == NULL || Max_Threads < (Main_Worker == NULL ? 2 : 3))
        return -1;
    thpool *new = calloc(1, sizeof(thpool));
    pthread_mutex_init(&(new->thpool_works_MUTEX), NULL);
    pthread_mutex_init(&(new->thpool_workers_MUTEX), NULL);
    pthread_mutex_init(&(new->thpool_noworks_MUTEX), NULL);
    pthread_cond_init(&(new->thpool_works_cond), NULL);
    pthread_cond_init(&(new->thpool_noworks_cond), NULL);
    sem_init(&(new->thpool_workers_sem), 0, Max_Threads - 1);
    (new->thpool_handler) = malloc(sizeof(pthread_t));
    (new->thpool_main_worker) = malloc(sizeof(pthread_t));
    (new->thpool_works) = NULL;
    (new->max_workers_count) = Max_Threads - 1;
    (new->thpool_n_works) = 0;
    (new->stop) = 0;

    (*ThreadPool) = new;

    pthread_create(
        (new->thpool_handler),
        NULL,
        thpool_handler,
        new);
    if (Main_Worker != NULL)
    {
        sem_wait(&(new->thpool_workers_sem));
        pthread_create(
            (new->thpool_main_worker),
            NULL,
            Main_Worker,
            arg);
    }
    return 0;
}
void thpool_addwork(thpool *pool, void *(*func)(), void *arg)
{
    thpool_work *work = calloc(1, sizeof(thpool_work));
    work->arg = arg;
    work->func = func;
    work->pool = pool;

    pthread_mutex_lock(&(pool->thpool_works_MUTEX));
    thpool_work *tmp = pool->thpool_works;
    if (tmp == NULL)
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
    (pool->thpool_n_works)++;
    pthread_cond_broadcast(&(pool->thpool_works_cond));
    pthread_mutex_unlock(&(pool->thpool_works_MUTEX));
}
void thpool_wait(thpool *pool)
{
    pthread_mutex_lock(&(pool->thpool_noworks_MUTEX));
    if ((pool->thpool_n_works))
        pthread_cond_wait(
            &(pool->thpool_noworks_cond),
            &(pool->thpool_noworks_MUTEX));
    pthread_mutex_unlock(&(pool->thpool_noworks_MUTEX));
}
void thpool_join(thpool *pool)
{
    if (pool == NULL || pool->thpool_handler == NULL)
        return;
    pthread_join(*(pool->thpool_handler), NULL);
}
void thpool_stop(thpool *pool)
{
    pool->stop = 1;
    thpool_wait(pool);
    thpool_join(pool);
}
