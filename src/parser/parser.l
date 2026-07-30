%option noyywrap

%{

#include <iostream>
#include <string.h>

%}

%x ERROR

%%


/* One line comments */

%.*$	{ ; }

[ \t] { ; }
\n		{ lineNum++; }

;			{ return STMT_END }

/* NUMBERS */

[0-9]+(\.[0-9]+)? 	{
		yylval.number = atof(yytext);
		return NUMBER;
	}

/* Symbols in table */

"-"		{ return MINUS; }
"+"		{ return PLUS; }
"*"		{ return TIMES; }
"/"		{ return DIV; }



%%
