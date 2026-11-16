#include "tree.hpp"

#include <cmath>
#include <iostream>

namespace {

void printTree(const ExpNode & node, std::ostream & out,
               const std::string & prefix, bool isLast, bool isRoot) {
	out << (isRoot ? "" : prefix + (isLast ? "└── " : "├── "));
	node.printLabel(out);
	out << '\n';

	const std::string childPrefix = isRoot
		? std::string()
		: prefix + (isLast ? "    " : "│   ");

	const std::vector<const ExpNode *> children = node.children();
	for (std::size_t i = 0; i < children.size(); ++i) {
		printTree(*children[i], out, childPrefix, i + 1 == children.size(), false);
	}
}

} // namespace

void ExpNode::printAST() const {
	printTree(*this, std::cout, "", true, true);
}

// ---------------------------------------------------------------------------
// Leaves
// ---------------------------------------------------------------------------

void NumNode::printLabel(std::ostream & out) const {
	out << this->_value;
}

std::vector<const ExpNode *> NumNode::children() const {
	return {};
}

GiNaC::ex NumNode::evaluate() const {
	// Keep integral literals exact so operations such as factorial and mod
	// receive a GiNaC integer instead of a floating-point number.
	if (std::isfinite(this->_value) && std::floor(this->_value) == this->_value)
		return GiNaC::numeric(static_cast<long long>(this->_value));
	return GiNaC::numeric(this->_value);
}

void SymNode::printLabel(std::ostream & out) const {
	out << this->_name;
}

std::vector<const ExpNode *> SymNode::children() const {
	return {};
}

GiNaC::ex SymNode::evaluate() const {
	return GiNaC::symbol(this->_name);
}

// ---------------------------------------------------------------------------
// Operator bases
// ---------------------------------------------------------------------------

void UnaryOpNode::printLabel(std::ostream & out) const {
	out << this->_label;
}

std::vector<const ExpNode *> UnaryOpNode::children() const {
	return {this->_arg.get()};
}

void BinaryOpNode::printLabel(std::ostream & out) const {
	out << this->_label;
}

std::vector<const ExpNode *> BinaryOpNode::children() const {
	return {this->_left.get(), this->_right.get()};
}

// ---------------------------------------------------------------------------
// Binary operators
// ---------------------------------------------------------------------------

GiNaC::ex PlusNode::evaluate() const {
	return this->_left->evaluate() + this->_right->evaluate();
}

GiNaC::ex MinusNode::evaluate() const {
	return this->_left->evaluate() - this->_right->evaluate();
}

GiNaC::ex TimesNode::evaluate() const {
	return this->_left->evaluate() * this->_right->evaluate();
}

GiNaC::ex DivNode::evaluate() const {
	return this->_left->evaluate() / this->_right->evaluate();
}

GiNaC::ex PowerNode::evaluate() const {
	return GiNaC::pow(this->_left->evaluate(), this->_right->evaluate());
}

GiNaC::ex ModNode::evaluate() const {
	return GiNaC::mod(GiNaC::ex_to<GiNaC::numeric>(this->_left->evaluate()),
	                  GiNaC::ex_to<GiNaC::numeric>(this->_right->evaluate()));
}

// ---------------------------------------------------------------------------
// Unary operators
// ---------------------------------------------------------------------------

GiNaC::ex PercentNode::evaluate() const {
	return this->_arg->evaluate() / 100;
}

GiNaC::ex FactorialNode::evaluate() const {
	return GiNaC::factorial(this->_arg->evaluate());
}

GiNaC::ex UnaryMinusNode::evaluate() const {
	return -this->_arg->evaluate();
}

GiNaC::ex UnaryPlusNode::evaluate() const {
	return this->_arg->evaluate();
}
