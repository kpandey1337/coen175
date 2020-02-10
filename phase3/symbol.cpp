#include <cassert>
#include "symbol.h"
#include <string>

Symbol::Symbol(Type type, string name){
    _type = type;
    _name = name;
}