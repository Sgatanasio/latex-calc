#include "symbol.hpp"

bool lp::Symbol::operator==(const lp::Symbol & s) const {
		return ( this->getName() == s.getName() );
}


bool lp::Symbol::operator <(const lp::Symbol & s) const {
   return (this->getName() < s.getName());
}

bool lp::Symbol::operator >(const lp::Symbol & s) const {
   return (this->getName() > s.getName());
}

