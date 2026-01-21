CC = gcc
CFLAGS = -Wall -Wextra -O2
PTHREAD = -pthread

ALL = pr9_1 sender receiver

all: $(ALL)

pr9_1: pr9_1.c
	$(CC) $(CFLAGS) $(PTHREAD) pr9_1.c -o pr9_1

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

receiver: receiver.c
	$(CC) $(CFLAGS) receiver.c -o receiver

clean:
	rm -f $(ALL)
