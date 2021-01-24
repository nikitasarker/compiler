#ifndef variable_hpp
#define variable_hpp

#include <string>

class Var { //this is the class for a variable - context.hpp will have a vector of Vars that contains all the variables in the program
public:
	Var(std::string _id, std::string _reg_id, std::string _type) :var_id(_id), reg_id(_reg_id), type(_type) { //creates register with the correct name
	}

	void setRegId(std::string _reg_id) {
		reg_id = _reg_id;
	}
	void setVarId(std::string _id) {
		var_id = _id;
	}
	void setType(std::string _type) {
		type = _type;
	}
	//void setFramePointer(uint32_t _fp) {
	//	frame_pointer.push(_fp); //frame pointer is pushed onto the frame_pointer stack
	//}

	std::string getRegId() const {
		return reg_id;
	}
	std::string getVarId() const {
		return var_id;
	}
	std::string getType() const {
		return type;
	}
	//std::string getFramePointer() const {
	//	return std::to_string(frame_pointer);
	//}
	size_t getSize() const { //returns the number of words that is the size of the variable type
		if ((type == "char") || (type == "unsigned char")
			|| (type == "signed char")) {
			return 1;
		}
		if ((type == "int") || (type == "unsigned int")
			|| (type == "long") || (type == "unsigned long")) {
			return 4;
		}
		if ((type == "short") || (type == "unsigned short")) {
			return 2;
		}
		if (type == "float") {
			return 4;
		}
		if (type == "double") {
			return 8;
		}
		if (type == "long double") {
			return 10;
		}
		return 4;

	}

	bool isFree() const { //tells us if the register is currenty holding any variables
		if (var_id == "") {
			return true;
		}
		else {
			return false;
		}
	}
private:
	std::string reg_id; //holds name of reg in which the variable is stored
	std::string var_id; //holds current name of variable
	std::string type;
	//stack<uint32_t> frame_pointer;
};


#endif
