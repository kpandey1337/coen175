#ifndef CHECKER_H
#define CHECKER_H


#include <vector>
#include <cassert>
#include <string>
#include <unordered_set>

#include "lexer.h"
#include "scope.h"
#include "symbol.h"

typedef std::string string;
typedef std::unordered_set<string> SymbolSet;

Scope* openScope();
Scope* closeScope();
Symbol* defineFunction(const string &name, const Type &type);
Symbol* declareFunction(const string &name, const Type &type);
Symbol* declareVariable(const string &name, const Type &type);
Symbol* checkID(const string &name);

#endif
