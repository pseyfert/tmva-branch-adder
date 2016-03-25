#include <stdio.h>
#include <string.h>
#include <iostream>

#include "TString.h"
#include "TMVA/Tools.h"


std::vector<TString> getVariables(TString xml_file_name) {
  /// mostly copied from TMVA MethodBase
  // TODO error handling
  void* doc = TMVA::gTools().xmlengine().ParseFile(xml_file_name,TMVA::gTools().xmlenginebuffersize()); // the default buffer size in TXMLEngine::ParseFile is 100k. Starting with ROOT 5.29 one can set the buffer size, see: http://savannah.cern.ch/bugs/?78864. This might be necessary for large XML files
  void* rootnode = TMVA::gTools().xmlengine().DocGetRootElement(doc); // node "MethodSetup"

  std::vector<TString> retval;
  void* mynode = TMVA::gTools().GetChild(rootnode);
  while (mynode!=0) {
    if (TString( TMVA::gTools().GetName(mynode) == "Variables")) {
      UInt_t readNVar;
      TMVA::gTools().ReadAttr( mynode , "NVar", readNVar);
      TMVA::VariableInfo readVarInfo;
      int varIdx = 0;
      void* ch = TMVA::gTools().GetChild(mynode);
      while (ch) {
        readVarInfo.ReadFromXML(ch);
        retval.push_back(TString(readVarInfo.GetExpression()));
        ch = TMVA::gTools().GetNextChild(ch);
      }
      if (retval.size()!=readNVar) {
        retval.clear();
        // TODO error message
      }
      return retval;
    }
    mynode = TMVA::gTools().GetNextChild(mynode);
  }
  // TODO error message
  return retval;
}



int main(int argc, char** argv) {
  if (1==argc || 0==strcmp("-h",argv[1])) {
    std::cout << "USAGE: ./" << argv[0] << " <root file> <tree name> <xml file> [<target branch>]" << std::endl;
    return 1;
  }
}
