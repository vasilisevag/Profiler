#pragma once
#include "Tree.h"
#include "FunctionNode.h"
#include <memory>
#include <string>
#include <fstream>

class InstrumentationDataParser {
public:
	InstrumentationDataParser();
	Tree FunctionCallTree(std::string filename);
private:
	void FunctionCallTreeHelper(std::shared_ptr<FunctionNode> functionNode, std::ifstream& dataStream);
};

