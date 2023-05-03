#pragma once
#include "Tree.h"
#include "InstrumentationDataParser.h"
#include "FunctionNode.h"
#include <memory>
#include <string>
#include <optional>
#include <vector>
#include <utility>
#include <unordered_map>

class InstrumentationDataAnalyzer {
public:
	InstrumentationDataAnalyzer();
	void CreateFunctionCallTree(const std::string& filename);
	std::vector<std::pair<std::string, double>> FunctionsInclusiveTimes();
	std::vector<std::pair<std::string, double>> FunctionsExclusiveTimes();
private:
	void FindFunctionNamesHelper(std::shared_ptr<FunctionNode> functionNode, std::vector<std::string>& functionNames);
	std::vector<std::string> FindFunctionNames();
    double FunctionInclusiveTimeHelper(std::shared_ptr<FunctionNode> functionNode, std::string functionName);
	double FunctionInclusiveTime(std::string functionName);
	void FunctionsExclusiveTimesHelper(std::shared_ptr<FunctionNode> functionNode, std::unordered_map<std::string, double>& functionsExclusiveTimes);
	std::optional<Tree> functionCallTree;
	std::shared_ptr<InstrumentationDataParser> instrumentationDataParser;
};
	
