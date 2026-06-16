CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -I./src -I/usr/include/postgresql -g -O0
LDFLAGS =
LIBS = -lpq

SRC_DIR = src
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/parser.c $(SRC_DIR)/router.c $(SRC_DIR)/db.c
OBJS = $(SRCS:.c=.o)

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)