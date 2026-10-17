CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude

TARGET = memalloc_demo

SRCS = src/memalloc.c src/main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

.PHONY: all clean
