CPPFLAGS+=$(shell root-config --cflags) -march=native -O2 -g -Wextra -Wall -Wshadow -Wpedantic -std=c++11
LDFLAGS+=$(shell root-config --libs) -lMinuit -lFoam -lMathMore -lTMVA -lTreePlayer -lXMLIO -lstdc++ -m64 -g -march=native
CPP=c++
CXX=c++
CC=c++

all: main

main: main.o reader_wrapper.o blacklist.o

reader_wrapper.o: blacklist.h reader_wrapper.h reader_wrapper.cpp

blacklist.o: blacklist.h blacklist.cpp

main.o: reader_wrapper.h blacklist.h main.cpp

clean:
	rm -f *.o main

clingclean:
	rm -f *_cpp.so *_cpp.d *_cpp_ACLiC_dict_rdict.pcm

.PHONY: clean
