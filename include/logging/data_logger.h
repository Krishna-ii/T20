#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

void init_data_logger(void);
void log_csv_event(const char *event_name);
void close_data_logger(void);

void write_gantt_csv(void);

#endif 
