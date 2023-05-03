#pragma once
#include "FunctionNode.h"
#include <unordered_map>

class Graph {
	Graph();
	std::shared_ptr<FunctionNode> AddEdgeWeight(std::string sourceFunctionName, std::string destinationFunctionName);
private:
	std::unordered_map<std::string, std::shared_ptr<FunctionNode>> nodes;
};

