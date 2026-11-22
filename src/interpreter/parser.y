%{

#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <ginac/ginac.h>

#include "../table/table.hpp"
#include "../ast/tree.hpp"

int yylex();
void yyerror(const char * msg);

extern int lineNumber;
extern SymbolTable t;

namespace {

// Takes ownership of a node produced by the grammar.
std::unique_ptr<ExpNode> own(ExpNode * node) {
	return std::unique_ptr<ExpNode>(node);
}

// Leaf used for constants and other literal values.
class LiteralNode : public ExpNode {
	private:
		std::string _label;
		GiNaC::ex _value;
	public:
		LiteralNode(std::string label, const GiNaC::ex & value)
			: _label(std::move(label)), _value(value) {}

		void printLabel(std::ostream & out) const override {
			out << this->_label;
		}

		std::vector<const ExpNode *> children() const override {
			return {};
		}

		GiNaC::ex evaluate() const override {
			return this->_value;
		}
};

// Function application with one or two arguments.
class CallNode : public ExpNode {
	private:
		std::string _name;
		std::vector<std::unique_ptr<ExpNode> > _args;
	public:
		CallNode(std::string name, std::vector<std::unique_ptr<ExpNode> > args)
			: _name(std::move(name)), _args(std::move(args)) {}

		CallNode(std::string name, std::unique_ptr<ExpNode> arg)
			: _name(std::move(name)) {
			this->_args.push_back(std::move(arg));
		}

		CallNode(std::string name, std::unique_ptr<ExpNode> left,
		         std::unique_ptr<ExpNode> right)
			: _name(std::move(name)) {
			this->_args.push_back(std::move(left));
			this->_args.push_back(std::move(right));
		}

		void printLabel(std::ostream & out) const override {
			out << this->_name;
		}

		std::vector<const ExpNode *> children() const override {
			std::vector<const ExpNode *> result;
			result.reserve(this->_args.size());
			for (std::size_t i = 0; i < this->_args.size(); ++i)
				result.push_back(this->_args[i].get());
			return result;
		}

		GiNaC::ex evaluate() const override {
			std::vector<GiNaC::ex> args;
			args.reserve(this->_args.size());
			for (std::size_t i = 0; i < this->_args.size(); ++i)
				args.push_back(this->_args[i]->evaluate());

			if (this->_name == "sqrt")
				return GiNaC::sqrt(args[0]);
			if (this->_name == "cbrt")
				return GiNaC::pow(args[0], GiNaC::numeric(1, 3));
			if (this->_name == "exp")
				return GiNaC::exp(args[0]);

			if (this->_name == "log" || this->_name == "ln")
				return GiNaC::log(args[0]);
			if (this->_name == "log10")
				return GiNaC::log(args[0]) / GiNaC::log(GiNaC::numeric(10));
			if (this->_name == "log2")
				return GiNaC::log(args[0]) / GiNaC::log(GiNaC::numeric(2));

			if (this->_name == "sin")
				return GiNaC::sin(args[0]);
			if (this->_name == "cos")
				return GiNaC::cos(args[0]);
			if (this->_name == "tan")
				return GiNaC::tan(args[0]);

			if (this->_name == "asin")
				return GiNaC::asin(args[0]);
			if (this->_name == "acos")
				return GiNaC::acos(args[0]);
			if (this->_name == "atan")
				return GiNaC::atan(args[0]);

			if (this->_name == "sinh")
				return GiNaC::sinh(args[0]);
			if (this->_name == "cosh")
				return GiNaC::cosh(args[0]);
			if (this->_name == "tanh")
				return GiNaC::tanh(args[0]);

			if (this->_name == "asinh")
				return GiNaC::asinh(args[0]);
			if (this->_name == "acosh")
				return GiNaC::acosh(args[0]);
			if (this->_name == "atanh")
				return GiNaC::atanh(args[0]);

			if (this->_name == "abs")
				return GiNaC::abs(args[0]);
			if (this->_name == "floor")
				return GiNaC::numeric(std::floor(
					GiNaC::ex_to<GiNaC::numeric>(args[0]).to_double()));
			if (this->_name == "ceil")
				return GiNaC::numeric(std::ceil(
					GiNaC::ex_to<GiNaC::numeric>(args[0]).to_double()));
			if (this->_name == "round")
				return GiNaC::numeric(std::round(
					GiNaC::ex_to<GiNaC::numeric>(args[0]).to_double()));

			if (this->_name == "gcd")
				return GiNaC::gcd(args[0], args[1]);
			if (this->_name == "lcm")
				return GiNaC::lcm(args[0], args[1]);
			if (this->_name == "ncr")
				return GiNaC::binomial(args[0], args[1]);
			if (this->_name == "npr")
				return GiNaC::factorial(args[0])
					/ GiNaC::factorial(args[0] - args[1]);

			throw std::runtime_error("unknown function: " + this->_name);
		}
};

} // namespace

%}

%union {
	double number;
	const char *string;
	ExpNode *exp;
}

%start program

%type <exp> exp
%type <string> unary_function binary_function

// Valores literales
%token <number> NUMBER
%token <string> VARIABLE UNDEFINED

// Operadores aritméticos
%left PLUS MINUS
%left TIMES DIV MOD
%right POWER
%precedence UNARY
%precedence PERCENT FACTORIAL

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
		%empty
		|	program exp STMT_END
		;

exp :	NUMBER
		{
			$$ = new NumNode($1);
		}
		|	VARIABLE
		{
			$$ = new SymNode($1);
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
			$$ = new LiteralNode("\\ans", GiNaC::symbol("ans"));
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
		|	exp PLUS exp
		{
			$$ = new PlusNode(own($1), own($3));
		}
		|	exp MINUS exp
		{
			$$ = new MinusNode(own($1), own($3));
		}
		|	exp TIMES exp
		{
			$$ = new TimesNode(own($1), own($3));
		}
		|	exp DIV exp
		{
			$$ = new DivNode(own($1), own($3));
		}
		|	exp POWER exp
		{
			$$ = new PowerNode(own($1), own($3));
		}
		|	exp MOD exp
		{
			$$ = new ModNode(own($1), own($3));
		}
		|	PLUS exp %prec UNARY
		{
			$$ = new UnaryPlusNode(own($2));
		}
		|	MINUS exp %prec UNARY
		{
			$$ = new UnaryMinusNode(own($2));
		}
		|	exp PERCENT
		{
			$$ = new PercentNode(own($1));
		}
		|	exp FACTORIAL
		{
			$$ = new FactorialNode(own($1));
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
	std::cerr << "line " << lineNumber << ": " << msg << std::endl;
}
