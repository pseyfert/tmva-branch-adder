all: cplusplus py

cplusplus:
	$(MAKE) -C src

py: cplusplus
	$(MAKE) -C module

clean:
	$(MAKE) -C src clean

.PHONY: clean cplusplus py
