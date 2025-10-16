CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = myls
SOURCE = master.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

.PHONY: clean
