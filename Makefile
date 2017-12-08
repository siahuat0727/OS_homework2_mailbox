all:master slave

CC := gcc
CFLAGS += -std=gnu99 -Wall

master:master.o
	$(CC) $(CFLAGS) -o master master.o

slave:slave.o
	$(CC) $(CFLAGS) -o slave slave.o

master.o:master.c
	$(CC) $(CFLAGS) -c master.c

slave.o:slave.c
	$(CC) $(CFLAGS) -c slave.c

clean:
	rm -rf *.o master slave
