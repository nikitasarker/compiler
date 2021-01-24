#ifndef ast_unary_hpp
#define ast_unary_hpp

#include <string>
#include <iostream>

class Unary
    : public Program
{
private:
    ProgramPtr expr;
protected:
    Unary(const ProgramPtr _expr)
        : expr(_expr)
    {}
public:
    virtual ~Unary()
    {
        delete expr;
    }

    virtual const char *getOpcode() const =0;

    ProgramPtr getExpr() const
    { return expr; }

    virtual void print(std::ostream &dst) const override
    {
        dst << "( ";
        dst << getOpcode();
        dst << " ";
        expr->print(dst);
        dst << " )";
    }

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		expr->printMIPS(dst,context);
		dst<<"\tsubu\t$v0,$zero,$v0"<<std::endl;
		dst<<"\tsw\t$v0,"<<context.stack_offset<<"($fp)"<<std::endl;
		context.DecrementStackOffset();
		}

    void push(ProgramPtr _stat) {
          std::cerr << "this shouldn't happen" <<std::endl;
    }

};

class NegOperator
    : public Unary
{
public:
    NegOperator(const ProgramPtr _expr)
        : Unary(_expr)
    {}

    virtual const char *getOpcode() const override
    { return "-"; }

};

#endif
