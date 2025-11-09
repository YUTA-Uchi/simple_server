CC = cc
CFLAGS = -Wall -Wextra -O2
SRCS_SERVER = server.c http_handler.c calc.c
SRCS_CLIENT = client.c
OBJS_SERVER = $(SRCS_SERVER:.c=.o)
OBJS_CLIENT = $(SRCS_CLIENT:.c=.o)
OBJS = $(OBJS_SERVER) $(OBJS_CLIENT)

all: server client

server: $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $@ $^

client: $(OBJS_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f server client $(OBJS)

.PHONY: all clean
