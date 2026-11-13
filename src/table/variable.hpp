#ifndef VARIABLE_HPP_
#define VARIABLE_HPP_

#include <string>
#include <utility>

#include "symbol.hpp"

class Variable : public Symbol {
	public:
		inline Variable(std::string name = "", int token = 0) : Symbol(name, token) {}

		// Conversion from a Symbol: copies its state, the source stays valid.
		inline Variable(const Symbol & s) : Symbol(s) {}

		// Move conversion: takes over the state, the source is left valid.
		inline Variable(Symbol && s) : Symbol(std::move(s)) {}

		bool operator==(const Variable & v) const;
		bool operator <(const Variable & v) const;
		bool operator >(const Variable & v) const;

};


#endif
