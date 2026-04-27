#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ring_buffer.h"
#include "interrupt.h"
#include "metrics.h"
#include "device.h"
#include "driver.h"
#include "logger.h"

static void ensure_dirs(void) {
    mkdir("logs", 0777);
    mkdir("results", 0777);
}

int main(int argc, char **argv) {
    int runtime_sec = DEVICE_DEFAULT_RUNTIME_SEC;
    int enable_dma = 0;
    FaultMode fault_mode = FAULT_NONE;

    if (argc >= 2) {
        runtime_sec = atoi(argv[1]);
        if (runtime_sec <= 0) runtime_sec = DEVICE_DEFAULT_RUNTIME_SEC;
    }

    if (argc >= 3) {
        enable_dma = atoi(argv[2]);
    }

    if (argc >= 4) {
        fault_mode = (FaultMode)atoi(argv[3]);
    }

    ensure_dirs();
    logger_init("logs/driver.log");
    srand((unsigned int)time(NULL));

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

    if (enable_dma) {
        driver_ioctl(&drv, IOCTL_ENABLE_DMA, NULL);
    }

    if (fault_mode != FAULT_NONE) {
        driver_ioctl(&drv, IOCTL_SET_FAULT_MODE, &fault_mode);
    }

    log_msg(LOG_INFO, "Starting simulation runtime=%d sec dma=%d fault=%d",
            runtime_sec, enable_dma, fault_mode);

    device_start(&dev);

    pthread_t consumer_thread;
    pthread_create(&consumer_thread, NULL, driver_consumer_thread, &drv);

    sleep((unsigned int)runtime_sec);

    device_stop(&dev);
    pthread_join(consumer_thread, NULL);

    metrics_print(&metrics);
    metrics_write_csv(&metrics, "results/metrics.csv");

    driver_close(&drv);

    rb_destroy(&rb);
    interrupt_destroy(&ic);
    metrics_destroy(&metrics);
    logger_close();

    return 0;
}
