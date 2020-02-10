/* type.cpp member function implementation */

#include <cassert>
#include "tokens.h"
#include "type.h"


using namespace std;


Type::Type(int specifier, unsigned indirection, unsigned length){ //array
    _specifier = specifier;
    _indirection = indirection;
    _length = length;
    _declarator = ARRAY;

}

Type::Type(int specifier, unsigned indirection){ //scalar
    _specifier = specifier;
    _indirection = indirection;
    _declarator = SCALAR;
    _length = 0;

}

Type::Type(int specifier, unsigned indirection, Parameters *parameters){ //function
    _specifier = specifier;
    _indirection = indirection;
    _parameters = parameters;
    _declarator = FUNCTION;
     
}

Type::Type(){      //error
    _declarator = ERROR;
}

bool Type::operator == (const Type &rhs) const{

    if(_declarator != rhs._declarator)
        return false;
    if(_declarator == ERROR) //error
        return true;
    if(_specifier != rhs._specifier) //checks int, double, char
        return false;
    if(_indirection != rhs._indirection) //checks pointer
        return false;
    if(_declarator == SCALAR) //scalar
        return true;
    if(_declarator == ARRAY) //array
        return (_length == rhs._length);
    if(_parameters->variadic != rhs._parameters->variadic) //function
        return false;
    return (_parameters->types == rhs._parameters->types);
}


bool Type::operator != (const Type &rhs) const{
    return !(operator ==(rhs));  
    //return !(this == rhs);   //??
}


//H E L P

std::ostream& operator << (std::ostream &ostr, const Type &t){
    if (t.isArray()){
            ostr << "ARRAY ";
        }
    else if (t.isFunction()){
            ostr << "FUNCTION ";
        }
    else{
            ostr << "SCALAR ";
        }
        
    ostr << "specifier: " << t.specifier();
    ostr << " indirection: " << t.indirection();

    if (t.isArray()){
        ostr << " length: " << t.length();
    }
    return ostr;
} 
