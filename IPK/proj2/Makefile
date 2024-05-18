CC = gcc
CFLAGS = -g -std=gnu99 -Wall -Wextra
LFLAGS = -lpcap

project = ipk-l4-scan

.PHONY: clean

$(project): $(project).c src/parse_args.o src/common.o src/tcp_scan.o src/udp_scan.o  
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS) 
	
clean:
	rm -rf src/*.o