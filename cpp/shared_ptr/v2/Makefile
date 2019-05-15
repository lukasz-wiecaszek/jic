.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread -DDEBUG_SHARED_PTR

all: shared_ptr_test

shared_ptr_test: shared_ptr_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

shared_ptr_test.o: Makefile shared_ptr_test.cpp shared_ptr.hpp
	$(CC) $(CXXFLAGS) --coverage -c shared_ptr_test.cpp

clean: clean_ut

clean_ut:
	@rm -f shared_ptr_test *.o *.gcno > /dev/null 2>&1
