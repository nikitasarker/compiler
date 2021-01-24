#ifndef ast_sequence_hpp
#define ast_sequence_hpp

#include "ast_program.hpp"
#include "context.hpp"

#include <string>
#include <iostream>
#include <cmath>
#include <vector>

class Statement : public Program {
protected:
	mutable std::vector<ProgramPtr> list;
	bool individual = true;

public:
	Statement() {}
	Statement(ProgramPtr _el) {
		list.push_back(_el);
	}
	Statement(ProgramPtr _if, ProgramPtr _else) { //not sure if correct implementation from parser
		list.push_back(_if);
		list.push_back(_else);
	}

	virtual void print(std::ostream& dst) const {
		for (size_t i = 0; i < list.size(); ++i) {
			list[i]->print(dst);
			dst << std::endl;
		}
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		for (size_t i = 0; i < list.size(); ++i) {
			list[i]->printMIPS(dst,context);
			dst << std::endl;
		}
	}

	virtual void getFunctionVars(std::vector<std::string> &variables) {
		for (size_t i = 0; i < list.size(); ++i) {
			list[i]->getFunctionVars(variables);
		}
	}

	virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {
		for (size_t i = 0; i < list.size(); ++i) {
			list[i]->getDeclaredVars(declaredVariables);
		}
	}
	virtual void getDepth(int& depthCounter) {//gets max depth of the sequence
		int tmpDepth;
		int maxDepth=0;
		for (size_t i = 0; i < list.size(); ++i) {
			tmpDepth = 0;
			list[i]->getDepth(tmpDepth);
			if (tmpDepth > maxDepth) {
				maxDepth =  tmpDepth;
			}
		}
		depthCounter = depthCounter + maxDepth;
	}

	virtual void push(ProgramPtr _var) const {
		list.push_back(_var);
	}

	void setIndividualFalse() {
		individual = false;
	}
};

class Sequence : public Statement{
public:
	Sequence(ProgramPtr _el) : Statement(_el) {}


};

class ExpressionStatement : public Statement {
private:
	ProgramPtr el;
public:
	ExpressionStatement() {}
	ExpressionStatement(ProgramPtr _el) : el(_el) {}

	virtual void print(std::ostream& dst) const override {
			for (int j = 0; j < getIndent(); j++) {
				dst << "	";
			}
			el->print(dst);
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const override {
		el->printMIPS(dst,context);
	}

	virtual void getFunctionVars(std::vector<std::string>& variables) {
		el->getFunctionVars(variables);
	}
	virtual void getDepth(int& depthCounter) {
		el->getDepth(depthCounter);

	}
	/*virtual void getDeclaredVars(std::vector<std::string>& declaredVariables) {
		el->getDeclaredVars(declaredVariables);
	}*/
};

class SelectionStatement : public Statement {
private:
	ProgramPtr cond, ifseq, elseseq;

public:
	SelectionStatement() : Statement() {}
	SelectionStatement(ProgramPtr _cond, ProgramPtr _ifseq) {
		cond = _cond;
		ifseq = _ifseq;
		elseseq = NULL;
	}
	SelectionStatement(ProgramPtr _cond, ProgramPtr _ifseq, ProgramPtr _elseseq) {
		cond = _cond;
		ifseq = _ifseq;
		elseseq = _elseseq;
	}

