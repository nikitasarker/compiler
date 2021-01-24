#ifndef ast_array_hpp
#define ast_array_hpp

#include "ast_program.hpp"

#include <string>
#include <iostream>
#include <cmath>
#include <vector>

class PostfixArray : public Program {
private:
	std::string array_id;
	ProgramPtr index_expr;

public:
	PostfixArray(ProgramPtr _array_id, ProgramPtr  _index_expr): index_expr(_index_expr) {
		_array_id->getId(array_id);
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const{
		//int size = context.bindings[array_id].arraySize;
		int startofarray_stack_pos = context.bindings[array_id].stack_offset;
		
		index_expr->printMIPS(dst, context); //gets value of expr into top of stack, so we can load it into $v0?
		dst << "\tlw\t$v0, " <<context.stack_offset<<"($fp)" << std::endl; //load value of index expr into $v0
		context.IncrementStackOffset();
		dst << "\tnop" << std::endl;
		dst << "\tsll\t$v0,$v0,2" << std::endl; //multiply index by 4
		//dst << "\tmul\t$v0,$v0,-1" << std::endl; //multiply index by -1 to get correct memory
		dst << "\tli\t$t0, " << startofarray_stack_pos << std::endl; //load offset of array[0] into $t0
		dst << "\tsub\t$t0,$t0,$v0" << std::endl; //add calculated index to array[0] offset
		dst << "\tadd\t$t0,$t0,$fp" << std::endl; //add index+array[0] offset to frame pointer (gives position of indexed element)
		dst << "\tlw\t$v0, ($t0)"<< std::endl; //load value of indexed array element into $v0
		context.IncrementStackOffset();
		dst << "\tnop" << std::endl;
		context.DecrementStackOffset();
		dst << "\tsw\t$v0," << context.stack_offset << "($fp)" << std::endl;
		dst << "\tnop" << std::endl;
	}
	virtual int getStackPosition(Context context) {
		return context.bindings[array_id].stack_offset;
	}
	virtual void getId(std::string& id) const {
		id = array_id;
	}
	virtual void calculateIndex(std::ostream& dst, Context &context) {
		index_expr->printMIPS(dst, context);
	}
	
	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {
		bool alreadyInDeclVariables = false;
		if (declaredVariables.size() == 0) {
			declaredVariables.push_back(array_id);
		}
		else {
			for (int i = 0; i < declaredVariables.size(); i++) {
				if (array_id == declaredVariables[i]) {
					alreadyInDeclVariables = true;
				}
			}
			if (!alreadyInDeclVariables) {
				declaredVariables.push_back(array_id);
			}
		}
	}
	virtual void getDepth(int& depthCounter) {
		int assignDepth = depthCounter;
		index_expr->getDepth(assignDepth);
		depthCounter = depthCounter + assignDepth +1;
	}
};

class EnumElement: public Program {
private:
	std::string enum_el_id;
	ProgramPtr assign_expr;

public:
	EnumElement(const std::string& _id): enum_el_id(_id), assign_expr(NULL) {}
	EnumElement(const std::string& _id, ProgramPtr _ass_expr) : enum_el_id(_id), assign_expr(_ass_expr) {}

	virtual void printMIPS(std::ostream& dst, Context& context) {
	}
};

class EnumList : public Program {
private:
	mutable std::vector<ProgramPtr> enum_list;

public:
	EnumList(ProgramPtr  _enum) {
		enum_list.push_back(_enum);
	}

	void push(ProgramPtr _enum)  {
		enum_list.push_back(_enum);
	}


	virtual void printMIPS(std::ostream& dst, Context& context) {
	}

};

class Enum : public Program {
private:
	std::string id;
	ProgramPtr EnumList;

public:
	Enum(const std::string& _id): id(_id), EnumList(NULL){}
	Enum(const std::string& _id, ProgramPtr _EnumList) : id(_id), EnumList(_EnumList) {}

	virtual void printMIPS(std::ostream& dst, Context& context) {

	}
};
#endif
