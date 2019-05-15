.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

all: semaphore_test

semaphore_test: semaphore_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

semaphore_test.o: Makefile semaphore_test.cpp semaphore.hpp
	$(CC) $(CXXFLAGS) --coverage -c semaphore_test.cpp

clean: clean_ut

clean_ut:
	@rm -f semaphore_test *.o *.gcno > /dev/null 2>&1
