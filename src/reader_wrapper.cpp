#include <string.h>
#include <stddef.h>
#include <iostream>
#include <string>
// at least g++ 4.9 or clang++
#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= 40900 || __clang__
#define branchadder_use_regex 1
#include <regex>
#else // older g++
#warning "Building test if branch name is a valid c++ variable name"
#endif // compiler version
#include "TTreeFormula.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/VariableInfo.h"
#include "reader_wrapper.h"


// IWYU pragma: no_include <ext/alloc_traits.h>
// IWYU pragma: no_include "TObject.h"
// IWYU pragma: no_include "TXMLEngine.h"


#if __cplusplus >= 201103L // at least C++11
#define branchadder_use_blocklist 1
#include <unordered_set>
#include "forbidden_branchnames.h"
#else // no C++11
#warning "Building without additional branch name validation"
#endif // C++11


/**
 * @brief boiler plate for most bare use case
 *
 * Obtain input filename and tree name, and output filename, all in strings. It
 * handles the opening of files and retrives the TTree from there.
 *
 * @param infile   filename of the input file. Any protocol that can be handled by TFile::Open should do.
 * @param treename name of the TTree object in the input file to be evaluated (can obtain slashes, anything that TFile::Get can handle should do).
 * @param outfile  filename of the output file.
 *
 * @return 0 in case of success. larger integers otherwise.
 */
int reader_wrapper::getTree(TString infile, TString treename, TString outfile)
{
   TDirectory *cwd = gDirectory;
   m_infile = TFile::Open(infile, "read");
   if (nullptr == m_infile || m_infile->IsZombie() || m_infile->GetNkeys() <= 0) {
      std::cerr << "File " << infile << " could not be opened properly." << std::endl;
      return 1;
   }

   TTree *intree = dynamic_cast<TTree *>(m_infile->Get(treename.Data()));
   if (nullptr == intree) {
      std::cerr << "Tree " << treename << " could not be opened properly." << std::endl;
      return 2;
   }
   TDirectory *dir = intree->GetDirectory();
   std::vector<TString> dirnamestack;
   std::vector<TString> dirtitlestack;
   while (0 != strcmp(dir->ClassName(), "TFile")) {
      dirnamestack.push_back(dir->GetName());
      dirtitlestack.push_back(dir->GetTitle());
      dir = dir->GetMotherDir();
   }
   TDirectoryFile *outdir = TFile::Open(outfile, "create");
   if (nullptr == outdir || outdir->IsZombie()) {
      std::cerr << "File " << outfile << " could not be opened properly." << std::endl;
      return 3;
   }
   outdir->cd();
   while (m_dirstack && !dirnamestack.empty()) {
      outdir = new TDirectoryFile(dirnamestack.back().Data(), dirtitlestack.back().Data());
      dirnamestack.pop_back();
      dirtitlestack.pop_back();
      outdir->Write();
      outdir->cd();
   }
   SetTree(intree);
   SetTargetFile(outdir);
   cwd->cd();
   return 0;
}

/**
 * @brief Evaluate the MVA.
 *
 * Evaluates all formulas of input variables and the MVA. Output is written to
 * the member m_response (which should be the branch address of an output
 * branch from reader_wrapper::initFormulas.
 *
 * @return 0 in case of success.
 */
int reader_wrapper::Evaluate()
{
#if __cplusplus >= 201103
   for (auto &v : m_variables) {
      v.value = v.ttreeformula->EvalInstance();
   }
#else
   for (size_t v = 0; v < m_variables.size(); ++v) {
      m_variables[v].value = m_variables[v].ttreeformula->EvalInstance();
   }
#endif
   if (m_regression) {
      m_response = m_reader->EvaluateRegression(m_methodName.Data())[0];
   } else {
      m_response = m_reader->EvaluateMVA(m_methodName.Data());
   }
   return 0;
}

/**
 * @brief Do all that's needed to be done on a per-event basis
 *
 * Calls TBranch::GetEntry for all branches that are needed for input variable
 * formulas, evaluates all formulas and evaluates the MVA.
 *
 * @param e entry number of the TTree
 *
 * @return 0 in case of success
 */
