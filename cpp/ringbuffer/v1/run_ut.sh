#!/bin/bash

set -e
set -x

UT_SRC='ringbuffer.hpp'
UT_BIN='ringbuffer_test'

make clean
make all

LCOV_EXTRACT=
for file in ${UT_SRC}
do
    LCOV_EXTRACT="${LCOV_EXTRACT} ${PWD}/${file}"
done

lcov --base-directory . --directory . --initial --capture --output-file coverage.init

for file in ${UT_BIN}
do
    ./${file}
done

lcov --rc lcov_branch_coverage=1 --directory . --capture  --output-file coverage.run
lcov --rc lcov_branch_coverage=1 --add-tracefile coverage.init --add-tracefile coverage.run --output-file coverage.total
lcov --rc lcov_branch_coverage=1 --extract coverage.total ${LCOV_EXTRACT} --output-file coverage.info
genhtml --rc lcov_branch_coverage=1 coverage.info --output-directory lcov.d
rm coverage.*

for file in ${UT_BIN}
do
    rm ${file}.gcda
done
