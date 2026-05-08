#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../../include/globals.h"
#include "../../include/resources/scoreboard.h"
#include "../../include/resources/crease.h"
#include "../../include/match/events.h"
#include "../../include/match/match_engine.h"
#include "../../include/logging/data_logger.h"
#include "../../include/utils/time_utils.h"
#include "../../include/scheduler/scheduler.h"

void *batsman_thread(void *arg)
{
    int thread_id = *(int *)arg;
    free(arg);

    enter_crease();

    pthread_mutex_lock(&pitch_mutex);
    while (!match_started)
        pthread_cond_wait(&ball_cond, &pitch_mutex);
    pthread_mutex_unlock(&pitch_mutex);

    while (1)
    {
        pthread_mutex_lock(&pitch_mutex);

        int my_id = (thread_id == 0) ? striker : non_striker;

        while (!ball_ready && match_running)
        {
            pthread_mutex_unlock(&pitch_mutex);
            usleep(5000);
            pthread_mutex_lock(&pitch_mutex);
            my_id = (thread_id == 0) ? striker : non_striker;
        }

        if (!match_running)
        {
            pthread_cond_broadcast(&ball_cond);
            pthread_mutex_unlock(&pitch_mutex);
            break;
        }

        if (my_id != striker)
        {
            pthread_mutex_unlock(&pitch_mutex);
            usleep(5000);
            continue;
        }

        if (start_time[my_id] == 0)
        {
            start_time[my_id] = get_time_ms() - match_start_ms;
            wait_time[my_id] = start_time[my_id] - arrival_time[my_id];
        }

        printf("  Striker: %s  vs  %s\n",
               (my_id < 11 && active_batsmen) ? active_batsmen[my_id] : "???",
               (current_bowler < 5 && active_bowlers) ? active_bowlers[current_bowler] : "???");

        int runs_this_ball = 0;
        int wicket_fell    = 0;

        switch (ball_event)
        {
            case DOT:
                print_event(DOT);
                break;

            case SINGLE:
                add_runs(1);
                batsman_runs[my_id]           += 1;
                bowler_runs_conceded[current_bowler] += 1;
                runs_this_ball = 1;
                print_event(SINGLE);
                break;

            case DOUBLE:
                add_runs(2);
                batsman_runs[my_id]           += 2;
                bowler_runs_conceded[current_bowler] += 2;
                runs_this_ball = 2;
                print_event(DOUBLE);
                break;

            case FOUR:
                add_runs(4);
                batsman_runs[my_id]           += 4;
                bowler_runs_conceded[current_bowler] += 4;
                runs_this_ball = 4;
                print_event(FOUR);
                break;

            case SIX:
                add_runs(6);
                batsman_runs[my_id]           += 6;
                bowler_runs_conceded[current_bowler] += 6;
                runs_this_ball = 6;
                print_event(SIX);
                break;

            case WIDE:
                add_runs(1);
                print_event(WIDE);
                break;

            case NO_BALL:
                add_runs(1);
                print_event(NO_BALL);
                break;

            case WICKET:
            {
                add_wicket();
                bowler_wickets[current_bowler]++;
                print_event(WICKET);
                printf("  %s is OUT! (Runs: %d)\n",
                       (my_id < 11 && active_batsmen) ? active_batsmen[my_id] : "Batsman",
                       batsman_runs[my_id]);

                wicket_fell = 1;

                leave_crease();

                int next = scheduler_next_batsman();

                if (next < MAX_BATSMEN)
                {
                    enter_crease();
                    printf("  %s walks to the crease.\n",
                           (next < 11 && active_batsmen) ? active_batsmen[next] : "Next batsman");
                }
                else
                {
                    printf("  No more batsmen — innings over.\n");
                }

                striker = next;
                break;
            }

            default:
                break;
        }

        if (!wicket_fell && (runs_this_ball % 2 == 1))
        {
            int tmp     = striker;
            striker     = non_striker;
            non_striker = tmp;
            printf("  Ends crossed — new striker: %s\n",
                   (striker < 11 && active_batsmen) ? active_batsmen[striker] : "???");
        }

        if (gantt_count < MAX_BALLS)
        {
            BallRecord *rec  = &gantt_log[gantt_count++];
            rec->ball_number = total_balls++;
            rec->over = current_over;
            rec->ball_in_over = current_ball;
            rec->bowler = current_bowler;
            rec->striker_id = my_id;
            rec->event = ball_event;
            rec->score_after = global_score;
            rec->wickets_after = wickets;
            rec->timestamp_ms = get_time_ms() - match_start_ms;
            rec->innings = current_innings;
            rec->batting_team_id = batting_team;
        }


        printf("Score: %d/%d\n", get_score(), get_wickets());

        ball_ready = 0;
        pthread_cond_broadcast(&ball_cond);
        pthread_mutex_unlock(&pitch_mutex);
    }

    return NULL;
}