	virtual void print(std::ostream& dst) const {
		for (int j = 0; j < getIndent(); j++) {
			dst << "	";
		}
		dst << "if ";
		cond->print(dst);
		dst << ":" << std::endl;
		indentIncr();
		ifseq->print(dst);
		indentDecr();
		dst << std::endl;
		if (elseseq != NULL) {
			for (int j = 0; j < getIndent(); j++) {
				dst << "	";
			}
			dst << "else:"<<std::endl;
			indentIncr();
			elseseq->print(dst);
			indentDecr();
		}
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const override {
		context.scope_counter++;
		std::string ifLabel = context.makeScopeLabel("if");
		std::string elseLabel = context.makeScopeLabel("else");
		std::string endLabel = context.makeScopeLabel("end");

		dst<<ifLabel<<":"<<std::endl;
		cond->printMIPS(dst,context);
		dst<<"\tlw"<<"\t$t0,"<<context.stack_offset<<"($fp)"<<std::endl;
		context.IncrementStackOffset();
		dst << "\tbeq " << "\t$t0, $zero, " << elseLabel << std::endl;
		ifseq->printMIPS(dst,context);
		dst<<"\tlw"<<"\t$v0,"<<context.stack_offset<<"($fp)"<<std::endl;
		context.IncrementStackOffset();
		dst << "\tbeq"<<"\t$zero,$zero, "<< endLabel << std::endl;
		dst << elseLabel<< ":" << std::endl;
		if (elseseq != NULL) {
			elseseq->printMIPS(dst, context);
		}
		dst << endLabel<< ":" << std::endl;

	}

	virtual void getFunctionVars(std::vector<std::string>& variables) {
		ifseq->getFunctionVars(variables);
		if (elseseq != NULL) {
			elseseq->getFunctionVars(variables);
		}
	}
	virtual void getDepth(int& depthCounter) {
		int condDepth = 0;
		int ifDepth =0;
		int elseDepth = 0;
		cond->getDepth(condDepth);
		ifseq->getDepth(ifDepth);
		if (elseseq != NULL) {
			elseseq->getDepth(elseDepth);
		}
		if ((condDepth >= ifDepth) && (condDepth >= elseDepth)) {
			depthCounter = condDepth;
		}
		else if ((ifDepth >= condDepth) && (ifDepth >= elseDepth)) {
			depthCounter = ifDepth;
		}
		else {
			depthCounter = elseDepth;
		}
	}
};

class CompoundStatement : public Statement {
public:
	CompoundStatement() : Statement() {}
	CompoundStatement(ProgramPtr _el) : Statement(_el) {}
	CompoundStatement(ProgramPtr _dec, ProgramPtr _statement) :
		Statement(_dec, _statement) {
	}

	virtual void print(std::ostream& dst) const override {
		for (size_t i = 0; i < list.size(); ++i) {
			list[i]->print(dst);
		}
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		for (size_t i = 0; i < list.size(); ++i) {
			list[i]->printMIPS(dst,context);
		}
	}
	virtual void getDepth(int& depthCounter) {
		int tmpDepth;
		int maxDepth = 0;
		for (size_t i = 0; i < list.size(); ++i) {
			tmpDepth = 0;
			list[i]->getDepth(tmpDepth);
			if (tmpDepth > maxDepth) {
				maxDepth = tmpDepth;
			}
		}
		depthCounter = depthCounter+maxDepth;
	}


};

class RetStatement : public Statement {
private:
	ProgramPtr expr;
public:
	RetStatement(ProgramPtr _expr) : expr(_expr) {}

	virtual void print(std::ostream& dst) const override {
		for (int j = 0; j < getIndent(); j++) {
			dst << "	";
		}
		dst << "return ";
		//list[list.size() - 1]->print(dst); //print the last thing in the list, because return statement will always be last?
		expr->print(dst);
	}

	void printMIPS(std::ostream& dst, Context& context) const{
		//list[list.size() - 1]->printMIPS(dst, context);
		expr->printMIPS(dst, context);
		dst<<"\tmove"<<"\t$sp,$fp"<<std::endl; //pointing $sp to end of stack
		dst<<"\tlw"<<"\t$ra,"<<context.memory_required-4<<"($sp)"<<std::endl; //loading previous return address into $ra
		dst<<"\tlw"<<"\t$fp,"<<context.memory_required-8<<"($sp)"<<std::endl; //pointing $fp at previous stack
		dst<<"\tnop"<<std::endl;
		dst<<"\taddiu"<<"\t$sp,$sp,"<<context.memory_required<<std::endl; //pointing $sp at end of previous stack
		dst << "\tjr \t$ra" << std::endl; //jump to return address
		dst << "\tnop" << std::endl;
	}

	virtual void getDepth(int& depthCounter) {
		expr->getDepth(depthCounter);
	}


};

class WhileLoop : public Statement {
private:
	ProgramPtr cond, seq;

public:
	WhileLoop() : Statement() {}
	WhileLoop(ProgramPtr _cond, ProgramPtr _seq) : cond(_cond), seq(_seq) {}

