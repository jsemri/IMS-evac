#######################################
# Compiler options
PROG=evac
CXX=g++
CXXFLAGS=-std=c++14 -Wall -Wextra -pedantic  -I 3rdparty -O3 -MMD

# Sources and targets
SRCDIR=src
SRC=$(wildcard $(SRCDIR)/*.cpp)
HDR=$(wildcard $(SRCDIR)/*.h)
OBJ=$(patsubst %.cpp, %.o, $(SRC))
DEP=$(patsubst %.o, %.d, $(OBJ))

DOCDIR = doc
DOC = report.pdf
DOX = Doxyfile
ZIP = xsemri00.zip

# Execution options
OPT = ./experiments/D1.bmp -p 200 -t 0 -s 5 -r 1
TEST_SH = exp.sh

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
	rm -f $(OBJ) $(PROG) $(DEP) $(DOC) $(ZIP) $(DOX) -r html;

# Run executable
run: $(PROG)
	./$(PROG) $(OPT) 

# Valgrind check
valgrind: $(PROG)
	valgrind --leak-check=full ./$(PROG) $(OPT)

# Run experiments
experiment: $(PROG) $(TEST_SH)
	./$(TEST_SH)

# Compile documentation
documentation: $(DOCDIR)
	make -C $(DOCDIR) && cp $(DOCDIR)/$(DOC) .;

# Zip
zip: $(SRCDIR) 3rdparty experiments Makefile $(DOC)
	zip $(ZIP) -r $^

# Set Doxygen generation up
$(DOX):
	doxygen -g $@
	
# Create Doxygen
doxygen: $(DOX)
	(cat $(DOX); echo "INPUT=src"; echo "JAVADOC_AUTOBRIEF=YES"; \
		echo "GENERATE_LATEX=NO") | doxygen -

#######################################
# Shortcuts and dependencies

c: clean
r: run
v: valgrind
cr: clean run
e: experiment
d: documentation
z: zip
dz: documentation zip

-include $(DEP)
