# User-Space Device Driver Simulation & Validation Framework in C

A Linux-based systems programming project that simulates a hardware device, a user-space driver layer, interrupt-driven communication, ring-buffer-based transfer, DMA-like copying, fault injection, validation tests, logging, and performance metrics.

## Why this project exists

This project is designed for software/validation/embedded roles in semiconductor companies such as Qualcomm, NVIDIA, AMD, ARM, and Intel. It demonstrates practical C programming, pointers, structs, pthreads, synchronization, driver-style abstractions, debugging, validation, and performance thinking.

## Features

- Simulated device registers: control, status, data, error code
- Device state machine: IDLE, BUSY, DATA_READY, ERROR, STOPPED
- Thread-safe ring buffer
- Producer-consumer model using pthreads
- Interrupt simulation using condition variables
- Driver APIs: init, open, close, read, ioctl
- DMA-like transfer using memory copy abstraction
- Fault injection:
  - corrupted packet
  - dropped interrupt
  - device error
- Validation framework with automated tests
- Logs written to `logs/driver.log`
- Metrics written to `results/metrics.csv`
- Valgrind script for memory checking

## Build

```bash
make
```

## Run normal mode

```bash
make run
```

or:

```bash
./build/driver_sim 8 0 0
```

Arguments:

```text
./build/driver_sim <runtime_seconds> <enable_dma> <fault_mode>
```

Fault modes:

```text
0 = no fault
1 = corrupt packet
2 = drop interrupt
3 = device error
```

## Run DMA mode

```bash
make run-dma
```

## Run fault mode

```bash
make run-fault
```

## Run tests

```bash
make test
```

## Run Valgrind

```bash
bash scripts/valgrind_check.sh
```

## Clean

```bash
make clean
```

## Architecture

```text
Validation/Test Layer
        |
Driver Layer
        |
Interrupt + Ring Buffer + DMA
        |
Simulated Hardware Device
```

## Interview Pitch

I built a Linux-based user-space device driver simulation framework in C. It models hardware registers and device states, uses pthreads for asynchronous producer-consumer behavior, implements interrupt-driven communication using condition variables, supports a thread-safe ring buffer, simulates DMA-like transfers, injects faults such as corrupted packets and dropped interrupts, and produces validation reports and performance metrics.

## Resume Bullet

Built a Linux-based user-space device driver simulation and validation framework in C, implementing register-level device modeling, interrupt-driven communication with pthread condition variables, thread-safe ring buffers, DMA-like transfer simulation, fault injection, automated validation tests, logging, and performance benchmarking.
