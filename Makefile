all: cplusplus py

cplusplus:
	$(MAKE) -C src

py: cplusplus
	$(MAKE) -C module

clean:
	$(MAKE) -C src clean
	$(MAKE) -C module clean

.PHONY: clean cplusplus py
