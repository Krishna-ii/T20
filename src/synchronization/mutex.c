#include <pthread.h>
#include "../../include/globals.h"
#include "../../include/synchronization/mutex.h"

pthread_mutex_t pitch_mutex;
pthread_mutex_t score_mutex;
pthread_mutex_t log_mutex;
pthread_mutex_t end1_mutex;
pthread_mutex_t end2_mutex;

void init_mutexes(void)
{
    pthread_mutex_init(&pitch_mutex, NULL);
    pthread_mutex_init(&score_mutex, NULL);
    pthread_mutex_init(&log_mutex,   NULL);
    pthread_mutex_init(&end1_mutex,  NULL);
    pthread_mutex_init(&end2_mutex,  NULL);
}

void destroy_mutexes(void)
{
    pthread_mutex_destroy(&pitch_mutex);
    pthread_mutex_destroy(&score_mutex);
    pthread_mutex_destroy(&log_mutex);
    pthread_mutex_destroy(&end1_mutex);
    pthread_mutex_destroy(&end2_mutex);
}
