#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <pthread.h>
#include <stdint.h>

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int pending;
    uint64_t raised_count;
    uint64_t handled_count;
    uint64_t dropped_count;
} InterruptController;

void interrupt_init(InterruptController *ic);
void interrupt_destroy(InterruptController *ic);
void interrupt_raise(InterruptController *ic);
int interrupt_wait(InterruptController *ic, int timeout_ms);
void interrupt_drop(InterruptController *ic);

#endif
