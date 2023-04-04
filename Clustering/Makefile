CFLAGS = `pkg-config --cflags python`
LIBS = `pkg-config --libs python`

main.out: ./src/kMeans.cpp ./src/matplotlibcpp.h
	g++ ./src/kMeans.cpp -fopenmp -o ./build/kMeans.out $(CFLAGS) $(LIBS)