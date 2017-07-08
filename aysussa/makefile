CCO = gcc -o
CC1 = gcc -c
CFLAGS = -Wall
LFLAGS = -lreadline

TARGETS = aysussa

default: aysussa

all: $(TARGETS)

aysussa: aysussa.o 
	$(CCO) $(CFLAGS) aysussa.o -o aysussa $(LFLAGS)

aysussa.o: aysussa.c completion.h utils.h parse.h command.h
	$(CC1)  $(CFLAGS) aysussa.c

clean:
	rm -f *.o *~ a.out *.txt $(TARGETS)

.c.o:
	$(CC) $(CFLAGS) -c $<
