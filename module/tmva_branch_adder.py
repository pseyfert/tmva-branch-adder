from ROOT import gInterpreter
gInterpreter.LoadMacro("../src/blacklist.cpp+")
gInterpreter.LoadMacro("../src/reader_wrapper.cpp+")
from ROOT import reader_wrapper
