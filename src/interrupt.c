#include <errno.h>
#include <time.h>
#include "interrupt.h"
#include "common.h"

static void make_abs_deadline(struct timespec *ts, int timeout_ms) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += timeout_ms / 1000;
    ts->tv_nsec += (long)(timeout_ms % 1000) * 1000000L;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

void interrupt_init(InterruptController *ic) {
    pthread_mutex_init(&ic->lock, NULL);
    pthread_cond_init(&ic->cond, NULL);
    ic->pending = 0;
    ic->raised_count = 0;
    ic->handled_count = 0;
    ic->dropped_count = 0;
}

void interrupt_destroy(InterruptController *ic) {
    pthread_cond_destroy(&ic->cond);
    pthread_mutex_destroy(&ic->lock);
}

void interrupt_raise(InterruptController *ic) {
    pthread_mutex_lock(&ic->lock);
    ic->pending = 1;
    ic->raised_count++;
    pthread_cond_signal(&ic->cond);
    pthread_mutex_unlock(&ic->lock);
}

int interrupt_wait(InterruptController *ic, int timeout_ms) {
    pthread_mutex_lock(&ic->lock);

    if (!ic->pending) {
        if (timeout_ms < 0) {
            while (!ic->pending) {
                pthread_cond_wait(&ic->cond, &ic->lock);
            }
        } else {
            struct timespec deadline;
            make_abs_deadline(&deadline, timeout_ms);
            while (!ic->pending) {
                int rc = pthread_cond_timedwait(&ic->cond, &ic->lock, &deadline);
                if (rc == ETIMEDOUT) {
                    pthread_mutex_unlock(&ic->lock);
                    return DRIVER_ERR_TIMEOUT;
                }
            }
        }
    }

    ic->pending = 0;
    ic->handled_count++;
    pthread_mutex_unlock(&ic->lock);
    return DRIVER_OK;
}

void interrupt_drop(InterruptController *ic) {
    pthread_mutex_lock(&ic->lock);
    ic->dropped_count++;
    pthread_mutex_unlock(&ic->lock);
}
