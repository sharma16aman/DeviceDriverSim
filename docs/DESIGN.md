# Design Document

## Project Goal

The goal is to build a user-space simulation of a device-driver environment in C. The system models a hardware device, driver APIs, interrupt-driven communication, DMA-like transfer, fault injection, validation, logging, and metrics.

## Major Components

### 1. Device Layer

The device layer simulates hardware behavior. It has fake registers and a state machine.

Registers:

- control
- status
- data
- error_code

States:

- DEVICE_IDLE
- DEVICE_BUSY
- DEVICE_DATA_READY
- DEVICE_ERROR
- DEVICE_STOPPED

The device runs on a producer thread and generates packets periodically.

### 2. Ring Buffer

The ring buffer is the shared communication queue between the simulated device and the driver.

It is protected using a pthread mutex.

### 3. Interrupt Controller

The interrupt controller uses a condition variable. The device raises an interrupt after producing data. The driver waits on that interrupt and wakes up when data is available.

### 4. Driver Layer

The driver exposes driver-like APIs:

- driver_init
- driver_open
- driver_close
- driver_read
- driver_ioctl

The driver validates packet checksums, reads packets from the ring buffer, handles corrupted data, and optionally uses DMA-like transfer.

### 5. DMA Simulation

The DMA module simulates bulk transfer through a controlled memory copy abstraction.

### 6. Fault Injection

Fault modes allow the validation layer to test robustness.

Supported faults:

- corrupted packet
- dropped interrupt
- device error

### 7. Metrics

The system tracks:

- packets produced
- packets consumed
- dropped packets
- corrupted packets
- timeouts
- DMA transfers
- normal transfers
- bytes transferred
- average read latency

### 8. Validation

The validation framework runs automated tests and writes a report to `results/test_report.txt`.

## Design Trade-offs

This is a user-space simulation, not a kernel module. That is intentional. It allows driver concepts to be demonstrated safely while still using realistic systems concepts such as threads, synchronization, buffers, and timing.
