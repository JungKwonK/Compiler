CXXFLAGS		+= 	-Wall
LDLIBS			+= 	-lfl

YACC			=	bison
YFLAGS			=	-v -d
LEX				=	flex

YACC_SOURCE 	= 	parser.y
LEX_SOURCE		= 	lexer.l

YACC_C			=	$(YACC_SOURCE:.y=.c)
LEX_C			=	$(LEX_SOURCE:.l=.c)

EXEC			= 	parser

TEST_CMD		=	./test/run-tests.sh $(realpath ${EXEC})
TEST_LOG		= 	test.log

SOURCES			= 	$(YACC_C) $(LEX_C) ast_node.cpp driver.cpp

BUILT_FILES		= 	$(YACC_C) $(YACC_C:.c=.h) \
					$(LEX_C) $(LEX_C:.c=.h) \
					location.hh position.hh stack.hh \
					$(EXEC) \
					$(TEST_LOG) \
					parser.output \
					*.o

all: $(EXEC)

$(EXEC): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(YACC_C): $(YACC_SOURCE) $(LEX_C)
	$(YACC) $(YFLAGS) -o $@ $<

$(LEX_C): $(LEX_SOURCE)
	$(LEX) $(LFLAGS) -o $@ $<

debug: CXXFLAGS += -g
debug: $(EXEC)

test: $(EXEC)
	$(TEST_CMD) > $(TEST_LOG)

clean:
	$(RM) $(BUILT_FILES)

.PHONY: clean test debug
