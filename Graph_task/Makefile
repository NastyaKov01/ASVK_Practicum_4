CC=g++
all: final clean

final: graph.o graph_types.o factory.o path_finder.o
	$(CC) -o g graph.o graph_types.o factory.o path_finder.o

graph.o: graph.cpp graph_types.h factory.h
	$(CC) -c graph.cpp

path_finder.o: path_finder.cpp path_finder.h
	$(CC) -c path_finder.cpp

factory.o: factory.cpp factory.h
	$(CC) -c factory.cpp

graph_types.o: graph_types.cpp graph_types.h
	$(CC) -c graph_types.cpp

clean:
	rm -rf *.o