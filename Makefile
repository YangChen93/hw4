
CC = g++-4.7
CFLAGS = -O3

all: randtrack 

randtrack: list.h hash.h defs.h randtrack_global_lock.cc
	$(CC) $(CFLAGS) -pthread randtrack_global_lock.cc -o randtrack_global_track


clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock
