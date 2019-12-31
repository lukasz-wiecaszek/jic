.PHONY = clean clean_ut clean_pt

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17 -fno-exceptions -pthread

all: shared_ptr_ut shared_ptr_pt

shared_ptr_ut: shared_ptr_ut.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -L/usr/local/lib/gtest -lgtest

shared_ptr_ut.o: Makefile shared_ptr_ut.cpp shared_ptr.hpp
	$(CC) $(CXXFLAGS) --coverage -c shared_ptr_ut.cpp

shared_ptr_pt: shared_ptr_pt.o
	$(CC) $(CXXFLAGS) -o $@ $^

shared_ptr_pt.o: Makefile shared_ptr_pt.cpp shared_ptr.hpp
	$(CC) $(CXXFLAGS) -O2 -c shared_ptr_pt.cpp

clean: clean_ut clean_pt

clean_ut:
	@rm -f shared_ptr_ut *.o *.gcno > /dev/null 2>&1

clean_pt:
	@rm -f shared_ptr_pt *.o > /dev/null 2>&1
