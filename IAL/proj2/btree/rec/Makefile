CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm -g
FILES=btree.c ../btree.c ../test_util.c ../test.c
ADV=btree.c ../btree.c advanced.c

.PHONY: test clean

test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)
	./test

adv: $(ADV)
	$(CC) $(CFLAGS) $^ -o $@
	./adv > adv.out
	diff adv.out advanced.out

clean:
	rm -f test adv
