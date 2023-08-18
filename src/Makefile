SHELL   = /bin/bash
#
CC	=	gcc
#
CFLAGS	=	-c

.c.o:
	$(CC) $(CFLAGS) $<

OBJ	=	crc32l.o getsynfiles.o  sockeye.o suckit.o words.o \
		data.o parse.o readit.o streq.o utils.o

all:	$(OBJ);
	gcc -o srvr crc32l.o getsynfiles.o sockeye.o words.o data.o parse.o readit.o streq.o utils.o -lcurl;
	gcc -o test suckit.o utils.o

clnt:	$(OBJ);
	gcc -o test suckit.o utils.o

srvr:	$(OBJ);
	gcc -o srvr crc32l.o getsynfiles.o sockeye.o words.o data.o parse.o readit.o streq.o utils.o -lcurl

crc32l.o:	crc32l.c
getsynfiles.o:	getsynfiles.c
sockeye.o:	sockeye.c
suckit.o:	suckit.c
words.o:	words.c
data.o:		data.c
parse.o:	parse.c
readit.o:	readit.c
streq.o:	streq.c
utils.o:	utils.c

clean:
	rm *.o
	rm srvr test

