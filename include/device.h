#ifndef DEVICE_H
#define DEVICE_H

#include <pthread.h>
#include "common.h"
#include "ring_buffer.h"
#include "interrupt.h"
#include "metrics.h"

typedef struct {
    uint32_t control;
    uint32_t status;
    uint32_t data;
    uint32_t error_code;
} DeviceRegisters;

typedef struct {
    DeviceRegisters regs;
    DeviceState state;
    RingBuffer *rx_buffer;
    InterruptController *interrupts;
    Metrics *metrics;
    FaultMode fault_mode;
    int running;
    uint32_t packet_id;
    pthread_t thread;
    pthread_mutex_t lock;
} Device;

void device_init(Device *dev, RingBuffer *rb, InterruptController *ic, Metrics *metrics);
int device_start(Device *dev);
int device_stop(Device *dev);
void device_reset(Device *dev);
void device_set_fault(Device *dev, FaultMode mode);
DeviceState device_get_state(Device *dev);

#endif
