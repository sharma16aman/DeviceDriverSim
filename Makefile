CC=gcc
CFLAGS=-D_DEFAULT_SOURCE -Wall -Wextra -Wpedantic -std=c11 -g -O2 -Iinclude
LDFLAGS=-pthread

SRC=src/device.c src/driver.c src/ring_buffer.c src/interrupt.c src/dma.c src/logger.c src/metrics.c
APP_SRC=src/main.c $(SRC)
TEST_SRC=src/validator.c tests/test_basic.c tests/test_buffer.c tests/test_interrupt.c tests/test_fault.c tests/test_performance.c $(SRC)

APP=build/driver_sim
TEST=build/test_runner

.PHONY: all run test clean dirs

all: dirs $(APP) $(TEST)

dirs:
	mkdir -p build logs results

$(APP): $(APP_SRC)
	$(CC) $(CFLAGS) $(APP_SRC) -o $(APP) $(LDFLAGS)

$(TEST): $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST) $(LDFLAGS)

run: all
	./$(APP) 8 0 0

run-dma: all
	./$(APP) 8 1 0

run-fault: all
	./$(APP) 8 0 1

test: all
	./$(TEST)

clean:
	rm -rf build
	rm -f logs/*.log results/*.csv results/*.txt
