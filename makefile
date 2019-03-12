LIB = -lm -fopenmp
CC = gcc

SRC = ./src/cnn.c ./src/mat.c 
OBJ = $(SRC:%.c=%.o)

SRC_trans = ./src/transform/transform.c
OBJ_trans = $(SRC_trans:%.c=%.o)

cnn.app: $(OBJ) ./src/main.o $(OBJ_trans) ./src/JpegCodecs/main.cpp ./src/JpegCodecs/src/JpegDecoder.o
	$(CC) $(OBJ) ./src/main.o $(LIB) -o $@
	$(CC) $(OBJ_trans) -o transform
	g++ -I ./src/JpegCodecs/src/  ./src/JpegCodecs/main.cpp ./src/JpegCodecs/src/JpegDecoder.o -o get_pixel	

./src/JpegCodecs/src/JpegDecoder.o: ./src/JpegCodecs/src/JpegDecoder.cpp
	g++ -c ./src/JpegCodecs/src/JpegDecoder.cpp -o $@

test:$(OBJ) ./src/test.o
	$(CC) $(OBJ) ./src/test.o $(LIB) -o $@
	
%.o:%.c
	$(CC) -c $(LIB) $< -o $@

./src/main.o: ./src/main.c
	$(CC) -c $(LIB) $< -o $@

./src/test.o: ./src/test.c
	$(CC) -c $(LIB) $< -o $@

clean:
	rm -rf *.app
	rm -rf test
	rm -rf ./src/*.o

clean_all:
	rm -rf *.app
	rm -rf test
	rm -rf ./src/*.o
	rm -rf ./src/transform/*.o
	rm -rf transform
	rm -rf get_pixel
	rm -rf ./src/JpegCodecs/src/*.o
