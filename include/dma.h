#ifndef DMA_H
#define DMA_H

#include <stddef.h>
#include <stdint.h>
#include "common.h"

typedef struct {
    uint64_t transfer_count;
    uint64_t bytes_copied;
} DmaEngine;

void dma_init(DmaEngine *dma);
int dma_transfer_packet(DmaEngine *dma, DevicePacket *dst, const DevicePacket *src);

#endif
