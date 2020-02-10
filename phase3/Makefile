CXX		= g++
CXXFLAGS	= -g -Wall -std=c++11
EXTRAS		= lexer.cpp
OBJS		= lexer.o checker.o parser.o string.o type.o symbol.o scope.o 
PROG		= scc


all:		$(PROG)

$(PROG):	$(EXTRAS) $(OBJS)
		$(CXX) -o $(PROG) $(OBJS)

clean:;		$(RM) $(PROG) core *.o

clobber:;	$(RM) $(EXTRAS) $(PROG) core *.o

lexer.cpp:	lexer.l
		$(LEX) $(LFLAGS) -t lexer.l > lexer.cpp
