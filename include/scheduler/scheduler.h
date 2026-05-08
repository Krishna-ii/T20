#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef enum {
    RR,       
    SJF,     
    PRIORITY  
} scheduler_type;

typedef enum {
    FCFS,     
    SJF_BAT   
} batting_mode_t;

void scheduler_init(scheduler_type type);
void scheduler_next(void);

int scheduler_next_batsman(void);

void set_batting_mode(batting_mode_t mode);

const char *scheduler_name(void);

#endif 