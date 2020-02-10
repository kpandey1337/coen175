#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include <vector>
#include <string>


typedef std::vector<Symbol*> Symbols;

class Scope{
    typedef std::string string;
    Scope* _enclosing;
    Symbols _symbols;

    public:
        //constructor
        Scope(Scope *enclosing = nullptr); 
        
        //insert symbol into scope
        void insert(Symbol* sym); 

        //find symbol in current scope
        Symbol* find(const string &name) const; 

        //look in current scope for symbol,
        //if not inside, go to next scope 
        //until first instance is found
        Symbol* lookup(const string &name) const;
        
        //accessor
        Scope* enclosing() const { return _enclosing;}
        
        //accessor
        const Symbols &symbols () { return _symbols;}

};
#endif