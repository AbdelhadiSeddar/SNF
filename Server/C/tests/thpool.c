#include <SNF.h>
#include <SNF/thpool.h>
#include <semaphore.h>

#define LIMIT 5000000
SNF_thpool *POOL;
sem_t sem;
sem_t end;

void *work(void *arg)
{
    uint32_t *i = (uint32_t *)arg;
    int f = 0;
    for(int u = 0 ; u < *i ; u++)
    {
        f+= u;
    }
    printf("F : %d \n", f);
    sem_post(&end);
    free(i);
}
void *worker(void *arg)
{
    sem_init(&sem, 0, 0);
    sem_init(&end, 0, 0);
    printf("Stating Workers\n");
    for (uint32_t i = 0; i < LIMIT ; i += 5)
    {
        uint32_t *u = calloc(1, sizeof(uint32_t));
        *u = i;
        snf_thpool_addwork(POOL, work, (void *)u);
    }
    for (uint32_t i = 0; i < LIMIT ; i += 5)
    {
        sem_post(&sem);
    }
    for (int i = 0; i < LIMIT ; i += 5)
    {
        sem_wait(&end);
    }
  printf("Workers Finished\n");
  snf_thpool_stop(POOL);
}

int main()
{
    snf_thpool_inis(&POOL, 25, worker, NULL);

    printf("Awaiting JOIN\n");
    snf_thpool_join(POOL);
    printf("JOINED\n");
}
