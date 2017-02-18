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

We encountered a root 5.34 / gcc 4.6 setup where this tool did not build (requires c++11).
There's now a separate branch (no-c++11) with compatibility on a minimum effort basis.

# Dependencies

 * CERN ROOT
 * zsh tab completion requires root_numpy
