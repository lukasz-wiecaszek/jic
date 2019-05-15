.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

all: endianness_test strtointeger_test

endianness_test: endianness_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

endianness_test.o: Makefile endianness_test.cpp endianness.hpp
	$(CC) $(CXXFLAGS) --coverage -c endianness_test.cpp

strtointeger_test: strtointeger_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

strtointeger_test.o: Makefile strtointeger_test.cpp strtointeger.hpp
	$(CC) $(CXXFLAGS) --coverage -c strtointeger_test.cpp

clean: clean_ut

clean_ut:
	@rm -f endianness_test strtointeger_test *.o *.gcno > /dev/null 2>&1
