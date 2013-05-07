#pragma once
#include "stdafx.h"

class CFunctionInformation
{
public:
	CFunctionInformation(FunctionID functionId, const std::wstring functionName);
	~CFunctionInformation();
	const std::wstring GetFunctionName();
	LONG GetCallCount();
	void IncCallCount();
	FunctionID GetFunctionId();
	std::unordered_map<std::wstring, CParameterInformation*> ParameterInformations;

private:
	FunctionID _functionId;
	std::wstring _functionName;
	LONG _callCount;
	
};


