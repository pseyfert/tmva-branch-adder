# tmva-branch-adder

A quick wrapper to add a TMVA classifier response to a TTree.
Ideally without boilerplate code - after all, the xml file knows about the input variables.

Not heavily debugged, but feel free to try it.

```
make
./main rootfile-with-tree.root name-of-tree path-to.xml
```

## Acknowledgements

Thanks for input from
ttekampe
@ibab
burney
@pcanal
fkruse
