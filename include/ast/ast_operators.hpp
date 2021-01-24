#ifndef ast_operators_hpp
#define ast_operators_hpp

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

class Operator
    : public Program
{
protected:
    ProgramPtr left;
    ProgramPtr right;

    Operator(ProgramPtr _left, ProgramPtr _right)
        : left(_left)
        , right(_right)
    {}
public:
    virtual const char *getOpcode() const {return "operator not supported in python translator";};
	  virtual void getMIPS(std::ostream& dst, Context& context) const =0;

    ProgramPtr getLeft() const
    { return left; }

    ProgramPtr getRight() const
    { return right; }

    virtual void print(std::ostream &dst) const override {
		left->print(dst);
        dst<<" ";
        dst<<getOpcode();
        dst<<" ";
        right->print(dst);
		dst << " ";
    }

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		left->printMIPS(dst,context);
		//int lhs_stack_offset = context.stack_offset;
		right->printMIPS(dst,context);
		//int rhs_stack_offset = context.stack_offset;

		dst<<"\tlw"<<"\t $t1,"<<context.stack_offset<<"($fp)"<<std::endl;
		context.IncrementStackOffset();
		dst<<"\tlw"<<"\t $t0,"<<context.stack_offset<<"($fp)"<<std::endl;
		context.IncrementStackOffset();
		getMIPS(dst, context);
		//context.stack_offset = lhs_stack_offset;
		context.DecrementStackOffset();
		dst<<"\tsw"<<"\t $v0,"<<context.stack_offset<<"($fp)"<<std::endl;
		dst<<"\tnop"<<std::endl;
	}

	virtual void getFunctionVars(std::vector<std::string> &variables) {
		left->getFunctionVars(variables);
		right->getFunctionVars(variables);
	}

	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {
		left->getDeclaredVars(declaredVariables);
	}
	virtual void getDepth(int& depthCounter) {
		depthCounter++;
		int LDepth = depthCounter;
		int RDepth = depthCounter;
		left->getDepth(LDepth);
		right->getDepth(RDepth);
		if (LDepth > RDepth) {
			depthCounter=LDepth;
		}
		else {
			depthCounter=RDepth;
		}
	}
    void push(ProgramPtr _stat) {
          std::cerr << "this shouldn't happen" <<std::endl;
    }
};

class AddOperator: public Operator{
	protected:
		virtual const char *getOpcode() const override
		{ return "+"; }
		virtual void getMIPS(std::ostream& dst, Context& context) const
		{
			dst <<"\tadd\t$v0, $t0, $t1"<<std::endl;
		}
	public:

		AddOperator(ProgramPtr _left, ProgramPtr _right)

        : Operator(_left, _right)
		 {}

};

class SubOperator: public Operator{
	protected:
		virtual const char *getOpcode() const override
		{ return "-"; }
		virtual void getMIPS(std::ostream& dst, Context& context) const override
		{
			dst <<"\tsub\t$v0, $t0, $t1"<<std::endl;
		}
	public:

		SubOperator(ProgramPtr _left, ProgramPtr _right): Operator(_left, _right)

		{}

};


class MulOperator
    : public Operator
{
protected:
    virtual const char *getOpcode() const override
    { return "*"; }
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tmult\t$t0, $t1"<<std::endl;
		dst <<"\tmflo\t$v0"<<std::endl;
	}
public:
    MulOperator(ProgramPtr _left, ProgramPtr _right)
        : Operator(_left, _right)
    {}

};

class DivOperator
	: public Operator
{
protected:
	virtual const char* getOpcode() const override
	{
		return "/";
	}
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tdiv\t$t0, $t1"<<std::endl;
		dst<<"\tmflo\t$v0"<<std::endl;
	}
public:
	DivOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)
	{}

};

class ModOperator	: public Operator {
protected:
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tdiv\t$t0, $t1"<<std::endl;
		dst<<"\tmfhi\t$v0"<<std::endl;
	}
public:
	ModOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)
	{}

};

class  LeftShiftOperator : public Operator {
protected:
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tsll\t$v0, $t0, $t1"<<std::endl;
	}
public:

	LeftShiftOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)	{}
};

class  RightShiftOperator : public Operator {
protected:
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tsra\t$v0, $t0, $t1"<<std::endl;
	}
public:

	RightShiftOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)	{}
};


class  LTOperator : public Operator {
protected:
	virtual const char* getOpcode() const override
	{
		return "<";
	}
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tslt\t$v0, $t0, $t1"<<std::endl;
	}
public:

	LTOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right){}
};

class  MTOperator : public Operator {
protected:
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tslt\t$v0, $t1, $t0"<<std::endl;
	}
public:

	MTOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)	{}
};

class  LesserThanEqualOperator : public Operator {
protected:
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tslt\t$v0, $t1, $t0"<<std::endl;
		dst <<"\taddi\t$t2,$zero,1"<<std::endl;
		dst <<"\tslt\t$v0,$v0,$t2"<<std::endl;
	}
public:
	LesserThanEqualOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)	{}
};

class  GreaterThanEqualOperator : public Operator {
protected:
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst <<"\tslt\t$v0, $t0, $t1"<<std::endl;
		dst <<"\taddi\t$t2,$zero,1"<<std::endl;
		dst <<"\tslt\t$v0,$v0,$t2"<<std::endl;
	}
public:
	GreaterThanEqualOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)	{}
};

class  NotEqualOperator : public Operator {
protected:
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		context.scope_counter++;
		std::string notEqualLabel = context.makeScopeLabel("notEqual");
		dst<<"\taddi\t$v0,$zero,0"<<std::endl;
    dst<<"\tbeq\t$t0,$t1,"<<notEqualLabel<<std::endl;
    dst<<"\tnop"<<std::endl;
    dst<<"\taddi \t$v0,$0,1"<<std::endl;
    dst<<notEqualLabel<<":"<<std::endl;
	}
public:
	NotEqualOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)	{}
};

class  EqOperator : public Operator {
protected:
	virtual const char* getOpcode() const override
	{
		return "==";
	}
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		context.scope_counter++;
		std::string equalLabel = context.makeScopeLabel("equal");

		dst<<"\taddi\t$v0,$zero,0"<<std::endl;
		dst<<"\tbne\t$t0,$t1,"<<equalLabel<<std::endl;
		dst<<"\tnop"<<std::endl;
		dst<<"\taddi \t$v0,$0,1"<<std::endl;
		dst<<equalLabel<<":"<<std::endl;
	}

public:
	EqOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right) {}
};

class  AndOperator : public Operator {
protected:
	virtual const char* getOpcode() const override
	{
		return "&";
	}
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst<<"\tand\t$v0,$t0,$t1"<<std::endl;
	}
public:
	AndOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)	{}
};

class  OrOperator : public Operator {
protected:
	virtual const char* getOpcode() const override
	{
		return "|";
	}
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst<<"\tor\t$v0,$t0,$t1"<<std::endl;
	}
public:

	OrOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)
	{}

};

class  XorOperator : public Operator {
protected:
	virtual const char* getOpcode() const override {}
	virtual void getMIPS(std::ostream& dst, Context& context) const override
	{
		dst<<"\txor\t$v0,$t0,$t1"<<std::endl;
	}
public:
	XorOperator(ProgramPtr _left, ProgramPtr _right)
		: Operator(_left, _right)	{}
};

#endif
