from ROOT import TString
from ROOT import TFile
from ROOT import TTree
from tmva_branch_adder import reader_wrapper

if __name__ == "__main__":
    from sys import argv
    import sys
    argc = len(argv)
    if ("-h" in argv) or (argc not in [5, 6]):
        print("USAGE: ", argv[0], " <root file> <tree name> <xml file> <output file> [<target branch>]")
        sys.exit(1)
    xmlfile  = TString(argv[3])
    infile   = TString(argv[1])
    outfile  = TString(argv[4])
    treename = TString(argv[2])

    errorcode = int(0)
    wrapper = reader_wrapper()
    of = TFile.Open(outfile.Data(), "create")
    errorcode |= wrapper.SetTargetFile(of)
    if errorcode:
        sys.exit(errorcode)
    if 6 == argc:
        errorcode |= wrapper.SetTargetBranch(argv[5])
    if errorcode:
        sys.exit(errorcode)
    errorcode |= wrapper.SetXMLFile(xmlfile)
    if errorcode:
        sys.exit(errorcode)

    if_ = TFile.Open(infile.Data(), "read")
    intree = if_.Get(treename.Data())
    errorcode |= wrapper.SetTree(intree)
    if errorcode:
        sys.exit(errorcode)

    errorcode |= wrapper.Process()
    if errorcode:
        sys.exit(errorcode)
    of.Close()
    sys.exit(errorcode)
