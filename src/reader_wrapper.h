#pragma once
#include <vector>
#include "TString.h"
#if __cplusplus >= 201103L // at least C++11
#include <unordered_set>
#endif // C++11


// Rtypes got split in root 6
#include "RVersion.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "Rtypes.h"
#else
#include "RtypesCore.h"
#endif

class TBranch;
class TDirectoryFile;
class TFile;
class TObject;
class TTree;
class TTreeFormula;
namespace TMVA { class Reader; }

class VariableWrapper {
public:
   VariableWrapper(TString form) : formula(form), value(0.f), ttreeformula(nullptr){};
   VariableWrapper() : formula(""), value(-999.f), ttreeformula(nullptr){};
   TString formula;
   Float_t value;
   TTreeFormula *ttreeformula;
};

class reader_wrapper {
public:
   //// new interface
   void CopyDirectoryStack() { m_dirstack = true; }
   int SetTargetFile(TDirectoryFile *file);
   int SetTargetBranch(TString name);
   int SetXMLFile(TString filename);
   int SetTree(TObject *tree);
   int SetOutTree(TTree *tree);
   int SetTree(TTree *tree);
   TTree *GetOutTree() { return m_outtree;}
   void Close();
   int Process();
#if __cplusplus >= 201103L
   std::unordered_set<TBranch *> getBranches();
#else
   std::vector<TBranch *> getBranches();
#endif

protected:
   TString m_xmlfilename;
   TString m_targetbranchname;
   int check_all_initialised();
   //// old interface
   TString m_methodName;
   std::vector<VariableWrapper> m_variables;
   std::vector<VariableWrapper> m_spectators;
   std::vector<TTreeFormula *> m_formulas;
   TTree *m_intree;
   TTree *m_outtree;
   TMVA::Reader *m_reader;
#if __cplusplus >= 201103L
   std::unordered_set<TBranch *> m_branches;
#else
   std::vector<TBranch *> m_branches;
#endif
   Float_t m_response;
   TBranch *m_responseBranch;
   TFile *m_infile;
   TDirectoryFile *m_outfile;
   bool m_dirstack;
   bool m_regression;

public:
   int getVariables(TString);
   int getVariables();
   int bookReader(TString);
   int bookReader();
   int activateBranches();
   int createTree();
   int initFormulas(TString, bool);
   int initFormulas(bool eval_on_in);
   int getTree(TString, TString, TString);
   int GetEntry(Long64_t);
   int Evaluate();
   reader_wrapper();
   virtual ~reader_wrapper();
};
