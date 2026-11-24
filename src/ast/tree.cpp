#include "tree.hpp"

#include <cctype>
#include <cmath>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

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

GiNaC::ex internedSymbol(const std::string & name) {
	static std::map<std::string, GiNaC::symbol> cache;
	std::map<std::string, GiNaC::symbol>::iterator it = cache.find(name);
	if (it == cache.end())
		it = cache.emplace(name, GiNaC::symbol(name)).first;
	return it->second;
}

// ---------------------------------------------------------------------------
// Expression printing with implicit multiplication
// ---------------------------------------------------------------------------

namespace {

// A printed expression plus the context needed to decide how it can be joined
// to the next factor without changing how the result is read.
struct PrintedExpr {
	std::string text;
	bool numeric = false;
	bool constant = false;
	bool function = false;
};

PrintedExpr printExpr(const GiNaC::ex & expression);

// True when a term of a sum is written with a leading minus sign.
bool isNegativeTerm(const GiNaC::ex & expression) {
	if (GiNaC::is_a<GiNaC::numeric>(expression))
		return GiNaC::ex_to<GiNaC::numeric>(expression).is_negative();
	if (GiNaC::is_a<GiNaC::mul>(expression) && expression.nops() > 0) {
		const GiNaC::ex & coefficient = expression.op(expression.nops() - 1);
		if (GiNaC::is_a<GiNaC::numeric>(coefficient))
			return GiNaC::ex_to<GiNaC::numeric>(coefficient).is_negative();
	}
	return false;
}

std::string toString(const GiNaC::ex & expression) {
	std::ostringstream out;
	out << expression;
	return out.str();
}

// True when a base needs parentheses under a power.
bool needsBaseParens(const GiNaC::ex & base) {
	if (GiNaC::is_a<GiNaC::add>(base) || GiNaC::is_a<GiNaC::mul>(base))
		return true;
	if (GiNaC::is_a<GiNaC::numeric>(base)) {
		const GiNaC::numeric & value = GiNaC::ex_to<GiNaC::numeric>(base);
		return value.is_negative() || (value.is_rational() && !value.is_integer());
	}
	return false;
}

// True when an exponent needs parentheses under a power.
bool needsExponentParens(const GiNaC::ex & exponent) {
	if (GiNaC::is_a<GiNaC::add>(exponent) || GiNaC::is_a<GiNaC::mul>(exponent))
		return true;
	if (GiNaC::is_a<GiNaC::numeric>(exponent)) {
		const GiNaC::numeric & value = GiNaC::ex_to<GiNaC::numeric>(exponent);
		return value.is_negative() || (value.is_rational() && !value.is_integer());
	}
	return false;
}

PrintedExpr printSum(const GiNaC::ex & expression) {
	PrintedExpr result;
	for (std::size_t i = 0; i < expression.nops(); ++i) {
		const GiNaC::ex & term = expression.op(i);
		const bool negative = isNegativeTerm(term);
		const PrintedExpr printed = printExpr(negative ? -term : term);

		if (i == 0) {
			if (negative)
				result.text += '-';
		} else {
			result.text += negative ? '-' : '+';
		}
		result.text += printed.text;
	}
	return result;
}

// Prints an expression as a factor of a product, parenthesizing sums.
PrintedExpr printFactor(const GiNaC::ex & expression) {
	PrintedExpr printed = printExpr(expression);
	if (GiNaC::is_a<GiNaC::add>(expression))
		printed.text = '(' + printed.text + ')';
	return printed;
}

PrintedExpr printPower(const GiNaC::ex & expression) {
	const GiNaC::ex & base = expression.op(0);
	const GiNaC::ex & exponent = expression.op(1);

	const PrintedExpr printedBase = printExpr(base);
	const PrintedExpr printedExponent = printExpr(exponent);

	PrintedExpr result;
	if (needsBaseParens(base))
		result.text += '(' + printedBase.text + ')';
	else
		result.text += printedBase.text;

	result.text += '^';
	if (needsExponentParens(exponent))
		result.text += '(' + printedExponent.text + ')';
	else
		result.text += printedExponent.text;

	return result;
}

PrintedExpr printProduct(const GiNaC::ex & expression) {
	std::size_t count = expression.nops();
	GiNaC::ex coefficient(1);
	bool hasCoefficient = false;

	// GiNaC stores a product's numeric coefficient as its last operand.
	if (count > 0 && GiNaC::is_a<GiNaC::numeric>(expression.op(count - 1))) {
		coefficient = expression.op(count - 1);
		hasCoefficient = true;
		--count;
	}

	bool negative = hasCoefficient
		&& GiNaC::ex_to<GiNaC::numeric>(coefficient).is_negative();
	if (negative)
		coefficient = -coefficient;

	std::vector<PrintedExpr> parts;
	if (hasCoefficient && !coefficient.is_equal(1)) {
		PrintedExpr printedCoefficient;
		printedCoefficient.text = toString(coefficient);
		printedCoefficient.numeric = true;
		parts.push_back(printedCoefficient);
	}
	for (std::size_t i = 0; i < count; ++i)
		parts.push_back(printFactor(expression.op(i)));

	PrintedExpr result;
	if (negative)
		result.text += '-';

	for (std::size_t i = 0; i < parts.size(); ++i) {
		if (i > 0) {
			const char left = parts[i - 1].text.back();
			const char right = parts[i].text.front();

			// Add an explicit '*' only where omitting it would merge tokens
			// or change the meaning of the expression.
			if (std::isalnum(static_cast<unsigned char>(left))
			    && std::isalnum(static_cast<unsigned char>(right))) {
				bool star = false;
				if (parts[i - 1].numeric)
					star = std::isdigit(static_cast<unsigned char>(right)) != 0;
				else if (parts[i].constant || parts[i].function
				         || parts[i - 1].constant)
					star = true;
				if (star)
					result.text += '*';
			}
		}
		result.text += parts[i].text;
	}

	return result;
}

PrintedExpr printExpr(const GiNaC::ex & expression) {
	if (GiNaC::is_a<GiNaC::add>(expression))
		return printSum(expression);

	if (GiNaC::is_a<GiNaC::mul>(expression))
		return printProduct(expression);

	if (GiNaC::is_a<GiNaC::power>(expression))
		return printPower(expression);

	if (GiNaC::is_a<GiNaC::symbol>(expression)) {
		PrintedExpr result;
		result.text = GiNaC::ex_to<GiNaC::symbol>(expression).get_name();
		return result;
	}

	if (GiNaC::is_a<GiNaC::constant>(expression)) {
		PrintedExpr result;
		result.text = GiNaC::ex_to<GiNaC::constant>(expression).get_name();
		result.constant = true;
		return result;
	}

	if (GiNaC::is_a<GiNaC::function>(expression)) {
		const GiNaC::function & function
			= GiNaC::ex_to<GiNaC::function>(expression);

		PrintedExpr result;
		result.text = function.get_name() + '(';
		for (std::size_t i = 0; i < function.nops(); ++i) {
			if (i > 0)
				result.text += ',';
			result.text += printExpr(function.op(i)).text;
		}
		result.text += ')';
		result.function = true;
		return result;
	}

	PrintedExpr result;
	result.text = toString(expression);
	result.numeric = GiNaC::is_a<GiNaC::numeric>(expression);
	return result;
}

} // namespace

