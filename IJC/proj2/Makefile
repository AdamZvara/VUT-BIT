CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra

LIB_SRC=$(wildcard htab*.c)
LIB_OBJ=$(patsubst %.c,%.o,$(LIB_SRC))
LIB_HEADERS=htab.h htab_private.h


.PHONY: clean

all: tail wordcount wordcount-dynamic

tail: tail.o
	$(CC) $(CLFAGS) $< -o $@

wordcount: wordcount.o io.o libhtab.a 
	$(CC) $(CFLAGS) wordcount.o io.o -o $@ -static -L. -lhtab

wordcount-dynamic: wordcount.o io.o libhtab.so
	$(CC) $(CFLAGS) wordcount.o io.o -o $@ -L. -lhtab

wordcount.o: wordcount.c
	$(CC) $(cflags) -c $<

#static library
libhtab.a: $(LIB_OBJ) $(LIB_HEADERS)
	ar rcs $@ $^

#dynamic library
libhtab.so: CFLAGS+=-fPIC
libhtab.so: $(LIB_OBJ) $(LIB_HEADERS)
	$(CC) $(CFLAGS) -shared $^ -o $@

#create all object files for library
htab_%.o: CFLAGS+=-fPIC
htab_%.o: htab_%.c LIB_HEADERS
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *.gch
