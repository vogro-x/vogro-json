CXXFLAGS = -Wall -g -std=c++11

all: test json-writer-test

clean:
	rm -rf *.o test json-writer

test: json-writer.hpp test.cpp
	$(CXX) $(CXXFLAGS) json_writer.hpp test.cpp -o test
json-writer-test:json_writer.hpp json-writer-test.cpp
	$(CXX) $(CXXFLAGS) json_writer.hpp json-writer-test.cpp -o json-writer-test