%{
// INCLUDES
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ginac/ginac.h>

#include "ast/tree.hpp"

int yylex();
void yyerror(const char * msg);

extern int lineNum;

namespace {

// Takes ownership of a node produced by the grammar.
std::unique_ptr<ExpNode> own(ExpNode * node) {
	return std::unique_ptr<ExpNode>(node);
}

} // namespace

// ASTs built for each expression in the input program, in input order.
std::vector<std::unique_ptr<ExpNode> > programAST;

const std::vector<std::unique_ptr<ExpNode> > & getProgramAST() {
	return programAST;
}

%}

%define parse.error verbose
%start program

// The generated header exposes ExpNode * in YYSTYPE, so the type must at
// least be declared there. The full definition is included by the parser
// prologue above.
%code requires {
class ExpNode;
}

%union {
	double number;
	char * string;
	const char * name;
	ExpNode * exp;
}

%type <exp> exp additive multiplicative unary power postfix primary
%type <name> unary_function binary_function

%destructor { delete $$; } <exp>
%destructor { std::free($$); } <string>

// Valores literales
%token <number> NUMBER
%token <string> VARIABLE

// Operadores aritméticos
// BAR has the lowest precedence so that, inside |...|, a BAR closes the
// absolute value instead of starting an implicit-multiplication operand.
%precedence BAR
%left PLUS MINUS
%left TIMES DIV MOD
%right POWER
%left PERCENT FACTORIAL

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
%token COMMA STMT_END EQUAL

%%

program:
		%empty
		{
			programAST.clear();
		}
		|	program statement
		;

statement:
		exp STMT_END
		{
			std::unique_ptr<ExpNode> expression = own($1);
			printExpression(std::cout, expression->evaluate().eval());
			std::cout << std::endl;
			programAST.push_back(std::move(expression));
		}
		|	STMT_END
		{
			/* Empty statement. */
		}
		|	error STMT_END
		{
			/* Skip the malformed statement and keep parsing the rest. */
			yyerrok;
		}
		;

exp :	additive
		{
			$$ = $1;
		}
		;

// %prec PLUS resolves the BAR conflict in favor of closing |...|.
additive :	multiplicative %prec PLUS
		{
			$$ = $1;
		}
		|	additive PLUS multiplicative
		{
			$$ = new PlusNode(own($1), own($3));
		}
		|	additive MINUS multiplicative
		{
			$$ = new MinusNode(own($1), own($3));
		}
		;

multiplicative :	unary
		{
			$$ = $1;
		}
		|	multiplicative TIMES unary
		{
			$$ = new TimesNode(own($1), own($3));
		}
		|	multiplicative DIV unary
		{
			$$ = new DivNode(own($1), own($3));
		}
		|	multiplicative MOD unary
		{
			$$ = new ModNode(own($1), own($3));
		}
		|	multiplicative power
		{
			// Implicit multiplication: "5x" is "5 * x".
			$$ = new TimesNode(own($1), own($2));
		}
		;

unary :	power
		{
			$$ = $1;
		}
		|	PLUS unary
		{
			$$ = new UnaryPlusNode(own($2));
		}
		|	MINUS unary
		{
			$$ = new UnaryMinusNode(own($2));
		}
		;

power :	postfix %prec POWER
		{
			$$ = $1;
		}
		|	postfix POWER unary
		{
			$$ = new PowerNode(own($1), own($3));
		}
		;

postfix :	primary
		{
			$$ = $1;
		}
		|	postfix PERCENT
		{
			$$ = new PercentNode(own($1));
		}
		|	postfix FACTORIAL
		{
			$$ = new FactorialNode(own($1));
		}
		;

primary :	NUMBER
		{
			$$ = new NumNode($1);
		}
		|	VARIABLE
		{
			std::string name($1 != nullptr ? $1 : "");
			std::free($1);
			$$ = new SymNode(std::move(name));
		}
		|	PI
		{
			$$ = new LiteralNode("\\pi", GiNaC::Pi);
		}
		|	E
		{
			$$ = new LiteralNode("e", GiNaC::numeric(std::exp(1.0)));
		}
		|	ANS
		{
			$$ = new LiteralNode("\\ans", internedSymbol("ans"));
		}
		|	DEG
		{
			$$ = new LiteralNode("deg", GiNaC::Pi / 180);
		}
		|	RAD
		{
			$$ = new LiteralNode("rad", GiNaC::numeric(1));
		}
		|	LPAREN exp RPAREN
		{
			$$ = $2;
		}
		|	LBRACKET exp RBRACKET
		{
			$$ = $2;
		}
		|	LBRACE exp RBRACE
		{
			$$ = $2;
		}
		|	BAR exp BAR
		{
			$$ = new CallNode("abs", own($2));
		}
		|	unary_function LPAREN exp RPAREN
		{
			$$ = new CallNode($1, own($3));
		}
		|	unary_function LBRACKET exp RBRACKET
		{
			$$ = new CallNode($1, own($3));
		}
		|	unary_function LBRACE exp RBRACE
		{
			$$ = new CallNode($1, own($3));
		}
		|	binary_function LPAREN exp COMMA exp RPAREN
		{
			$$ = new CallNode($1, own($3), own($5));
		}
		|	binary_function LBRACKET exp COMMA exp RBRACKET
		{
			$$ = new CallNode($1, own($3), own($5));
		}
		|	binary_function LBRACE exp COMMA exp RBRACE
		{
			$$ = new CallNode($1, own($3), own($5));
		}
		;

unary_function :	SQRT
		{
			$$ = "sqrt";
		}
		|	CBRT
		{
			$$ = "cbrt";
		}
		|	EXP
		{
			$$ = "exp";
		}
		|	LOG
		{
			$$ = "log";
		}
		|	LOG10
		{
			$$ = "log10";
		}
		|	LOG2
		{
			$$ = "log2";
		}
		|	LN
		{
			$$ = "ln";
		}
		|	SIN
		{
			$$ = "sin";
		}
		|	COS
		{
			$$ = "cos";
		}
		|	TAN
		{
			$$ = "tan";
		}
		|	ARCSIN
		{
			$$ = "asin";
		}
		|	ARCCOS
		{
			$$ = "acos";
		}
		|	ARCTAN
		{
			$$ = "atan";
		}
		|	SINH
		{
			$$ = "sinh";
		}
		|	COSH
		{
			$$ = "cosh";
		}
		|	TANH
		{
			$$ = "tanh";
		}
		|	ARCSINH
		{
			$$ = "asinh";
		}
		|	ARCCOSH
		{
			$$ = "acosh";
		}
		|	ARCTANH
		{
			$$ = "atanh";
		}
		|	ABS
		{
			$$ = "abs";
		}
		|	FLOOR
		{
			$$ = "floor";
		}
		|	CEIL
		{
			$$ = "ceil";
		}
		|	ROUND
		{
			$$ = "round";
		}
		;

binary_function :	GCD
		{
			$$ = "gcd";
		}
		|	LCM
		{
			$$ = "lcm";
		}
		|	NCR
		{
			$$ = "ncr";
		}
		|	NPR
		{
			$$ = "npr";
		}
		;

%%

void yyerror(const char * msg) {
	std::cerr << "line " << lineNum << ": " << msg << std::endl;
}
