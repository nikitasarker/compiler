#ifndef ast_declaration_hpp
#define ast_declaration_hpp

#include "ast_program.hpp"

#include <string>
#include <iostream>
#include <cmath>
#include <vector>


class DeclarationList : public Program {
private:
	mutable std::vector<ProgramPtr> dec_list;

public:
	DeclarationList(ProgramPtr _el) {
		dec_list.push_back(_el);
	}
	DeclarationList() {}
	DeclarationList(ProgramPtr _id, ProgramPtr _contents) {}

	virtual void getFunctionVars(std::vector<std::string>& variables) {
		for (int i = 0; i < dec_list.size(); i++) {
			dec_list[i]->getFunctionVars(variables);
		}
	}

	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {
		for (size_t i = 0; i < dec_list.size(); ++i) {
			dec_list[i]->getDeclaredVars(declaredVariables);
		}
	}

	virtual void print(std::ostream& dst) const {
		for (size_t i = 0; i < dec_list.size(); ++i) {
			dec_list[i]->print(dst);
		}
	}

	virtual void printMIPS(std::ostream& dst,  Context& context) const {
		for (size_t i = 0; i < dec_list.size(); ++i) {
			dec_list[i]->printMIPS(dst,context);
		}
	}


	virtual void push(ProgramPtr _var) const {
		dec_list.push_back(_var);
	}

	virtual void getDepth(int& depthCounter) {
		int tmpDepth;
		int maxDepth = 0;
		for (size_t i = 0; i < dec_list.size(); ++i) {
			tmpDepth = 0;
			dec_list[i]->getDepth(tmpDepth);
			if (tmpDepth > maxDepth) {
				maxDepth = tmpDepth;
			}
		}
		depthCounter = depthCounter + maxDepth;
	}

	virtual void setInitializer(ProgramPtr _assign_expr){}


};

class initDeclarationList : public DeclarationList {
private:
	mutable std::vector<ProgramPtr> init_dec_list;

public:
	initDeclarationList() {}

	initDeclarationList(ProgramPtr _el) {
		init_dec_list.push_back(_el);
	}

	virtual void getFunctionVars(std::vector<std::string>& variables) {
		for (int i = 0; i < init_dec_list.size(); i++) {
			init_dec_list[i]->getFunctionVars(variables);
		}
	}

	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {
		for (size_t i = 0; i < init_dec_list.size(); ++i) {
			init_dec_list[i]->getDeclaredVars(declaredVariables);
		}
	}

	virtual void getGlobals(std::vector<std::string>& globals) {
		for (size_t i = 0; i < init_dec_list.size(); ++i) {
			init_dec_list[i]->getGlobals(globals);
		}
	}
	virtual void print(std::ostream& dst) const {
		for (size_t i = 0; i < init_dec_list.size(); ++i) {
			for (int j = 0; j < getIndent(); j++) {
				dst << "	";
			}
			init_dec_list[i]->print(dst);
			dst << "\n";
		}
	}

	virtual void printMIPS(std::ostream& dst,  Context& context) const {
		for (size_t i = 0; i < init_dec_list.size(); ++i) {
			init_dec_list[i]->printMIPS(dst,context);
		}
	}

	virtual void getDepth(int& depthCounter) {
		int tmpDepth;
		int maxDepth = 0;
		for (size_t i = 0; i < init_dec_list.size(); ++i) {
			tmpDepth = 0;
			init_dec_list[i]->getDepth(tmpDepth);
			if (tmpDepth > maxDepth) {
				maxDepth = tmpDepth;
			}
		}
		depthCounter = depthCounter+tmpDepth;
	}

	virtual void push(ProgramPtr _var) const {
		init_dec_list.push_back(_var);
	}

};

class IdentifierDeclaration : public DeclarationList {
private:
	std::string id;
	ProgramPtr assign_expr;
	ProgramPtr params;
	bool funcDec = false;
public:
	IdentifierDeclaration(const std::string& _id) : id(_id), assign_expr(NULL),
	params(NULL){}

	void setInitializer(ProgramPtr _assign_expr) {
		assign_expr = _assign_expr;
	}

	void addParams(ProgramPtr _params) {
		params = _params;
	}

	void addParams() {}

	std::string getID() {
		return id;
	}
	virtual void getId(std::string& getid) const
	{
		getid = id;
	}

	ProgramPtr getParams() {
		return params;
	}

	void setFuncDec() {
		funcDec = true;
	}

	void print(std::ostream& dst) const {
		dst << id << "=";
		if (assign_expr == NULL) {
			dst << "0";
		}
		else {
			assign_expr->print(dst);
		}
		dst << std::endl;
	}

	virtual void printMIPS(std::ostream& dst,  Context& context) const {
		if (!funcDec) {
			if(assign_expr==NULL) {
				context.DecrementStackOffset();
				dst<<"\tsw\t$zero,"<<context.stack_offset<<"($fp)"<<std::endl;
			}
			else{
				assign_expr->printMIPS(dst, context);
			}
			context.addBinding(id, "int", context.stack_offset);
		}
	}

	virtual void getFunctionVars(std::vector<std::string>& variables) {
		bool alreadyInVariables = false;
		if (variables.size() == 0) {
			variables.push_back(id);

		}
		else {
			for (int i = 0; i < variables.size(); i++) {
				if (id == variables[i]) {
					alreadyInVariables = true;
				}
			}
			if (!alreadyInVariables) {
				variables.push_back(id);
			}
		}
	}
	virtual void getDepth(int& depthCounter) {
		depthCounter++; //accounts for = operator which is left out by assign_expr
		if (assign_expr != NULL) {
			assign_expr->getDepth(depthCounter);
		}
	}

	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {

		bool alreadyInDeclVariables = false;
		if (declaredVariables.size() == 0) {
			declaredVariables.push_back(id);

		}
		else {
			for (int i = 0; i < declaredVariables.size(); i++) {
				if (id == declaredVariables[i]) {
					alreadyInDeclVariables = true;
				}
			}
			if (!alreadyInDeclVariables) {
				declaredVariables.push_back(id);
			}
		}

	}

