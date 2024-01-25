CC = gcc
CFLAGS = -Wall -pedantic

SRC = main.c logger.c parse.c child.c parent.c
TARGET = as1

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) log.log
