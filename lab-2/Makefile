CXXFLAGS		+= 	-Wall
LDLIBS			+= 	-lfl

YACC			=	bison
YFLAGS			=	-v -d
LEX				=	flex

EXEC			= 	parser

YACC_SOURCES	= 	parser.y
LEX_SOURCES		= 	lexer.l
SOURCES			= 	$(LEX_SOURCES:.l=.c) $(YACC_SOURCES:.y=.c) ast_node.cpp driver.cpp

TEST_DIR		= 	test
BUILT_FILES		= 	$(YACC_SOURCES:.y=.c) $(YACC_SOURCES:.y=.h) \
					$(LEX_SOURCES:.l=.c) $(LEX_SOURCES:.l=.h) \
					location.hh position.hh stack.hh \
					$(EXEC) *.o \
					parser.output \
					$(TEST_DIR)/output.log


all: $(EXEC)

$(EXEC): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

parser.c: $(YACC_SOURCES) lexer.c
	$(YACC) $(YFLAGS) -o $@ $<

lexer.c: $(LEX_SOURCES)
	$(LEX) $(LFLAGS) -o $@ $<

debug: CXXFLAGS += -g
debug: $(EXEC)

test: $(EXEC)
	cd $(TEST_DIR) && ./run-tests.sh $(realpath $(EXEC))

clean:
	rm -f  $(BUILT_FILES) 

.PHONY: clean test debug
