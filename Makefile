#######################################
# Compiler options
PROG=evac
CXX=g++
CXXFLAGS=-std=c++14 -Wall -Wextra -pedantic  -I 3rdparty -MMD

# Sources and targets
SRCDIR=src
SRC=$(wildcard $(SRCDIR)/*.cpp)
HDR=$(wildcard $(SRCDIR)/*.h)
OBJ=$(patsubst %.cpp, %.o, $(SRC))
DEP=$(patsubst %.o, %.d, $(OBJ))

# Execution options
OPT = "sample1.bmp"

#######################################
# Primary rule
all: $(PROG)

# Generic rule
$(PROG): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# (this rule is implicit)
#%.o: %.cpp %.h
#	$(CXX) $(CXXFLAGS) -c $< -o $@

#######################################
# Specific rules

# Tidy up
clean:
	rm -f $(OBJ) $(PROG) $(DEP)

# Run executable
run: $(PROG)
	./$(PROG) $(OPT)

# Valgrind check
valgrind: $(PROG)
	valgrind --leak-check=full ./$(PROG) $(OPT)

#######################################
# Shortcuts and dependencies

c: clean
r: run
v: valgrind
cr: clean run

-include $(DEP)
