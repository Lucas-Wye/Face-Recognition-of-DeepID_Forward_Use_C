CC = gcc
SRC = transform.c
OBJ = $(SRC:%.c=%.o)

transform: $(OBJ)
	$(CC) -g $(OBJ) -o ../../transform

clean:
	rm -rf *.o
	rm -rf ../../transform
