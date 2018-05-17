CXXFLAGS	+= -Wall

EXEC 		= parser
SOURCES 	= $(EXEC).cpp util.cpp
OBJECTS 	= $(SOURCES:.cpp=.o)

TEST_DIR	= test

all: $(OBJECTS) $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS += -g
debug: $(EXEC)

test: $(EXEC)
	cd $(TEST_DIR) && ./run-tests.sh $(realpath $(EXEC))

clean:
	rm -f $(EXEC) *.o $(TEST_DIR)/output.log

.PHONY: clean test
