# simple makefile for fft

ifeq ($(OSTYPE), darwin)
CC = clang
CXX = clang++
CFLAGS = -g -O0 -I/opt/local/include -I/usr/local/include -I.. -DWITH_FFTW
LIBS = -L/opt/local/lib/ \
	-framework GLUT -framework Cocoa -framework OpenCL -framework OpenGL \
	-lboost_program_options -lboost_system \
	-lfftw3 -lfftw3f -lm
else
CC = gcc
CXX = g++
CFLAGS = -g -O0 -I/usr/include -I/usr/local/include -I.. -DWITH_FFTW
LIBS = -L/usr/lib64 -L/usr/local/lib \
	-lglut -lGL -lGLU -lOpenCL \
	-lboost_program_options -lboost_system \
	/usr/lib64/libfftw3f.so.3 -lm
endif
ALL=fft

all: $(ALL)

cl_util.o : cl_util.cpp cl_util.h
	$(CXX) -o cl_util.o -c cl_util.cpp $(CFLAGS)
cl_fft.o : cl_fft.cpp cl_fft.h
	$(CXX) -o cl_fft.o -c cl_fft.cpp $(CFLAGS)
main.o : main.cpp cl_fft.h cl_util.h fftw_fft.h txt_file.h
	$(CXX) -o main.o -c main.cpp $(CFLAGS)

fft : main.o cl_util.o cl_fft.o 
	$(CXX) -o fft main.o cl_util.o cl_fft.o $(LIBS)	

clean:
	rm -f $(ALL) *.o

