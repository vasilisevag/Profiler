#pragma once
#include <string>

struct FunctionData {
	FunctionData(std::string functionName);

	std::string functionName;
	double inclusiveExecutionTime;
};