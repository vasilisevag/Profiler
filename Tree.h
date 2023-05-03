#pragma once
#include "FunctionNode.h"
#include <unordered_map>
#include <memory>

struct Tree {
	Tree();

	std::shared_ptr<FunctionNode> root;
};

