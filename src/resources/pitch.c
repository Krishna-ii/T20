#include "../../include/globals.h"
#include "../../include/resources/pitch.h"

void lock_pitch(void)
{
    pthread_mutex_lock(&pitch_mutex);
}

void unlock_pitch(void)
{
    pthread_mutex_unlock(&pitch_mutex);
}
