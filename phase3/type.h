#ifndef TYPE_H
#define TYPE_H

#include <vector>
#include <iostream>
#include <cassert>



struct Parameters{
    bool variadic;
    std::vector<class Type> types;
};

class Type{
    enum {ARRAY, ERROR, FUNCTION, SCALAR};
    short _declarator, _specifier;
    unsigned _indirection;
    unsigned _length;
    Parameters *_parameters;


    public:
    Type();                                                                  //error
    Type(int specifier, unsigned indirection = 0);                           //scalar
    Type(int specifier, unsigned indirection, unsigned length);              //array
    Type(int specifier, unsigned indirection, Parameters *parameters);       //function
    
    bool operator == (const Type &rhs) const;
    bool operator != (const Type &rhs) const;

    int specifier() const { return _specifier;}
    int indirection() const { return _indirection;}
    int declarator() const { return _declarator;}
    int length() const { return _length;}
    Parameters* parameters() const { assert(_declarator == FUNCTION); return _parameters;}

    

    bool isArray() const {if(_declarator == ARRAY) return true; return false;}
    bool isFunction() const {if(_declarator == FUNCTION) return true; return false;}
    bool isScalar() const {if(_declarator == SCALAR) return true; return false;}
    bool isError() const {if(_declarator == ERROR) return true; return false;}
};

std::ostream&operator <<(std::ostream &ost, const Type &type);

#endif /* TYPE.H */
