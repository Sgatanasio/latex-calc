%{
// INCLUDES
#include <iostream>

int yylex();
void yyerror(const char * msg);

%}

%define parse.error verbose
%start program

%union {
	double number;
}

// Valores literales
%token <number> NUMBER

// Operadores aritméticos
%token PLUS MINUS TIMES DIV POWER PERCENT FACTORIAL MOD

// Funciones matemáticas
%token SQRT CBRT EXP
%token LOG LOG10 LOG2 LN
%token SIN COS TAN
%token ARCSIN ARCCOS ARCTAN
%token SINH COSH TANH
%token ARCSINH ARCCOSH ARCTANH
%token ABS FLOOR CEIL ROUND GCD LCM NCR NPR

// Constantes
%token PI E ANS
%token DEG RAD

// Símbolos de agrupación, separación y asignación
%token LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token BAR COMMA STMT_END EQUAL

%%

program:
	/* Gramática pendiente de definir. */
	;

%%

void yyerror(const char * msg) {
	std::cerr << msg << std::endl;
}
