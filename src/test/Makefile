# simple makefile for a test program

UNAME = $(shell uname)

ifeq ($(UNAME), Darwin)
CC=clang
CXX=clang++
CFLAGS = -g -O0 -I/opt/local/include -I/opt/local/include/opencv2 -I..
LIBS= -L/opt/local/lib/ \
	-framework OpenCL \
	-lboost_program_options-mt -lboost_system-mt
endif

ifeq ($(UNAME), Linux)
CC=gcc
CXX=g++
CFLAGS = -g -O0 -I/opt/local/include -I..
LIBS= -L/usr/local/lib \
	-lOpenCL \
	-lboost_program_options -lboost_system
endif
ALL=list_device

all: $(ALL)

test.o : test.cpp
	$(CXX) -o test.o -c test.cpp $(CFLAGS)

list_device : test.o
	$(CXX) -o list_device test.o $(LIBS)

clean:
	rm -f $(ALL) *.o

