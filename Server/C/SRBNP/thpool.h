#ifndef thpool_h
#define thpool_h
#include <pthread.h>
#include <semaphore.h>

typedef struct thpool_work_t {
    void*           (*func)();
    void*           arg;
    thpool*         pool;
    thpool_work*    next;
} thpool_work;

typedef struct thpool_worker_t {
    pthread_t*      worker;
    thpool_worker*  next;
} thpool_worker;

typedef struct thpool_t {
    thpool_work*    thpool_works;
    thpool_worker*  thpool_workers;
    pthread_mutex_t thpool_workers_MUTEX;
    pthread_mutex_t thpool_works_MUTEX;
    pthread_cond_t  thpool_works_cond;
    pthread_t       thpool_handler;
    pthread_t       thpool_main_worker;
    sem_t           thpool_sem;
    int             max_workers_count;
    int             stop;
} thpool;

extern thpool *inis_thpool(int Max_Threads, void *(*Main_Worker)(), void *arg);

extern void thpool_addwork(thpool *pool, void *(*func)(), void *arg);
extern void thpool_stop(thpool* pool);
extern void thpool_wait(thpool* pool);
#endif