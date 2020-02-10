#include "checker.h"

#define E1 "redefinition of '%s'"
#define E2 "redeclaration of '%s'"
#define E3 "conflicting types for '%s'"
#define E4 "’%s’ undeclared"


SymbolSet symbolHash;

static Scope* globalScope = nullptr;
static Scope* currentScope = nullptr;


Scope* openScope(){
    //std::cout << "opening scope ! ! !" << std::endl; //delete
    currentScope = new Scope(currentScope);
    if(globalScope == nullptr){
        globalScope = currentScope;
        assert(globalScope->enclosing() == nullptr);
    }
    return currentScope;
}
Scope* closeScope(){
    //std::cout << "closing scope ! ! !" << std::endl; //delete
    Scope* oldScope = currentScope;
    currentScope = currentScope->enclosing();
    return oldScope;
    
}
Symbol* defineFunction(const string &name, const Type &type){
    Symbol *sym;
    
    sym = globalScope->find(name);
    if(sym == nullptr)
        sym = declareFunction(name, type);
    if(symbolHash.count(name) >0){
        report(E1, name);
    }
    else
        symbolHash.insert(name);
    return sym;
}


Symbol* declareFunction(const string &name, const Type &type){
    
    Symbol* symbol = globalScope->find(name);
    if(symbol == nullptr){
        symbol = new Symbol(type, name);
        globalScope->insert(symbol);
    }
    else if(symbol->type() != type){ //function previously declared. must be same type.
        report(E3, symbol->name());
        //currentScope->remove(symbol->name());
        //currentScope->insert(new Symbol(type, name));
    }
    return symbol;
    
}



Symbol* declareVariable(const string &name, const Type &type){
    Symbol *sym = currentScope->find(name);
    if(sym == nullptr){
        sym = new Symbol(type, name);
        currentScope->insert(sym);
        //std::cout << "created variable of type: " << type << std::endl;
    }
    else if(currentScope != globalScope)
        report(E2, name);
    else if(type != sym->type()){
        report(E3,name);
    }
    return sym;
    
}
Symbol* checkID(const string &name){
    Symbol* sym = currentScope->lookup(name);
    if(sym == nullptr){
        Type error;
        sym = new Symbol(error, name);
        currentScope->insert(sym);
    
        report(E4, name);
    }
    return sym;
    
}
