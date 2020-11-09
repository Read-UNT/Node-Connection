Boomer: main.o
	g++  -o Boomer main.o

main.o: main.cpp boomer.h
	g++ -std=c++11 -c main.cpp