#include <stdlib.h>
#include "../../include/match/ball_generator.h"
#include "../../include/match/events.h"
#include "../../include/globals.h"
#include "../../include/scheduler/priority.h"

#define EXPECTED_BALLS_TOPORDER    20   
#define EXPECTED_BALLS_MIDDLEORDER 10  

#define DEATH_BOWLER_WICKET_BONUS   10  
#define DEATH_BOWLER_BOUNDARY_MALUS  5  

#define TAIL_WICKET_BONUS           12  
#define TAIL_BOUNDARY_MALUS         10  
#define TAIL_DOT_BONUS               8             

#define MID_WICKET_BONUS             5  
#define MID_BOUNDARY_MALUS           3  

int generate_ball_event(void)
{
    int p_dot, p_single, p_double, p_four, p_six, p_air, p_wide, p_wicket;

    if (match_intensity == INTENSITY_HIGH)
    {
        p_dot    = 12;  p_single = 8;  p_double =  8;
        p_four   = 20;  p_six    = 20;  p_air    =  8;
        p_wide   =  8;  p_wicket = 16;
    }
    else
    {
        p_dot    = 30;  p_single = 22;  p_double = 10;
        p_four   = 14;  p_six    =  6;  p_air    =  8;
        p_wide   =  7;  p_wicket =  3;
    }

    int is_death_bowler = (current_bowler == DEATH_SPECIALIST);
    if (is_death_bowler)
    {
        p_wicket += DEATH_BOWLER_WICKET_BONUS;
        p_four   -= DEATH_BOWLER_BOUNDARY_MALUS / 2;
        p_six    -= DEATH_BOWLER_BOUNDARY_MALUS / 2;
        p_dot    += DEATH_BOWLER_BOUNDARY_MALUS - (DEATH_BOWLER_BOUNDARY_MALUS / 2) * 2;
    }

    int exp = expected_balls[striker];  

    if (exp < EXPECTED_BALLS_MIDDLEORDER)
    {
        p_wicket += TAIL_WICKET_BONUS;
        p_four   -= TAIL_BOUNDARY_MALUS / 2;
        p_six    -= TAIL_BOUNDARY_MALUS / 2;
        p_dot    += TAIL_DOT_BONUS;
        
        p_single -= 3;
        
        p_double -= (TAIL_WICKET_BONUS + TAIL_BOUNDARY_MALUS + TAIL_DOT_BONUS - 3
                     - TAIL_WICKET_BONUS - TAIL_BOUNDARY_MALUS - TAIL_DOT_BONUS + 3);
    }
    else if (exp < EXPECTED_BALLS_TOPORDER)
    {
        p_wicket += MID_WICKET_BONUS;
        p_four   -= MID_BOUNDARY_MALUS / 2;
        p_six    -= MID_BOUNDARY_MALUS / 2;
        p_dot    += MID_WICKET_BONUS + MID_BOUNDARY_MALUS
                    - (MID_BOUNDARY_MALUS / 2) * 2
                    - (MID_BOUNDARY_MALUS - (MID_BOUNDARY_MALUS / 2) * 2);
    }
    
if (p_dot < 0) p_dot = 0;
if (p_single < 0) p_single = 0;
if (p_double < 0) p_double = 0;
if (p_four < 0) p_four = 0;
if (p_six < 0) p_six = 0;
if (p_air < 0) p_air = 0;
if (p_wide < 0) p_wide = 0;
if (p_wicket < 0) p_wicket = 0;

    int total = p_dot + p_single + p_double + p_four + p_six
              + p_air + p_wide + p_wicket;
    if (total <= 0) return WICKET;   

    int r = rand() % total;

    int cummulative = 0;
    cummulative += p_dot;    if (r < cummulative) return DOT;
    cummulative += p_single; if (r < cummulative) return SINGLE;
    cummulative += p_double; if (r < cummulative) return DOUBLE;
    cummulative += p_four;   if (r < cummulative) return FOUR;
    cummulative += p_six;    if (r < cummulative) return SIX;
    cummulative += p_air;    if (r < cummulative) return BALL_IN_AIR;
    cummulative += p_wide;   if (r < cummulative) return WIDE;
    return WICKET;
}