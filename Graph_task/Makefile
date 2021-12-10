CC=g++
TESTFLAGS=-lgtest -pthread
all: final clean

final: test.o graph_types.o factory.o path_finder.o
	$(CC) -o t test.o graph_types.o factory.o path_finder.o $(TESTFLAGS)

test.o: test.cpp graph_types.h factory.h path_finder.h
	$(CC) -c test.cpp $(TESTFLAGS)

path_finder.o: path_finder.cpp path_finder.h
	$(CC) -c path_finder.cpp

factory.o: factory.cpp factory.h
	$(CC) -c factory.cpp

graph_types.o: graph_types.cpp graph_types.h
	$(CC) -c graph_types.cpp

clean:
	rm -rf *.o