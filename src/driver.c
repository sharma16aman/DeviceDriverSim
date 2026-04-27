#include <time.h>
#include <unistd.h>
#include "driver.h"
#include "logger.h"

static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

int driver_init(Driver *drv, Device *dev) {
    if (!drv || !dev) return DRIVER_ERR_INVALID;

    drv->device = dev;
    drv->is_open = 0;
    drv->use_interrupts = 1;
    drv->use_dma = 0;
    dma_init(&drv->dma);
    log_msg(LOG_INFO, "Driver initialized");
    return DRIVER_OK;
}

int driver_open(Driver *drv) {
    if (!drv || !drv->device) return DRIVER_ERR_INVALID;
    drv->is_open = 1;
    log_msg(LOG_INFO, "Driver opened");
    return DRIVER_OK;
}

int driver_close(Driver *drv) {
    if (!drv) return DRIVER_ERR_INVALID;
    drv->is_open = 0;
    log_msg(LOG_INFO, "Driver closed");
    return DRIVER_OK;
}

int driver_read(Driver *drv, DevicePacket *out, int timeout_ms) {
    if (!drv || !out) return DRIVER_ERR_INVALID;
    if (!drv->is_open) return DRIVER_ERR_CLOSED;

    uint64_t start = now_ns();

    if (drv->use_interrupts) {
        int wait_rc = interrupt_wait(drv->device->interrupts, timeout_ms);
        if (wait_rc != DRIVER_OK) {
            metrics_inc(&drv->device->metrics->timeouts, drv->device->metrics);
            log_msg(LOG_WARN, "Interrupt wait timeout");
            return wait_rc;
        }
    }

    DevicePacket pkt;
    int rc = rb_dequeue(drv->device->rx_buffer, &pkt);
    if (rc != DRIVER_OK) {
        return rc;
    }

    if (pkt.checksum != packet_checksum(pkt.id, pkt.value)) {
        metrics_inc(&drv->device->metrics->corrupted, drv->device->metrics);
        log_msg(LOG_ERROR, "Corrupted packet detected id=%u", pkt.id);
        return DRIVER_ERR_CORRUPT;
    }

    if (drv->use_dma) {
        dma_transfer_packet(&drv->dma, out, &pkt);
        metrics_inc(&drv->device->metrics->dma_transfers, drv->device->metrics);
    } else {
        *out = pkt;
        metrics_inc(&drv->device->metrics->normal_transfers, drv->device->metrics);
    }

    metrics_inc(&drv->device->metrics->consumed, drv->device->metrics);
    drv->device->metrics->bytes_transferred += sizeof(DevicePacket);

    uint64_t end = now_ns();
    metrics_add_latency(drv->device->metrics, end - start);

    return DRIVER_OK;
}

int driver_ioctl(Driver *drv, IoctlCommand cmd, void *arg) {
    if (!drv || !drv->device) return DRIVER_ERR_INVALID;

    switch (cmd) {
        case IOCTL_SET_FAULT_MODE:
            if (!arg) return DRIVER_ERR_INVALID;
            device_set_fault(drv->device, *(FaultMode*)arg);
            return DRIVER_OK;

        case IOCTL_CLEAR_FAULTS:
            device_set_fault(drv->device, FAULT_NONE);
            return DRIVER_OK;

        case IOCTL_ENABLE_DMA:
            drv->use_dma = 1;
            log_msg(LOG_INFO, "DMA-like transfer mode enabled");
            return DRIVER_OK;

        case IOCTL_DISABLE_DMA:
            drv->use_dma = 0;
            log_msg(LOG_INFO, "DMA-like transfer mode disabled");
            return DRIVER_OK;

        case IOCTL_RESET_DEVICE:
            device_reset(drv->device);
            return DRIVER_OK;

        default:
            return DRIVER_ERR_INVALID;
    }
}

void* driver_consumer_thread(void *arg) {
    Driver *drv = (Driver*)arg;
    DevicePacket pkt;

    log_msg(LOG_INFO, "Driver consumer thread started");

    while (drv->device->running) {
        int rc = driver_read(drv, &pkt, 500);

        if (rc == DRIVER_OK) {
            log_msg(LOG_INFO, "Driver consumed packet id=%u value=%u", pkt.id, pkt.value);
        } else if (rc == DRIVER_ERR_TIMEOUT) {
            log_msg(LOG_DEBUG, "No interrupt received in timeout window");
        } else if (rc == DRIVER_ERR_CORRUPT) {
            log_msg(LOG_WARN, "Driver rejected corrupted packet");
        }

        usleep(50000);
    }

    log_msg(LOG_INFO, "Driver consumer thread stopped");
    return NULL;
}
