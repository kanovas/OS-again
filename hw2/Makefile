CC = gcc
SRC = joiner.c
C_FLAGS = -fPIC -L../lib/ -I../lib/ -Wl,-R../lib/

all: join

join:
	$(CC) -o join $(C_FLAGS) $(SRC)

clean:
	rm -f join
