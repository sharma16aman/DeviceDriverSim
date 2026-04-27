#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "common.h"
#include "interrupt.h"

static void* signaler(void *arg) {
    InterruptController *ic = (InterruptController*)arg;
    usleep(100000);
    interrupt_raise(ic);
    return NULL;
}

int test_interrupt(FILE *report) {
    (void)report;

    InterruptController ic;
    interrupt_init(&ic);

    pthread_t t;
    pthread_create(&t, NULL, signaler, &ic);

    int rc = interrupt_wait(&ic, 1000);

    pthread_join(t, NULL);
    int ok = (rc == DRIVER_OK && ic.raised_count == 1 && ic.handled_count == 1);

    interrupt_destroy(&ic);
    return ok ? 0 : 1;
}
