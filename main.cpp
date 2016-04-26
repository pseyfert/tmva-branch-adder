#include <stdio.h>
#include <string.h>
#include <iostream>
#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "reader_wrapper.h"

int main(int argc, char** argv) {
  if ((5!=argc&&6!=argc) || 0==strcmp("-h",argv[1])) {
    std::cout << "USAGE: " << argv[0] << " <root file> <tree name> <xml file> <output file> [<target branch>]" << std::endl;
    return 1;
  }
  TString xmlfile(argv[3]);
  TString infile(argv[1]);
  TString outfile(argv[4]);
  TString treename(argv[2]);

  int errorcode = 0;
  reader_wrapper wrapper;
  TFile* of = TFile::Open(outfile,"create");
  errorcode |= wrapper.SetTargetFile(of);
  if (errorcode) return errorcode;
  if (6==argc) errorcode |= wrapper.SetTargetBranch(argv[5]);
  if (errorcode) return errorcode;
  errorcode |= wrapper.SetXMLFile(xmlfile);
  if (errorcode) return errorcode;

  TFile* if_ = TFile::Open(infile,"read");
  TTree* intree = (TTree*)if_->Get(treename);
  errorcode |= wrapper.SetTree(intree);
  if (errorcode) return errorcode;


  errorcode |= wrapper.Process();
  if (errorcode) return errorcode;
  of->Close();
  return errorcode;
}
