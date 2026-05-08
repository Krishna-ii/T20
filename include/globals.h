#ifndef GLOBALS_H
#define GLOBALS_H

#define NUM_TEAMS 2
#define TEAM_INDIA 0
#define TEAM_AUSTRALIA 1

extern int batting_team;
extern int bowling_team;

extern int current_innings;

extern const char *india_batsmen[11];
extern const char *australia_batsmen[11];

extern const char *india_bowlers[5];
extern const char *australia_bowlers[5];

extern const char **active_batsmen;
extern const char **active_bowlers;

extern int innings_score[2];
extern int innings_wickets[2];

#include <pthread.h>
#include <semaphore.h>

extern pthread_mutex_t pitch_mutex;
extern pthread_mutex_t score_mutex;
extern pthread_mutex_t log_mutex;
extern pthread_mutex_t end1_mutex;
extern pthread_mutex_t end2_mutex;

extern pthread_cond_t ball_cond;
extern pthread_cond_t match_start_cond;

extern sem_t crease_sem;

extern int global_score;
extern int wickets;
extern int current_over;
extern int current_ball;
extern int total_balls;
extern int match_started;
extern int match_running;

extern int match_intensity;

#define INTENSITY_HIGH 1
#define INTENSITY_NORMAL 0

extern int ball_in_air;
extern int ball_event;
extern int ball_ready;

extern int current_bowler;
extern int total_bowlers;

#define MAX_BOWLERS 5
extern int bowler_balls_bowled[MAX_BOWLERS];
extern int bowler_runs_conceded[MAX_BOWLERS];
extern int bowler_wickets[MAX_BOWLERS];

#define MAX_BATSMEN 11

extern int striker;
extern int non_striker;
extern int next_batsman_index;

extern int expected_balls[MAX_BATSMEN];
extern int batsman_runs[MAX_BATSMEN];

extern long arrival_time[MAX_BATSMEN];
extern long start_time[MAX_BATSMEN];
extern long wait_time[MAX_BATSMEN];

extern int current_batsman;

extern int runout_want[2];
extern int runout_at[2];
extern int runout_active;

#define MAX_BALLS 200

typedef struct {
    int ball_number;
    int over;
    int ball_in_over;
    int bowler;
    int striker_id;
    int event;
    int score_after;
    int wickets_after;
    long timestamp_ms;
    int innings;
    int batting_team_id;
} BallRecord;

extern BallRecord gantt_log[MAX_BALLS];
extern int gantt_count;
extern long match_start_ms;

#endif