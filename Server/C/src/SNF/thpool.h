//////////////////////////////////////////////////////////////
///
/// \brief this file Defines everything related to ThreadPool
/// \file thpool.h
///
/// Thread Pools By Abdelhadi Seddar
/// This implemantation of a Threadpool is by no means perfect
/// nor optimized, because I made it based of a graph depicting
/// how a threadpool works, without seeing any documentation about it,
/// this is just a learning experience for
/// me and im learning through trial and error.
///
/// **UPDATE 0.0.2-alpha**:
/// After research and trial and error, and feedback from
/// other people and after trying it in a project, i realized
/// that i made the implementation wrong, as i used to create
/// each threads whenever there is a `work` to do, which added
/// CPU Time and latency, so after Reading this [Wikipedia
/// Page](https://en.wikipedia.org/wiki/Thread_pool) (Last visited on Augest 2,
/// 2024 at 2:37 PM GMT +1) I noticed that i should've created the threads **At
/// once** and **from the beggining**. so i did just that in this update and it
/// did do wonders after comparing the performance of the two (and doesnt crash
/// as often as 0.0.1-alpha)
/// //////////////////////////////////////////////////////////
#ifndef thpool_h
#define thpool_h

#include <SNF/SNF.h>

typedef struct SNF_ThreadPool_work_t SNF_thpool_work;
typedef struct SNF_ThreadPool_worker_t SNF_thpool_worker;
typedef struct SNF_ThreadPool_t SNF_thpool;

typedef enum SNF_ThreadPool_Status_e {
  /// @brief Used to Signify that the Thread Pool was stopped
  /// or been ordred to.
  /// @warning: Only set this status using snf_thpool_stop
  SNF_THPOOL_STOP,
  /// @brief Used to Signify that the Thread Pool is running
  /// or been ordred to.
  /// @warning: Only set this status using snf_thpool_run
  SNF_THPOOL_RUN,
  /// @brief Used to Signify that the Thread Pool is being ordred to be
  /// destroyied
  /// @warning Only Set this Value by calling snf_thpool_destroy
  SNF_THPOOL_DESTROY,
} SNF_thpool_status;

/// @brief Defines the 'work' needed to do by defining a 'func'tion and it's
/// 'arg'ument
struct SNF_ThreadPool_work_t {
  /// @brief The function to be executed upon
  void *(*func)();
  /// @brief the argument to be given upon execution
  void *arg;
  /// @brief the source Thread Pool
  SNF_thpool *pool;
  /// @brief Saves the next work ( Linked List )
  SNF_thpool_work *next;
};
/// @brief Defines the 'worker'that would handle a certain 'work'
struct SNF_ThreadPool_worker_t {
  /// @brief The worker's thread id
  pthread_t worker;
};
/// @brief The structure for a Thread Pool
struct SNF_ThreadPool_t {
  /// @brief This LinkedList will store the "works" or "jobs"
  SNF_thpool_work *thpool_works;
  /// @brief This is the semaphore for handling
  sem_t thpool_works_sem;
  /// @brief This LinkedList will store the currently working "workers"
  ///         or more exactly the worker thread info
  /// @note Linked List saves \ref SNF_ThreadPool_work_t . See \ref
  /// SNF_ThreadPool_worker_t .
  SNF_thpool_worker *thpool_workers;
  /// @brief Used to synchronize the acces to thpool_works
  /// @warning Must not be touched or the Thread Pool may not work as intended
  pthread_mutex_t thpool_works_MUTEX;
  /// @brief Used to synchronize the acces to thpool_workers
  /// @warning Must not be touched or the Thread Pool may not work as intended
  pthread_mutex_t thpool_workers_MUTEX;
  /// @brief  Used to synchronize if all "works" are handled and all "workers"
  /// had finished their assigned "work"
  ///         See \ref snf_thpool_wait(SNF_thpool *pool)
  /// @note Use \ref snf_thpool_wait(SNF_thpool *pool) .
  pthread_mutex_t thpool_noworks_MUTEX;
  /// @brief Thread that manages "workers" and assign their "work" to them
  pthread_t *thpool_handler;
  /// @brief  The main function that is runs first when the Thread pool has
  /// started
  pthread_t *thpool_main_worker;
  /// @brief  Semaphore used for synchronization;
  ///         Used primarly to notify the conclusion of "workers" to the Thread
  ///         pool handler, and to Limit the amount of "worker" threads with the
  ///         max being thpool_n_workers
  /// @warning Must not be touched or the Thread Pool may not work as intended
  sem_t thpool_workers_sem;
  /// @brief Limiter for the amount of threads that could be created.
  /// @warning Must not be touched or the Thread Pool may not work as intended
  int thpool_n_workers;
  /// @brief  If set = 1 the thpool_handler will stop creating "workers"
  ///         and wait till all already exising "workers" finish their "work"
  /// @note Use \ref snf_thpool_stop(SNF_thpool *pool) .
  SNF_thpool_status thpool_status;
  pthread_mutex_t thpool_status_MUTEX;
  pthread_cond_t thpool_status_COND;
  sem_t thpool_status_sem;
};
/// @brief This defines the amount of **seconds** to wait a `worker`
/// to stop, if the `worker` does not stop during this time period
/// it will be forcefully be stopped.
/// @note Default Value is 1 Second
extern time_t SNF_THPOOL_STOPWAIT;

/// @brief Insitanciates a new thread pool, with a Limiter ( Max_Threads )
/// @param ThreadPool Returns the thread pool by writes the newly created
/// instance in the pointer given in this parameter
/// @param Max_Threads Maximum Total allowed threads ( Must be 2+ if No Main
/// Worker given, else must me 3+ for proper work)
/// @param Main_Worker Main Function that would be called upon finishing the
/// intialisation of the
/// @param arg the argument that would be given to the Main_worker function.
/// @return 0 On Success || -1 On fail { Shall fail only if 1 - ThreadPool is
/// NULL || 2 - Max_Thread is below recommended/required value }
extern int snf_thpool_inis(SNF_thpool **ThreadPool, int Max_Threads,
                           void *(*Main_Worker)(void *), void *arg);

/// @brief Creates a "work" that will call the *func* function with argument
/// *arg*
/// @param pool The Thead Pool to be operated on.
/// @param func The function that would be called once a "worker" picks up the
/// "work".
/// @param arg argument to be given to the *func* upon call.
extern void snf_thpool_addwork(SNF_thpool *pool, void *(*func)(), void *arg);

/// @brief Blocks current thread until the Thread Pool's last changed status has
/// taken effect
/// @param pool The Thead Pool to be operated on.
extern void snf_thpool_wait(SNF_thpool *pool);

/// @brief Will Block till the Main_worker finishes
/// @param pool The Thead Pool to be operated on.
extern void snf_thpool_join(SNF_thpool *pool);

/// @brief will order the Thread Pool to stop.
/// @note you can block till the Thread Pool has stopped
/// @param pool The Thead Pool to be operated on.
extern void snf_thpool_stop(SNF_thpool *pool);
extern void snf_thpool_run(SNF_thpool *pool);
/// @brief
/// @param pool
/// @private
extern void snf_thpool_destroy(SNF_thpool **pool);
#endif
