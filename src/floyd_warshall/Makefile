# a simple makefile for Floyd-Warshall

ifeq ($(OSTYPE), darwin)
CC = clang
CXX = clang++
CFLAGS = -g -O0 -I/opt/local/include -I/usr/local/include -I..
LIBS = -L/opt/local/lib/ \
	-framework GLUT -framework Cocoa -framework OpenCL -framework OpenGL \
	-lboost_program_options -lboost_system
else
CC = gcc
CXX = g++
CFLAGS = -g -O0 -I/usr/include -I/usr/local/include -I..
LIBS = -L/usr/lib64 -L/usr/local/lib \
	-lglut -lGL -lGLU -lOpenCL \
	-lboost_program_options -lboost_system
endif
ALL=floyd_warshall

all: $(ALL)

cl_util.o : cl_util.cpp cl_util.h
	$(CXX) -o cl_util.o -c cl_util.cpp $(CFLAGS)
cl_floyd_warshall.o : cl_floyd_warshall.cpp cl_floyd_warshall.h
	$(CXX) -o cl_floyd_warshall.o -c cl_floyd_warshall.cpp $(CFLAGS)
ewd_file.o : ewd_file.cpp ewd_file.h
	$(CXX) -o ewd_file.o -c ewd_file.cpp $(FLAGS)
main.o : main.cpp cl_floyd_warshall.h ewd_file.h cl_util.h
	$(CXX) -o main.o -c main.cpp $(CFLAGS)

floyd_warshall : main.o cl_util.o cl_floyd_warshall.o ewd_file.o
	$(CXX) -o floyd_warshall main.o cl_util.o ewd_file.o cl_floyd_warshall.o $(LIBS)	

clean:
	rm -f $(ALL) *.o

