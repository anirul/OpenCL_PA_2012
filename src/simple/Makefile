# a simple makefile for a simple example

ifeq ($(OSTYPE), darwin)
CC = clang
CXX = clang++
CFLAGS = -O3 -I/opt/local/include -I/usr/local/include -I..
LIBS = -L/opt/local/lib/ \
	-framework GLUT -framework Cocoa -framework OpenCL -framework OpenGL \
	-lboost_program_options -lboost_system
else
CC = gcc
CXX = g++
CFLAGS = -O3 -I/usr/include -I/usr/local/include -I..
LIBS = -L/usr/lib64 -L/usr/local/lib -lglut -lGL -lGLU -lOpenCL -lboost_program_options -lboost_system
endif
ALL=simple

all: $(ALL)

simple.o : simple.cpp
	$(CXX) -o simple.o -c simple.cpp $(CFLAGS)

simple : simple.o
	$(CXX) -o simple simple.o $(LIBS)

clean:
	rm -f $(ALL) *.o

