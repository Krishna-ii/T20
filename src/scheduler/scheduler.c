#include <stdio.h>
#include "../../include/scheduler/scheduler.h"
#include "../../include/scheduler/round_robin.h"
#include "../../include/scheduler/sjf.h"
#include "../../include/scheduler/priority.h"
#include "../../include/globals.h"

static scheduler_type current_scheduler = RR;

static batting_mode_t batting_mode = FCFS;

void scheduler_init(scheduler_type type)
{
    current_scheduler = type;

    printf("[SCHEDULER] Initialised: %s\n\n", scheduler_name());

    if (batting_mode == SJF_BAT)
        sjf_init();
}

void set_batting_mode(batting_mode_t mode)
{
    batting_mode = mode;
}

void scheduler_next(void)
{
    switch (current_scheduler)
    {
        case RR:
            rr_next_bowler();
            break;

        case PRIORITY:
            priority_next_bowler();
            break;

        case SJF: 
            rr_next_bowler();
            break;
    }
}

int scheduler_next_batsman(void)
{
    if (batting_mode == SJF_BAT)
    {
        return sjf_next_batsman();
    }
    else 
    {
        if (next_batsman_index >= MAX_BATSMEN)
            return MAX_BATSMEN;

        int next = next_batsman_index++;
        printf("[SCHEDULER:FCFS] Next batsman in: Batsman %d\n", next);
        return next;
    }
}

const char *scheduler_name(void)
{
    switch (current_scheduler)
    {
        case RR:       return "Round-Robin (RR)";
        case SJF:      return "Shortest Job First (SJF)";
        case PRIORITY: return "Priority (Death-Over Specialist)";
        default:       return "Unknown";
    }
}