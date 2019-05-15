.PHONY = clean clean_ut

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions -pthread

READ_PEEK_TESTS := \
    read_peek_u8.test  \
    read_peek_s8.test  \
    read_peek_u16.test \
    read_peek_s16.test \
    read_peek_u32.test \
    read_peek_s32.test \
    read_peek_u64.test \
    read_peek_s64.test

all: istream_position_test istream_test

istream_position_test: istream_position_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

istream_position_test.o: Makefile istream_position_test.cpp istream_position.hpp
	$(CC) $(CXXFLAGS) --coverage -c istream_position_test.cpp

istream_test: istream_test.o
	$(CC) $(CXXFLAGS) --coverage -o $@ $^ -lgtest

istream_test.o: Makefile istream_test.cpp istream.hpp $(READ_PEEK_TESTS)
	$(CC) $(CXXFLAGS) --coverage -c istream_test.cpp

clean: clean_ut

clean_ut:
	@rm -f istream_position_test istream_test *.o *.gcno > /dev/null 2>&1
