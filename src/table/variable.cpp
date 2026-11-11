#include "variable.hpp"

bool Variable::operator==(const Variable & v) const { return this->getName() == v.getName(); }
bool Variable::operator<(const Variable & v) const { return this->getName() < v.getName(); }
bool Variable::operator>(const Variable & v) const { return this->getName() > v.getName(); }

