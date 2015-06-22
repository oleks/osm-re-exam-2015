.phony: all run run-sequential run-concurrent clean

CC=gcc
CFLAGS=-Werror -Wall -Wextra -pedantic -std=c99 -g
LDFLAGS=-lm

all: sequential-heap concurrent-heap

sequential-heap: sequential-heap.h sequential-heap.c
	$(CC) $(CFLAGS) -DUNITTEST_BINARY_HEAP -o sequential-heap \
    sequential-heap.c $(LDFLAGS)

concurrent-heap: concurrent-heap.h concurrent-heap.c
	$(CC) $(CFLAGS) -DUNITTEST_BINARY_HEAP -o concurrent-heap \
    concurrent-heap.c $(LDFLAGS)

run: run-sequential run-concurrent

run-sequential: data.txt sequential-heap
	@echo "Testing the sequential implementation.."
	cat data.txt | ./sequential-heap

run-concurrent: data.txt sequential-heap
	@echo "Testing the concurrent implementation.."
	cat data.txt | ./concurrent-heap

clean:
	rm -f sequential-heap
	rm -f concurrent-heap
