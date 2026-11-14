%option noyywrap nounput noinput

%{

#include <iostream>
#include <string.h>
#include <cstdlib>

#include "parser.tab.h"
#include "table/init.hpp"

int lineNum = 1;

%}

%x ERROR

%%

	/* One line comments */

%.*$	{ ; }

[ \t] { ; }
\n		{ lineNum++; }

;			{ return STMT_END; }

	/* NUMBERS */

([0-9]+\.[0-9]*|[0-9]*\.[0-9]+|[0-9]+)([eE][+\-]?[0-9]+)? 	{
		yylval.number = atof(yytext);
		return NUMBER;
}

%%
