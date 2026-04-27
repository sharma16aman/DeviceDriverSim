#ifndef METRICS_H
#define METRICS_H

#include <stdint.h>
#include <pthread.h>

typedef struct {
    uint64_t produced;
    uint64_t consumed;
    uint64_t dropped;
    uint64_t corrupted;
    uint64_t timeouts;
    uint64_t dma_transfers;
    uint64_t normal_transfers;
    uint64_t bytes_transferred;
    uint64_t total_latency_ns;
    pthread_mutex_t lock;
} Metrics;

void metrics_init(Metrics *m);
void metrics_destroy(Metrics *m);
void metrics_inc(uint64_t *field, Metrics *m);
void metrics_add_latency(Metrics *m, uint64_t ns);
void metrics_print(Metrics *m);
int metrics_write_csv(Metrics *m, const char *path);

#endif
