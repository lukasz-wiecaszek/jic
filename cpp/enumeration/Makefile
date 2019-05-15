.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

all: enumeration_test

enumeration_test: enumeration_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

enumeration_test.o: Makefile enumeration_test.cpp card_colour.hpp enumeration.hpp
	$(CC) $(CXXFLAGS) --coverage -c enumeration_test.cpp

clean: clean_ut

clean_ut:
	@rm -f enumeration_test *.o *.gcno > /dev/null 2>&1
