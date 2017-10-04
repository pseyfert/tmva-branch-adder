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

test/dataset/weights/TMVAClassification_MLPBNN.weights.xml:
	mkdir -p test
	(cd test; root -l -b -q -n ${ROOTSYS}/tutorials/tmva/TMVAClassification.C\(\"MLPBNN\"\) )

test/files/tmva_class_example.root:
	mkdir -p test
	(cd test; root -l -b -q -n ${ROOTSYS}/tutorials/tmva/TMVAClassification.C\(\"MLPBNN\"\) )

test: cplusplus test/files/tmva_class_example.root test/dataset/weights/TMVAClassification_MLPBNN.weights.xml
	rm -f test/TESTOUTPUT.root
	src/main test/files/tmva_class_example.root TreeS test/dataset/weights/TMVAClassification_MLPBNN.weights.xml test/TESTOUTPUT.root MLPBNN_from_branchadder

.PHONY: clean cplusplus py install test
