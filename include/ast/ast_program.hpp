#ifndef ast_program_hpp
#define ast_program_hpp

#include "context.hpp"

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

class Program;

typedef Program *ProgramPtr;

class Program
{

public:
	static int indent; //level of scope (how many indents we are currently at - incremented and used by printIndent function)
	virtual ~Program() {
	}
	int getIndent() const {
		return indent;
	}
	void InitIndent() {
		indent = 0;
	}
	void indentIncr() const {
		ProgramPtr ptr = const_cast<ProgramPtr> (this); //workaround to allow us to change indent within the const function
		ptr->indent++;
	};
	void indentDecr() const {
		ProgramPtr ptr = const_cast<ProgramPtr> (this); //workaround to allow us to change indent within the const function
		ptr->indent--;
	};
	static bool mainPresent; //detects whether main is present in program (set within ast_function.hpp)
	void setMainPresent() {
		mainPresent = true;
	}

	virtual void getId(std::string &id) const {}; //used for getting string id from t_id ProgramPtr

	//! Tell and program to print itself to the given stream
	virtual void print(std::ostream& dst) const {};
	virtual void printMIPS(std::ostream& dst, Context& context) const {};
	virtual void printIndent(std::ostream& dst) {};
	virtual void calculateIndex(std::ostream& dst,Context &context) {};


	virtual void getFunctionVars(std::vector<std::string>& variables) {};
	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {};
	virtual void getGlobals(std::vector<std::string>& globals) {};
	virtual void setGlobals(std::vector<std::string>& allGlobals) {};
	virtual void getDepth(int &depthCounter) {}; //initialise depthCounter to 0 when using
	virtual int getStackPosition(Context context) {}; //get stack_offset

	virtual void push(ProgramPtr _stat) const {};

	virtual void setInitializer(ProgramPtr _assign_expr){}
	virtual void addParams(ProgramPtr _params) {}
	virtual void addParams() {}
	virtual std::string getID() {return "";}
	virtual ProgramPtr getParams() {}
	virtual std::string getType() {return "";}
	virtual void setFuncDec() {}
	virtual void setIndividualFalse() {}
	double getValue(){};

};


#endif
