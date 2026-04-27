#include <stdio.h>
#include <unistd.h>
#include "ring_buffer.h"
#include "interrupt.h"
#include "metrics.h"
#include "device.h"
#include "driver.h"

int test_performance(FILE *report) {
    (void)report;

    RingBuffer rb;
    InterruptController ic;
    Metrics metrics;
    Device dev;
    Driver drv;

    rb_init(&rb);
    interrupt_init(&ic);
    metrics_init(&metrics);
    device_init(&dev, &rb, &ic, &metrics);
    driver_init(&drv, &dev);
    driver_open(&drv);
    driver_ioctl(&drv, IOCTL_ENABLE_DMA, NULL);

    device_start(&dev);

    int reads = 0;
    for (int i = 0; i < 20; i++) {
        DevicePacket out;
        if (driver_read(&drv, &out, 500) == DRIVER_OK) {
            reads++;
        }
    }

    device_stop(&dev);
    driver_close(&drv);

    int ok = (reads > 0 && metrics.consumed > 0 && metrics.dma_transfers > 0);

    rb_destroy(&rb);
    interrupt_destroy(&ic);
    metrics_destroy(&metrics);

    return ok ? 0 : 1;
}
