import sys
try:
    import tmva_branch_adder
except ImportError:
    sys.exit(1)
else:
    sys.exit(0)
