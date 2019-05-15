#!/bin/bash

set -e
set -x

make clean
make all

perf stat ./pt -c 1000 -i100000000

