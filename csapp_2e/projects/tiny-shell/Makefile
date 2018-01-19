
CC = gcc
CFLAGS = -Wall

PROGRAM += ts

OBJS += tiny_shell.o
OBJS += random_fork.o
OBJS += job.o


PHONY += all
PHONY += clean
PHONY += TAGS

.PHONY: $(PHONY)

all: $(PROGRAM)

ts: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

tiny_shell.o: shellex.c
	$(CC) $(CFLAGS) -c $< -o $@

job.o: job.c job.h

clean:
	rm -f $(OBJS) $(TOBJS)
