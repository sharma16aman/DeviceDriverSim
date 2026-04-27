#include <string.h>
#include "dma.h"

void dma_init(DmaEngine *dma) {
    dma->transfer_count = 0;
    dma->bytes_copied = 0;
}

int dma_transfer_packet(DmaEngine *dma, DevicePacket *dst, const DevicePacket *src) {
    if (!dma || !dst || !src) {
        return DRIVER_ERR_INVALID;
    }

    memcpy(dst, src, sizeof(DevicePacket));
    dma->transfer_count++;
    dma->bytes_copied += sizeof(DevicePacket);
    return DRIVER_OK;
}
