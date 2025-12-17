CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -pthread

TARGET = lab8
SRC = lab8.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
