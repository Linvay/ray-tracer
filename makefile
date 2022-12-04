all: main

CC = g++

# main: main.cpp algebra3.o imageIO.o
# 	$(CC) -g -o main main.cpp algebra3.o imageIO.o

main: main.cpp algebra3.o imageIO.o Object.o
	$(CC) -g -o main main.cpp algebra3.o imageIO.o Object.o

algebra3.o: algebra3.cpp algebra3.h
	$(CC) -c algebra3.cpp

imageIO.o: imageIO.cpp imageIO.h
	$(CC) -c imageIO.cpp

Object.o: Object.cpp Object.h
	$(CC) -c Object.cpp

clean:
	del main.exe algebra3.o imageIO.o Object.o