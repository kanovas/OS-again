CC = gcc
SRC = sum.c
C_FLAGS = -fPIC -pthread -L../lib/ -I../lib/ -Wl,-R../lib/

all: sum

sum:
	$(CC) -o sum $(C_FLAGS) $(SRC) -lrt

clean:
	rm -f sum
