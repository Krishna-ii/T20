#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "../../include/globals.h"
#include "../../include/resources/scoreboard.h"
#include "../../include/resources/crease.h"

const char *india_batsmen[11] = {
    "Rohit Sharma",     
    "Virat Kohli",      
    "Shubman Gill",     
    "KL Rahul",         
    "Hardik Pandya",    
    "Ravindra Jadeja",  
    "Jasprit Bumrah",   
    "Axar Patel",       
    "Kuldeep Yadav",    
    "Mohammed Shami",   
    "Arshdeep Singh"
};

const char *australia_batsmen[11] = {
    "David Warner",     
    "Travis Head",      
    "Steve Smith",     
    "Glenn Maxwell",   
    "Marnus Labuschagne",
    "Marcus Stoinis",  
    "Matthew Wade",    
    "Pat Cummins",     
    "Mitchell Starc",  
    "Josh Hazlewood",  
    "Adam Zampa"        
};

const char *india_bowlers[5] = {
    "Jasprit Bumrah",
    "Mohammed Shami",
    "Arshdeep Singh",
    "Hardik Pandya",
    "Kuldeep Yadav"
};

const char *australia_bowlers[5] = {
    "Pat Cummins",
    "Mitchell Starc",
    "Josh Hazlewood",
    "Adam Zampa",
    "Marcus Stoinis"
};

int    batting_team  = TEAM_INDIA;
int    bowling_team  = TEAM_AUSTRALIA;
int    current_innings = 1;

const char **active_batsmen = NULL;
const char **active_bowlers = NULL;

int innings_score[2]   = {0, 0};
int innings_wickets[2] = {0, 0};

int global_score   = 0;
int wickets        = 0;
int current_over   = 0;
int current_ball   = 0;
int total_balls    = 0;
int match_started  = 0;
int match_running  = 1;
int match_intensity = INTENSITY_NORMAL;

int ball_in_air = 0;
int ball_event  = 0;
int ball_ready  = 0;

int current_bowler = 0;
int total_bowlers  = 5;

int bowler_balls_bowled[MAX_BOWLERS]   = {0};
int bowler_runs_conceded[MAX_BOWLERS]  = {0};
int bowler_wickets[MAX_BOWLERS]        = {0};

int striker            = 0;
int non_striker        = 1;
int next_batsman_index = 2;
int current_batsman    = 0;

int  expected_balls[MAX_BATSMEN] = {0};
int  batsman_runs[MAX_BATSMEN]   = {0};
long arrival_time[MAX_BATSMEN]   = {0};
long start_time[MAX_BATSMEN]     = {0};
long wait_time[MAX_BATSMEN]      = {0};

int runout_want[2]  = {-1, -1};
int runout_at[2]    = {-1, -1};
int runout_active   = 0;

BallRecord gantt_log[MAX_BALLS];
int  gantt_count    = 0;
long match_start_ms = 0;

void add_runs(int runs)
{
    pthread_mutex_lock(&score_mutex);
    global_score += runs;
    pthread_mutex_unlock(&score_mutex);
}

void add_wicket(void)
{
    pthread_mutex_lock(&score_mutex);
    wickets++;
    pthread_mutex_unlock(&score_mutex);
}

int get_score(void)
{
    return global_score;
}

int get_wickets(void)
{
    return wickets;
}
