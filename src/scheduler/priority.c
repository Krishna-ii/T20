#include <stdio.h>
#include "../../include/globals.h"
#include "../../include/scheduler/priority.h"
#include "../../include/scheduler/round_robin.h"

void priority_next_bowler(void)
{
    if (match_intensity == INTENSITY_HIGH)
    {
        current_bowler = DEATH_SPECIALIST;

        printf("[SCHEDULER:PRIORITY] HIGH intensity detected!\n");
        printf("Death Over Specialist (Bowler %d)"
               " gets HIGHEST PRIORITY.\n", DEATH_SPECIALIST);
    }
    else
    {
        //if not high intensity matlab its not last over so do normal Round
        rr_next_bowler();
    }
}
