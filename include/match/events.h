#ifndef EVENTS_H
#define EVENTS_H

#define DOT        0
#define SINGLE     1
#define DOUBLE     2
#define FOUR       3
#define SIX        4
#define WICKET     5
#define WIDE       6
#define BALL_IN_AIR 7
#define NO_BALL    8

void print_event(int event);
const char *event_name(int event);

#endif 
