#include <iostream>
#include <memory>

#include "ast/tree.hpp"
#include "table/table.hpp"
#include "table/variable.hpp"
#include "table/init.hpp"

SymbolTable table;
using std::make_unique;

int main(){
	init(table);
	table.print();

	yyparse();

	return 0;
}
