#include <SNF.h>
#include <SNF/thpool.h>

#define LIMIT 124511484UL
thpool *POOL;

void *work(void *arg)
{
    uint32_t i = *(int *)arg;
    while (i < 500)
    {
        i++;
    }
}

void *worker(void *arg)
{
    printf("Stating Workers\n");
    for (int i = 0; i < 500 ; i += 5)
        snf_thpool_addwork(POOL, work, (void *)&i);
    snf_thpool_wait(POOL);
  printf("Workers Finished\n");
  snf_thpool_stop(POOL);
}

int main()
{
    snf_thpool_inis(&POOL, 4, worker, NULL);

    printf("Awaiting JOIN\n");
    snf_thpool_join(POOL);
    printf("JOINED\n");
}
