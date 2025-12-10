CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lpthread -lrt

SENDER = sender_new
RECEIVER = receiver_new

all: $(SENDER) $(RECEIVER)

$(SENDER): sender_new.c
	$(CC) $(CFLAGS) sender_new.c -o $(SENDER) $(LIBS)

$(RECEIVER): receiver_new.c
	$(CC) $(CFLAGS) receiver_new.c -o $(RECEIVER) $(LIBS)

run_sender: $(SENDER)
	./$(SENDER)

run_receiver: $(RECEIVER)
	./$(RECEIVER)

clean:
	rm -f $(SENDER) $(RECEIVER)

rebuild: clean all

.PHONY: all clean rebuild run_sender run_receiver
