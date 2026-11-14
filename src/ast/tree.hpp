#ifndef TREE_HPP_
#define TREE_HPP_

#include <string>
#include <GiNaC>



class ExpNode {
	public:
		virtual void printAST() = 0;
		virtual GinacExp evaluate();
};

#endif