	virtual void print(std::ostream& dst) const {
		for (int j = 0; j < getIndent(); j++) {
			dst << "	";
		}
		dst << "while ";
		cond->print(dst);
		dst << ":" << std::endl;
		indentIncr();
		seq->print(dst);
		indentDecr();
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		context.scope_counter++;
		context.break_counter++;
		std::string labelTop = context.makeScopeLabel("top");
		std::string labelEnd = context.makeScopeLabel("end");
		std::string breakLabel = context.makeScopedBreakLabel();

		dst<<labelTop<<":"<<std::endl;
		cond->printMIPS(dst,context);
		dst<<"\tlw \t$t0,"<<context.stack_offset<<"($sp)"<<std::endl;
		context.IncrementStackOffset();
		dst << "\tbeq \t$t0, $0, " << labelEnd <<std::endl;
		seq->printMIPS(dst, context);
		dst<<"\tbeq \t$0, $0,"<<labelTop<<std::endl;
		dst<<labelEnd<<":"<<std::endl;

		dst <<breakLabel<<":"<<std::endl;
	}
	virtual void getFunctionVars(std::vector<std::string>& variables) {
		seq->getFunctionVars(variables);
	}

	virtual void getDepth(int& depthCounter) {
		int condDepth = depthCounter;
		int seqDepth = depthCounter;
		cond->getDepth(condDepth);
		seq->getDepth(seqDepth);


		if (condDepth > seqDepth) {
			depthCounter = condDepth;
		}
		else {
			depthCounter = seqDepth;
		}
	}

};

class ForLoop : public Statement {
private:
	ProgramPtr init, cond, incr, seq;

public:
	ForLoop(ProgramPtr _init, ProgramPtr _cond, ProgramPtr _incr, ProgramPtr _seq) :
		init(_init), cond(_cond), incr(_incr), seq(_seq) {}

	virtual void printMIPS(std::ostream& dst, Context& context) const{
		context.scope_counter++;
		std::string startLabel = context.makeScopeLabel("top");
		std::string breakLabel = context.makeScopeLabel("break");

		init->printMIPS(dst, context);
		dst<<startLabel<<":"<<std::endl;
		dst<<"\tlw \t$v0,"<<context.stack_offset<<"($sp)"<<std::endl;
		context.IncrementStackOffset();
		cond->printMIPS(dst,context);
		dst << "\tbeq \t$v0, $0, " << breakLabel <<std::endl;
		seq->printMIPS(dst, context);
		incr->printMIPS(dst, context);
		dst<<"\tj \t"<<startLabel<<std::endl;

		dst<<breakLabel<<":"<<std::endl;
	}

	virtual void getDepth(int& depthCounter) {
		int initDepth = depthCounter;
		int condDepth = depthCounter;
		int incrDepth = depthCounter;
		int seqDepth = depthCounter;
		init->getDepth(initDepth);
		cond->getDepth(condDepth);
		incr->getDepth(incrDepth);
		seq->getDepth(seqDepth); //this will *probably* be the longest one each time


		if ((initDepth > condDepth) && (initDepth > incrDepth) && (initDepth > seqDepth)) {
			depthCounter = initDepth;
		}
		else if ((condDepth > initDepth) && (condDepth > incrDepth) && (condDepth > seqDepth)) {
			depthCounter = condDepth;
		}
		else if ((incrDepth > initDepth) && (incrDepth > condDepth) && (incrDepth > seqDepth)) {
			depthCounter = incrDepth;
		}
		else {
			depthCounter = seqDepth;
		}
	}
};

class SwitchStatement : public Statement {
private:
	ProgramPtr cond, seq;

public:
	SwitchStatement(ProgramPtr _cond, ProgramPtr _seq) :
	cond(_cond), seq(_seq) {}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		context.scope_counter++;
		context.break_counter++;
		std::string breakLabel = context.makeScopedBreakLabel();

		cond->printMIPS(dst,context);
		dst<<"\tlw\t$t3,"<<context.stack_offset<<"($fp)"<<std::endl;
		context.IncrementStackOffset();
		seq->printMIPS(dst,context);

