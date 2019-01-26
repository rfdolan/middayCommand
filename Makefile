CC = gcc

CFLAGS = -g -Wall

all: mc0 mc1 mc2

mc0: v0/mc0.c
	$(CC) $(CFLAGS) -o mc0 v0/mc0.c

mc1: v1/mc1.c
	$(CC) $(CFLAGS) -o mc1 v1/mc1.c

mc2: v2/mc2.c
	$(CC) $(CFLAGS) -o mc2 v2/mc2.c

clean:
	$(RM) mc0 mc1 mc2 
