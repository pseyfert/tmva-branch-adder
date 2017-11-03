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

Thanks for input from (cern, slack, mattermost, github, stackoverflow,
robust-irc, root-forum usernames):
 * ttekampe
 * @ibab
 * burney
 * @pcanal
 * fkruse
 * sur5r
 * @petitcactusorange
 * fredi
 * lisovsky
 * mvesteri
 * rquaglia

# TODO

 * provide/test pyroot access

# legacy ROOT/GCC

Since 0.4 (and on master at the time of writing), tmva-branch-adder should
build w/ and w/o c++11, as well as with root5 and root6, and with c++11 also
when using gcc 4.8 (not fully c++11 compatible).

# Dependencies

 * [CERN ROOT](https://root.cern.ch/) and its dependencies
 * zsh tab completion requires [root_numpy](https://pypi.python.org/pypi/root_numpy)

# Contributing

I would be happy if someone can review/test the python interface, suggest an
interface that fits users better, maybe even add a numpy interface (well, that
might as well be an independent project on its own). For those, feel invited to
submit pull requests or issues, such that even if you only have a suggestion
and I cannot implement it, other visitors can pick up your suggestion to make
the world a little bit better.

# License

The project is licensed under GPL. If this is an issue for you (you want to
include it / link against it / reuse parts of it in your project and use a
LGPL, MIT, Apache, BSD, whathaveyou license), you can get in contact with me to
discuss double-licensing the tmva-branch-adder (e.g. for an inclusion in the
"upstream project" TMVA).
