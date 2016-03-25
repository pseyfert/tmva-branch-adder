CPPFLAGS+=$(shell root-config --cflags) -march=native -O2 -g -Wextra -Wall -Wshadow
LDFLAGS+=$(shell root-config --libs) -lMinuit -lFoam -lMathMore -lTMVA -lTreePlayer -lXMLIO -lstdc++ -m64 -g -march=native
CPP=c++
CXX=c++
CC=c++

all: main

main: main.o

clean:
	rm -f *.o

.PHONY: clean