	virtual void getGlobals(std::vector<std::string>& globals) {

		bool alreadyInGlobalVariables = false;
		if (globals.size() == 0) {
			globals.push_back(id);

		}
		else {
			for (int i = 0; i < globals.size(); i++) {
				if (id == globals[i]) {
					alreadyInGlobalVariables = true;
				}
			}
			if (!alreadyInGlobalVariables) {
				globals.push_back(id);
			}
		}

	}
};

class ArgExprList : public Program {
private:
	mutable std::vector<ProgramPtr> arg_list;

public:
	ArgExprList(ProgramPtr _el) {
		arg_list.push_back(_el);
	}

	virtual void print(std::ostream& dst) const {
		for (size_t i = 0; i < arg_list.size(); ++i) {
			if (i > 0 && i < arg_list.size()) {
				dst << ", ";
			}
			arg_list[i]->print(dst);

		}
	}

	virtual void printMIPS(std::ostream& dst,  Context& context) const {
		if(arg_list.size()>4) {
			//freak out
		}
		else{
			for(int i=0; i<arg_list.size(); i++) {
				arg_list[i]->printMIPS(dst, context);
				dst<<"\tlw\t$a"<<i<<","<<context.stack_offset<<"($fp)"<<std::endl;
				dst<<"\tnop"<<std::endl;
				context.IncrementStackOffset();
			}
		}
	}

	virtual void getDepth(int& depthCounter) {
		int tmpDepth;
		int maxDepth = 0;
		for (size_t i = 0; i < arg_list.size(); ++i) {
			tmpDepth = 0;
			arg_list[i]->getDepth(tmpDepth);
			if (tmpDepth > maxDepth) {
				maxDepth = tmpDepth;
			}
		}
		depthCounter = depthCounter + maxDepth;
	}
	virtual void push(ProgramPtr _var) const {
		arg_list.push_back(_var);
	}
};

class Array : public Program {
private:
	std::string id;
	int array_size;
public:
	Array(ProgramPtr _id) {
		_id->getId(id);
	}
	Array(ProgramPtr _id, ProgramPtr _contents) {
		_id->getId(id);
		array_size = _contents->getValue();
	}
	virtual void printMIPS(std::ostream& dst, Context& context) const {
		context.DecrementStackOffset();
		
		context.addBinding(id, "array", context.stack_offset, array_size);
		dst << "\tsw\t$zero," << context.stack_offset << "($fp)" << std::endl;
		for (int i = 0; i < array_size - 1; i++) {
			context.DecrementStackOffset(); //ends last loop with context.stackoffset on final element of array in stack
			dst << "\tsw\t$zero," << context.stack_offset << "($fp)" << std::endl; //set each value to 0
		}
	}
	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {
		bool alreadyInDeclVariables = false;
		if (declaredVariables.size() == 0) {
			declaredVariables.push_back(id);
		}
		else {
			for (int i = 0; i < declaredVariables.size(); i++) {
				if (id == declaredVariables[i]) {
					alreadyInDeclVariables = true;
				}
			}
			if (!alreadyInDeclVariables) {
				declaredVariables.push_back(id);
			}
		}
	}
	virtual void getGlobals(std::vector<std::string>& globals) {

		bool alreadyInGlobalVariables = false;
		if (globals.size() == 0) {
			globals.push_back(id);

		}
		else {
			for (int i = 0; i < globals.size(); i++) {
				if (id == globals[i]) {
					alreadyInGlobalVariables = true;
				}
			}
			if (!alreadyInGlobalVariables) {
				globals.push_back(id);
			}
		}

	}
	virtual void getDepth(int& depthCounter) {
		depthCounter = depthCounter + array_size;
	}
	
};

class InitializerList : public Program {
private:
	mutable std::vector<ProgramPtr> init_list;


public:
	InitializerList(ProgramPtr _el) {
		init_list.push_back(_el);
	}

	virtual void print(std::ostream& dst) const {
		for(int i; init_list.size(); i++) {
			init_list[i]->print(dst);
		}
	}

	virtual void printMIPS(std::ostream& dst,  Context& context) const {}

	virtual void push(ProgramPtr _var) const {
		init_list.push_back(_var);
	}
	virtual void getDepth(int& depthCounter) {

		int tmpDepth;
		int maxDepth = 0;
		for (size_t i = 0; i < init_list.size(); ++i) {
			tmpDepth = 0;
			init_list[i]->getDepth(tmpDepth);
			if (tmpDepth > maxDepth) {
				maxDepth = tmpDepth;
			}
		}
		depthCounter = depthCounter + maxDepth;
	}
	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {
		for (size_t i = 0; i < init_list.size(); ++i) {
			init_list[i]->getDeclaredVars(declaredVariables);
		}
	}
};

class Initializer : public Program {
private:
	ProgramPtr initializer;

public:
	Initializer(ProgramPtr _initializer) : initializer(_initializer) {}

	virtual void print(std::ostream& dst) const {
		initializer->print(dst);
	}
	virtual void printMIPS(std::ostream& dst,  Context& context) const {
		initializer->printMIPS(dst, context);
	}

	virtual void getDepth(int& depthCounter) {

		initializer->getDepth(depthCounter);
	}
};

class FunctionDeclaration: public Program {
public:
	FunctionDeclaration(){}
};

#endif
