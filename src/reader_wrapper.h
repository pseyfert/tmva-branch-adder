#pragma once
#include <stdio.h>
#include <string.h>
#include <unordered_set>
#include <regex>
#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TMVA/Reader.h"
#include "TTreeFormula.h"
#include "blacklist.h"

class VariableWrapper {
  public:
    VariableWrapper(TString form) : formula(form), value(0.f), ttreeformula(nullptr) {} ;
    VariableWrapper()  : formula(""), value(-999.f), ttreeformula(nullptr) {} ;
    TString formula;
    Float_t value;
    TTreeFormula* ttreeformula;
};

class reader_wrapper {
  public:
    //// new interface
    int                          SetTargetFile(TFile* file) {
      m_outfile = file;
      return 0;
    }
    int                          SetTargetBranch(TString name) {
      /// TODO check if name is valid as in none of +-/*()#[]<><space><leading digit>  --- more?
      // http://stackoverflow.com/questions/12993187/regular-expression-to-recognize-variable-declarations-in-c
      // https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=21407&p=93337&sid=5f32a5ca9aa01003e4dec96a2f92a2e0#p93337
      if (std::regex_match(name.Data(),std::regex("([a-zA-Z_][a-zA-Z0-9_]*)"))) {
        if (blacklisted(name)) {
          return 9;
        }
        m_targetbranchname = name;
        return 0;
      }
      return 8;
    }
    int                          SetXMLFile(TString filename) {
      m_xmlfilename = filename;
      return 0;
      /// TODO: check if file can be parsed
    }
    int                          SetTree(TTree* tree) {
      /// if it's a nullptr will be checked later
      m_intree = tree;
      return 0;
    }
    int                          Process() {
      int errorcode = 0;
      errorcode |= check_all_initialised();
      if (errorcode) return errorcode;
      errorcode |= getVariables(m_xmlfilename);
      if (m_targetbranchname == TString("")) {
        m_targetbranchname = m_methodName;
      }
      if (errorcode) return errorcode;
      errorcode |= bookReader(m_xmlfilename);
      if (errorcode) return errorcode;
      errorcode |= initFormulas(m_targetbranchname);
      if (errorcode) return errorcode;
      Long64_t entries = m_outtree->GetEntries();
      for (Long64_t e = 0 ; e < entries ; ++e) {
        errorcode |= GetEntry(e);
        if (errorcode) return errorcode;
      }
      m_outtree->SetBranchStatus("*",1);
      if (nullptr!=m_outfile) {
        /// maybe the tree is supposed to be kept in RAM and not written to disk?
        m_outfile->WriteTObject(m_outtree);
        /// TODO check return value
      }
      return errorcode;
    }
  protected:
    TString                      m_xmlfilename;
    TString                      m_targetbranchname;
    int                          check_all_initialised() {
      int errorcode = 0;
      if (nullptr==m_intree) {
        std::cerr << "no TTree to process provided" << std::endl;
        errorcode |= 1 << 0;
      }
      if (m_xmlfilename == TString("")) {
        std::cerr << "no XML file with classifier provided" << std::endl;
        errorcode |= 1 << 1;
      }
      return errorcode;
    }
    //// old interface
  public:
    TString                      m_methodName;
    std::vector<VariableWrapper> m_variables;
    std::vector<VariableWrapper> m_spectators;
    std::vector<TTreeFormula*>   m_formulas;
    TTree*                       m_intree;
    TTree*                       m_outtree;
    TMVA::Reader*                m_reader;
    std::unordered_set<TBranch*> m_branches;
    Float_t                      m_response;
    TBranch*                     m_responseBranch;
    TFile*                       m_infile;
    TFile*                       m_outfile;
    int                          getVariables(TString);
    int                          bookReader(TString) ;
    int                          initFormulas(TString);
    int                          getTree(TString,TString,TString);
    int                          GetEntry(Long64_t);
    reader_wrapper() :
                       m_xmlfilename(""),
                       m_targetbranchname(""),
                       m_methodName(""),
                       m_variables(),
                       m_spectators(),
                       m_formulas(0,nullptr),
                       m_intree(nullptr),
                       m_outtree(nullptr),
                       m_reader(nullptr),
                       m_branches(),
                       m_response(0.f),
                       m_responseBranch(nullptr),
                       m_infile(nullptr),
                       m_outfile(nullptr) {}
    virtual ~reader_wrapper() {
      if (m_reader) delete m_reader;
      for (auto& var : m_variables) {
        if (var.ttreeformula) delete var.ttreeformula;
      }
    }
};

