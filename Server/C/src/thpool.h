/*
#   Thread Pools By Abdelhadi Seddar
#   This implemantation of a Threadpool is by no means perfect
#   nor optimized, because I made it based of a graph depicting
#   how a threadpool works, without seeing any documentation about it,
#   this is just a learning experience for
#   me and im learning through trial and error
*/
#ifndef thpool_h
#define thpool_h

#include <SNF/SNF.h>

typedef struct SNF_ThreadPool_work_t SNF_thpool_work;
typedef struct SNF_ThreadPool_worker_t SNF_thpool_worker;
typedef struct SNF_ThreadPool_t thpool;

/// @brief Defines the 'work' needed to do by defining a 'func'tion and it's 'arg'ument
struct SNF_ThreadPool_work_t
{
    //  The function to be execute
    void *(*func)();
    //  the argument to be given upon execution
    void *arg;
    //  the source Thread Pool
    thpool *pool;
    //  The handling worker
    SNF_thpool_worker *worker;
    //  Saves the next work ( Linked List )
    SNF_thpool_work *next;
};
/// @brief Defines the 'worker'that would handle a certain 'work'
struct SNF_ThreadPool_worker_t
{
    //  Saves the worker's thread id
    pthread_t *worker;
    //  Pointer for Linked list
    SNF_thpool_worker *next;
};
/// @brief Defines the ThreadPool itself
struct SNF_ThreadPool_t
{
    // This LinkedList will store the "works" or "jobs"
    SNF_thpool_work *thpool_works;
    //  Defines current amount of currently awaiting "works"
    _Atomic int thpool_n_works;
    // This LinkedList will store the currently working "workers"
    // or more exactly the worker thread info
    // ( See SNF_ThreadPool_worker_t definition )
    SNF_thpool_worker *thpool_workers;
    //  Used to synchronize the acces to thpool_works
    //  [ NOTE : Must not be touched or the Thread Pool may not work as intended ]
    pthread_mutex_t thpool_works_MUTEX;
    //  Used to synchronize the acces to thpool_workers
    //  [ NOTE : Must not be touched or the Thread Pool may not work as intended ]
    pthread_mutex_t thpool_workers_MUTEX;
    //  Used to synchronize if all "works" are handled and all "workers" had finished their assigned "work"
    //  (See thpool_wait(..) )
    //  [ NOTE : Use thpool_wait() ]
    pthread_mutex_t thpool_noworks_MUTEX;
    //  Used to synchronize if all "works" are handled and all "workers" had finished their assigned "work"
    //  (See thpool_wait(..) )
    //  [ NOTE : Use thpool_wait() ]
    pthread_cond_t thpool_noworks_cond;
    //  Used to synchronize the acces to thpool_works
    //  [ NOTE : Must not be touched or the Thread Pool may not work as intended ]
    pthread_cond_t thpool_works_cond;
    //  Thread that manages "workers" and assign their "work" to them
    pthread_t *thpool_handler;
    //  The main function that is runs first when the Thread pool has started
    pthread_t *thpool_main_worker;
    //  Semaphore used for synchronization;
    //  Used primarly to notify the conclusion of "workers" to the Thread pool handler,
    //  and to Limit the amount of "worker" threads with the max being max_workers_count
    //  [ NOTE : Must not be touched or the Thread Pool may not work as intended ]
    sem_t thpool_workers_sem;
    //  Limiter for the amount of threads that could be created.
    //  [ NOTE : Must not be touched after the initialisation ]
    int max_workers_count;
    //  If set = 1 the thpool_handler will stop creating "workers"
    //  and wait till all already exising "workers" finish their "work"
    _Atomic int stop;
};

/// @brief Insitanciates a new thread pool, with a Limiter ( Max_Threads )
/// @param ThreadPool Returns the thread pool by writes the newly created instance in the pointer given in this parameter
/// @param Max_Threads Maximum Total allowed threads ( Must be 2+ if No Main Worker given, else must me 3+ for proper work)
/// @param Main_Worker Main Function that would be called upon finishing the intialisation of the
/// @param arg the argument that would be given to the Main_worker function.
/// @return 0 On Success || -1 On fail { Shall fail only if 1 - ThreadPool is NULL || 2 - Max_Thread is below recommended/required value }
extern int snf_thpool_inis(thpool **ThreadPool, int Max_Threads, void *(*Main_Worker)(), void *arg);

/// @brief Creates a "work" that will call the *func* function with argument *arg*
/// @param pool The Thead Pool to be operated on.
/// @param func The function that would be called once a "worker" picks up the "work".
/// @param arg argument to be given to the *func* upon call.
extern void snf_thpool_addwork(thpool *pool, void *(*func)(), void *arg);

/// @brief Blocks current thread until there is No "Worker" is working and no "work" is waiting in queue
/// @param pool The Thead Pool to be operated on.
extern void snf_thpool_wait(thpool *pool);

/// @brief Will Block till the Main_worker finishes
/// @param pool The Thead Pool to be operated on.
extern void snf_thpool_join(thpool *pool);

/// @brief will stop creating "workers" and wait till all already exising "workers" finish their "work"
/// @param pool The Thead Pool to be operated on.
extern void snf_thpool_stop(thpool *pool);
#endif
