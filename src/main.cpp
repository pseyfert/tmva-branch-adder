#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "reader_wrapper.h"

void usage(char** argv) {
  std::cout << "USAGE: " << argv[0] << " [options] <root file> <tree name> <xml file> <output file> [<target branch>]" << std::endl;
  std::cout << "       " << argv[0] << " -h: print usage message" << std::endl;
  std::cout << "  OPTIONS: -d    keep directory structure." << std::endl;
  return;
}


int main(int argc, char** argv) {
  std::vector<TString> args;
  reader_wrapper wrapper;
  for (size_t i = 1 ; i < argc ; ++i) {
    if (0==strcmp("-h",argv[i])) {
      usage(argv);
      return 0;
    } else if (0==strcmp("-d",argv[i])) {
      wrapper.m_dirstack = true;
    } else {
      args.push_back(argv[i]);
    }
  }
  if (4!=args.size()&&5!=args.size()) {
    usage(argv);
    return 1;
  }

  TString xmlfile(args[2]);
  TString infile(args[0]);
  TString outfile(args[3]);
  TString treename(args[1]);

  int errorcode = 0;
  errorcode |= wrapper.getTree(infile,treename,outfile);
  if (errorcode) return errorcode;
  if (5==args.size()) errorcode |= wrapper.SetTargetBranch(args[4]);
  if (errorcode) return errorcode;
  errorcode |= wrapper.SetXMLFile(xmlfile);
  if (errorcode) return errorcode;

  errorcode |= wrapper.Process();
  if (errorcode) return errorcode;
  wrapper.Close();
  return errorcode;
}
