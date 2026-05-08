#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/globals.h"
#include "../include/synchronization/mutex.h"
#include "../include/synchronization/semaphore.h"
#include "../include/synchronization/condition_variables.h"
#include "../include/resources/scoreboard.h"
#include "../include/resources/crease.h"
#include "../include/match/match_engine.h"
#include "../include/threads/bowler.h"
#include "../include/threads/batsman.h"
#include "../include/threads/fielder.h"
#include "../include/scheduler/scheduler.h"
#include "../include/logging/data_logger.h"
#include "../include/utils/time_utils.h"
#include "../include/deadlock/deadlock.h"
#include "../include/match/events.h"

#define NUM_OVERS 10
#define NUM_FIELDERS 10

static void reset_globals(void)
{
    global_score = wickets = current_over = current_ball = total_balls = 0;
    match_started = 0;
    match_running = 1;
    match_intensity = INTENSITY_NORMAL;

    ball_in_air = ball_event = ball_ready = 0;

    current_bowler = striker = 0;
    non_striker = 1;
    next_batsman_index = 2;
    current_batsman = 0;

    memset(bowler_balls_bowled, 0, sizeof(bowler_balls_bowled));
    memset(bowler_runs_conceded, 0, sizeof(bowler_runs_conceded));
    memset(bowler_wickets, 0, sizeof(bowler_wickets));
    memset(batsman_runs, 0, sizeof(batsman_runs));
    memset(start_time, 0, sizeof(start_time));
    memset(wait_time, 0, sizeof(wait_time));

    gantt_count = 0;
    match_start_ms = 0;
}

static void init_batsmen(void)
{
    for (int i = 0; i < MAX_BATSMEN; i++)
    {
        if (i <= 1) expected_balls[i] = rand() % 20 + 20;
        else if (i <= 5) expected_balls[i] = rand() % 20 + 10;
        else expected_balls[i] = rand() % 10 + 1;

        arrival_time[i] = 0;
    }
}

static void run_innings(scheduler_type stype,
                        const char **bat_names,
                        const char **bowl_names)
{
    reset_globals();
    init_batsmen();

    active_batsmen = bat_names;
    active_bowlers = bowl_names;

    init_mutexes();
    init_semaphores();
    init_condition_vars();
    scheduler_init(stype);

    pthread_t bowler;
    pthread_t batsmen[2];
    pthread_t fielders[NUM_FIELDERS];

    pthread_create(&bowler, NULL, bowler_thread, NULL);

    for (int i = 0; i < 2; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&batsmen[i], NULL, batsman_thread, id);
    }

    for (int i = 0; i < NUM_FIELDERS; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&fielders[i], NULL, fielder_thread, id);
    }

    start_match(NUM_OVERS);

    pthread_join(bowler, NULL);
    for (int i = 0; i < 2; i++) pthread_join(batsmen[i], NULL);
    for (int i = 0; i < NUM_FIELDERS; i++) pthread_join(fielders[i], NULL);

    innings_score[current_innings - 1] = global_score;
    innings_wickets[current_innings - 1] = wickets;

    write_gantt_csv();
    
    destroy_semaphores();
    destroy_condition_vars();
    destroy_mutexes();
}

static void print_match_result(void)
{
    int ind_score = innings_score[0];
    int ind_wkts = innings_wickets[0];
    int aus_score = innings_score[1];
    int aus_wkts = innings_wickets[1];

    printf("\nMatch Result\n");
    printf("India     : %d / %d (FCFS)\n", ind_score, ind_wkts);
    printf("Australia : %d / %d (SJF)\n", aus_score, aus_wkts);

    if(aus_score > ind_score)
        printf("Australia wins by %d run%s\n",
               aus_score - ind_score,
               (aus_score - ind_score == 1) ? "" : "s");
    else if (ind_score > aus_score)
        printf("India wins by %d run%s\n",
               ind_score - aus_score,
               (ind_score - aus_score == 1) ? "" : "s");
    else printf("Match tied\n");
}

int main(int argc, char *argv[])
{
    printf("CRICKET OS SIMULATOR\n\n");

    scheduler_type bowl_sched = RR;

    if (argc < 2)
    {
        printf("Usage: ./simulator <RR|PRIORITY>\n");
        printf("Innings 1: FCFS (India)\n");
        printf("Innings 2: SJF (Australia)\n\n");
    }

    if (argc >= 2)
    {
        if (strcmp(argv[1], "RR") == 0) bowl_sched = RR;
        else if (strcmp(argv[1], "PRIORITY") == 0) bowl_sched = PRIORITY;
    }

    printf("Configuration:\n");
    printf("Bowling Scheduler: %s\n",
           bowl_sched == RR ? "Round-Robin" : "Priority");
    printf("Innings 1: FCFS (India)\n");
    printf("Innings 2: SJF (Australia)\n\n");

    srand(time(NULL));

    printf("Innings 1: India batting (FCFS)\n");

    batting_team = TEAM_INDIA;
    bowling_team = TEAM_AUSTRALIA;
    current_innings = 1;

    set_batting_mode(FCFS);
    run_innings(bowl_sched, india_batsmen, australia_bowlers);

    printf("Innings 1 complete: %d/%d\n", innings_score[0], innings_wickets[0]);

    printf("Innings 2: Australia batting (SJF)\n");
    printf("Target: %d\n", innings_score[0] + 1);

    batting_team = TEAM_AUSTRALIA;
    bowling_team = TEAM_INDIA;
    current_innings = 2;

    set_batting_mode(SJF_BAT);
    run_innings(bowl_sched, australia_batsmen, india_bowlers);

    printf("Innings 2 complete: %d/%d\n", innings_score[1], innings_wickets[1]);

    print_match_result();

    printf("\nDeadlock demo\n");
    simulate_run_out();

    return 0;
}