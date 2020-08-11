CC=gcc -std=c99 -Wall -fstack-protector
CFLAGS=-I.
DEPS = hash_shield.h tests.h
CLIBS=-lssl -lcrypto
%.o: %.c $(DEPS)
	$(CC) -c -o $@  $< $(CFLAGS) 
	
all: hash_shield_main.o util.o hash_core.o tests.o fs_notification.o
	$(CC) -o hash_shield_main hash_shield_main.o util.o hash_core.o fs_notification.o tests.o $(CLIBS) 
	
.PHONY: clean
clean:
	rm *.o hash_shield_main
