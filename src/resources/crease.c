#include "../../include/globals.h"
#include "../../include/resources/crease.h"

void enter_crease(void)
{
    sem_wait(&crease_sem);
}

void leave_crease(void)
{
    sem_post(&crease_sem);
}
