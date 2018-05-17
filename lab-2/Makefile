CXXFLAGS		+= -Wall
LDLIBS			+= -lfl

EXEC			= parser
BUILT_SOURCES	= lexer.c parser.c
BUILT_HEADERS	= $(BUILT_SOURCES:.c=.h)
SOURCES			= $(BUILT_SOURCES) main.cpp 

TEST_DIR		= test

all: $(EXEC)

$(EXEC): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

parser.c: parser.y lexer.c
	bison -d $<

lexer.c: lexer.l
	flex $<

debug: CXXFLAGS += -g
debug: $(EXEC)

test: $(EXEC)
	cd $(TEST_DIR) && ./run-tests.sh $(realpath $(EXEC))

clean:
	rm -f $(EXEC) *.o $(TEST_DIR)/output.log $(BUILT_SOURCES) $(BUILT_HEADERS)

.PHONY: clean test debug
