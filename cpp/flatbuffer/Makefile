.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

all: flatbuffer_test

flatbuffer_test: flatbuffer_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

flatbuffer_test.o: Makefile flatbuffer_test.cpp flatbuffer.hpp
	$(CC) $(CXXFLAGS) --coverage -c flatbuffer_test.cpp

clean: clean_ut

clean_ut:
	@rm -f flatbuffer_test *.o *.gcno > /dev/null 2>&1
