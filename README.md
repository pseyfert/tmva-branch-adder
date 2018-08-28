# tmva-branch-adder
[![DOI](https://zenodo.org/badge/54739571.svg)](https://zenodo.org/badge/latestdoi/54739571)
[![Licence: GPL v3](https://img.shields.io/github/license/pseyfert/tmva-branch-adder.svg)](LICENSE)
[![gitlab CI status](https://gitlab.cern.ch/pseyfert/tmva-branch-adder/badges/master/pipeline.svg)](https://gitlab.cern.ch/pseyfert/tmva-branch-adder/commits/master)
[![travis Status](https://travis-ci.org/pseyfert/tmva-branch-adder.svg?branch=master)](https://travis-ci.org/pseyfert/tmva-branch-adder)

A quick wrapper to add a TMVA classifier response to a TTree.
Ideally without boilerplate code - after all, the xml file knows about the input variables.

Not heavily debugged, but feel free to try it.

# quick usage
```
cd src
make
./main rootfile-with-tree.root name-of-tree path-to.xml output-rootfile.root
```
or specify the output branch name
```
./main rootfile-with-tree.root name-of-tree path-to.xml output-rootfile.root response-branchname
```

# advanced usages

Be sure you have ROOT set up. Usually by sourcing the right `thisroot.sh` file.

## installation (Makefile)

The make target `install` should install the tmva-branch-adder, as an
executable binary and a shared library with C++ header in the standard
directories (read the Makefile if you want to know more, the `install` target
should be human readable). You can also specify the `PREFIX` variable:

```shell
PREFIX=$(HOME)/.local make install
```

## installation (CMake)

I'm putting more effort on the `Makefile` approach (as then I also don't
require you to have CMake). But as you might want to include the
tmva-branch-adder in your CMake project, it might have advantages for you to
use CMake. (Or you might have other reasons to favour CMake as well.)

```shell
mkdir -p build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=${HOME}/.local -DCMAKE_RULE_MESSAGES:BOOL=OFF -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
cmake --build . --clean-first --target install
```

I usually recommend out-of-source build, but in source build should work as
well. Most of the CMake settings I provide above are just my personal taste,
feel free to omit or adjust.

### IWYU

The CMake build will try to run include-what-you-use *if it is available*. This
can be switched off by invalidating the `iwyu_path` CMake variable. The most
beginner friendly way to do that is probably `ccmake`:

```shell
mkdir -p build
cd build
cmake <path_to_source> <options as above>
```
then
 - go to the line with `iwyu_path`
 - press `[enter]` to edit the line
 - `[space][backspace]` to actually edit the line
 - make it empty `[enter]` to save the change
 - `[c]`, `[g]` to apply the configuration

Reversely, running CMake with `-Diwyu_path=<path_to_iwyu_binary>` should
activate include-what-you-use even if it is not found in the `$PATH`.

## examples

If a command line program is not what you need, please check the C++ program in
the `examples` directory for a rough sketch how to include the tmva-branch-adder
in a C++ program.

## Dependencies

 * [CERN ROOT](https://root.cern.ch/) and its dependencies
 * zsh tab completion requires [root-ls-tree](https://github.com/pseyfert/go-list-root-trees)

# Organisatorial matters

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
 * ndawe
 * 0xb1ns

## TODO / Wishlist

 * provide/test pyroot access
 * clang-format (+test)
 * IWYU test

## legacy ROOT/GCC

Since 0.4 (and on master at the time of writing), tmva-branch-adder should
build w/ and w/o c++11, as well as with root5 and root6, and with c++11 also
when using gcc 4.8 (not fully c++11 compatible).

## Contributing

I would be happy if someone can review/test the python interface, suggest an
interface that fits users better, maybe even add a numpy interface (well, that
might as well be an independent project on its own). For those, feel invited to
submit pull requests or issues, such that even if you only have a suggestion
and I cannot implement it, other visitors can pick up your suggestion to make
the world a little bit better.

## License

The project is licensed under GPL. If this is an issue for you (you want to
include it / link against it / reuse parts of it in your project and use a
LGPL, MIT, Apache, BSD, whathaveyou license), you can get in contact with me to
discuss double-licensing the tmva-branch-adder (e.g. for an inclusion in the
"upstream project" TMVA).