int reader_wrapper::GetEntry(Long64_t e)
{
/// don't care about spectators here
#if __cplusplus >= 201103
   for (auto b : m_branches) {
      b->GetEntry(e);
   }
#else
   for (size_t b = 0; b < m_branches.size(); ++b) {
      m_branches[b]->GetEntry(e);
   }
#endif
   return Evaluate();
}

int reader_wrapper::createTree()
{
   /// don't care about spectators here
   // TODO: does this tree get created in the outfile?
   m_intree->SetBranchStatus("*", 1);
   TDirectory *cwd = gDirectory;
   if (nullptr != m_outfile) {
      m_outfile->cd();
   }
   m_outtree = m_intree->CloneTree(-1, "fast");
   m_outtree->SetDirectory(m_outfile);
   cwd->cd();
   return 0;
}

/**
 * @brief Activate all branches of the input TTree which are needed for MVA evaluation
 *
 * @return 0 in case of success.
 */
int reader_wrapper::activateBranches()
{
#if __cplusplus >= 201103
   for (auto b : m_branches) {
      b->SetStatus(1);
   }
#else
   for (size_t b = 0; b < m_branches.size(); ++b) {
      m_branches[b]->SetStatus(1);
   }
#endif
   m_responseBranch->SetStatus(1);
   return 0;
}

/**
 * @brief Instantiate TTreeFormulas and create target branch
 *
 * To be called once the output tree exists and after the XML file has been
 * parsed. From the internal input variable objects, TTreeFormulas get
 * configured to be evaluated on the tree with the input variables. A branch
 * for the response on the output tree is created and the branch set to use the
 * internal response storage as memory location.
 *
 * @param targetbranch name of the branch for the MVA response
 * @param eval_on_in   if the formulas should be evaluated on the input tree
 *
 * @return 0 in case of success. Higher numbers in case of error.
 */
int reader_wrapper::initFormulas(TString targetbranch, bool eval_on_in)
{
   int buffer(0);
#if __cplusplus >= 201103
   for (auto &var : m_variables) {
      var.ttreeformula =
         new TTreeFormula(Form("local_var_%d", buffer++), var.formula, (eval_on_in ? m_intree : m_outtree));
      for (size_t v = 0; v < var.ttreeformula->GetNcodes(); ++v) {
         m_branches.insert(var.ttreeformula->GetLeaf(v)->GetBranch());
      }
   }
#else
   for (size_t var = 0; var < m_variables.size(); var++) {
      m_variables[var].ttreeformula = new TTreeFormula(Form("local_var_%d", buffer++), m_variables[var].formula,
                                                       (eval_on_in ? m_intree : m_outtree));
      for (size_t v = 0; v < m_variables[var].ttreeformula->GetNcodes(); ++v) {
         m_branches.push_back(m_variables[var].ttreeformula->GetLeaf(v)->GetBranch());
      }
   }
#endif
   if (targetbranch == TString("")) {
      targetbranch = m_methodName;
   }
   // check if output branch exists already
   if (nullptr == m_outtree->GetBranch(targetbranch.Data())) {
      m_responseBranch = m_outtree->Branch(targetbranch.Data(), &m_response, (targetbranch + "/F").Data());
      return 0;
   }
   std::cout << "Output branch exists already. Aborting." << std::endl;
   return 4;
}

/**
 * @brief Parse the XML file. Can be called before input or output tree are assigned.
 *
 * @param xml_file_name filename of the XML file
 *
 * @return 0 in case of success, higher integer otherwise.
 */
