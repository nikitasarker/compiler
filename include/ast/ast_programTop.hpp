#ifndef ast_programTop_hpp
#define ast_programTop_hpp

#include "ast_program.hpp"

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <memory>

class ProgramTop : public Program
{
protected:
	mutable std::vector<ProgramPtr> vec;
public:
    virtual ~ProgramTop()
    {}
	ProgramTop(ProgramPtr _el) {
		vec.push_back(_el);
	}

    //! Tell and program to print itself to the given stream
	virtual void print(std::ostream& dst) const {
		std::vector<std::string> globals;
		for (size_t i = 0; i < vec.size(); ++i) { //gets global vars from standalone declarations
			vec[i]->getGlobals(globals);
		}
		for (size_t i = 0; i < vec.size(); ++i) { //sets global vars inside functions
			vec[i]->setGlobals(globals);
		}

		for (size_t i = 0; i < vec.size(); ++i)  {
			vec[i]->print(dst);
			dst << "\n";
		}

		dst << "\n";
		if (mainPresent) {
		dst << "if __name__ == \"__main__\":" << std::endl;
		dst << "	import sys" << std::endl;
		dst << "	ret = main()" << std::endl;
		dst << "	sys.exit(ret)" << std::endl;
		}
	};

	virtual void printMIPS(std::ostream& dst, Context& context) const {
		for (size_t i = 0; i < vec.size(); ++i) {
			vec[i]->printMIPS(dst,context);
			dst << "\n";
		}
	}

	virtual void push(ProgramPtr _stat) const {
		vec.push_back(_stat);
	};

};


#endif
