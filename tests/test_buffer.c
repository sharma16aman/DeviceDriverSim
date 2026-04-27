#include <stdio.h>
#include "ring_buffer.h"

int test_buffer(FILE *report) {
    (void)report;

    RingBuffer rb;
    rb_init(&rb);

    DevicePacket pkt = { .id = 1, .value = 10, .checksum = packet_checksum(1, 10) };
    int ok = 1;

    ok &= (rb_enqueue(&rb, &pkt) == DRIVER_OK);
    ok &= (rb_count(&rb) == 1);

    DevicePacket out;
    ok &= (rb_dequeue(&rb, &out) == DRIVER_OK);
    ok &= (out.id == pkt.id && out.value == pkt.value);
    ok &= (rb_dequeue(&rb, &out) == DRIVER_ERR_EMPTY);

    rb_destroy(&rb);
    return ok ? 0 : 1;
}
