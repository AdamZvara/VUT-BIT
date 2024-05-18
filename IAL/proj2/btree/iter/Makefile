CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -g -Wextra -lm 
FILES=btree.c ../btree.c stack.c ../test_util.c ../test.c
ADV=btree.c ../btree.c advanced.c stack.c

.PHONY: test clean adv

test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)
	./test

adv: $(ADV)
	$(CC) $(CFLAGS) $^ -o $@
	./adv > adv.out
	diff adv.out advanced.out

clean:
	rm -f test adv
