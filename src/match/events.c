#include <stdio.h>
#include "../../include/match/events.h"

const char *event_name(int event)
{
    switch (event)
    {
        case DOT:        return "DOT";
        case SINGLE:     return "SINGLE";
        case DOUBLE:     return "DOUBLE";
        case FOUR:       return "FOUR";
        case SIX:        return "SIX";
        case WICKET:     return "WICKET";
        case WIDE:       return "WIDE";
        case BALL_IN_AIR: return "IN_AIR";
        case NO_BALL:    return "NO_BALL";
        default:         return "UNKNOWN";
    }
}

void print_event(int event)
{
    switch (event)
    {
        case DOT:        printf("  Dot ball.\n");            break;
        case SINGLE:     printf("  1 run.\n");               break;
        case DOUBLE:     printf("  2 runs.\n");              break;
        case FOUR:       printf("  !!!!FOUR!!!! \n");          break;
        case SIX:        printf("  !!!!!!SIX!!!!!!  \n");        break;
        case WICKET:     printf("  WICKET! OUT!\n");         break;
        case WIDE:       printf("  Wide ball (+1).\n");      break;
        case BALL_IN_AIR: printf("  Ball in the air!\n");    break;
        case NO_BALL:    printf("  No ball (+1).\n");        break;
        default:         printf("  Unknown event.\n");       break;
    }
}
