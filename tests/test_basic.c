#include <stdio.h>
#include "validator.h"
#include "ring_buffer.h"
#include "interrupt.h"
#include "metrics.h"
#include "device.h"
#include "driver.h"

int test_basic(FILE *report) {
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
    int ok = 1;

    ok &= (device_get_state(&dev) == DEVICE_IDLE);
    ok &= (driver_init(&drv, &dev) == DRIVER_OK);
    ok &= (driver_open(&drv) == DRIVER_OK);
    ok &= (driver_close(&drv) == DRIVER_OK);

    rb_destroy(&rb);
    interrupt_destroy(&ic);
    metrics_destroy(&metrics);

    return ok ? 0 : 1;
}

int main(void) {
    return validator_run_all("results/test_report.txt");
}
