#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"
#include <vector>
#include <string>




class Symbol{

    typedef std::string string;
    Type _type;
    string _name;


    public:
    //constructor
    Symbol(Type type, string name);

    //accessor
    Type type(){ return _type;}
    string name(){ return _name;}
};

#endif /* SYMBOL.H */
