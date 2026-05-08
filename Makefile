CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude -pthread
LDFLAGS = -pthread

SRC = src/main.c \
      src/synchronization/mutex.c \
      src/synchronization/semaphore.c \
      src/synchronization/condition_variables.c \
      src/resources/pitch.c \
      src/resources/crease.c \
      src/resources/scoreboard.c \
      src/match/ball_generator.c \
      src/match/events.c \
      src/match/match_engine.c \
      src/threads/bowler.c \
      src/threads/batsman.c \
      src/threads/fielder.c \
      src/scheduler/round_robin.c \
      src/scheduler/sjf.c \
      src/scheduler/priority.c \
      src/scheduler/scheduler.c \
      src/logging/data_logger.c \
      src/deadlock/deadlock.c \
      src/utils/time_utils.c

OUT = simulator

all: dirs $(OUT)

dirs:
	@mkdir -p logs data

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: all
	./$(OUT)

clean:
	rm -f $(OUT) logs/match_log.txt data/events.csv data/gantt_log.csv

.PHONY: all dirs run clean