int reader_wrapper::getVariables(TString xml_file_name)
{
   /// mostly copied from TMVA MethodBase
   // TODO error handling
   void *doc = TMVA::gTools().xmlengine().ParseFile(
      xml_file_name, TMVA::gTools().xmlenginebuffersize()); // the default buffer size in TXMLEngine::ParseFile is 100k.
                                                            // Starting with ROOT 5.29 one can set the buffer size, see:
                                                            // http://savannah.cern.ch/bugs/?78864. This might be
                                                            // necessary for large XML files
   void *rootnode = TMVA::gTools().xmlengine().DocGetRootElement(doc); // node "MethodSetup"
   TString fullMethodName;
   TMVA::gTools().ReadAttr(rootnode, "Method", fullMethodName);
   // http://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
   std::string fullMethodName_string(fullMethodName.Data());
   std::string delimiter = "::";

   size_t pos = 0;
   std::string token;
   while ((pos = fullMethodName_string.find(delimiter)) != std::string::npos) {
      fullMethodName_string.substr(0, pos);
      fullMethodName_string.erase(0, pos + delimiter.length());
   }
   m_methodName = TString(fullMethodName_string.c_str());

   void *mynode = TMVA::gTools().GetChild(rootnode);
   while (mynode != 0) {
      if (TString(TMVA::gTools().GetName(mynode)) == "GeneralInfo") {
         void *ch = TMVA::gTools().GetChild(mynode);
         TString optName, optValue;
         while (ch) {
            TMVA::gTools().ReadAttr(ch, "name", optName);
            if (optName == "AnalysisType") {
               TMVA::gTools().ReadAttr(ch, "value", optValue);
               if (optValue == "Regression")
                  m_regression = true;
               else
                  m_regression = false;
            }
            ch = TMVA::gTools().GetNextChild(ch);
         }
      }
      if (TString(TMVA::gTools().GetName(mynode)) == "Spectators") {
         UInt_t readNSpec;
         TMVA::gTools().ReadAttr(mynode, "NSpec", readNSpec);
         TMVA::VariableInfo readSpecInfo;
         void *ch = TMVA::gTools().GetChild(mynode);
         while (ch) {
            readSpecInfo.ReadFromXML(ch);
            m_spectators.push_back(VariableWrapper(TString(readSpecInfo.GetExpression())));
            ch = TMVA::gTools().GetNextChild(ch);
         }
         if (m_spectators.size() != readNSpec) {
            m_spectators.clear();
            // TODO error message
            return 2;
         }
      }
      if (TString(TMVA::gTools().GetName(mynode)) == "Variables") {
         UInt_t readNVar;
         TMVA::gTools().ReadAttr(mynode, "NVar", readNVar);
         TMVA::VariableInfo readVarInfo;
         void *ch = TMVA::gTools().GetChild(mynode);
         while (ch) {
            readVarInfo.ReadFromXML(ch);
            m_variables.push_back(VariableWrapper(TString(readVarInfo.GetExpression())));
            ch = TMVA::gTools().GetNextChild(ch);
         }
         if (m_variables.size() != readNVar) {
            m_variables.clear();
            // TODO error message
            return 2;
         }
      }
      mynode = TMVA::gTools().GetNextChild(mynode);
   }
   // TODO error message
   return m_variables.empty() ? 1 : 0;
}

/**
 * @brief Instantiate the TMVA reader. Must be called after getVariables. Can be called before initFormulas.
 *
 * @param xml_file_name filename of the XML file.
 *
 * @return 0 in case of success.
 */
int reader_wrapper::bookReader(TString xml_file_name)
{
   m_reader = new TMVA::Reader("!Color:Silent");
#if __cplusplus >= 201103
   for (auto var : m_spectators) {
      m_reader->AddSpectator(var.formula, &var.value);
   }
   for (auto &var : m_variables) {
      m_reader->AddVariable(var.formula, &var.value);
   }
#else
   for (size_t var = 0; var < m_spectators.size(); ++var) {
      m_reader->AddSpectator(m_spectators[var].formula, &(m_spectators[var].value));
   }
   for (size_t var = 0; var < m_variables.size(); var++) {
      m_reader->AddVariable(m_variables[var].formula, &(m_variables[var].value));
   }
#endif
   m_reader->BookMVA(m_methodName, xml_file_name);
   return 0;
}

int reader_wrapper::SetTargetFile(TDirectoryFile *file)
{
   m_outfile = file;
   return 0;
}

/**
 * @brief Specify the target branch name, which will be created in initFormulas. Checks the validity of the branch name.
 *
 * If possible, it is checked if the branch name would create trouble in
 * TTree::Draw or TTree::MakeClass when using the output file.
 *
 * @param name response branch name
 *
 * @return 0 in case of success
 */
