#include <iostream>
#include <memory>

#include "ast/tree.hpp"
#include "table/table.hpp"
#include "table/variable.hpp"
#include "table/init.hpp"

using std::make_unique;

int main(){
	SymbolTable table;
	init(table);

	table.print();

	// Example tree covering every AST node type:
	//
	//   ((3 * x + y / 2) ^ (-2)) - ((7 \mod 3)! + (+z)%)
	//
	// Binary: +, -, *, /, ^, \mod
	// Unary:  -, +, !, %
	// Leaves: number and symbol
	auto tree = make_unique<MinusNode>(
		make_unique<PowerNode>(
			make_unique<PlusNode>(
				make_unique<TimesNode>(
					make_unique<NumNode>(3.0),
					make_unique<SymNode>("x")),
				make_unique<DivNode>(
					make_unique<SymNode>("y"),
					make_unique<NumNode>(2.0))),
			make_unique<UnaryMinusNode>(
				make_unique<NumNode>(2.0))),
		make_unique<PlusNode>(
			make_unique<FactorialNode>(
				make_unique<ModNode>(
					make_unique<NumNode>(7.0),
					make_unique<NumNode>(3.0))),
			make_unique<PercentNode>(
				make_unique<UnaryPlusNode>(
					make_unique<SymNode>("z")))));

	std::cout << std::endl;
	tree->printAST();
	std::cout << "evaluate() = " << tree->evaluate() << std::endl;

	return 0;
}
