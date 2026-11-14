#include "init.hpp"
#include "table.hpp"


void initOperators(SymbolTable &t){
	for(int i = 0; i < static_cast<int>(sizeof(operators)/sizeof(SymbolEntry)); i++){
		t.addSymbol(new Symbol(operators[i].name,operators[i].token));
	}
}

void initSymbols(SymbolTable &t){
	for(int i = 0; i < static_cast<int>(sizeof(symbols)/sizeof(SymbolEntry)); i++){
		t.addSymbol(new Symbol(symbols[i].name,symbols[i].token));
	}
}

void initFunctions(SymbolTable &t){
	for(int i = 0; i < static_cast<int>(sizeof(functions)/sizeof(SymbolEntry)); i++){
		t.addSymbol(new Symbol(functions[i].name,functions[i].token));
	}
}

void initConstants(SymbolTable &t){
	for(int i = 0; i < static_cast<int>(sizeof(constants)/sizeof(SymbolEntry)); i++){
		t.addSymbol(new Symbol(constants[i].name,constants[i].token));
	}
}

void init(SymbolTable &t){
	initOperators(t);
	initSymbols(t);
	initFunctions(t);
	initConstants(t);
}
