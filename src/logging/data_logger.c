#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "../../include/globals.h"
#include "../../include/logging/data_logger.h"
#include "../../include/match/events.h"
#include "../../include/utils/time_utils.h"

void write_gantt_csv(void)
{
    int first_innings = (current_innings == 1);
    FILE *f = fopen("data/gantt_log.csv", first_innings ? "w" : "a");
    if (!f)
    {
        perror("Could not open data/gantt_log.csv");
        return;
    }

    if (first_innings)
    {
        fprintf(f, "innings,batting_team,"
                   "ball_number,over,ball_in_over,bowler,striker,"
                   "event,score_after,wickets_after,timestamp_ms\n");
    }

    for (int i = 0; i < gantt_count; i++)
    {
        BallRecord *r = &gantt_log[i];
        fprintf(f,
                "%d,%d,%d,%d,%d,%d,%d,%s,%d,%d,%ld\n",
                r->innings,
                r->batting_team_id,
                r->ball_number,
                r->over,
                r->ball_in_over,
                r->bowler,
                r->striker_id,
                event_name(r->event),
                r->score_after,
                r->wickets_after,
                r->timestamp_ms);
    }

    fflush(f);
    fclose(f);
    printf("[LOG] Gantt data written -> data/gantt_log.csv (innings %d)\n", current_innings);
}