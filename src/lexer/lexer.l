%option noyywrap

%{

#include <iostream>
#include <string.h>

#include "../table/init.hpp"

%}

%x ERROR

%%


/* One line comments */

%.*$	{ ; }

[ \t] { ; }
\n		{ lineNum++; }

;			{ return STMT_END }

/* NUMBERS */

(\d+\.\d*|\d*\.\d+|\d+)([eE][+\-]?\d+)? 	{
		yylval.number = atof(yytext);
		return NUMBER;
}



%%
