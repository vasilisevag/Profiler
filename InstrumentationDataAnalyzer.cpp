#include "InstrumentationDataAnalyzer.h"
#include <unordered_map>
#include <algorithm>

InstrumentationDataAnalyzer::InstrumentationDataAnalyzer() : instrumentationDataParser(std::make_shared<InstrumentationDataParser>()), functionCallTree() {}

void InstrumentationDataAnalyzer::CreateFunctionCallTree(const std::string& filename) {
	functionCallTree = instrumentationDataParser->FunctionCallTree(filename);
}

void InstrumentationDataAnalyzer::FindFunctionNamesHelper(std::shared_ptr<FunctionNode> functionNode, std::vector<std::string>& functionNames) {
	if (!functionNode)
		return;
	
    std::string functionName = functionNode->functionData.functionName;
	if (find(begin(functionNames), end(functionNames), functionName) == end(functionNames))
		functionNames.push_back(functionName);

    for (auto neighbor : functionNode->neighbors)
        FindFunctionNamesHelper(neighbor.second.first, functionNames);
}

std::vector<std::string> InstrumentationDataAnalyzer::FindFunctionNames() {
	if (!functionCallTree) //we have to decide on an error handling technic
		return {};

	std::vector<std::string> functionNames;
	FindFunctionNamesHelper(functionCallTree.value().root, functionNames);

	return functionNames;
}

double InstrumentationDataAnalyzer::FunctionInclusiveTimeHelper(std::shared_ptr<FunctionNode> functionNode, std::string functionName) {
	if (!functionNode)
		return .0;
	
    if (functionNode->functionData.functionName == functionName)
        return functionNode->functionData.inclusiveExecutionTime;
	else {
		double inclusiveExecutionTime = .0;
		for (auto neighbor : functionNode->neighbors)
            inclusiveExecutionTime += FunctionInclusiveTimeHelper(neighbor.second.first, functionName);
		return inclusiveExecutionTime;
	}
}

double InstrumentationDataAnalyzer::FunctionInclusiveTime(std::string functionName) {
	if (!functionCallTree)
		return .0;
	
	return FunctionInclusiveTimeHelper(functionCallTree.value().root, functionName);
}

std::vector<std::pair<std::string, double>> InstrumentationDataAnalyzer::FunctionsInclusiveTimes() {
	std::vector<std::string> functionNames = FindFunctionNames();

	std::vector<std::pair<std::string, double>> functionsInclusiveTimes;
	for (auto functionName : functionNames)
		functionsInclusiveTimes.push_back({ functionName, FunctionInclusiveTime(functionName) });

    sort(std::begin(functionsInclusiveTimes), std::end(functionsInclusiveTimes), [](auto x, auto y) {return x.second <= y.second; });

	return functionsInclusiveTimes;
}

void InstrumentationDataAnalyzer::FunctionsExclusiveTimesHelper(std::shared_ptr<FunctionNode> functionNode, std::unordered_map<std::string, double>& functionsExclusiveTimes) {
	if (!functionNode)
		return;

	std::string functionName = functionNode->functionData.functionName;
    if (functionsExclusiveTimes.find(functionName) == functionsExclusiveTimes.end())
		functionsExclusiveTimes[functionName] = .0;

	double neighborsInclusiveExecutionTimes = .0;
    for (auto neighbor : functionNode->neighbors)
        neighborsInclusiveExecutionTimes += neighbor.second.first->functionData.inclusiveExecutionTime;
    double functionExclusiveExecutionTime = functionNode->functionData.inclusiveExecutionTime - neighborsInclusiveExecutionTimes;

	functionsExclusiveTimes[functionName] += functionExclusiveExecutionTime;

    for (auto neighbor : functionNode->neighbors)
        FunctionsExclusiveTimesHelper(neighbor.second.first, functionsExclusiveTimes);
}

std::vector<std::pair<std::string, double>> InstrumentationDataAnalyzer::FunctionsExclusiveTimes() {
	if (!functionCallTree)
		return {};
	
	std::unordered_map<std::string, double> functionsExclusiveTimesMap;
    FunctionsExclusiveTimesHelper(functionCallTree.value().root, functionsExclusiveTimesMap);

	std::vector<std::pair<std::string, double>> functionsExclusiveTimesVector(begin(functionsExclusiveTimesMap), end(functionsExclusiveTimesMap));
    sort(std::begin(functionsExclusiveTimesVector), std::end(functionsExclusiveTimesVector), [](auto x, auto y) {return x.second <= y.second; });

	return functionsExclusiveTimesVector;
}
