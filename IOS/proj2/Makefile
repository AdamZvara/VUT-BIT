CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -g
LDFLAGS=-lrt -lpthread

SRC=proj2.o parse_args.o 

.PHONY: clean

proj2: $(SRC)
	$(CC) $^ -o $@ $(LDFLAGS)

%.c: %.o

clean:
	rm -rf *.o 

