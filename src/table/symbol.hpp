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
		
		inline Symbol(const Symbol & s){
			this->setName(s.getName());
			this->setToken(s.getToken());
		}

		inline const std::string & getName() const { return this->_name; }
		inline void setName(const std::string & name) { this->_name = name;}
		
		inline int getToken() const { return this->_token; }
		inline void setToken(int token) { this->_token = token;}

		bool operator==(const Symbol & s) const;
		bool operator <(const Symbol & s) const;
		bool operator >(const Symbol & s) const;

};


#endif
