CXX			= clang++

EXEC 		= parser
SOURCES 	= $(EXEC).cpp
OBJECTS 	= $(SOURCES:.cpp=.o)

TEST_DIR	= test

all: $(OBJECTS) $(EXEC)

.SECONDEXPANSION:
$(EXEC): $$@.o
	$(CXX) -g $^ -o $@

%.o: %.c
	$(CXX) -g -c $< -o $@

test: $(EXEC)
	cd $(TEST_DIR) && ./run-tests.sh $(realpath $(EXEC))

clean:
	rm -f $(EXEC) *.o $(TEST_DIR)/output.log

.PHONY: clean test
