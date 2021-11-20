CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -Wpedantic
GMPLIB = `pkg-config --libs gmp`

all: keygen encrypt decrypt

randstate.o: randstate.c randstate.h
	$(CC) $(CFLAGS) randstate.c -c

numtheory.o: numtheory.c numtheory.h
	$(CC) $(CFLAGS) numtheory.c -c

rsa.o: rsa.c rsa.h
	$(CC) $(CFLAGS) rsa.c -c

keygen.o: keygen.c
	$(CC) $(CFLAGS) keygen.c -c

encrypt.o: encrypt.c
	$(CC) $(CFLAGS) encrypt.c -c

decrypt.o: decrypt.c
	$(CC) $(CFLAGS) decrypt.c -c

keygen: randstate.o numtheory.o rsa.o keygen.o
	$(CC) $(CFLAGS) $^ -o keygen ${GMPLIB}

encrypt: randstate.o numtheory.o rsa.o encrypt.o
	$(CC) $(CFLAGS) $^ -o encrypt ${GMPLIB}

decrypt: randstate.o numtheory.o rsa.o decrypt.o
	$(CC) $(CFLAGS) $^ -o decrypt ${GMPLIB}

format:
	clang-format -i -style=file randstate.c randstate.h numtheory.c numtheory.h rsa.c rsa.h keygen.c encrypt.c decrypt.c

clean:
	rm -f *.o keygen encrypt decrypt