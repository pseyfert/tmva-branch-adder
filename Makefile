all: cplusplus py

cplusplus:
	$(MAKE) -C src

py: cplusplus
	$(MAKE) -C module

clean:
	$(MAKE) -C src clean
	$(MAKE) -C module clean

PREFIX ?= /usr

install: cplusplus
	install -d -m 0755 $(PREFIX)/include
	install -d -m 0755 $(PREFIX)/bin
	install -d -m 0755 $(PREFIX)/lib
	install -m 0755 src/main $(PREFIX)/bin/tmva-branch-adder
	install -m 0664 src/reader_wrapper.h $(PREFIX)/include
	install -m 0644 src/blacklist.h $(PREFIX)/include
	install -m 0644 src/libBranchAdder.so $(PREFIX)/lib

.PHONY: clean cplusplus py install
