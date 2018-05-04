CXX			= clang++

EXEC 		= parser
SOURCES 	= $(EXEC).cpp
OBJECTS 	= $(SOURCES:.cpp=.o)

all: $(OBJECTS) $(EXEC)

.SECONDEXPANSION:
$(EXEC): $$@.o
	$(CXX) $^ -o $@

%.o: %.c
	$(CXX) -c $< -o $@

clean:
	rm -f $(EXEC) *.o

.PHONY:
	clean
