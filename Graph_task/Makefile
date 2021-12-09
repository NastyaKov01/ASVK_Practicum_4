CC=g++
all: final clean

final: graph.o graph_types.o factory.o
	$(CC) -o g graph.o graph_types.o factory.o

graph.o: graph.cpp graph_types.h factory.h
	$(CC) -c graph.cpp

factory.o: factory.cpp factory.h
	$(CC) -c factory.cpp

graph_types.o: graph_types.cpp graph_types.h
	$(CC) -c graph_types.cpp

clean:
	rm -rf *.o