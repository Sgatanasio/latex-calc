#ifndef TABLE_HPP_
#define TABLE_HPP_

#include <string>
#include <map>

#include "symbol.hpp"

class SymbolTable	{
	private:
		std::map<std::string, Symbol> _data;
	public:
		SymbolTable(){
			this->_data.clear();
		}

		~SymbolTable(){
			this->_data.clear();
		}

		bool lookupSymbol(const std::string & name) const;
		Symbol * getSymbol(const std::string & name);
	
		void addSymbol(Symbol * s);
		void rmSymbol(const std::string & name);

		// Prints the table contents in an aligned, human-readable format.
		void print() const;
		
		inline bool empty() const {
			return this->_data.empty();
		}

		inline int size() const	{
			return this->_data.size();
		}


};

#endif
