#include "FunctionNode.h"
#include <utility>
#include <memory>

FunctionNode::FunctionNode(std::string functionName) : functionData(functionName) {}

bool FunctionNode::IsNeighbor(std::string functionName) {
	return neighbors.find(functionName) != neighbors.end();
}

void FunctionNode::IncrementEdgeWeight(std::string functionName) {
	neighbors[functionName].second++;
}

std::shared_ptr<FunctionNode> FunctionNode::operator[](std::string functionName) {
	return neighbors[functionName].first;
}

void FunctionNode::AddNeighbor(std::string functionName) {
	neighbors[functionName] = std::make_pair(std::make_shared<FunctionNode>(functionName), 0);
}

void FunctionNode::UpdateNeighbor(std::string functionName) {
	if (IsNeighbor(functionName)) {
		IncrementEdgeWeight(functionName);
	}
	else {
		AddNeighbor(functionName);
	}
}
