#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <pthread.h>
#include "common.h"

typedef struct {
    DevicePacket buffer[DEVICE_BUFFER_SIZE];
    int head;
    int tail;
    int count;
    uint64_t overflow_count;
    uint64_t underflow_count;
    pthread_mutex_t lock;
} RingBuffer;

void rb_init(RingBuffer *rb);
void rb_destroy(RingBuffer *rb);
int rb_enqueue(RingBuffer *rb, const DevicePacket *pkt);
int rb_dequeue(RingBuffer *rb, DevicePacket *pkt);
int rb_count(RingBuffer *rb);
void rb_clear(RingBuffer *rb);

#endif
