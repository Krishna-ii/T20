#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../../include/globals.h"
#include "../../include/match/events.h"

void *fielder_thread(void *arg)
{
    int fielder_id = *(int *)arg;
    free(arg);

    int is_keeper = (fielder_id == 9);

    pthread_mutex_lock(&pitch_mutex);
    while (!match_started)
        pthread_cond_wait(&ball_cond, &pitch_mutex);
    pthread_mutex_unlock(&pitch_mutex);

    while (1)
    {
        pthread_mutex_lock(&pitch_mutex);

        while (!ball_in_air && match_running)
            pthread_cond_wait(&ball_cond, &pitch_mutex);

        if (!match_running)
        {
            pthread_mutex_unlock(&pitch_mutex);
            break;
        }

        if (!ball_in_air)
        {
            pthread_mutex_unlock(&pitch_mutex);
            continue;
        }
        ball_in_air = 0;

        int catch_prob = is_keeper ? 70 : 55;   
        int caught     = (rand() % 100) < catch_prob;

        if (is_keeper)
            printf("  [Wicket Keeper] Attempts catch...");
        else
            printf("  [Fielder %d] Attempts catch...", fielder_id);

        if (caught)
        {
            printf(" CAUGHT! Batsman OUT.\n");
            ball_event = WICKET;
        }
        else
        {
            printf(" DROPPED! Ball to ground.\n");
            ball_event = DOT;
        }

        ball_ready = 1;
        pthread_cond_broadcast(&ball_cond);
        pthread_mutex_unlock(&pitch_mutex);

        usleep(200000);  
    }

    return NULL;
}
