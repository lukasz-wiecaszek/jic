.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

all: binary_semaphore_test

binary_semaphore_test: binary_semaphore_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

binary_semaphore_test.o: Makefile binary_semaphore_test.cpp binary_semaphore.hpp
	$(CC) $(CXXFLAGS) --coverage -c binary_semaphore_test.cpp

clean: clean_ut

clean_ut:
	@rm -f binary_semaphore_test *.o *.gcno > /dev/null 2>&1
