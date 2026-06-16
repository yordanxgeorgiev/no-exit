CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./src -g -O0

SRC_DIR = src
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/parser.c $(SRC_DIR)/router.c
OBJS = $(SRCS:.c=.o)

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)