#include <stdio.h>
#include <stdlib.h>
#include "../../include/scheduler/sjf.h"
#include "../../include/globals.h"

static int order[MAX_BATSMEN];
static int idx = 0;

void sjf_init(void)
{
    for (int i = 0; i < MAX_BATSMEN - 2; i++)
        order[i] = i + 2;

    int n = MAX_BATSMEN - 2;

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (expected_balls[order[j]] < expected_balls[order[i]])
            {
                int tmp  = order[i];
                order[i] = order[j];
                order[j] = tmp;
            }
        }
    }

    idx = 0;

    printf("[SCHEDULER:SJF] Batting order (by expected balls):\n");
    for (int i = 0; i < n; i++)
    {
        printf("    Position %d → Batsman %-2d (expected %d balls)\n",
               i + 3, order[i], expected_balls[order[i]]);
    }
    printf("\n");
}

int sjf_next_batsman(void)
{
    if (idx >= MAX_BATSMEN - 2)
        return MAX_BATSMEN;

    int next = order[idx++];

    printf("[SCHEDULER:SJF] Next batsman in: Batsman %d (expected %d balls)\n",
           next, expected_balls[next]);

    return next;
}