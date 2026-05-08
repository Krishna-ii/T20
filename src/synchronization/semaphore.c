#include <semaphore.h>
#include "../../include/globals.h"
#include "../../include/synchronization/semaphore.h"

sem_t crease_sem;

void init_semaphores(void)
{
    sem_init(&crease_sem, 0, 2);
}

void destroy_semaphores(void)
{
    sem_destroy(&crease_sem);
}
