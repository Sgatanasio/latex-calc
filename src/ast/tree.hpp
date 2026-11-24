#ifndef TREE_HPP_
#define TREE_HPP_

#include <ginac/ginac.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

// Returns the same GiNaC symbol for the same name, so equal variables can
// be combined by GiNaC's simplifier.
GiNaC::ex internedSymbol(const std::string & name);

// Prints an evaluated expression using implicit multiplication: 3*x is shown
// as "3x" and x*y as "xy".
void printExpression(std::ostream & out, const GiNaC::ex & expression);

class ExpNode {
	public:
		virtual ~ExpNode() = default;

		// Prints the whole tree using Unicode box-drawing characters.
		void printAST() const;

		// Label shown for this node in printAST().
		virtual void printLabel(std::ostream & out) const = 0;

		// Direct children, in display order.
		virtual std::vector<const ExpNode *> children() const = 0;

		virtual GiNaC::ex evaluate() const = 0;
};

class NumNode: public ExpNode {
	private:
		double _value;
	public:
		explicit NumNode(double value): _value(value) {}

		void printLabel(std::ostream & out) const override;
		std::vector<const ExpNode *> children() const override;
		GiNaC::ex evaluate() const override;
};

class SymNode: public ExpNode {
	private:
		std::string _name;
	public:
		explicit SymNode(std::string name): _name(std::move(name)) {}

		void printLabel(std::ostream & out) const override;
		std::vector<const ExpNode *> children() const override;
		GiNaC::ex evaluate() const override;
};

// Base class for nodes with a single operand.
class UnaryOpNode: public ExpNode {
	protected:
		std::unique_ptr<ExpNode> _arg;
		std::string _label;
	public:
		UnaryOpNode(std::unique_ptr<ExpNode> arg, std::string label)
			: _arg(std::move(arg)), _label(std::move(label)) {}

		void printLabel(std::ostream & out) const override;
		std::vector<const ExpNode *> children() const override;
};

// Base class for nodes with two operands.
class BinaryOpNode: public ExpNode {
	protected:
		std::unique_ptr<ExpNode> _left;
		std::unique_ptr<ExpNode> _right;
		std::string _label;
	public:
		BinaryOpNode(std::unique_ptr<ExpNode> left, std::unique_ptr<ExpNode> right,
		             std::string label)
			: _left(std::move(left)), _right(std::move(right)), _label(std::move(label)) {}

		void printLabel(std::ostream & out) const override;
		std::vector<const ExpNode *> children() const override;
};

// ---------------------------------------------------------------------------
// Binary operators (from operators[] in table/init.hpp)
// ---------------------------------------------------------------------------

// "+"
class PlusNode: public BinaryOpNode {
	public:
		PlusNode(std::unique_ptr<ExpNode> left, std::unique_ptr<ExpNode> right)
			: BinaryOpNode(std::move(left), std::move(right), "+") {}

		GiNaC::ex evaluate() const override;
};

// "-"
class MinusNode: public BinaryOpNode {
	public:
		MinusNode(std::unique_ptr<ExpNode> left, std::unique_ptr<ExpNode> right)
			: BinaryOpNode(std::move(left), std::move(right), "-") {}

		GiNaC::ex evaluate() const override;
};

// "*"
class TimesNode: public BinaryOpNode {
	public:
		TimesNode(std::unique_ptr<ExpNode> left, std::unique_ptr<ExpNode> right)
			: BinaryOpNode(std::move(left), std::move(right), "*") {}

		GiNaC::ex evaluate() const override;
};

// "/"
class DivNode: public BinaryOpNode {
	public:
		DivNode(std::unique_ptr<ExpNode> left, std::unique_ptr<ExpNode> right)
			: BinaryOpNode(std::move(left), std::move(right), "/") {}

		GiNaC::ex evaluate() const override;
};

// "^"
class PowerNode: public BinaryOpNode {
	public:
		PowerNode(std::unique_ptr<ExpNode> left, std::unique_ptr<ExpNode> right)
			: BinaryOpNode(std::move(left), std::move(right), "^") {}

		GiNaC::ex evaluate() const override;
};

// "\mod"
class ModNode: public BinaryOpNode {
	public:
		ModNode(std::unique_ptr<ExpNode> left, std::unique_ptr<ExpNode> right)
			: BinaryOpNode(std::move(left), std::move(right), "\\mod") {}

		GiNaC::ex evaluate() const override;
};

// ---------------------------------------------------------------------------
// Unary operators
// ---------------------------------------------------------------------------

// "%" (postfix percent: x% == x / 100)
class PercentNode: public UnaryOpNode {
	public:
		explicit PercentNode(std::unique_ptr<ExpNode> arg)
			: UnaryOpNode(std::move(arg), "%") {}

		GiNaC::ex evaluate() const override;
};

// "!" (postfix factorial)
class FactorialNode: public UnaryOpNode {
	public:
		explicit FactorialNode(std::unique_ptr<ExpNode> arg)
			: UnaryOpNode(std::move(arg), "!") {}

		GiNaC::ex evaluate() const override;
};

// Prefix unary "-"
class UnaryMinusNode: public UnaryOpNode {
	public:
		explicit UnaryMinusNode(std::unique_ptr<ExpNode> arg)
			: UnaryOpNode(std::move(arg), "-") {}

		GiNaC::ex evaluate() const override;
};

// Prefix unary "+"
class UnaryPlusNode: public UnaryOpNode {
	public:
		explicit UnaryPlusNode(std::unique_ptr<ExpNode> arg)
			: UnaryOpNode(std::move(arg), "+") {}

		GiNaC::ex evaluate() const override;
};

// ---------------------------------------------------------------------------
// Literal values (constants)
// ---------------------------------------------------------------------------

// Leaf used for constants and other literal values.
class LiteralNode: public ExpNode {
	private:
		std::string _label;
		GiNaC::ex _value;
	public:
		LiteralNode(std::string label, const GiNaC::ex & value)
			: _label(std::move(label)), _value(value) {}

		void printLabel(std::ostream & out) const override;
		std::vector<const ExpNode *> children() const override;
		GiNaC::ex evaluate() const override;
};

// ---------------------------------------------------------------------------
// Function calls
// ---------------------------------------------------------------------------

// Function application with one or two arguments.
class CallNode: public ExpNode {
	private:
		std::string _name;
		std::vector<std::unique_ptr<ExpNode> > _args;
	public:
		CallNode(std::string name, std::vector<std::unique_ptr<ExpNode> > args)
			: _name(std::move(name)), _args(std::move(args)) {}

		CallNode(std::string name, std::unique_ptr<ExpNode> arg)
			: _name(std::move(name)) {
			this->_args.push_back(std::move(arg));
		}

		CallNode(std::string name, std::unique_ptr<ExpNode> left,
		         std::unique_ptr<ExpNode> right)
			: _name(std::move(name)) {
			this->_args.push_back(std::move(left));
			this->_args.push_back(std::move(right));
		}

		void printLabel(std::ostream & out) const override;
		std::vector<const ExpNode *> children() const override;
		GiNaC::ex evaluate() const override;
};

// Placeholder for a future program-level AST container.
class AST {
};

#endif
