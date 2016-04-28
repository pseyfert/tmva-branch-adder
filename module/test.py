import sys
try:
    from tmva_branch_adder import *
    aaa = reader_wrapper()
except ImportError:
    sys.exit(1)
except NameError:
    sys.exit(2)
else:
    sys.exit(0)
