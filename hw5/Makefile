CC = gcc
SRC = sum.c
C_FLAGS = -pthread

all: clean sum

sum:
	$(CC) -o sum $(C_FLAGS) $(SRC)

.PHONY : clean
clean:
	rm -rf sum
