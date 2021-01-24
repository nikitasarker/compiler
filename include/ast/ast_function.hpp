#ifndef AST_FUNCTION_HPP
#define AST_FUNCTION_HPP

#include "ast_program.hpp"

#include <string>
#include <vector>
#include <string>
#include <iostream>

class Function : public Program {
private:
	std::string id;
	ProgramPtr params;
	ProgramPtr comp_statement;
public:
	std::vector<std::string> funcVariableList; //variables that are used within the function sequence
	std::vector<std::string> funcParamsList;//variables that are in the function parameter list
	std::vector<std::string> declaredVariablesList;//variables that are declared within the function sequence
	std::vector<std::string> globalVars;
	Function(const std::string& _id, ProgramPtr _params, ProgramPtr _comp_statement) :
		id(_id), params(_params), comp_statement(_comp_statement) {
		comp_statement->getFunctionVars(funcVariableList);
		comp_statement->getDeclaredVars(declaredVariablesList);

		params->getFunctionVars(funcParamsList);

		//int depth = 0;
		//comp_statement->getDepth(depth); //gets the maximum depth of any expression in the function and stores it in depth
		//just a number or variable returns a depth of 1, x+y returns depth of 2, z=x+y returns depth of 3...


		if (id == "main") {
			setMainPresent();
		}
	}

	virtual void print(std::ostream& dst) const {

		dst << "def " << id << "(";
		params->print(dst);
		dst << ")" << ":" << std::endl;
		indentIncr();
		for (int i = 0; i < globalVars.size(); i++) {
			for (int j = 0; j < getIndent(); j++) {
				dst << "	";
			}
			dst << "global " << globalVars[i] << std::endl;
		}
		comp_statement->print(dst);
		indentDecr();
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		context.declVariableList = declaredVariablesList; //sets local variables within the context of the scope

		Context original_context = context;
		int numOfParams = funcParamsList.size();
		int numOfDeclaredVars = declaredVariablesList.size();
		int depth = 0;
		comp_statement->getDepth(depth);
		int req_memory_size = 4*(numOfParams+numOfDeclaredVars+depth+2+context.function_call_memory);
		context.stack_offset = req_memory_size-8;
		context.memory_required = req_memory_size;

		dst<<"\t.text"<<std::endl;
		dst<<"\t.align 2"<<std::endl;
		dst << "\t.global " << id << std::endl;
		dst << id << ":" << std::endl;

		dst<<"\taddiu"<<"\t$sp,$sp,-"<<req_memory_size<<std::endl; //pointing $sp at end of new stack
		dst<<"\tsw"<<"\t$ra,"<<req_memory_size-4<<"($sp)"<<std::endl; //storing current return address into new stack
		dst<<"\tsw"<<"\t$fp,"<<req_memory_size-8<<"($sp)"<<std::endl; //storing current $fp into new stack
		dst<<"\tmove"<<"\t$fp,$sp"<<std::endl; //pointing frame pointer to new stack, WE ARE IN NEW STACK

		printMipsArgs(dst, context, funcParamsList);

		comp_statement->printMIPS(dst, context);

		dst<<"\tmove"<<"\t$sp,$fp"<<std::endl; //pointing $sp to end of stack
		dst<<"\tlw"<<"\t$ra,"<<req_memory_size-4<<"($sp)"<<std::endl; //loading previous return address into $ra
		dst<<"\tlw"<<"\t$fp,"<<req_memory_size-8<<"($sp)"<<std::endl; //pointing $fp at previous stack
		dst<<"\tnop"<<std::endl;
		dst<<"\taddiu"<<"\t$sp,$sp,"<<req_memory_size<<std::endl; //pointing $sp at end of previous stack
		dst << "\tjr \t$ra" << std::endl; //jump to return address
		dst << "\tnop" << std::endl;
		context = original_context;
	}

	void printMipsArgs(std::ostream& dst, Context& context, std::vector<std::string> arg_list) const {
		for(int i=0; i<arg_list.size(); i++) {
			context.DecrementStackOffset();
			context.addBinding(arg_list[i], "int", context.stack_offset);
			dst<<"\tsw"<<"\t$a"<<i<<","<<context.stack_offset<<"($fp)"<<std::endl;
		}

	}
	virtual void push(ProgramPtr var) const {
		std::cerr << "Error: Can't call this function on this class" << std::endl;
	}
	virtual void setGlobals(std::vector<std::string>& allGlobals) {//thisis automatically called and set by programTop
		globalVars = allGlobals;
	}
};


class FunctionCall : public Program {
private:
	ProgramPtr id;
	ProgramPtr args;

public:
	FunctionCall(ProgramPtr _id, ProgramPtr _args) : id(_id), args(_args) {}

	void print(std::ostream& dst) const {
		id->print(dst);
		dst << "(";
		args->print(dst);
		dst << ")";

	}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		context.function_call_memory+=4;
		dst<<"\taddiu"<<"\t$sp,$sp,-"<<12<<std::endl;
		args->printMIPS(dst,context);
		dst << "\tjal ";
		id->print(dst);
		dst << std::endl; //jumping into new scope with clean registers
		dst<<"\tnop"<<std::endl;
		context.DecrementStackOffset();
		dst<<"\tsw\t$v0,"<<context.stack_offset<<"($fp)"<<std::endl;
	}

	virtual void getDepth(int& depthCounter) {
		args->getDepth(depthCounter);
	}
};

class ParamList : public Program {
private:
	mutable std::vector<ProgramPtr> param_list;

public:
	ParamList() {}

	ParamList(ProgramPtr param) {
		param_list.push_back(param);
	}

	void print(std::ostream& dst) const {
		for (size_t i = 0; i < param_list.size(); ++i) {
			if (i > 0 && i < param_list.size()) {
				dst << ", ";
			}
			param_list[i]->print(dst);
		}
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		for(int i = 0; i<param_list.size(); i++) {
			param_list[i]->printMIPS(dst, context);
		}
	}

	virtual void getFunctionVars(std::vector<std::string>& variables) {
		for (size_t i = 0; i < param_list.size(); ++i) {
			param_list[i]->getFunctionVars(variables);
		}
	}


	virtual void push(ProgramPtr _var) const {
		param_list.push_back(_var);
	}
};

#endif
