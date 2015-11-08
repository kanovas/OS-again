CC = gcc
SRC = sum.c
C_FLAGS = -fPIC -L../lib/ -I../lib/ -Wl,-R../lib/

all: sum

sum:
	$(CC) -o sum $(C_FLAGS) $(SRC)

clean:
	rm -f sum