int reader_wrapper::SetTargetBranch(TString name)
{
/// TODO check if name is valid as in none of +-/*()#[]<><space><leading digit>  --- more?
// http://stackoverflow.com/questions/12993187/regular-expression-to-recognize-variable-declarations-in-c
// https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=21407&p=93337&sid=5f32a5ca9aa01003e4dec96a2f92a2e0#p93337
#ifdef branchadder_use_regex
   if (std::regex_match(name.Data(), std::regex("([a-zA-Z_][a-zA-Z0-9_]*)")))
#else
   if (true)
#endif
   {
#ifdef branchadder_use_blocklist
      if (isForbiddenBranchName(name))
#else
      if (false)
#endif
      {
         return 9;
      }
      m_targetbranchname = name;
      return 0;
   }
   return 8;
}
int reader_wrapper::SetXMLFile(TString filename)
{
   m_xmlfilename = filename;
   return 0;
   /// TODO: check if file can be parsed
}
int reader_wrapper::SetTree(TObject *tree)
{
   /// work around my inabilities to use pyroot
   return SetTree((TTree *)tree);
}
int reader_wrapper::SetOutTree(TTree *tree)
{
   /// TODO: will it be checked for nullptr???
   m_outtree = tree;
   return 0;
}
int reader_wrapper::SetTree(TTree *tree)
{
   /// if it's a nullptr will be checked later
   m_intree = tree;
   return 0;
}
void reader_wrapper::Close()
{
   if (nullptr != m_outfile) {
      m_outfile->GetFile()->Close();
   }
}
int reader_wrapper::Process()
{
   int errorcode = 0;
   errorcode |= check_all_initialised();
   if (errorcode) return errorcode;
   errorcode |= getVariables(m_xmlfilename);
   if (errorcode) return errorcode;
   errorcode |= bookReader(m_xmlfilename);
   if (errorcode) return errorcode;
   errorcode |= createTree();
   if (errorcode) return errorcode;
   errorcode |= initFormulas(m_targetbranchname, false);
   if (errorcode) return errorcode;
   m_outtree->SetBranchStatus("*", 0);
   errorcode |= activateBranches();
   if (errorcode) return errorcode;
   Long64_t entries = m_outtree->GetEntries();
   for (Long64_t e = 0; e < entries; ++e) {
      errorcode |= GetEntry(e);
      if (m_responseBranch->Fill() > 0)
         errorcode |= 0;
      else
         errorcode |= 3;
      if (errorcode) return errorcode;
   }
   m_outtree->SetBranchStatus("*", 1);
   if (nullptr != m_outfile) {
      /// maybe the tree is supposed to be kept in RAM and not written to disk?
      m_outfile->WriteTObject(m_outtree);
      /// TODO check return value
   }
   return errorcode;
}

#if __cplusplus >= 201103L
std::unordered_set<TBranch *> reader_wrapper::getBranches()
{
   return m_branches;
}
#else
std::vector<TBranch *> reader_wrapper::getBranches()
{
   return m_branches;
}
#endif

int reader_wrapper::check_all_initialised()
{
   int errorcode = 0;
   if (nullptr == m_intree) {
      std::cerr << "no TTree to process provided" << std::endl;
      errorcode |= 1 << 0;
   }
   if (m_xmlfilename == TString("")) {
      std::cerr << "no XML file with classifier provided" << std::endl;
      errorcode |= 1 << 1;
   }
   return errorcode;
}

int reader_wrapper::getVariables()
{
   return getVariables(m_xmlfilename);
}
int reader_wrapper::bookReader()
{
   return bookReader(m_xmlfilename);
}
int reader_wrapper::initFormulas(bool eval_on_in)
{
   return initFormulas(m_targetbranchname, eval_on_in);
}

reader_wrapper::reader_wrapper()
   : m_xmlfilename(""), m_targetbranchname(""), m_methodName(""), m_variables(), m_spectators(),
#if __cplusplus >= 201103L
     m_formulas(0, nullptr),
#endif
     m_intree(nullptr), m_outtree(nullptr), m_reader(nullptr), m_branches(), m_response(0.f), m_responseBranch(nullptr),
     m_infile(nullptr), m_outfile(nullptr), m_dirstack(false), m_regression(false)
{
}

reader_wrapper::~reader_wrapper()
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
