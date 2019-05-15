.PHONY = clean

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions

parser_mpeg2ts_test: parser_mpeg2ts_test.o parser_mpeg2ts.o
	$(CC) $(CXXFLAGS) -o $@ $^

parser_mpeg2ts_test.o: parser_mpeg2ts_test.cpp parser_mpeg2ts.hpp
	$(CC) $(CXXFLAGS) -c parser_mpeg2ts_test.cpp

parser_mpeg2ts.o: parser_mpeg2ts.cpp parser_mpeg2ts.hpp
	$(CC) $(CXXFLAGS) -c parser_mpeg2ts.cpp

clean:
	@rm -f parser_mpeg2ts_test *.o > /dev/null 2>&1
