/*
 * File:	parser.c
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include "tokens.h"
# include "lexer.h"
# include "checker.h"



using namespace std;

static void expression();
static void statement();
static int lookahead, nexttoken;
static string lexbuf, nextbuf;


/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", yytext);

    exit(EXIT_FAILURE);
}


/*
 * Function:	peek
 *
 * Description:	Return the next word from the lexer, but do not fetch it.
 */

static int peek()
{
    if (nexttoken == 0) {
	nexttoken = yylex();
	nextbuf = yytext;
    }

    return nexttoken;
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    if (nexttoken != 0) {
	lookahead = nexttoken;
	lexbuf = nextbuf;
	nexttoken = 0;
    } else {
	lookahead = yylex();
	lexbuf = yytext;
    }
}
string identifier(){
	string temptoken = lexbuf;
	match(ID);
	return temptoken;
}

unsigned integer(){
	unsigned temptoken = stoi(lexbuf);
	match(INTEGER);
	return temptoken;
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    return token == CHAR || token == INT || token == DOUBLE;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has only char, int, and
 *		double types.
 *
 *		specifier:
 *		  char
 *		  int
 *		  double
 */

int specifier()
{
    int temptoken;
	if (isSpecifier(lookahead)){
		temptoken = lookahead;
		match(lookahead);
	}
    else
		error();
	return temptoken;
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

unsigned pointers()
{
	int indirection = 0;
    while (lookahead == '*'){
        indirection++;
        match('*');
	}
	return indirection;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ integer ]
 */

static void declarator(int typespec)
{
    unsigned indirection = pointers();
    string name = identifier();

    if (lookahead == '[') {
        match('[');
        int arraysize = integer(); //does the match(INTEGER),returns that matched integer value
        match(']');
        declareVariable(name, Type(typespec, indirection, arraysize));
        return;
    }
    declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
    int typespec = specifier();
    declarator(typespec);

    while (lookahead == ',') {
	match(',');
	declarator(typespec);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *typespec
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  character
 *		  string
 *		  integer
 *		  real
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static void primaryExpression()
{
    string name;
    if (lookahead == '(') {
	match('(');
	expression();
	match(')');

    } else if (lookahead == CHARACTER) {
	match(CHARACTER);

    } else if (lookahead == STRING) {
	match(STRING);

    } else if (lookahead == INTEGER) {
	match(INTEGER);

    } else if (lookahead == REAL) {
	match(REAL);

    } else if (lookahead == ID) {
        name = identifier();
        checkID(name);
        if (lookahead == '(') {
            match('(');
            
            if (lookahead != ')') {
                expression();
                
                while (lookahead == ',') {
                    match(',');
                    expression();
                }
            }

	    match(')');
        }
    } else
	error();
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 *		  postfix-expression ++
 *		  postfix-expression --
 */

static void postfixExpression()
{
    primaryExpression();

    while (1) {
	if (lookahead == '[') {
	    match('[');
	    expression();
	    match(']');
	    //cout << "index" << endl;

	} else if (lookahead == INC) {
	    match(INC);
	    //cout << "inc" << endl;

	} else if (lookahead == DEC) {
	    match(DEC);
	    //cout << "dec" << endl;

	} else
	    break;
    }
}


/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 *		  sizeof ( specifier pointers )
 *		  ( specifier pointers ) prefix-expression
 *
 *		This grammar is still ambiguous since "sizeof(type) * n"
 *		could be interpreted as a multiplicative expression or as a
 *		cast of a dereference.  The correct interpretation is the
 *		former, as the latter makes little sense semantically.  We
 *		resolve the ambiguity here by always consuming the "(type)"
 *		as part of the sizeof expression.
 */

static void prefixExpression()
{
    if (lookahead == '!') {
	match('!');
	prefixExpression();
	//cout << "not" << endl;

    } else if (lookahead == '-') {
	match('-');
	prefixExpression();
	//cout << "neg" << endl;

    } else if (lookahead == '*') {
	match('*');
	prefixExpression();
	//cout << "deref" << endl;

    } else if (lookahead == '&') {
	match('&');
	prefixExpression();
	//cout << "addr" << endl;

    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(' && isSpecifier(peek())) {
	    match('(');
	    specifier();
	    pointers();
	    match(')');
	} else
	    prefixExpression();

	//cout << "sizeof" << endl;

    } else if (lookahead == '(' && isSpecifier(peek())) {
	match('(');
	specifier();
	pointers();
	match(')');
	prefixExpression();
	//cout << "cast" << endl;

    } else
	postfixExpression();
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  prefix-expression
 *		  multiplicative-expression * prefix-expression
 *		  multiplicative-expression / prefix-expression
 *		  multiplicative-expression % prefix-expression
 */

static void multiplicativeExpression()
{
    prefixExpression();

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    prefixExpression();
	    //cout << "mul" << endl;

	} else if (lookahead == '/') {
	    match('/');
	    prefixExpression();
	   // cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    prefixExpression();
	    //cout << "rem" << endl;

	} else
	    break;
    }
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static void additiveExpression()
{
    multiplicativeExpression();

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    multiplicativeExpression();
	   // cout << "add" << endl;

	} else if (lookahead == '-') {
	    match('-');
	    multiplicativeExpression();
	    //cout << "sub" << endl;

	} else
	    break;
    }
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static void relationalExpression()
{
    additiveExpression();

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    additiveExpression();
	  //  cout << "ltn" << endl;

	} else if (lookahead == '>') {
	    match('>');
	    additiveExpression();
	  //  cout << "gtn" << endl;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    additiveExpression();
	   // cout << "leq" << endl;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    additiveExpression();
	   // cout << "geq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static void equalityExpression()
{
    relationalExpression();

    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    relationalExpression();
	    //cout << "eql" << endl;

	} else if (lookahead == NEQ) {
	    match(NEQ);
	    relationalExpression();
	    //cout << "neq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static void logicalAndExpression()
{
    equalityExpression();

    while (lookahead == AND) {
	match(AND);
	equalityExpression();
	//cout << "and" << endl;
    }
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static void expression()
{
    logicalAndExpression();

    while (lookahead == OR) {
	match(OR);
	logicalAndExpression();
	//cout << "or" << endl;
    }
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
    while (lookahead != '}')
	statement();
}


/*
 * Function:	Assignment
 *
 * Description:	Parse an assignment statement.
 *
 *		assignment:
 *		  expression = expression
 *		  expression
 */

static void assignment()
{
    expression();

    if (lookahead == '=') {
	match('=');
	expression();
    }
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  break ;
 *		  return expression ;
 *		  while ( expression ) statement
 *		  for ( assignment ; expression ; assignment ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  assignment ;
 *
 *		This grammar still suffers from the "dangling-else"
 *		ambiguity.  We resolve it here by always consuming the
 *		"else" token, thus matching an else with the closest
 *		unmatched if.
 */

static void statement()
{
    if (lookahead == '{') {
	match('{');
	openScope(); // open statement scope
	declarations();
	statements();
    closeScope(); // close statement scope;
	match('}');
	
	
    } else if (lookahead == BREAK) {
	match(BREAK);
	match(';');

    } else if (lookahead == RETURN) {
	match(RETURN);
	expression();
	match(';');

    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	expression();
	match(')');
	statement();

    } else if (lookahead == FOR) {
	match(FOR);
	match('(');
	assignment();
	match(';');
	expression();
	match(';');
	assignment();
	match(')');
	statement();

    } else if (lookahead == IF) {
	match(IF);
	match('(');
	expression();
	match(')');
	statement();

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	assignment();
	match(';');
    }
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers identifier
 */

static void parameter()
{
    int typespec;
    unsigned indirection;
    string name;
    Type t;
    
    typespec = specifier();
    indirection = pointers();
    name = identifier();
    t = Type(typespec, indirection);
    declareVariable(name, t);
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *		  parameter-list , ...
 */

Parameters*  parameters()
{
    int spec;
    if (lookahead == VOID){
        spec = VOID;
        match(VOID);
        if(lookahead == RPAREN){
            return nullptr;
        }
    }
    else if(lookahead == RPAREN){
        return nullptr;
    }
    else
    {
        spec = specifier();
    }
    

    Parameters* params = new Parameters();
    const unsigned indirection = pointers();
    const string name = identifier();
    const Type type = Type(spec,indirection);

    declareVariable(name,type);
    params->types.push_back(type);

    while (lookahead == ',') {
        match(',');
        params->types.push_back(type);
        if (lookahead == ELLIPSIS) {
            match(ELLIPSIS);
            params->variadic = true;
            break;
        }
        parameter();
    }
    return params;
}


/*
 * Function:	globalDeclarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable, an array, or a funct	openscope(); // globalscope	    match('}');
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier [ integer ]
 *		  pointers identifier ( parameters )
 */

static void globalDeclarator(int typespec)
{
    int indirection = pointers();
    string name = identifier();
    if (lookahead == '[') {
	match('[');
	//match(INTEGER);
	int arraysize = integer();
	match(']');
        declareVariable(name, Type(typespec, indirection, arraysize));
    } else if (lookahead == '(') {
        match('(');
        openScope();
        Parameters* params = new Parameters();
        parameters();
        declareFunction(name, Type(typespec, indirection, params));
        match(')');
        closeScope();
    }
    else
        declareVariable(name, Type(typespec, indirection));
}


/*
 * Function:	remainingDeclarators
 *
 * Description:	Parse any remaining global declarators after the first.
 *
 * 		remaining-declarators
 * 		  ;
 * 		  , global-declarator remaining-declarators
 */

static void remainingDeclarators(int typespec)
{
    while (lookahead == ',') {
	match(',');
	globalDeclarator(typespec);
    }

    match(';');
}


/*
 * Function:	topLevelDeclaration
 *
 * Description:	Parse a global declaration or function definition.
 *
 * 		global-or-function:
 * 		  specifier pointers identifier remaining-decls
 * 		  specifier pointers identifier [ integer ] remaining-decls
 * 		  specifier pointers identifier ( parameters ) remaining-decls 
 * 		  specifier pointers identifier ( parameters ) { ... }
 */

static void topLevelDeclaration()
{

    Parameters* params = new Parameters();
    int typespec = specifier();
    unsigned indirection = pointers();
    string name =  identifier();

    if (lookahead == '[') {
        match('[');
        //match(INTEGER);
        int arraysize = integer();
        match(']');
        declareVariable(name, Type(typespec, indirection, arraysize));
        remainingDeclarators(typespec);
    }
    else if (lookahead == '(') {
        match('(');
        openScope(); //function scope: ex. f(intx) {x =1;} scope starts at f(_(here)_  int x){ ... }
        params = parameters();
        match(')');
        if(lookahead != '{'){
            closeScope(); //closes function declaration scope: ex. f(int x);
            declareFunction(name , Type(typespec, indirection, params));
        }
        if (lookahead == '{') {
            defineFunction(name, Type(typespec, indirection, params));
            match('{');
            
            declarations();
            statements();
            match('}');
            closeScope(); //closes function declaration scope: ex. f(int x){ x=1 }_(here)_
        } else
            remainingDeclarators(typespec);
    }
    else{
        declareVariable(name, Type(typespec, indirection));
        remainingDeclarators(typespec);
    }

	
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
    lookahead = yylex();
	openScope(); // globalscope

    while (lookahead != DONE)
	topLevelDeclaration();

	closeScope(); // closes globalscope
    exit(EXIT_SUCCESS);
}
