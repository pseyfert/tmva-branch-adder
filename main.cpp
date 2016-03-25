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
    std::vector<VariableWrapper> m_spectators;
    std::vector<TTreeFormula*>   m_formulas;
    TTree*                       m_intree;
    TTree*                       m_outtree;
    TMVA::Reader*                m_reader;
    std::set<TBranch*>           m_branches;
    Float_t                      m_response;
    TBranch*                     m_responseBranch;
    TFile*                       m_infile;
    int                          getVariables(TString);
    int                          bookReader(TString) ;
    int                          initFormulas();
    int                          getTree(TString,TString);
    int                          GetEntry(Long64_t);
};

int reader_wrapper::getTree(TString rootfile, TString treename) {
  m_infile = TFile::Open(rootfile,"update");
  if (nullptr == m_infile || m_infile->IsZombie() || m_infile->GetNkeys() <= 0) {
    std::cerr << "File " << rootfile << " could not be opened properly." << std::endl;
    return 1;
  }

  m_intree = dynamic_cast<TTree*>(m_infile->Get(treename.Data()));
  if (nullptr == m_intree) {
    std::cerr << "Tree " << treename << " could not be opened properly." << std::endl;
    return 2;
  }
  return 0;
}

int reader_wrapper::GetEntry(Long64_t e) {
  /// don't care about spectators here
  for (auto b: m_branches) {
    b->GetEntry(e);
  }
  for (auto& v : m_variables) {
    v.value = v.ttreeformula->EvalInstance();
  }
  m_response = m_reader->EvaluateMVA(m_methodName.Data());
  m_responseBranch->Fill();
  return 0;
}

int reader_wrapper::initFormulas() {
  /// don't care about spectators here
  m_outtree = m_intree->CloneTree(-1,"fast");
  int buffer(0);
  for (auto& var : m_variables) {
    var.ttreeformula = new TTreeFormula(Form("local_var_%d",buffer++),var.formula,m_outtree);
    for (size_t v = 0 ; v < var.ttreeformula->GetNcodes() ; ++v) {
      m_branches.insert(var.ttreeformula->GetLeaf(v)->GetBranch());
    }
  }
  m_outtree->SetBranchStatus("*",0);
  for (auto b : m_branches) {
    b->SetStatus(1);
  }
  m_responseBranch = m_outtree->Branch(m_methodName.Data(),&m_response,(m_methodName + "/F").Data());
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
    if (TString( TMVA::gTools().GetName(mynode)) == "Spectators") {
      UInt_t readNSpec;
      TMVA::gTools().ReadAttr( mynode , "NSpec", readNSpec);
      TMVA::VariableInfo readSpecInfo;
      int specIdx = 0;
      void* ch = TMVA::gTools().GetChild(mynode);
      while (ch) {
        readSpecInfo.ReadFromXML(ch);
        m_spectators.push_back(VariableWrapper(TString(readSpecInfo.GetExpression())));
        ch = TMVA::gTools().GetNextChild(ch);
      }
      if (m_spectators.size()!=readNSpec) {
        m_spectators.clear();
        // TODO error message
        return 2;
      }
    }
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
    }
    mynode = TMVA::gTools().GetNextChild(mynode);
  }
  // TODO error message
  return m_variables.empty()?1:0;
}

int reader_wrapper::bookReader( TString xml_file_name) {
  m_reader = new TMVA::Reader("!Color:Silent");
  for (auto var : m_spectators) {
    m_reader->AddSpectator(var.formula, &var.value);
  }
  for (auto& var : m_variables) {
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
  errorcode |= wrapper.bookReader(xmlfile);
  errorcode |= wrapper.getTree(rootfile,treename);
  errorcode |= wrapper.initFormulas();
  for (Long64_t e = 0 ; e < wrapper.m_outtree->GetEntries() ; ++e) {
    errorcode |= wrapper.GetEntry(e);
  }
  wrapper.m_infile->WriteTObject(wrapper.m_outtree);

  return errorcode;

}
