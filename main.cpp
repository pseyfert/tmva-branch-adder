#include <stdio.h>
#include <string.h>
#include <iostream>

#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TTreeFormula.h"

class VariableWrapper {
  public:
    VariableWrapper(TString form) : formula(form), value(0.f), ttreeformula(nullptr) {} ;
    TString formula;
    Float_t value;
    TTreeFormula* ttreeformula;
};

class reader_wrapper {
  public:
    TString                      m_methodName;
    std::vector<VariableWrapper> m_variables;
    std::vector<TTreeFormula*>   m_formulas;
    TTree*                       m_intree;
    TMVA::Reader*                m_reader;
    int                          getVariables(TString);
    int                          bookReader(TString) ;
    int                          initFormulas();
    int                          getTree(TString,TString);
};

int reader_wrapper::getTree(TString rootfile, TString treename) {
  TFile* infile = TFile::Open(rootfile,"read");
  if (nullptr == infile || infile->IsZombie() || infile->GetNkeys() <= 0) {
    std::cerr << "File " << rootfile << " could not be opened properly." << std::endl;
    return 1;
  }

  m_intree = dynamic_cast<TTree*>(infile->Get(treename.Data()));
  if (nullptr == m_intree) {
    std::cerr << "Tree " << treename << " could not be opened properly." << std::endl;
    return 2;
  }
}

// TODO book branches
// TODO set branch statusses
// TODO the actual loop
// TODO and writing

int reader_wrapper::initFormulas() {
  int buffer(0);
  for (auto var : m_variables) {
    var.ttreeformula = new TTreeFormula(Form("local_var_%d",buffer++),var.formula,m_intree);
  }
  // TODO error handling
  return 0;
}

int reader_wrapper::getVariables(TString xml_file_name) {
  /// mostly copied from TMVA MethodBase
  // TODO error handling
  void* doc = TMVA::gTools().xmlengine().ParseFile(xml_file_name,TMVA::gTools().xmlenginebuffersize()); // the default buffer size in TXMLEngine::ParseFile is 100k. Starting with ROOT 5.29 one can set the buffer size, see: http://savannah.cern.ch/bugs/?78864. This might be necessary for large XML files
  void* rootnode = TMVA::gTools().xmlengine().DocGetRootElement(doc); // node "MethodSetup"
  TString fullMethodName;
  TMVA::gTools().ReadAttr( rootnode, "Method", fullMethodName );
  //http://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
  std::string fullMethodName_string(fullMethodName.Data());
  std::string delimiter = "::";

  size_t pos = 0;
  std::string token;
  while ((pos = fullMethodName_string.find(delimiter)) != std::string::npos) {
    fullMethodName_string.substr(0, pos);
    fullMethodName_string.erase(0, pos + delimiter.length());
  }
  m_methodName = TString(fullMethodName_string.c_str());


  void* mynode = TMVA::gTools().GetChild(rootnode);
  while (mynode!=0) {
    if (TString( TMVA::gTools().GetName(mynode)) == "Variables") {
      UInt_t readNVar;
      TMVA::gTools().ReadAttr( mynode , "NVar", readNVar);
      TMVA::VariableInfo readVarInfo;
      int varIdx = 0;
      void* ch = TMVA::gTools().GetChild(mynode);
      while (ch) {
        readVarInfo.ReadFromXML(ch);
        m_variables.push_back(VariableWrapper(TString(readVarInfo.GetExpression())));
        ch = TMVA::gTools().GetNextChild(ch);
      }
      if (m_variables.size()!=readNVar) {
        m_variables.clear();
        // TODO error message
        return 2;
      }
      return 0;
    }
    mynode = TMVA::gTools().GetNextChild(mynode);
  }
  // TODO error message
  return 1;
}

int reader_wrapper::bookReader( TString xml_file_name) {
  m_reader = new TMVA::Reader("!Color:Silent");
  for (auto var : m_variables) {
    m_reader->AddVariable(var.formula, &var.value);
  }
  m_reader->BookMVA( m_methodName, xml_file_name );
  return 0;
}

int main(int argc, char** argv) {
  if ((5!=argc&&4!=argc) || 0==strcmp("-h",argv[1])) {
    std::cout << "USAGE: ./" << argv[0] << " <root file> <tree name> <xml file> [<target branch>]" << std::endl;
    return 1;
  }
  TString xmlfile(argv[3]);
  TString rootfile(argv[1]);
  TString treename(argv[2]);
  reader_wrapper wrapper;
  int errorcode = wrapper.getVariables(xmlfile);
  wrapper.bookReader(xmlfile);
  wrapper.getTree(rootfile,treename);
  wrapper.initFormulas();

}
