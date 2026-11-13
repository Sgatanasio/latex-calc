#ifndef SYMBOL_HPP_
#define SYMBOL_HPP_

#include <string>

class Symbol {
	private:
		int _token;
		std::string _name;
	public:
		
		inline Symbol(std::string name = "", int token = 0){ 
			_name	=	name;
			_token = token;
		}
		
		Symbol(const Symbol & s) = default;
		Symbol(Symbol && s) = default;
		Symbol & operator=(const Symbol & s) = default;
		Symbol & operator=(Symbol && s) = default;
		virtual ~Symbol() = default;

		inline const std::string & getName() const { return this->_name; }
		inline void setName(const std::string & name) { this->_name = name;}
		
		inline int getToken() const { return this->_token; }
		inline void setToken(int token) { this->_token = token;}

		bool operator==(const Symbol & s) const;
		bool operator <(const Symbol & s) const;
		bool operator >(const Symbol & s) const;

};


#endif
