#include <stdio.h>
#include <string>

#include "table/table.hpp"
#include "table/variable.hpp"

int main(){
	SymbolTable table;

	// A few predefined symbols.
	Symbol plus("+", 258);
	Symbol minus("-", 259);
	Symbol times("*", 260);
	Symbol pi("pi", 300);

	// User-defined variables.
	Variable x("x", 400);
	Variable alpha("alpha", 401);
	Variable ans("ans", 402);
	Variable arcsinh("arcsinh", 403);

	table.addSymbol(&plus);
	table.addSymbol(&minus);
	table.addSymbol(&times);
	table.addSymbol(&pi);
	table.addSymbol(&x);
	table.addSymbol(&alpha);
	table.addSymbol(&ans);
	table.addSymbol(&arcsinh);

	printf("Initial symbol table:\n\n");
	table.print();

	// Lookups.
	printf("\nlookupSymbol(\"pi\")    = %s\n", table.lookupSymbol("pi") ? "true" : "false");
	printf("lookupSymbol(\"gamma\") = %s\n", table.lookupSymbol("gamma") ? "true" : "false");

	Symbol * found = table.getSymbol("x");
	if (found != nullptr)
		printf("getSymbol(\"x\") token = %d\n", found->getToken());

	// Removal and re-print.
	printf("\nRemoving \"minus\" and \"alpha\"...\n\n");
	table.rmSymbol("minus");
	table.rmSymbol("alpha");
	table.print();

	return 0;
}
