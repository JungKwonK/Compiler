CXX			= clang++

EXEC 		= parser
SOURCES 	= $(EXEC).cpp
OBJECTS 	= $(SOURCES:.cpp=.o)

all: $(OBJECTS) $(EXEC)

.SECONDEXPANSION:
$(EXEC): $$@.o
	$(CXX) -g $^ -o $@

%.o: %.c
	$(CXX) -g -c $< -o $@

test: $(EXEC)
	cd test && ./run-tests.sh $(realpath $(EXEC))

clean:
	rm -f $(EXEC) *.o test/output.log

.PHONY: clean test
