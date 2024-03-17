CC = gcc
CFLAGS = -Wall -std=c99 -pedantic
CLIENT = client
CLIENT_OBJS = client.o
HTTPD = httpd
HTTPD_OBJS = httpd.o net.o
PROGS = $(CLIENT) $(HTTPD)

all : $(PROGS)

$(CLIENT) : $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJS)

client.o : client.c header.h
	$(CC) $(CFLAGS) -c client.c

$(HTTPD) : $(HTTPD_OBJS)
	$(CC) $(CFLAGS) -o $(HTTPD) $(HTTPD_OBJS)

httpd.o : httpd.c header.h
	$(CC) $(CFLAGS) -c httpd.c

net.o : net.c header.h
	$(CC) $(CFLAGS) -c net.c

clean :
	rm *.o $(PROGS) core