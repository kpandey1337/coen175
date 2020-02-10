#include <cassert>
#include "scope.h"
#include <string>

Scope::Scope(Scope *enclosing)
{
    _enclosing = enclosing;
    //_symbols = new Symbols;
}
        
        //insert symbol into scope
void Scope::insert(Symbol* sym) 
{
    assert(find(sym->name()) == nullptr);
    _symbols.push_back(sym);
}



//find symbol in current scope
Symbol* Scope::find(const string &name) const 
{
    
    for(auto curSymbol: _symbols){
        if(name == (curSymbol->name())){
            return curSymbol;
            }
        }
        //none found
    Symbol* nonefound = nullptr;
    return nonefound;
    }

//look in current scope for symbol,
//if not inside, go to next scope 
//until first instance is found

//THIS MAY BE INCORRECT. CH E C K
Symbol* Scope::lookup(const string &name) const
{
    Symbol* result;
    Scope* lookupScope;
    lookupScope = (Scope*) this;
    while(lookupScope != nullptr){
        result = lookupScope->find(name);
        if(result != nullptr)
            return result;
        lookupScope = lookupScope->_enclosing;
    }
    return nullptr;
    
   /*
    
    if(symbol == nullptr){
        if(_enclosing != nullptr){
            return _enclosing->lookup(name);
        }
    }
    if(symbol->name() == name){
        return symbol;
    }
    //none found
    Symbol *temppointer = nullptr;
    return temppointer; */
}
