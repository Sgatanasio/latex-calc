#ifndef VARIABLE_HPP_
#define VARIABLE_HPP_

#include<string>

class Variable {
	private:
		std::string _name;
		int _token;
	public:
		inline Variable(std::string name = "", int token = 0){
			_name = name; 
			_token = token;
		}
		
		inline Variable(const Variable & v){
			this->setName(s.getName());
			this->setToken(s.getToken());
		}

		inline const std::string & getName() const { return this->_name; }
		inline void setName(const std::string & name) { this->_name = name; }

		inline int getToken() const { return this->_token; }
		inline void setToken(const int & token) { this->_token = token; }


		bool operator==(const Variable & v) const;
		bool operator <(const Variable & v) const;
		bool operator >(const Variable & v) const;

};


#endif