		dst<<breakLabel<<":"<<std::endl;
	}


	virtual void getDepth(int& depthCounter) {
		int condDepth = depthCounter;
		int seqDepth = depthCounter;
		cond->getDepth(condDepth);
		seq->getDepth(seqDepth);


		if (condDepth > seqDepth) {
			depthCounter = condDepth;
		}
		else {
			depthCounter = seqDepth;
		}
	}
};

class CaseStatement : public Statement {
private:
	ProgramPtr case_expr, seq;
public:
	CaseStatement(ProgramPtr _case_expr, ProgramPtr _seq) : case_expr(_case_expr), seq(_seq){}
	CaseStatement(ProgramPtr _seq) : seq(_seq), case_expr(NULL) {}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		if(case_expr!=NULL) {
			context.scope_counter++;
			std::string skipLabel = context.makeScopeLabel("skipLabel");

			case_expr->printMIPS(dst,context);
			dst<<"\tlw\t$t4,"<<context.stack_offset<<"($fp)"<<std::endl;
			context.IncrementStackOffset();
			dst<<"\tbne\t$t3,$t4,"<<skipLabel<<std::endl;
			seq->printMIPS(dst, context);

			dst<<skipLabel<<":"<<std::endl;
		}
		else {
			seq->printMIPS(dst, context);
		}
	}


	virtual void getDepth(int& depthCounter) {
		int case_exprDepth = depthCounter;
		int seqDepth = depthCounter;
		case_expr->getDepth(case_exprDepth);
		seq->getDepth(seqDepth);


		if (case_exprDepth > seqDepth) {
			depthCounter = case_exprDepth;
		}
		else {
			depthCounter = seqDepth;
		}
	}
};

class BreakStatement : public Statement {
public:
	BreakStatement(){}
	virtual void printMIPS(std::ostream& dst, Context& context) const {
		dst << "\tj \t"<< "_break_"<< context.break_counter << std::endl; //jump to return address
		dst << "\tnop" << std::endl;
	}
};

class ContinueStatement : public Statement {
public:
	ContinueStatement(){}
	virtual void printMIPS(std::ostream& dst, Context& context) const {
		dst << "\tj \t"<< "_top_"<< context.scope_counter << std::endl; //jump to return address
		dst << "\tnop" << std::endl;
	}
};

class Scope : public Statement {
private:
	ProgramPtr seq;
public:
	Scope(ProgramPtr _seq): seq(_seq) {}

	virtual void print(std::ostream& dst) const override {
		seq->print(dst);
	}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		if(individual) {
			std::vector<std::string> declaredVariablesList;
			int depth = 0;
			seq->getDeclaredVars(declaredVariablesList);
			seq->getDepth(depth);

			Context original_context = context;

			int req_memory_size = 4*(declaredVariablesList.size()+depth+1);

			context.stack_offset = req_memory_size-4;
			context.memory_required = req_memory_size;

			dst<<"\taddiu"<<"\t$sp,$sp,-"<<context.memory_required<<std::endl; //pointing $sp at end of new stack
			dst<<"\tsw"<<"\t$fp,"<<context.memory_required-4<<"($sp)"<<std::endl; //storing current $fp into new stack
			dst<<"\tmove"<<"\t$fp,$sp"<<std::endl; //pointing frame pointer to new stack, WE ARE IN NEW STACK
			seq->printMIPS(dst, context);

			dst<<"\tmove"<<"\t$sp,$fp"<<std::endl; //pointing $sp to end of stack
			dst<<"\tlw"<<"\t$fp,"<<context.memory_required-4<<"($sp)"<<std::endl; //pointing $fp at previous stack
			dst<<"\tnop"<<std::endl;
			dst<<"\taddiu"<<"\t$sp,$sp,"<<context.memory_required<<std::endl; //pointing $sp at end of previous stack
			dst << "\tnop" << std::endl;

			context = original_context;
		}
		else {
			seq->printMIPS(dst, context);
		}

	}

	virtual void getDepth(int& depthCounter) {
		seq->getDepth(depthCounter);
	}
};
#endif
