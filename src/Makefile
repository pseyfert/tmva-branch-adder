TMVALOC  ?= $(shell root-config --libdir)/..
TMVALIBS += -L$(TMVALOC)/lib -lTMVA
LDLIBS   += $(TMVALIBS)

CPPFLAGS += $(shell root-config --cflags) -O2 -g -Wextra -Wall -Wshadow -fPIC # -Wpedantic (not available in gcc 4.6)
LDFLAGS  += $(shell root-config --ldflags)
LDLIBS   += $(shell root-config --libs) -lMinuit -lFoam -lMathMore -lTreePlayer -lXMLIO -lstdc++
CPP       = $(shell root-config --cxx)
CXX       = $(shell root-config --cxx)
CC        = $(shell root-config --cxx)

all: main libBranchAdder.so

main: main.o reader_wrapper.o forbidden_branchnames.o

reader_wrapper.o: forbidden_branchnames.h reader_wrapper.h reader_wrapper.cpp

forbidden_branchnames.o: forbidden_branchnames.h forbidden_branchnames.cpp

main.o: reader_wrapper.h forbidden_branchnames.h main.cpp

libBranchAdder.so: SOFLAGS+=-shared -lc
libBranchAdder.so: reader_wrapper.o forbidden_branchnames.o
	$(CXX) $(SOFLAGS) -o $@ $^

clean:
	rm -f *.o main *.so

clingclean:
	rm -f *_cpp.so *_cpp.d *_cpp_ACLiC_dict_rdict.pcm

.PHONY: clean
