#pragma once
#include "FunctionData.h"
#include <unordered_map>
#include <memory>
#include <utility>

struct FunctionNode {
	FunctionNode(std::string functionName);
	bool IsNeighbor(std::string functionName);
	void IncrementEdgeWeight(std::string functionName);
	std::shared_ptr<FunctionNode> operator[](std::string functionName);
	void AddNeighbor(std::string functionName);
	void UpdateNeighbor(std::string functionName);

	FunctionData functionData;
	std::unordered_map<std::string, std::pair<std::shared_ptr<FunctionNode>, int>> neighbors;
};

