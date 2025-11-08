CC = cc
CFLAGS = -Wall -Wextra -O2
TARGET = calc_server
OBJS = server.o http_handler.o calc.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

server.o: server.c http_handler.h
	$(CC) $(CFLAGS) -c server.c

http_handler.o: http_handler.c http_handler.h calc.h
	$(CC) $(CFLAGS) -c http_handler.c

calc.o: calc.c calc.h
	$(CC) $(CFLAGS) -c calc.c

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
