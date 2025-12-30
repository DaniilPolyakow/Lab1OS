CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread

TARGET = lab11_condvar
SRC = lab11_condvar.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
