CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread

TARGET = lab10_rwlock
SRC = lab10_rwlock.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
