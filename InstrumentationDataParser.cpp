#include "InstrumentationDataParser.h"
#include <fstream>
#include <cassert>
#include <string>

InstrumentationDataParser::InstrumentationDataParser() {}

void InstrumentationDataParser::FunctionCallTreeHelper(std::shared_ptr<FunctionNode> functionNode, std::ifstream& dataStream) {
	std::string functionName, functionState;
	
	dataStream >> functionName >> functionState;
	while(functionState != "end") {
		functionNode->UpdateNeighbor(functionName);
		FunctionCallTreeHelper(functionNode->operator[](functionName), dataStream);
		dataStream >> functionName >> functionState;
	} 

	double executionTime;
	dataStream >> executionTime;
	functionNode->functionData.inclusiveExecutionTime += executionTime;
	
	return;
}

Tree InstrumentationDataParser::FunctionCallTree(std::string filename) {
	Tree functionCallTree;

	std::ifstream dataStream;
	dataStream.open(filename.c_str());
	assert(dataStream.fail());

	FunctionCallTreeHelper(functionCallTree.root, dataStream);

	return functionCallTree;
}


