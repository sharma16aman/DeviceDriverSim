#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>

#define DEVICE_BUFFER_SIZE 32
#define DEVICE_PAYLOAD_MAX 64
#define DEVICE_DEFAULT_RUNTIME_SEC 8
#define DEVICE_PRODUCER_DELAY_US 100000

typedef enum {
    DEVICE_IDLE = 0,
    DEVICE_BUSY,
    DEVICE_DATA_READY,
    DEVICE_ERROR,
    DEVICE_STOPPED
} DeviceState;

typedef enum {
    DRIVER_OK = 0,
    DRIVER_ERR_INVALID = -1,
    DRIVER_ERR_BUSY = -2,
    DRIVER_ERR_EMPTY = -3,
    DRIVER_ERR_TIMEOUT = -4,
    DRIVER_ERR_CORRUPT = -5,
    DRIVER_ERR_CLOSED = -6
} DriverStatus;

typedef enum {
    IOCTL_SET_FAULT_MODE = 1,
    IOCTL_CLEAR_FAULTS,
    IOCTL_ENABLE_DMA,
    IOCTL_DISABLE_DMA,
    IOCTL_RESET_DEVICE
} IoctlCommand;

typedef enum {
    FAULT_NONE = 0,
    FAULT_CORRUPT_PACKET = 1,
    FAULT_DROP_INTERRUPT = 2,
    FAULT_DEVICE_ERROR = 3
} FaultMode;

typedef struct {
    uint32_t id;
    uint32_t value;
    uint32_t checksum;
} DevicePacket;

static inline uint32_t packet_checksum(uint32_t id, uint32_t value) {
    return (id ^ value ^ 0xA5A5A5A5u);
}

#endif
