#include <pthread.h>
#include "../../include/globals.h"
#include "../../include/synchronization/condition_variables.h"

pthread_cond_t ball_cond;
pthread_cond_t match_start_cond;

void init_condition_vars(void)
{
    pthread_cond_init(&ball_cond,        NULL);
    pthread_cond_init(&match_start_cond, NULL);
}

void destroy_condition_vars(void)
{
    pthread_cond_destroy(&ball_cond);
    pthread_cond_destroy(&match_start_cond);
}
