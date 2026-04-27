#ifndef DRIVER_H
#define DRIVER_H

#include "device.h"
#include "dma.h"

typedef struct {
    Device *device;
    DmaEngine dma;
    int is_open;
    int use_interrupts;
    int use_dma;
} Driver;

int driver_init(Driver *drv, Device *dev);
int driver_open(Driver *drv);
int driver_close(Driver *drv);
int driver_read(Driver *drv, DevicePacket *out, int timeout_ms);
int driver_ioctl(Driver *drv, IoctlCommand cmd, void *arg);
void* driver_consumer_thread(void *arg);

#endif
