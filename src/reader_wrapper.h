#pragma once
#include <stdio.h>
#include <string.h>

#if __cplusplus >= 201103L // at least C++11
#define branchadder_use_blacklist 1
#include <unordered_set>
#include "blacklist.h"
#else // no C++11
#warning "Building without additional branch name validation"
#endif // C++11

// at least g++ 4.9 or clang++
#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= 40900 || __clang__
#define branchadder_use_regex 1
#include <regex>
#else // older g++
#warning "Building test if branch name is a valid c++ variable name"
#endif // compiler version

#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TMVA/Reader.h"
#include "TTreeFormula.h"

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
   int SetTargetFile(TDirectoryFile *file);
   int SetTargetBranch(TString name);
   int SetXMLFile(TString filename);
   int SetTree(TObject *tree);
   int SetOutTree(TTree *tree);
   int SetTree(TTree *tree);
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
public:
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
   reader_wrapper()
      : m_xmlfilename(""), m_targetbranchname(""), m_methodName(""), m_variables(), m_spectators(),
#if __cplusplus >= 201103L
        m_formulas(0, nullptr),
#endif
        m_intree(nullptr), m_outtree(nullptr), m_reader(nullptr), m_branches(), m_response(0.f),
        m_responseBranch(nullptr), m_infile(nullptr), m_outfile(nullptr), m_dirstack(false), m_regression(false)
   {
   }
   virtual ~reader_wrapper()
   {
      if (m_reader) delete m_reader;
#if __cplusplus >= 201103L
      for (auto &var : m_variables) {
         if (var.ttreeformula) delete var.ttreeformula;
      }
#else
      for (size_t var = 0; var < m_variables.size(); var++) {
         if (m_variables[var].ttreeformula) delete m_variables[var].ttreeformula;
      }
#endif
   }
};
