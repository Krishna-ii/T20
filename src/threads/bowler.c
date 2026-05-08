#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "../../include/globals.h"
#include "../../include/resources/pitch.h"
#include "../../include/match/ball_generator.h"
#include "../../include/match/events.h"
#include "../../include/match/match_engine.h"

void *bowler_thread(void *arg)
{
    (void)arg;

    pthread_mutex_lock(&pitch_mutex);
    while (!match_started)
        pthread_cond_wait(&ball_cond, &pitch_mutex);
    pthread_mutex_unlock(&pitch_mutex);

    while (1)
    {
        pthread_mutex_lock(&pitch_mutex);

        while (ball_ready && match_running)
        {
            pthread_mutex_unlock(&pitch_mutex);
            usleep(5000);
            pthread_mutex_lock(&pitch_mutex);
        }

        if (!match_running)
        {
            pthread_cond_broadcast(&ball_cond);
            pthread_mutex_unlock(&pitch_mutex);
            break;
        }

        printf("\n%d.%d  %s running up...\n",
               current_over, current_ball + 1,
               (current_bowler < 5 && active_bowlers) ? active_bowlers[current_bowler] : "Bowler");

        ball_event = generate_ball_event();

        int is_legal = (ball_event != WIDE && ball_event != NO_BALL);
        if (is_legal)
            current_ball++;

        if (is_legal)
            bowler_balls_bowled[current_bowler]++;

        if (!is_legal)
            bowler_runs_conceded[current_bowler]++;

        if (ball_event == BALL_IN_AIR)
        {
            ball_in_air = 1;
            pthread_cond_broadcast(&ball_cond);
            pthread_mutex_unlock(&pitch_mutex);
            sleep(1);
            continue;
        }

        ball_ready = 1;
        pthread_cond_broadcast(&ball_cond);
        pthread_mutex_unlock(&pitch_mutex);

        sleep(1);   
    }

    return NULL;
}
