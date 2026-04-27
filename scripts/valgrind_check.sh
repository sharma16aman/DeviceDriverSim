#!/bin/bash
set -e
make
valgrind --leak-check=full --track-origins=yes ./build/driver_sim 3 0 0
