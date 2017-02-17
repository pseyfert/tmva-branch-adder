#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unordered_set>
#include <regex>
#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "reader_wrapper.h"

int reader_wrapper::getTree(TString infile, TString treename, TString outfile) {
  m_infile = TFile::Open(infile,"read");
  if (nullptr == m_infile || m_infile->IsZombie() || m_infile->GetNkeys() <= 0) {
    std::cerr << "File " << infile << " could not be opened properly." << std::endl;
    return 1;
  }

  m_intree = dynamic_cast<TTree*>(m_infile->Get(treename.Data()));
  if (nullptr == m_intree) {
    std::cerr << "Tree " << treename << " could not be opened properly." << std::endl;
    return 2;
  }
  m_outfile = TFile::Open(outfile,"create");
  if (nullptr == m_outfile || m_outfile->IsZombie()) {
    std::cerr << "File " << outfile << " could not be opened properly." << std::endl;
    return 3;
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
  if (m_responseBranch->Fill() > 0) return 0;
  else return 3;
}

int reader_wrapper::initFormulas(TString targetbranch) {
  /// don't care about spectators here
  // TODO: does this tree get created in the outfile?
  m_intree->SetBranchStatus("*",1);
  TDirectory* cwd = gDirectory;
  if (nullptr!=m_outfile) {
    m_outfile->cd();
  }
  m_outtree = m_intree->CloneTree(-1,"fast");
  m_outtree->SetDirectory(m_outfile);
  cwd->cd();
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
  // check if output branch exists already
  if (nullptr == m_outtree->GetBranch(targetbranch.Data())) {
    m_responseBranch = m_outtree->Branch(targetbranch.Data(),&m_response,(targetbranch + "/F").Data());
    return 0;
  }
  std::cout << "Output branch exists already. Aborting." << std::endl;
  return 4;
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

