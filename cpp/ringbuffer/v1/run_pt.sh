#!/bin/bash

set -e
set -x

make clean
make all

perf stat ./pt

