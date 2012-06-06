# simple makefile for a test program

ifeq ($(OSTYPE), darwin)
CC=clang
CXX=clang++
CFLAGS = -g -O0 -I/opt/local/include -I/opt/local/include/opencv2 -I..
LIBS= -L/opt/local/lib/ \
	-framework OpenCL \
	-lboost_program_options -lboost_system
else
CC=gcc
CXX=g++
CFLAGS = -g -O0 -I/opt/local/include -I..
LIBS= -L/usr/local/lib \
	-lOpenCL \
	-lboost_program_options -lboost_system
endif
ALL=test 

all: $(ALL)

test.o : test.cpp
	$(CXX) -o test.o -c test.cpp $(CFLAGS)

test : test.o
	$(CXX) -o test test.o $(LIBS)

clean:
	rm -f $(ALL) *.o

