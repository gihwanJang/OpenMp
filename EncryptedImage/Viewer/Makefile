CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

Viewr.out: main.cpp ../Src/common.h ../Src/ImageViewer.h ../Src/ImageLoader.h
	g++ $(CFLAGS) -o Viewr.out main.cpp ../Src/ImageLoader.cpp ../Src/ImageViewer.cpp $(LIBS)