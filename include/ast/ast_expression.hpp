#ifndef ast_expression_hpp
#define ast_expression_hpp

#include "ast_program.hpp"

#include <string>
#include <iostream>


class Expression
	: public Program
{
private:


public:
	Expression() {} //constructor

	virtual void print(std::ostream& dst) const {}

	virtual void printMIPS(std::ostream& dst,Context& context) const {}

	// virtual void getDepth(int& depthCounter) {
	// 	depthCounter++;
	// 	int LDepth = depthCounter;
	// 	int RDepth = depthCounter;
	// 	left->getDepth(LDepth);
	// 	right->getDepth(RDepth);
	// 	if (LDepth > RDepth) {
	// 		depthCounter=LDepth;
	// 	}
	// 	else {
	// 		depthCounter=RDepth;
	// 	}
	// }

};

class  AssignExpression : public Expression {
private:
	ProgramPtr left;
	ProgramPtr right;


public:
	virtual void print(std::ostream& dst) const {
		left->print(dst);
		dst << "=";
		right->print(dst);
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		std::string id;
		left->getId(id);
		
		int lhs_stack_offset = left->getStackPosition(context);
		
		if (context.bindings[id].type == "array") { //arrays
			
			left->calculateIndex(dst, context);//gets index expression answer into where stack is pointing
			dst << "\tlw\t$t6, " << context.stack_offset << "($fp)" << std::endl; //load value of index expr into $v0
			context.IncrementStackOffset();
			dst << "\tsll\t$t6,$t6,2" << std::endl; //multiply index by 4 to get correct memory
			//dst << "\tmul\t$t6,$t6,-1" << std::endl; //multiply index by -1 to get correct memory
			dst << "\tli\t$t0, " << lhs_stack_offset << std::endl; //load offset of array[0] into $t0
			dst << "\tsub\t$t0,$t0,$t6" << std::endl; //get correct offset from fp
			dst << "\tadd\t$t6,$t0,$fp" << std::endl; //add index+array[0] offset to frame pointer (gives position of indexed element)
			right->printMIPS(dst, context);
			dst << "\tlw\t$v0," << context.stack_offset << "($fp)" << std::endl;//load rhs answer into $v0
			context.IncrementStackOffset();
			dst << "\tnop" << std::endl;
			context.DecrementStackOffset();
			dst << "\tsw\t$v0,0($t6)" << std::endl;
		}
		else { //everything else
			right->printMIPS(dst, context);
			dst << "\tlw\t$v0," << context.stack_offset << "($fp)" << std::endl;
			context.IncrementStackOffset();
			dst << "\tnop" << std::endl;
			dst << "\tsw\t$v0," << lhs_stack_offset << "($fp)" << std::endl;
		}
	}

	AssignExpression(ProgramPtr _left, ProgramPtr _right)
		: left(_left), right(_right) {}

	virtual void getDepth(int& depthCounter) {
		depthCounter++;
		int LDepth = depthCounter;
		int RDepth = depthCounter;
		left->getDepth(LDepth);
		right->getDepth(RDepth);
		if (LDepth > RDepth) {
			depthCounter = LDepth;
		}
		else {
			depthCounter = RDepth;
		}
	}
};


class  AndExpression : public Expression {
private:
	ProgramPtr left;
	ProgramPtr right;

public:
	virtual void print(std::ostream& dst) const {
		left->print(dst);
		dst<<"and";
		right->print(dst);
	}

	virtual void printMIPS(std::ostream& dst,Context& context) const {
		context.scope_counter++;
		std::string labelA = context.makeScopeLabel("labelA");
		std::string labelB = context.makeScopeLabel("labelB");

		left->printMIPS(dst,context);
		dst<<"\tbeq\t$v0,$zero, "<<labelA<<std::endl;
		dst<<"\tnop"<<std::endl;

		right->printMIPS(dst,context);
		dst<<"\tbeq\t$v0,$zero, "<<labelA<<std::endl;
		dst<<"\tnop"<<std::endl;
		dst<<"\tli\t$v0,1"<<std::endl;
		dst<<"\tb\t"<<labelB<<std::endl;
		dst<<"\tnop"<<std::endl;

		dst<<labelA<<":"<<std::endl;
		dst<<"\tmove\t$v0,$zero"<<std::endl;
		dst<<labelB<<":"<<std::endl;

		context.DecrementStackOffset();
		dst<<"\tsw\t$v0,"<<context.stack_offset<<"($fp)"<<std::endl;

	}

	AndExpression(ProgramPtr _left, ProgramPtr _right)
		: left(_left), right(_right){}

};

class  OrExpression : public Expression {
private:
	ProgramPtr left;
	ProgramPtr right;

public:
	virtual void print(std::ostream& dst) const {
		left->print(dst);
		dst<<"or";
		right->print(dst);
	}

	virtual void printMIPS(std::ostream& dst,Context& context) const {
		context.scope_counter++;
		std::string labelA = context.makeScopeLabel("labelA");
		std::string labelB = context.makeScopeLabel("labelB");
		std::string labelC = context.makeScopeLabel("labelC");

		left->printMIPS(dst,context);
		dst<<"\tbne\t$v0,$zero, "<<labelB<<std::endl;
		dst<<"\tnop"<<std::endl;

		right->printMIPS(dst,context);
		dst<<"\tbeq\t$v0,$zero, "<<labelA<<std::endl;
		dst<<"\tnop"<<std::endl;
		dst<<labelB<<":"<<std::endl;
		dst<<"\tli\t$v0,1"<<std::endl;
		dst<<"\tb\t"<<labelC<<std::endl;
		dst<<"\tnop"<<std::endl;

		dst<<labelA<<":"<<std::endl;
		dst<<"\tmove\t$v0,$zero"<<std::endl;
		dst<<labelC<<":"<<std::endl;

		context.DecrementStackOffset();
		dst<<"\tsw\t$v0,"<<context.stack_offset<<"($fp)"<<std::endl;
	}

	OrExpression(ProgramPtr _left, ProgramPtr _right)
		: left(_left), right(_right){}

};

class UnaryOperatorExpression : public Program {
private:
	std::string op;
	ProgramPtr CastExpr;

public:
	UnaryOperatorExpression(std::string _op, ProgramPtr _CastExpr): op(_op),
	CastExpr(_CastExpr) {}

	virtual void print(std::ostream& dst) const {
		CastExpr->print(dst);
	}
	virtual void printMIPS(std::ostream& dst, Context& context) const {
		if(op=="!") {
			CastExpr->printMIPS(dst, context);
			dst<<"\tsltu\t$v0,$v0,1"<<std::endl;
			dst<<"\taddi\t$v0,$v0,0x00ff"<<std::endl;
		}
		else {}

		dst<<"\tsw\t$v0,"<<context.stack_offset<<"($fp)"<<std::endl;
		context.DecrementStackOffset();
	}
};
#endif
