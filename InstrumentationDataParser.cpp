#include "InstrumentationDataParser.h"
#include <fstream>
#include <cassert>
#include <string>
#include <iostream>

InstrumentationDataParser::InstrumentationDataParser() {}

void InstrumentationDataParser::FunctionCallTreeHelper(std::shared_ptr<FunctionNode> functionNode, std::ifstream& dataStream) {
	std::string functionName, functionState;
	
    dataStream >> functionName;
    if(dataStream.eof()){
        dataStream.close();
        return;
    }
    dataStream >> functionState;
    if(dataStream.eof()){
        dataStream.close();
        return;
    }
	while(functionState != "end") {
		functionNode->UpdateNeighbor(functionName);
		FunctionCallTreeHelper(functionNode->operator[](functionName), dataStream);
        dataStream >> functionName;
        if(dataStream.eof()){
            dataStream.close();
            return;
        }
        dataStream >> functionState;
        if(dataStream.eof()){
            dataStream.close();
            return;
        }
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
    assert(dataStream.fail() == 0);

	FunctionCallTreeHelper(functionCallTree.root, dataStream);

	return functionCallTree;
}


