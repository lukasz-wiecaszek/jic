.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

all: listener_test

listener_test: listener_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

listener_test.o: Makefile listener_test.cpp listener.hpp
	$(CC) $(CXXFLAGS) --coverage -c listener_test.cpp

clean: clean_ut

clean_ut:
	@rm -f listener_test *.o *.gcno > /dev/null 2>&1
