%option noyywrap nounput noinput always-interactive

%{

#include <iostream>
#include <string.h>
#include <cstdlib>

#include "parser.tab.h"
#include "table/init.hpp"
#include "table/table.hpp"
#include "table/variable.hpp"

int lineNum = 1;
extern SymbolTable table;

%}

%x ERROR

%%

	/* One line comments (only when % starts a line) */

^[ \t]*%.*$	{ ; }

[ \t]		{ ; }
\r?\n		{ lineNum++; return STMT_END; }

	/* NUMBERS */

([0-9]+\.[0-9]*|[0-9]*\.[0-9]+|[0-9]+)([eE][+\-]?[0-9]+)? 	{
		yylval.number = atof(yytext);
		return NUMBER;
}

[a-zA-Z]+ {
	std::string id(yytext);
	yylval.string = strdup(yytext);

	if(!table.lookupSymbol(id)){
		Variable * n = new Variable(id,VARIABLE);
		table.addSymbol(n);
		return VARIABLE;
	}else{
		Symbol * s = table.getSymbol(id);
		return s->getToken();
	}
}



	/* ARITHMETIC OPERATORS */

"+"			{ return PLUS; }
"-"			{ return MINUS; }
"*"			{ return TIMES; }
"/"			{ return DIV; }
"^"			{ return POWER; }
"**"		{ return POWER; }
"%"			{ return PERCENT; }
"!"			{ return FACTORIAL; }

	/* GROUPING, SEPARATION AND ASSIGNMENT SYMBOLS */

"("			{ return LPAREN; }
")"			{ return RPAREN; }
"["			{ return LBRACKET; }
"]"			{ return RBRACKET; }
"{"			{ return LBRACE; }
"}"			{ return RBRACE; }
"|"			{ return BAR; }
","			{ return COMMA; }
";"			{ return STMT_END; }
"="			{ return EQUAL; }

<<EOF>> { return 0; }

. {
		BEGIN(ERROR);
		yymore();
	}


<ERROR>[^0-9+\-*/()\^% \t\n\;a-zA-Z=<>!&] {
		yymore();
	}
<ERROR>(.|\n) {
		yyless(yyleng-1);
		//DISPLAY LEXICAL ERROR
		BEGIN(INITIAL);
	}
%%