void printExpression(std::ostream & out, const GiNaC::ex & expression) {
	out << printExpr(expression).text;
}

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
	return internedSymbol(this->_name);
}

// ---------------------------------------------------------------------------
// Literal values (constants)
// ---------------------------------------------------------------------------

void LiteralNode::printLabel(std::ostream & out) const {
	out << this->_label;
}

std::vector<const ExpNode *> LiteralNode::children() const {
	return {};
}

GiNaC::ex LiteralNode::evaluate() const {
	return this->_value;
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

// ---------------------------------------------------------------------------
// Function calls
// ---------------------------------------------------------------------------

void CallNode::printLabel(std::ostream & out) const {
	out << this->_name;
}

std::vector<const ExpNode *> CallNode::children() const {
	std::vector<const ExpNode *> result;
	result.reserve(this->_args.size());
	for (std::size_t i = 0; i < this->_args.size(); ++i)
		result.push_back(this->_args[i].get());
	return result;
}

GiNaC::ex CallNode::evaluate() const {
	std::vector<GiNaC::ex> args;
	args.reserve(this->_args.size());
	for (std::size_t i = 0; i < this->_args.size(); ++i)
		args.push_back(this->_args[i]->evaluate());

	if (this->_name == "sqrt")
		return GiNaC::sqrt(args[0]);
	if (this->_name == "cbrt")
		return GiNaC::pow(args[0], GiNaC::numeric(1, 3));
	if (this->_name == "exp")
		return GiNaC::exp(args[0]);

	if (this->_name == "log" || this->_name == "ln")
		return GiNaC::log(args[0]);
	if (this->_name == "log10")
		return GiNaC::log(args[0]) / GiNaC::log(GiNaC::numeric(10));
	if (this->_name == "log2")
		return GiNaC::log(args[0]) / GiNaC::log(GiNaC::numeric(2));

	if (this->_name == "sin")
		return GiNaC::sin(args[0]);
	if (this->_name == "cos")
		return GiNaC::cos(args[0]);
	if (this->_name == "tan")
		return GiNaC::tan(args[0]);

	if (this->_name == "asin")
		return GiNaC::asin(args[0]);
	if (this->_name == "acos")
		return GiNaC::acos(args[0]);
	if (this->_name == "atan")
		return GiNaC::atan(args[0]);

	if (this->_name == "sinh")
		return GiNaC::sinh(args[0]);
	if (this->_name == "cosh")
		return GiNaC::cosh(args[0]);
	if (this->_name == "tanh")
		return GiNaC::tanh(args[0]);

	if (this->_name == "asinh")
		return GiNaC::asinh(args[0]);
	if (this->_name == "acosh")
		return GiNaC::acosh(args[0]);
	if (this->_name == "atanh")
		return GiNaC::atanh(args[0]);

	if (this->_name == "abs")
		return GiNaC::abs(args[0]);
	if (this->_name == "floor")
		return GiNaC::numeric(std::floor(
			GiNaC::ex_to<GiNaC::numeric>(args[0]).to_double()));
	if (this->_name == "ceil")
		return GiNaC::numeric(std::ceil(
			GiNaC::ex_to<GiNaC::numeric>(args[0]).to_double()));
	if (this->_name == "round")
		return GiNaC::numeric(std::round(
			GiNaC::ex_to<GiNaC::numeric>(args[0]).to_double()));

	if (this->_name == "gcd")
		return GiNaC::gcd(args[0], args[1]);
	if (this->_name == "lcm")
		return GiNaC::lcm(args[0], args[1]);
	if (this->_name == "ncr")
		return GiNaC::binomial(args[0], args[1]);
	if (this->_name == "npr")
		return GiNaC::factorial(args[0])
			/ GiNaC::factorial(args[0] - args[1]);

	throw std::runtime_error("unknown function: " + this->_name);
}
