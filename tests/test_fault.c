#include <stdio.h>
#include <unistd.h>
#include "ring_buffer.h"
#include "interrupt.h"
#include "metrics.h"
#include "device.h"
#include "driver.h"

int test_fault(FILE *report) {
    (void)report;

    RingBuffer rb;
    InterruptController ic;
    Metrics metrics;
    Device dev;
    Driver drv;
    FaultMode fault = FAULT_CORRUPT_PACKET;

    rb_init(&rb);
    interrupt_init(&ic);
    metrics_init(&metrics);
    device_init(&dev, &rb, &ic, &metrics);
    driver_init(&drv, &dev);
    driver_open(&drv);
    driver_ioctl(&drv, IOCTL_SET_FAULT_MODE, &fault);

    device_start(&dev);

    int saw_corrupt = 0;
    for (int i = 0; i < 30; i++) {
        DevicePacket out;
        int rc = driver_read(&drv, &out, 500);
        if (rc == DRIVER_ERR_CORRUPT) {
            saw_corrupt = 1;
            break;
        }
        usleep(20000);
    }

    device_stop(&dev);
    driver_close(&drv);
    rb_destroy(&rb);
    interrupt_destroy(&ic);
    metrics_destroy(&metrics);

    return saw_corrupt ? 0 : 1;
}
