#include <SNF.h>
#include <SNF/thpool.h>
#include <semaphore.h> 
#include <time.h>

#define LIMIT 500000
SNF_thpool *POOL;
sem_t sem;
sem_t end;

_Atomic int Finished_Jobs = 0;

void *work(void *arg)
{
    uint32_t *i = (uint32_t *)arg;
    int f = 0;
    for(int u = 0 ; u < *i ; u++)
    {
        f+= u;
    }
    //printf("F : %d \n", f);
    Finished_Jobs++;
    sem_post(&end);
    free(i);
    return NULL;
}
void *worker(void *arg)
{
    sem_init(&sem, 0, 0);
    sem_init(&end, 0, 0);
    for (uint32_t i = 0; i < LIMIT ; i += 5)
    {
        uint32_t *u = calloc(1, sizeof(uint32_t));
        *u = i;
        snf_thpool_addwork(POOL, work, (void *)u);
    }

  return NULL;
}

int main()
{
    clock_t Start_Time = clock();
    snf_thpool_inis(&POOL, 25, worker, NULL);
    
    int amount_jobs = LIMIT /5 ;
    while( Finished_Jobs < amount_jobs)
    {
        printf("--- Finished Jobs: %d/%d (%d %% ) | Elapsed Time : %.2fs \n",
            Finished_Jobs,
            amount_jobs,
            (Finished_Jobs *100)/amount_jobs,
            ((float)((double) clock() - Start_Time) / CLOCKS_PER_SEC)
            );
        sleep(1);
    }
    clock_t end = clock();
    snf_thpool_stop(POOL);

    printf("--- Finished %d Jobs. Time : %.2fs \n",
        amount_jobs,
        ((float)((double) end - Start_Time) / CLOCKS_PER_SEC)
    );
}
