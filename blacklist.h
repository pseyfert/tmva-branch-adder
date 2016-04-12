#include "TString.h"
#include <unordered_set>

/// http://stackoverflow.com/questions/17885563/using-c11-unordered-set-in-visual-c-and-clang
//
namespace std
{
  template<>
    struct hash<TString> {
      size_t operator()(const TString &s) const {
        return s.Hash();
      }
    };
}

bool blacklisted(TString checkme) {
  // mathematical function names
  if (checkme.BeginsWith("TMath")) return true;

  static const std::unordered_set<TString> blacklist = {
    "cos" "sin", "tan", "exp", "log", "acos", "asin", "atan", "cosh", "sinh",
    "tanh", "acosh", "asinh", "atanh", "sq", "log10", "abs", "sign", "sqrt",
    "rndm", "min", "max", "atan2", "fmod", "pi", "int", "double", "char",
    "unsigned", "float", "bool", "class", "static", "const", "extern", "auto",
    "signed", "volatile", "void", "short", "long", "complex", "public",
    "protected", "private", "friend", "enum", "constexpr", "template", "new",
    "delete", "Long64_t", "Bool_t", "Float_t", "Short_t", "Double_t", "Int_t",
    "AddBranchToCache", "AddClone", "AddFriend", "AddTotBytes", "AddZipBytes",
    "AutoSave", "Branch", "BranchImp", "BranchImpRef", "BranchOld", "BranchRef",
    "Bronch", "BronchExec", "Browse", "BuildIndex", "BuildStreamerInfo",
    "ChangeFile", "CheckBranchAddressType", "ClassDef", "CloneTree",
    "CopyAddresses", "CopyEntries", "CopyTree", "CreateBasket", "Debug", "Delete",
    "DirectoryAutoAdd", "Draw", "DropBaskets", "DropBranchFromCache",
    "DropBuffers", "ELockStatusBits", "ESetBranchAddressStatus", "fAliases",
    "fAutoFlush", "fAutoSave", "fBranches", "fBranchRef", "fCacheDoAutoInit",
    "fCacheSize", "fCacheUserSet", "fChainOffset", "fClones", "fClusterRangeEnd",
    "fClusterSize", "fDebug", "fDebugMax", "fDebugMin", "fDefaultEntryOffsetLen",
    "fDirectory", "fEntries", "fEntryList", "fEstimate", "fEventList",
    "fFileNumber", "fFlushedBytes", "fFriendLockStatus", "fFriends",
    "fgBranchStyle", "fgMaxTreeSize", "Fill", "fIMTEnabled", "FindBranch",
    "fIndex", "fIndexValues", "FindLeaf", "Fit", "fLeaves", "FlushBaskets",
    "fMakeClass", "fMaxClusterRange", "fMaxEntries", "fMaxEntryLoop",
    "fMaxVirtualSize", "fNClusterRange", "fNEntriesSinceSorting", "fNfill",
    "fNotify", "fPacketSize", "fPerfStats", "fPlayer", "fReadEntry", "fSavedBytes",
    "fScanField", "fSortedBranches", "fTimerInterval", "fTotalBuffers",
    "fTotBytes", "fTransientBuffer", "fTreeIndex", "fUpdate", "fUserInfo",
    "fWeight", "fZipBytes", "GetAlias", "GetAutoFlush", "GetAutoSave", "GetBranch",
    "GetBranchRef", "GetBranchStatus", "GetBranchStyle", "GetCacheAutoSize",
    "GetCacheSize", "GetChainEntryNumber", "GetChainOffset", "GetClusterIterator",
    "GetCurrentFile", "GetDebugMax", "GetDebugMin", "GetDefaultEntryOffsetLen",
    "GetDirectory", "GetEntries", "GetEntriesFast", "GetEntriesFriend", "GetEntry",
    "GetEntryList", "GetEntryNumber", "GetEntryNumberWithBestIndex",
    "GetEntryNumberWithIndex", "GetEntryWithIndex", "GetEstimate", "GetEvent",
    "GetEventList", "GetFileNumber", "GetFriend", "GetFriendAlias", "GetHistogram",
    "GetImplicitMT", "GetIndex", "GetIndexValues", "GetIteratorOnAllLeaves",
    "GetLeaf", "GetLeafImpl", "GetListOfAliases", "GetListOfBranches",
    "GetListOfClones", "GetListOfFriends", "GetListOfLeaves", "GetMakeClass",
    "GetMaxEntryLoop", "GetMaximum", "GetMaxTreeSize", "GetMaxVirtualSize",
    "GetMinimum", "GetNbranches", "GetNewlineValue", "GetNotify", "GetPacketSize",
    "GetPerfStats", "GetPlayer", "GetReadCache", "GetReadEntry", "GetReadEvent",
    "GetScanField", "GetSelect", "GetSelectedRows", "GetTimerInterval",
    "GetTotBytes", "GetTransientBuffer", "GetTree", "GetTreeIndex",
    "GetTreeNumber", "GetUpdate", "GetUserInfo", "GetV1", "GetV2", "GetV3",
    "GetV4", "GetVal", "GetVar", "GetVar1", "GetVar2", "GetVar3", "GetVar4",
    "GetW", "GetWeight", "GetZipBytes", "ImportClusterRanges",
    "IncrementTotalBuffers", "InitializeSortedBranches", "IsFolder", "kCircular",
    "kClassMismatch", "KeepCircular", "kFindBranch", "kFindLeaf", "kForceRead",
    "kGetAlias", "kGetBranch", "kGetEntry", "kGetEntryWithIndex", "kGetFriend",
    "kGetFriendAlias", "kGetLeaf", "kInternalError", "kLoadTree", "kMakeClass",
    "kMatch", "kMatchConversion", "kMatchConversionCollection", "kMaxEntries",
    "kMismatch", "kMissingBranch", "kMissingCompiledCollectionProxy", "kNoCheck",
    "kPrint", "kRemoveFriend", "kSetBranchStatus", "kSplitCollectionOfPointers",
    "kVoidPtr", "LoadBaskets", "LoadTree", "LoadTreeFriend", "MakeClass",
    "MakeCode", "MakeProxy", "MakeSelector", "MemoryFull", "Merge", "MergeTrees",
    "MoveReadCache", "Notify", "OptimizeBaskets", "Principal", "Print",
    "PrintCacheStats", "Process", "Project", "Query", "ReadFile", "ReadStream",
    "RecursiveRemove", "Refresh", "RemoveFriend", "Reset", "ResetAfterMerge",
    "ResetBranchAddress", "ResetBranchAddresses", "Scan", "SetAlias",
    "SetAutoFlush", "SetAutoSave", "SetBasketSize", "SetBranchAddress",
    "SetBranchAddressImp", "SetBranchStatus", "SetBranchStyle",
    "SetCacheEntryRange", "SetCacheLearnEntries", "SetCacheSize",
    "SetCacheSizeAux", "SetChainOffset", "SetCircular", "SetDebug",
    "SetDefaultEntryOffsetLen", "SetDirectory", "SetEntries", "SetEntryList",
    "SetEstimate", "SetEventList", "SetFileNumber", "SetImplicitMT",
    "SetMakeClass", "SetMaxEntryLoop", "SetMaxTreeSize", "SetMaxVirtualSize",
    "SetName", "SetNotify", "SetObject", "SetParallelUnzip", "SetPerfStats",
    "SetScanField", "SetTimerInterval", "SetTreeIndex", "SetUpdate", "SetWeight",
    "Show", "SortBranchesByTime", "StartViewer", "StopCacheLearningPhase",
    "TClusterIterator", "TFriendLock", "TTree", "TTreeBranchImpRef", "tVal",
    "tVar", "UnbinnedFit", "UseCurrentStyle", "Write"};

  if (blacklist.end() == blacklist.find(checkme)) return false;
  return true;
}
