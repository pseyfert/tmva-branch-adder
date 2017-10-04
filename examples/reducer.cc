#include "TEntryList.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "reader_wrapper.h"
#include <TTreeFormula.h>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <regex>

int main()
{
  std::vector<reader_wrapper> mvas;
  std::set<TBranch*> m_branches;
  /// FOR USER: add branches for reading which you're interested in

  TChain* chain = new TChain( "TreeS" );
  chain->Add( "files/tmva_class_example.root" );
  TTree* inTree = (TTree*)chain; // to facilitate type casting

  TFile* outFile = TFile::Open( "outputfile.root", "recreate");
  if (nullptr == outFile) {
    std::cout << "could not open outputfile" << std::endl;
    return 1;
  }

  std::vector<TString> mva_files = {"BDT",     "FDA_GA", "KNN",     "LD", "LikelihoodPCA", "Likelihood", "MLPBNN",
                                    "PDEFoam", "PDERS",  "RuleFit", "SVM"};

  for ( auto s : mva_files ) {
    reader_wrapper temp;
    temp.SetTree( (TTree*)chain );
    temp.SetXMLFile( Form( "dataset/weights/TMVAClassification_%s.weights.xml", s.Data() ) );
    temp.SetTargetBranch( s.Data() );
    mvas.push_back( temp );
  }

  TTree* outTree = new TTree("outTree","output tree");

  int mva_errorcode = 0;
  for ( auto& mva : mvas ) {
    mva_errorcode |= mva.getVariables();
    mva_errorcode |= mva.bookReader();
    mva_errorcode |= mva.SetOutTree( outTree );


    mva_errorcode |= mva.initFormulas( true );

    /// True is whether the formulas should be evaluated on the input tree,
    /// instead of the output tree.
    ///
    /// Evaluating on the output makes sense when a full tree is cloned with
    /// all entries, / and then one branch is added, or if a branch just gets
    /// added to an existing tee.
    ///
    /// Evaluating on the input makes sense if all branches of the output get
    /// filled in one big fill, / or when not even all input variables get
    /// written to the output tree.

    for ( auto branch : mva.getBranches() ) {
      /// different classifiers could use different input variables
      /// the reader_wrapper *in principle* takes care of enabling them
      /// (though you might be able to outsmart that)
      /// but you might still want to know about them
      m_branches.insert( branch );
    }
    if ( mva_errorcode ) {
      std::cerr << "MVA complains" << std::endl;
      return mva_errorcode;
    }
  }

  /// reading optimisation: disable all branches
  inTree->SetBranchStatus( "*", 0 );
  /// safety measure if nullptr somehow ended up here to avoid segfaults later
  m_branches.erase( (TBranch*)nullptr );
  for ( auto b : m_branches ) b->SetStatus( 1 );
  for ( auto& mva : mvas ) {
    /// activate all branches for reading that the MVAs need
    mva_errorcode |= mva.activateBranches();
    if ( mva_errorcode ) {
      std::cerr << "MVA complains" << std::endl;
      return mva_errorcode;
    }
  }

  // -- finally, loop through the tree and copy the entries
  std::cout << " Looping over events and copy the entries" << std::endl;

  /// https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=21165&sid=003e047c399ecb5cb8a303266c0ede18
  Long64_t entries = inTree->GetEntries();
  TEntryList *entryList = new TEntryList("mysel","mysel",inTree);
  /// FOR USER: add a selection here
  inTree->Draw(">>mysel","var1-var2*var3>var4","entrylist");
  inTree->SetEntryList(entryList);

  /// just like the "usual" makeclass example, loop over all entries
  for ( Long64_t i = 0;; ++i ) {
    Long64_t entryNumber = inTree->GetEntryNumber( i );
    if ( entryNumber < 0 ) {
      std::cout << entryNumber << std::endl;
      break;
    }
    Long64_t localEntry = inTree->LoadTree( entryNumber );
    if ( localEntry < 0 ) break;

    for ( auto b : m_branches ) {
      /// only get input data for all the branches you're interested in and
      /// don't check the branch statuses over and over again
      b->GetEntry( localEntry );
    }

    for ( auto& mva : mvas ) {
      mva.Evaluate();
    }

    outTree->Fill();
  }

  outFile->WriteTObject( outTree );
  outFile->Close();

  std::cout << "DONE" << std::endl;

  delete outFile;
  return EXIT_SUCCESS;
}
