#pragma once
#include "TString.h"
#if __cplusplus >= 201103L
#include <unordered_set>

/// http://stackoverflow.com/questions/17885563/using-c11-unordered-set-in-visual-c-and-clang

bool blacklisted(TString checkme) ;
#endif /// __cplusplus >= 201103L
