#include "table/table.hpp"
#include "table/init.hpp"

SymbolTable table;

int main(){
	init(table);
	return yyparse() != 0;
}
