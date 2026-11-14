#include <stdio.h>
#include <string>

#include "table/table.hpp"
#include "table/variable.hpp"
#include "table/init.hpp"
int main(){
	SymbolTable table;
	init(table);

	table.print();

	return 0;
}
