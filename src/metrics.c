#include <stdio.h>
#include <string.h>
#include "metrics.h"

void metrics_init(Metrics *m) {
    memset(m, 0, sizeof(*m));
    pthread_mutex_init(&m->lock, NULL);
}

void metrics_destroy(Metrics *m) {
    pthread_mutex_destroy(&m->lock);
}

void metrics_inc(uint64_t *field, Metrics *m) {
    pthread_mutex_lock(&m->lock);
    (*field)++;
    pthread_mutex_unlock(&m->lock);
}

void metrics_add_latency(Metrics *m, uint64_t ns) {
    pthread_mutex_lock(&m->lock);
    m->total_latency_ns += ns;
    pthread_mutex_unlock(&m->lock);
}

void metrics_print(Metrics *m) {
    pthread_mutex_lock(&m->lock);
    double avg_latency_us = 0.0;
    if (m->consumed > 0) {
        avg_latency_us = (double)m->total_latency_ns / (double)m->consumed / 1000.0;
    }

    printf("\n================ METRICS ================\n");
    printf("Packets produced       : %lu\n", m->produced);
    printf("Packets consumed       : %lu\n", m->consumed);
    printf("Packets dropped        : %lu\n", m->dropped);
    printf("Corrupted detected     : %lu\n", m->corrupted);
    printf("Interrupt timeouts     : %lu\n", m->timeouts);
    printf("Normal transfers       : %lu\n", m->normal_transfers);
    printf("DMA-like transfers     : %lu\n", m->dma_transfers);
    printf("Bytes transferred      : %lu\n", m->bytes_transferred);
    printf("Average read latency   : %.3f us\n", avg_latency_us);
    printf("=========================================\n\n");
    pthread_mutex_unlock(&m->lock);
}

int metrics_write_csv(Metrics *m, const char *path) {
    pthread_mutex_lock(&m->lock);

    FILE *fp = fopen(path, "w");
    if (!fp) {
        pthread_mutex_unlock(&m->lock);
        return -1;
    }

    double avg_latency_us = 0.0;
    if (m->consumed > 0) {
        avg_latency_us = (double)m->total_latency_ns / (double)m->consumed / 1000.0;
    }

    fprintf(fp, "produced,consumed,dropped,corrupted,timeouts,normal_transfers,dma_transfers,bytes_transferred,avg_latency_us\n");
    fprintf(fp, "%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%.3f\n",
            m->produced, m->consumed, m->dropped, m->corrupted,
            m->timeouts, m->normal_transfers, m->dma_transfers,
            m->bytes_transferred, avg_latency_us);

    fclose(fp);
    pthread_mutex_unlock(&m->lock);
    return 0;
}
