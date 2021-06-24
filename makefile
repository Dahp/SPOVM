CC = g++
FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lpthread

main: main.cpp
	g++ -c main.cpp
	$(CC) main.o -o main $(FLAGS)
	rm *.o