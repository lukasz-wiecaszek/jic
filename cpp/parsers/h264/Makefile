.PHONY = clean

CC := g++
CXXFLAGS := -Wall -Wextra -pedantic -O2 -std=c++17 -fno-exceptions

parser_h264_test: parser_h264_test.o parser_h264.o
	$(CC) $(CXXFLAGS) -o $@ $^

parser_h264_test.o: parser_h264_test.cpp
	$(CC) $(CXXFLAGS) -c parser_h264_test.cpp

parser_h264.o: parser_h264.cpp parser_h264.hpp
	$(CC) $(CXXFLAGS) -c parser_h264.cpp

clean:
	@rm -f parser_h264_test *.o > /dev/null 2>&1
