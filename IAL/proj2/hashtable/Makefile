CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm -g
FILES=hashtable.c test.c test_util.c

.PHONY: test clean advanced

test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)
	./test

advanced: hashtable.c test-advanced.c
	$(CC) $(CFLAGS) $^ -o $@ -lm
	./advanced > test-advanced.out
	diff test-advanced.out test-advanced.output

clean:
	rm -f test advanced
