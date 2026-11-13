#include "table.hpp"

#include <algorithm>
#include <iostream>
#include <string>

namespace {

std::string padLeft(const std::string & text, std::size_t width) {
	if (text.size() >= width)
		return text;
	return std::string(width - text.size(), ' ') + text;
}

std::string padRight(const std::string & text, std::size_t width) {
	if (text.size() >= width)
		return text;
	return text + std::string(width - text.size(), ' ');
}

std::string repeat(char c, std::size_t count) {
	return std::string(count, c);
}

} // namespace

bool SymbolTable::lookupSymbol(const std::string & name) const {
	return this->_data.find(name) != this->_data.end();
}

Symbol * SymbolTable::getSymbol(const std::string & name) {
	std::map<std::string, Symbol>::iterator it = this->_data.find(name);
	if (it == this->_data.end())
		return nullptr;
	return &it->second;
}

void SymbolTable::addSymbol(Symbol * s) {
	if (s == nullptr)
		return;
	this->_data[s->getName()] = *s;
}

void SymbolTable::rmSymbol(const std::string & name) {
	this->_data.erase(name);
}

void SymbolTable::print() const {
	if (this->_data.empty()) {
		std::cout << "Symbol table is empty." << std::endl;
		return;
	}

	const std::string indexHeader = "#";
	const std::string nameHeader = "Name";
	const std::string tokenHeader = "Token";

	std::size_t indexWidth = indexHeader.size();
	std::size_t nameWidth = nameHeader.size();
	std::size_t tokenWidth = tokenHeader.size();

	std::size_t index = 0;
	for (std::map<std::string, Symbol>::const_iterator it = this->_data.begin();
		 it != this->_data.end(); ++it) {
		++index;
		indexWidth = std::max(indexWidth, std::to_string(index).size());
		nameWidth = std::max(nameWidth, it->first.size());
		tokenWidth = std::max(tokenWidth, std::to_string(it->second.getToken()).size());
	}

	const std::string border = "+" + repeat('-', indexWidth + 2)
		+ "+" + repeat('-', nameWidth + 2)
		+ "+" + repeat('-', tokenWidth + 2) + "+";

	std::cout << border << std::endl;
	std::cout << "| " << padLeft(indexHeader, indexWidth)
		<< " | " << padRight(nameHeader, nameWidth)
		<< " | " << padLeft(tokenHeader, tokenWidth) << " |" << std::endl;
	std::cout << border << std::endl;

	index = 0;
	for (std::map<std::string, Symbol>::const_iterator it = this->_data.begin();
		 it != this->_data.end(); ++it) {
		++index;
		std::cout << "| " << padLeft(std::to_string(index), indexWidth)
			<< " | " << padRight(it->first, nameWidth)
			<< " | " << padLeft(std::to_string(it->second.getToken()), tokenWidth)
			<< " |" << std::endl;
	}

	std::cout << border << std::endl;
	std::cout << this->_data.size() << " symbol(s) in table." << std::endl;
}
