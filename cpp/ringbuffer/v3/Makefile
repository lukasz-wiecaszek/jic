.PHONY = clean clean_ut clean_pt

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

all: ringbuffer_test pt

ringbuffer_test: ringbuffer_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

ringbuffer_test.o: Makefile ringbuffer_test.cpp ringbuffer.hpp
	$(CC) $(CXXFLAGS) --coverage -DDEBUG_RINGBUFFER -c ringbuffer_test.cpp

pt: pt.o
	$(CC) $(CXXFLAGS) -o $@ $^

pt.o: Makefile pt.cpp ringbuffer.hpp
	$(CC) $(CXXFLAGS) -c pt.cpp

clean: clean_ut clean_pt

clean_ut:
	@rm -f ringbuffer_test *.o *.gcno > /dev/null 2>&1

clean_pt:
	@rm -f pt *.o > /dev/null 2>&1
