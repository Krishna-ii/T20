#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "../../include/globals.h"
#include "../../include/match/match_engine.h"
#include "../../include/scheduler/scheduler.h"
#include "../../include/utils/time_utils.h"

void start_match(int overs)
{
    printf("\n");
    printf("  T20 CRICKET SIMULATOR  —  %d overs\n", overs);
    printf("  Scheduler : %s\n", scheduler_name());
    printf("\n\n");

    current_over = 0;
    current_ball = 0;
    match_started = 1;
    match_start_ms = get_time_ms();

    pthread_mutex_lock(&pitch_mutex);
    pthread_cond_broadcast(&ball_cond);
    pthread_mutex_unlock(&pitch_mutex);

    while (1)
    {
        usleep(80000);

        pthread_mutex_lock(&pitch_mutex);

        if (current_innings == 2 && global_score >= innings_score[0] + 1)
        {
            printf("\n  TARGET ACHIEVED!\n");
            printf("  Score: %d/%d\n\n", global_score, wickets);

            match_running = 0;
            pthread_cond_broadcast(&ball_cond);
            pthread_mutex_unlock(&pitch_mutex);
            return;
        }
        if (wickets >= 10)
        {
            printf("\n  ALL OUT for %d    \n", global_score);
            match_running = 0;
            pthread_cond_broadcast(&ball_cond);
            pthread_mutex_unlock(&pitch_mutex);
            return;
        }

        if (current_ball >= 6)
        {
            current_ball = 0;
            current_over++;

            printf("\n\n");
            printf("  End of Over %d  |  Score: %d/%d\n",
                   current_over, global_score, wickets);
            printf("  %s stats: %d balls, %d runs, %d wkts\n",
                   (current_bowler < 5 && active_bowlers) ? active_bowlers[current_bowler] : "Bowler",
                   bowler_balls_bowled[current_bowler],
                   bowler_runs_conceded[current_bowler],
                   bowler_wickets[current_bowler]);
            printf("\n\n");

            if (current_over >= overs)
            {
                printf("   MATCH FINISHED   \n");
                printf("    Final Score: %d/%d in %d overs\n\n", global_score, wickets, overs);
                match_running = 0;
                pthread_cond_broadcast(&ball_cond);
                pthread_mutex_unlock(&pitch_mutex);
                return;
            }

            if (current_innings == 2)
            {
                int target = innings_score[0] + 1;
                int runs_required = target - global_score;

                if (runs_required <= 10 && match_intensity != INTENSITY_HIGH)
                {
                    match_intensity = INTENSITY_HIGH;

                    printf("\nOnly %d runs needed.\n", runs_required);
                    printf("THIRD UMPIRE: Match intensity HIGH.\n");
                    printf("Captain: Priority scheduler activated.\n\n");
                }
            }

            if (current_over == overs - 1 && match_intensity != INTENSITY_HIGH)
            {
                match_intensity = INTENSITY_HIGH;
                printf("THIRD UMPIRE:Last over! Match intensity HIGH.\n");
                printf("Captain: Priority scheduler activated.\n\n");
            }

            scheduler_next();
        }

        pthread_mutex_unlock(&pitch_mutex);
    }
}
