#include <stdio.h>
#include "../../include/globals.h"
#include "../../include/scheduler/round_robin.h"

void rr_next_bowler(void)
{
    int prev = current_bowler;
    current_bowler = (current_bowler + 1) % total_bowlers;

    printf("[SCHEDULER:RR] Context Switch → Bowler %d out, Bowler %d in\n",
           prev, current_bowler);
}
