.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

all: workqueue_test

workqueue_test: workqueue_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

workqueue_test.o: Makefile workqueue_test.cpp workqueue.hpp work.hpp
	$(CC) $(CXXFLAGS) --coverage -c workqueue_test.cpp

clean: clean_ut

clean_ut:
	@rm -f workqueue_test *.o *.gcno  > /dev/null 2>&1
