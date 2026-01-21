CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS = -pthread

TARGET = rwlock_demo
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
