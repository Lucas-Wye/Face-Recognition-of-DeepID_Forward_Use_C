LIB = -lm
CC = gcc

SRC = ./src/cnn.c ./src/mat.c 
OBJ = $(SRC:%.c=%.o)

cnn.app: $(OBJ) ./src/main.o
	$(CC) -g $(OBJ) ./src/main.o $(LIB) -o $@

test:$(OBJ) ./src/test.o
	$(CC) -g $(OBJ) ./src/test.o $(LIB) -o $@
	
%.o:%.c
	$(CC) -g -c $(LIB) $< -o $@

./src/main.o: ./src/main.c
	$(CC) -g -c $(LIB) $< -o $@

./src/test.o: ./src/test.c
	$(CC) -g -c $(LIB) $< -o $@

clean:
	rm -f *.app
	rm -f test
	rm -f ./src/*.o
