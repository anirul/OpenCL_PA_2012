# once more a crappy makefile home made!

UNAME = $(shell uname)

ifeq ($(UNAME), Darwin)
CC = clang
CXX = clang++
CFLAGS = -g -O0 -I/opt/local/include -I/usr/local/include -I..
LIBS = -L/opt/local/lib/ \
	-framework GLUT -framework Cocoa -framework OpenCL -framework OpenGL \
	-lboost_program_options-mt -lboost_system-mt
endif

ifeq ($(UNAME), Linux)
CC = gcc
CXX = g++
CFLAGS = -g -O0 -I/usr/include -I/usr/local/include -I..
LIBS = -L/usr/lib64 -L/usr/local/lib -lglut -lGL -lGLU -lOpenCL -lboost_program_options -lboost_system
endif

ALL=cellular_automata

all: $(ALL)

cl_util.o : cl_util.cpp cl_util.h
	$(CXX) -o cl_util.o -c cl_util.cpp $(CFLAGS)
glut_win.o : glut_win.cpp glut_win.h
	$(CXX) -o glut_win.o -c glut_win.cpp $(CFLAGS)
cl_cellular_automata.o : cl_cellular_automata.cpp cl_cellular_automata.h
	$(CXX) -o cl_cellular_automata.o -c cl_cellular_automata.cpp $(CFLAGS)
win_cellular_automata.o : win_cellular_automata.cpp win_cellular_automata.h
	$(CXX) -o win_cellular_automata.o -c win_cellular_automata.cpp $(CFLAGS)
main.o : main.cpp win_cellular_automata.h cl_cellular_automata.h
	$(CXX) -o main.o -c main.cpp $(CFLAGS)

cellular_automata : main.o cl_util.o glut_win.o cl_cellular_automata.o win_cellular_automata.o
	$(CXX) -o cellular_automata main.o cl_util.o glut_win.o cl_cellular_automata.o win_cellular_automata.o $(LIBS)	

clean:
	rm -f $(ALL) *.o

