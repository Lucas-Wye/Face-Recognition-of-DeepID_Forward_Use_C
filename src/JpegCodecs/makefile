main: main.cpp JpegDecoder.o
	g++ -I src/  main.cpp JpegDecoder.o -o ../../get_pixel
	rm -rf *.o
JpegDecoder.o : src/JpegDecoder.cpp
	g++ -c src/JpegDecoder.cpp
clean:
	rm -rf *.o ../../get_pixel
