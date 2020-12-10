CXX = g++

CXXFLAGS = -Ofast -std=c++11 -Wall -mtune=native -DNDEBUG

ofiles = test.o write_pvd.o write_vtu.o grid.o

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

all: test

test: $(ofiles) grid.h
	$(CXX) $(ofiles) $(CXXFLAGS) -lm -o test

clean:
	rm -rf test *.o *.ps ./data/*.vtu ./data/*.pvd

printout:
	a2ps -1 -T 2 -o out.ps grid.h
