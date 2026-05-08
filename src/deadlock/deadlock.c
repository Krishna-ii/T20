#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "../../include/globals.h"
#include "../../include/deadlock/deadlock.h"

static volatile int holder_end1 = 0;
static volatile int holder_end2 = 0;

// wait_for[i] = j means i is waiting for j 
static volatile int wait_for[3] = {0, 0, 0};

static volatile int deadlock_resolved = 0;

static pthread_t bat_a_tid, bat_b_tid;

static void *umpire_thread(void *arg)
{
    (void)arg;

    printf("  [Umpire] Monitoring for deadlock...\n");

    while (!deadlock_resolved)
    {
        usleep(100000);

        int wA = wait_for[1];
        int wB = wait_for[2];

        if (wA != 0 && wB != 0 && wait_for[wA] == 1)
        {
            printf("\n  [Umpire] !!! DEADLOCK DETECTED !!!\n");
            printf("  [Umpire] Wait-For Graph: A->%d, B->%d  (cycle)\n", wA, wB);
            printf("  [Umpire] Declaring Batsman A RUN OUT (victim).\n");
            printf("  [Umpire] Cancelling Batsman A thread...\n");

            pthread_cancel(bat_a_tid);

            if (holder_end1 == 1)
            {
                holder_end1 = 0;
                pthread_mutex_unlock(&end1_mutex);
            }

            wait_for[1] = 0;
            deadlock_resolved = 1;

            printf("Umpire: Batsman A OUT - Run Out!\n");
            printf("Umpire: Batsman B can now complete the run.\n\n");
        }
    }

    return NULL;
}

static void *batsman_A(void *arg)
{
    (void)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    printf("Batsman A At End 1. Calling for a run, wants End 2.\n");

    pthread_mutex_lock(&end1_mutex);
    holder_end1 = 1;
    printf("Batsman A Holds End 1. Attempting to reach End 2...\n");

    usleep(50000);

    wait_for[1] = 2;
    pthread_mutex_lock(&end2_mutex);
    holder_end2 = 1;
    wait_for[1] = 0;

    printf("Batsman A Reached End 2. Run completed.\n");

    pthread_mutex_unlock(&end2_mutex);
    holder_end2 = 0;
    pthread_mutex_unlock(&end1_mutex);
    holder_end1 = 0;

    return NULL;
}

static void *batsman_B(void *arg)
{
    (void)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    printf("Batsman B At End 2. Calling for a run, wants End 1.\n");

    pthread_mutex_lock(&end2_mutex);
    holder_end2 = 2;
    printf("Batsman B Holds End 2. Attempting to reach End 1...\n");

    usleep(50000);

    wait_for[2] = 1;
    pthread_mutex_lock(&end1_mutex);
    holder_end1 = 2;
    wait_for[2] = 0;

    printf("Batsman B Reached End 1. Run completed.\n");

    pthread_mutex_unlock(&end1_mutex);
    holder_end1 = 0;
    pthread_mutex_unlock(&end2_mutex);
    holder_end2 = 0;

    deadlock_resolved = 1;

    return NULL;
}

void simulate_run_out(void)
{
    printf("\n");
    printf("deadlock simulation attempt through runout\n");
    printf("\n\n");
    printf("Scenario:\n");
    printf("Batsman A holds End 1, wants End 2.\n");
    printf("Batsman B holds End 2, wants End 1.\n");
    printf("Both call for a run simultaneously.\n");
    printf("Circular Wait leads to Deadlock.\n\n");

    deadlock_resolved = 0;
    holder_end1 = 0;
    holder_end2 = 0;
    memset((void *)wait_for, 0, sizeof(wait_for));

    pthread_mutex_init(&end1_mutex, NULL);
    pthread_mutex_init(&end2_mutex, NULL);

    pthread_t umpire_tid;
    pthread_create(&umpire_tid, NULL, umpire_thread, NULL);

    pthread_create(&bat_a_tid, NULL, batsman_A, NULL);
    usleep(10000);
    pthread_create(&bat_b_tid, NULL, batsman_B, NULL);

    pthread_join(bat_a_tid, NULL);
    pthread_join(bat_b_tid, NULL);
    pthread_join(umpire_tid, NULL);

    printf("\n");
    printf("simulation complete.\n");
}