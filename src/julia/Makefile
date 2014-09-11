# a simple makefile for julia

UNAME = $(shell uname -s)

ifeq ($(UNAME), Darwin)
CC = clang
CXX = clang++
CFLAGS = -O3 -I/opt/local/include -I/usr/local/include -I..
LIBS = -L/opt/local/lib/ \
	-framework GLUT -framework Cocoa -framework OpenCL -framework OpenGL \
	-lboost_program_options-mt -lboost_system-mt
else
CC = gcc
CXX = g++
CFLAGS = -O3 -I/usr/include -I/usr/local/include -I..
LIBS = -L/usr/lib64 -L/usr/local/lib -lglut -lGL -lGLU -lOpenCL -lboost_program_options -lboost_system
endif
ALL=julia

all: $(ALL)

cl_util.o : cl_util.cpp cl_util.h
	$(CXX) -o cl_util.o -c cl_util.cpp $(CFLAGS)
cl_julia.o : cl_julia.cpp cl_julia.h
	$(CXX) -o cl_julia.o -c cl_julia.cpp $(CFLAGS)
glut_win.o : glut_win.cpp glut_win.h
	$(CXX) -o glut_win.o -c glut_win.cpp $(CFLAGS)
win_julia.o : win_julia.cpp win_julia.h
	$(CXX) -o win_julia.o -c win_julia.cpp $(CFLAGS)
main.o : main.cpp cl_julia.h win_julia.h
	$(CXX) -o main.o -c main.cpp $(CFLAGS)

julia : cl_julia.o main.o cl_util.o glut_win.o win_julia.o
	$(CXX) -o julia cl_julia.o cl_util.o main.o glut_win.o win_julia.o $(LIBS)

clean:
	rm -f $(ALL) *.o

