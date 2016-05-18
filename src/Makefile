TMVALIBS += -L$(TMVALOC)/lib -lTMVA
LDFLAGS  += $(TMVALIBS)

CPPFLAGS += $(shell root-config --cflags) -march=native -O2 -g -Wextra -Wall -Wshadow -std=c++0x
LDFLAGS  += $(shell root-config --libs) -lMinuit -lFoam -lMathMore -lTreePlayer -lXMLIO -lstdc++ -m64 -g -march=native
CPP       = $(shell root-config --cxx)
CXX       = $(shell root-config --cxx)
CC        = $(shell root-config --cxx)

all: main

main: main.o reader_wrapper.o

reader_wrapper.o: reader_wrapper.h reader_wrapper.cpp

main.o: reader_wrapper.h main.cpp

clean:
	rm -f *.o

.PHONY: clean
