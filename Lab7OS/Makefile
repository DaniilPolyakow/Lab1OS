CC = gcc
CFLAGS = -Wall -Wextra -O2

SENDER = sender_sysv
RECEIVER = receiver_sysv

all: $(SENDER) $(RECEIVER)

$(SENDER): sender_sysv.c
	$(CC) $(CFLAGS) sender_sysv.c -o $(SENDER)

$(RECEIVER): receiver_sysv.c
	$(CC) $(CFLAGS) receiver_sysv.c -o $(RECEIVER)

run_sender: $(SENDER)
	./$(SENDER)

run_receiver: $(RECEIVER)
	./$(RECEIVER)

clean:
	rm -f $(SENDER) $(RECEIVER)

rebuild: clean all

.PHONY: all clean rebuild run_sender run_receiver
