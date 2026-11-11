#include "symbol.hpp"

bool Symbol::operator==(const Symbol & s) const {
		return ( this->getName() == s.getName() );
}


bool Symbol::operator <(const Symbol & s) const {
   return (this->getName() < s.getName());
}

bool Symbol::operator >(const Symbol & s) const {
   return (this->getName() > s.getName());
}

