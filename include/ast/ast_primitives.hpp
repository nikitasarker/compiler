#ifndef ast_primitives_hpp
#define ast_primitives_hpp

#include <string>
#include <vector>
#include <iostream>

class Variable
    : public Program
{
private:
    std::string id;
public:
    Variable(const std::string &_id) : id(_id) {}

   virtual void getId(std::string &getid) const
    { getid = id; }

    virtual void print(std::ostream &dst) const
    {dst<<id;}

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		int stack_position_of_variable = context.bindings[id].stack_offset;
		dst<<"\tlw\t$v0,"<<stack_position_of_variable<<"($fp)"<<std::endl;
		dst<<"\tnop"<<std::endl;
		context.DecrementStackOffset();
		dst << "\tsw\t$v0,"<<context.stack_offset<<"($fp)"<<std::endl;
		dst<<"\tnop"<<std::endl;
	}

	virtual void getFunctionVars(std::vector<std::string> &variables) {
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

	virtual void getDeclaredFunctionVars(std::vector<std::string>& declaredVariables) {
		declaredVariables.push_back(id);
	}

	virtual void getDepth(int& depthCounter) {
		depthCounter++;
	}
    void push(const ProgramPtr _stat) {
          std::cerr << "this shouldn't happen" <<std::endl;
    }

	virtual int getStackPosition(Context context) {
		return context.bindings[id].stack_offset;
	}

};

class Parameter
    : public Program
{
private:
    std::string type;
		std::string id;
		ProgramPtr declarator;
public:
	Parameter(const std::string& _type, ProgramPtr _declarator) : type(_type), declarator(_declarator) {
		id = getID();
	}

    const std::string getID() const
    { return declarator->getID(); }

		std::string getType() {
			return type;
		}

    virtual void print(std::ostream &dst) const
    {dst<<declarator->getID();}

	virtual void printMIPS(std::ostream& dst, Context& context) const {}

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


    void push(const ProgramPtr _stat) {
          std::cerr << "this shouldn't happen" <<std::endl;
    }

};


class Number : public Program{

private:
    double value;
public:
    Number(double _value): value(_value) {}

    double getValue() const{
		return value; }

    virtual void print(std::ostream &dst) const
    {
      dst<<value;
    }

		virtual void printMIPS(std::ostream& dst, Context& context) const{
			dst << "\tli \t$v0," << value << std::endl;
			context.DecrementStackOffset();
			dst << "\tsw"<<"\t$v0,"<<context.stack_offset<<"($fp)"<<std::endl;
			dst<<"\tnop"<<std::endl;
		}
		virtual void getFunctionVars(std::vector<std::string> &variables) {
			//do nothing for numbers
		}

		virtual void getDepth(int& depthCounter) {
			depthCounter++;
		}
	    void push(ProgramPtr _stat) {
	          std::cerr << "this shouldn't happen" <<std::endl;
	    }
      double getValue(){
		      return value;
	    }

};


#endif
