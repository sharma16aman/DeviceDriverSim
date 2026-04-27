#include <stdlib.h>
#include <unistd.h>
#include "device.h"
#include "logger.h"

static DevicePacket device_make_packet(Device *dev) {
    DevicePacket pkt;
    pkt.id = ++dev->packet_id;
    pkt.value = (uint32_t)(rand() % 1000);
    pkt.checksum = packet_checksum(pkt.id, pkt.value);

    if (dev->fault_mode == FAULT_CORRUPT_PACKET && pkt.id % 5 == 0) {
        pkt.checksum ^= 0xFFFFFFFFu;
    }

    return pkt;
}

void device_init(Device *dev, RingBuffer *rb, InterruptController *ic, Metrics *metrics) {
    dev->regs.control = 0;
    dev->regs.status = 0;
    dev->regs.data = 0;
    dev->regs.error_code = 0;
    dev->state = DEVICE_IDLE;
    dev->rx_buffer = rb;
    dev->interrupts = ic;
    dev->metrics = metrics;
    dev->fault_mode = FAULT_NONE;
    dev->running = 0;
    dev->packet_id = 0;
    pthread_mutex_init(&dev->lock, NULL);
}

static void* device_thread_fn(void *arg) {
    Device *dev = (Device*)arg;

    log_msg(LOG_INFO, "Device producer thread started");

    while (dev->running) {
        pthread_mutex_lock(&dev->lock);

        if (dev->fault_mode == FAULT_DEVICE_ERROR) {
            dev->state = DEVICE_ERROR;
            dev->regs.error_code = 0xDEAD;
            pthread_mutex_unlock(&dev->lock);
            log_msg(LOG_ERROR, "Device entered ERROR state");
            usleep(DEVICE_PRODUCER_DELAY_US);
            continue;
        }

        dev->state = DEVICE_BUSY;
        DevicePacket pkt = device_make_packet(dev);
        dev->regs.data = pkt.value;
        dev->regs.status = DEVICE_DATA_READY;

        pthread_mutex_unlock(&dev->lock);

        int rc = rb_enqueue(dev->rx_buffer, &pkt);

        if (rc == DRIVER_OK) {
            metrics_inc(&dev->metrics->produced, dev->metrics);

            if (dev->fault_mode == FAULT_DROP_INTERRUPT && pkt.id % 4 == 0) {
                interrupt_drop(dev->interrupts);
                log_msg(LOG_WARN, "Dropped interrupt for packet id=%u", pkt.id);
            } else {
                interrupt_raise(dev->interrupts);
                log_msg(LOG_DEBUG, "Produced packet id=%u value=%u", pkt.id, pkt.value);
            }
        } else {
            metrics_inc(&dev->metrics->dropped, dev->metrics);
            log_msg(LOG_WARN, "RX ring buffer full; dropped packet id=%u", pkt.id);
        }

        pthread_mutex_lock(&dev->lock);
        dev->state = DEVICE_DATA_READY;
        pthread_mutex_unlock(&dev->lock);

        usleep(DEVICE_PRODUCER_DELAY_US);
    }

    pthread_mutex_lock(&dev->lock);
    dev->state = DEVICE_STOPPED;
    pthread_mutex_unlock(&dev->lock);

    log_msg(LOG_INFO, "Device producer thread stopped");
    return NULL;
}

int device_start(Device *dev) {
    if (!dev) return DRIVER_ERR_INVALID;

    pthread_mutex_lock(&dev->lock);
    dev->running = 1;
    dev->state = DEVICE_IDLE;
    pthread_mutex_unlock(&dev->lock);

    if (pthread_create(&dev->thread, NULL, device_thread_fn, dev) != 0) {
        return DRIVER_ERR_INVALID;
    }

    return DRIVER_OK;
}

int device_stop(Device *dev) {
    if (!dev) return DRIVER_ERR_INVALID;

    pthread_mutex_lock(&dev->lock);
    dev->running = 0;
    pthread_mutex_unlock(&dev->lock);

    pthread_join(dev->thread, NULL);
    return DRIVER_OK;
}

void device_reset(Device *dev) {
    pthread_mutex_lock(&dev->lock);
    dev->regs.control = 0;
    dev->regs.status = 0;
    dev->regs.data = 0;
    dev->regs.error_code = 0;
    dev->state = DEVICE_IDLE;
    dev->fault_mode = FAULT_NONE;
    dev->packet_id = 0;
    pthread_mutex_unlock(&dev->lock);

    rb_clear(dev->rx_buffer);
    log_msg(LOG_INFO, "Device reset completed");
}

void device_set_fault(Device *dev, FaultMode mode) {
    pthread_mutex_lock(&dev->lock);
    dev->fault_mode = mode;
    pthread_mutex_unlock(&dev->lock);
    log_msg(LOG_WARN, "Device fault mode changed to %d", mode);
}

DeviceState device_get_state(Device *dev) {
    pthread_mutex_lock(&dev->lock);
    DeviceState s = dev->state;
    pthread_mutex_unlock(&dev->lock);
    return s;
}
