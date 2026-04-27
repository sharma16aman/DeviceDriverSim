# Runtime Flow

## Normal Flow

```text
1. Program starts
2. Logger, ring buffer, interrupt controller, metrics are initialized
3. Device is initialized with register state
4. Driver is initialized and opened
5. Device producer thread starts
6. Device generates a packet
7. Packet is inserted into ring buffer
8. Device raises interrupt
9. Driver waits on interrupt
10. Driver wakes up
11. Driver dequeues packet
12. Driver validates checksum
13. Driver transfers packet normally or through DMA-like path
14. Metrics are updated
15. Logs are written
16. Simulation stops after configured runtime
17. Metrics are printed and saved to CSV
```

## Fault Flow: Corrupted Packet

```text
1. Fault mode is set to FAULT_CORRUPT_PACKET
2. Device intentionally corrupts checksum of selected packets
3. Driver reads packet
4. Driver recomputes checksum
5. Driver detects mismatch
6. Corrupted counter is incremented
7. Error is logged
8. Packet is rejected
```

## Fault Flow: Dropped Interrupt

```text
1. Fault mode is set to FAULT_DROP_INTERRUPT
2. Device produces data but skips interrupt for selected packets
3. Driver waits for interrupt
4. Timeout occurs if no interrupt arrives
5. Timeout counter is incremented
6. Driver continues safely
```

## DMA Mode Flow

```text
1. DMA mode is enabled through ioctl
2. Driver reads packet from ring buffer
3. Instead of direct assignment, packet is copied through DMA module
4. DMA transfer count is updated
5. Bytes transferred are tracked
```
