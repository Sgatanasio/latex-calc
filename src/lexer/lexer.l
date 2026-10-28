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

(\d+\.\d*|\d*\.\d+|\d+)([eE][+\-]?\d+)? 	{
		yylval.number = atof(yytext);
		return NUMBER;
}

/* Symbols in table */

"-"		{ return MINUS; }
"+"		{ return PLUS; }
"*"		{ return TIMES; }
"/"		{ return DIV; }

"^"		{ return POWER; }
"log"	{	return LOG; }
"ln"	{ return LN; }

"sin"	{	return SIN; }
"cos"	{	return COS;	}
"tan"	{	return TAN;	}

"arcsin"	{ return ARCSIN;	}
"arccos"	{	return ARCCOS;	}
"arctan"	{	return ARCTAN;	}


%%
