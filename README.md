# tmva-branch-adder

A quick wrapper to add a TMVA classifier response to a TTree.
Ideally without boilerplate code - after all, the xml file knows about the input variables.

Not heavily debugged, but feel free to try it.

```
make
./main rootfile-with-tree.root name-of-tree path-to.xml output-rootfile.root
```
or specify the output branch name
```
./main rootfile-with-tree.root name-of-tree path-to.xml output-rootfile.root response-branchname
```

## Acknowledgements

Thanks for input from
 * ttekampe
 * @ibab
 * burney
 * @pcanal
 * fkruse
 * sur5r
 * @petitcactusorange

# TODO

 * provide pyroot access
 * provide c++ / cling access

# legacy ROOT/GCC

Since 0.4 (and on master at the time of writing), tmva-branch-adder should
build w/ and w/o c++11, as well as with root5 and root6, and with c++11 also
when using gcc 4.8 (not fully c++11 compatible).

# Dependencies

 * CERN ROOT
 * zsh tab completion requires root_numpy
