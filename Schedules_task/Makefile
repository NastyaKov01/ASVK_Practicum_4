CC=g++
all: final

final: schedule.o generator.o
	$(CC) -o sc schedule.o generator.o

schedule.o: schedule.cpp generator.h
	$(CC) -c schedule.cpp

generator.o: generator.cpp generator.h
	$(CC) -c generator.cpp

clean:
	rm -rf *.o sc