#ifndef context_hpp
#define context_hpp

#include "variable.hpp"//might not need either of these - they were from before

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

/*
|Register | Assembly name | Comment |
|r0       | $zero         | Always 0 |
|r1       | $at           | Reserved for assembler |
|r2-r3    | $v0-$v1       | Stores result |
|r4-r7    | $a0-$a3       | Stores arguments |
|r8-r15   | $t0-$t7       | Temporaries, not saved |
|r16-r23  | $s0-$s7       | Contents saved for later |
|r24-r25  | $t8-$t9       | More temporaries not saved |
|r26-r27  | $k0-$k1       | Reserved by OS |
|r28      | $gp           | Global pointer |
|r29      | $sp           | Stack pointer |
|r30      | $fp           | Frame pointer |
|r31      | $ra           | Return address |
*/

struct VariableData{
	int stack_offset;
	std::string type;
	int array_size = 0;
	//std::string regID; //holds what register the variable is currently held in ("" if not currently in a reg)
	//something extra for arrays
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
};

class Context {
public:
	std::unordered_map<std::string, VariableData> bindings;
	int stack_offset = 0;
	int memory_required = 0;
	int scope_counter = 0;
	int break_counter = 0;
	int function_call_memory = 0;
	Context() {}

	void addBinding(const std::string& id, std::string type, const int& stack_offset){
		VariableData var_data;
		var_data.type = type;
		var_data.stack_offset = stack_offset;
		bindings.insert(std::make_pair(id, var_data));
	}

	void addBinding(const std::string& id, std::string type, const int& _stack_offset, int _arraySize) { //addbinding for array
		VariableData var_data;
		var_data.type = type;
		var_data.array_size = _arraySize;
		var_data.stack_offset = _stack_offset;
		bindings.insert(std::make_pair(id, var_data));
	}

	VariableData getBinding(const std::string id) {
		std::unordered_map<std::string, VariableData>::const_iterator got = bindings.find(id);
		return got->second;
	}

	void IncrementStackOffset() {
		stack_offset = stack_offset + 4;
	}

	void DecrementStackOffset() {
		stack_offset = stack_offset - 4;
	}

	std::string makeScopeLabel(std::string base)  { //makes unique label for branches and jumps
		std::string unqLabel = "_" + base + "_" + std::to_string(scope_counter);
		return unqLabel;
	}

	std::string makeScopedBreakLabel() {
		std::string unqLabel = "_break_" + std::to_string(break_counter);
		return unqLabel;
	}

	void updateVarStackPosition(std::string id, int stack_pos) {
		std::unordered_map<std::string, VariableData>::iterator got = bindings.find(id);
		got->second.stack_offset = stack_pos;
	}

	/*void setRegId(std::string id, std::string _reg_id) {
		std::unordered_map<std::string, VariableData>::const_iterator got = bindings.find(id);
		got->second.regID= _reg_id;
	}
	std::string getRegId() const {
		std::unordered_map<std::string, VariableData>::const_iterator got = bindings.find(id);
		return got->second.regID;
	}*/
	std::vector<std::string> declVariableList;
	std::vector<std::string> paramsList;
};

#endif
