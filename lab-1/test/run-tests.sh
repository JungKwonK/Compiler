#!/bin/sh

# to run this script, call `make test` on the top-level Makefile

USAGE="Usage: $(basename "$0") PARSER_EXEC\nPARSER_EXEC is the parser executable file that is being tested"

# check arguments
if [ $# -ne 1 ]; then
	echo "Error: Incorrect number of arguments"
	echo "$USAGE"
	exit 1
fi
EXEC="$1"
if [ ! -f $EXEC ] && [ ! -L $EXEC ]; then
	echo "Error: Executable not found: $EXEC" 
	echo "$USAGE"
	exit 1
fi

OUTPUT_LOG=output.log
echo "Begin testing..."
echo "test results $(date)" > $OUTPUT_LOG
# testing loop
for TEST in $(ls -d */)
do
	# run test
	OUTPUT=$($EXEC $TEST/input.txt) 2>&1
	if [ "$OUTPUT" = "$(cat $TEST/output.txt)" ]; then
		OUTCOME="[PASS]"
	else
		OUTCOME="[FAIL]"
	fi
	
	# print results to log file
	echo \
		"Test $TEST: $OUTCOME\
		----------------------------------BEGIN-OUTPUT---------------------------------\
		$OUTPUT\
		-----------------------------------END-OUTPUT----------------------------------"\
		 >> $OUTPUT_LOG
	
	# print results to terminal
	PADDING=$((-80 + ${#OUTCOME}))
	printf "%${PADDING}s%s\n" "$TEST:" "$OUTCOME"
done
echo "...end of testing."

exit 0
