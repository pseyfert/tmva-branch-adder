# tmva-branch-adder

A quick wrapper to add a TMVA classifier response to a TTree.
Ideally without boilerplate code - after all, the xml file knows about the input variables.

Not heavily debugged, but feel free to try it.

## quick usage
```
make
./main rootfile-with-tree.root name-of-tree path-to.xml output-rootfile.root
```
or specify the output branch name
```
./main rootfile-with-tree.root name-of-tree path-to.xml output-rootfile.root response-branchname
```

## installation

The make target `install` should install the tmva-branch-adder, as an
executable binary and a shared library with C++ header in the standard
directories (read the Makefile and specify a `PREFIX`).

## examples

If a command line program is not what you need, please check the C++ program in
the examples directory for a rough sketch how to include the tmva-branch-adder
in a C++ program.

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

 * provide/test pyroot access

# legacy ROOT/GCC

We encountered a root 5.34 / gcc 4.6 setup where this tool did not build (requires c++11).
There's now a separate branch (no-c++11) with compatibility on a minimum effort basis.

# Dependencies

 * [CERN ROOT](https://root.cern.ch/) and its dependencies
 * zsh tab completion requires [root_numpy](https://pypi.python.org/pypi/root_numpy)
