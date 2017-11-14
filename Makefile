PROG=evac
CXX=g++
CXXFLAGS=-std=c++14 -Wall -Wextra -pedantic

SRCDIR=src
SRC=$(wildcard $(SRCDIR)/*.cpp)
HDR=$(wildcard $(SRCDIR)/*.h)
OBJ=$(patsubst %.cpp, %.o, $(SRC))

all: $(PROG)

evac: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) evac
