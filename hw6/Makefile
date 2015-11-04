CC = gcc
SRC = semaphore.c
C_FLAGS = -fPIC -L../lib/ -I../lib/ -Wl,-R../lib/

all: run

run:
	$(CC) -o semaphore $(C_FLAGS) $(SRC)

clean:
	rm -f semaphore
