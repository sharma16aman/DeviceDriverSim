#include "ring_buffer.h"

void rb_init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->overflow_count = 0;
    rb->underflow_count = 0;
    pthread_mutex_init(&rb->lock, NULL);
}

void rb_destroy(RingBuffer *rb) {
    pthread_mutex_destroy(&rb->lock);
}

int rb_enqueue(RingBuffer *rb, const DevicePacket *pkt) {
    pthread_mutex_lock(&rb->lock);

    if (rb->count == DEVICE_BUFFER_SIZE) {
        rb->overflow_count++;
        pthread_mutex_unlock(&rb->lock);
        return DRIVER_ERR_BUSY;
    }

    rb->buffer[rb->head] = *pkt;
    rb->head = (rb->head + 1) % DEVICE_BUFFER_SIZE;
    rb->count++;

    pthread_mutex_unlock(&rb->lock);
    return DRIVER_OK;
}

int rb_dequeue(RingBuffer *rb, DevicePacket *pkt) {
    pthread_mutex_lock(&rb->lock);

    if (rb->count == 0) {
        rb->underflow_count++;
        pthread_mutex_unlock(&rb->lock);
        return DRIVER_ERR_EMPTY;
    }

    *pkt = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % DEVICE_BUFFER_SIZE;
    rb->count--;

    pthread_mutex_unlock(&rb->lock);
    return DRIVER_OK;
}

int rb_count(RingBuffer *rb) {
    pthread_mutex_lock(&rb->lock);
    int count = rb->count;
    pthread_mutex_unlock(&rb->lock);
    return count;
}

void rb_clear(RingBuffer *rb) {
    pthread_mutex_lock(&rb->lock);
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    pthread_mutex_unlock(&rb->lock);
}
