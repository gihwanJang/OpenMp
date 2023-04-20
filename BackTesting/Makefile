CC = g++
CFLAGS = -std=c++11 -Wall
INCLUDE_PATH = -I./include
LDFLAGS = -lcurl

all: program

program: BackTesting.o DS_timer.o
	$(CC) $(CFLAGS) -o ./build/program DS_timer.o BackTesting.o $(LDFLAGS)

BackTesting.o: ./src/BackTesting.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -c ./src/BackTesting.cpp

DS_timer.o: ./src/DS_timer.cpp
	$(CC) $(CFLAGS) $(INCLUDE_PATH) -c ./src/DS_timer.cpp

clean:
	rm -f *.o