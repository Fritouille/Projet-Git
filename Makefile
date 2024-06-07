#CFLAGS = -g -Wall -Wextra -pedantic -Wno-unused-parameter
CFLAGS = -g -Wall -Wextra -pedantic 
CC = gcc

PROGRAMS = myGit

.PHONY:	all clean

all: $(PROGRAMS)

myGit: List.o WorkTree.o Commit.o myGit.o
	$(CC) -o $@ $(CFLAGS) $^

List.o: List.c
	$(CC) $(CFLAGS) -c $^

WorkTree.o: WorkTree.c
	$(CC) $(CFLAGS) -c $^ 

Commit.o: Commit.c
	$(CC) $(CFLAGS) -c $^

myGit.o: myGit.c
	$(CC) $(CFLAGS) -c $^ 
	
clean:
	rm -f *.o *~ $(PROGRAMS)