CC = gcc
CFLAGS = -Wall

all: pipe_example process1 process2

pipe_example: pipe_example.c
	$(CC) $(CFLAGS) -o $@ $<

process1: process1.c
	$(CC) $(CFLAGS) -o $@ $<

process2: process2.c
	$(CC) $(CFLAGS) -o $@ $<

run_pipe: pipe_example
	./pipe_example

run_fifo: process1 process2
	./process2 &
	sleep 1
	./process1

clean:
	rm -f pipe_example process1 process2 time_fifo

.PHONY: all run_pipe run_fifo clean