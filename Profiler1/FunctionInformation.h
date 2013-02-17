#pragma once
#include "stdafx.h"

class CFunctionInformation
{
public:
	CFunctionInformation(FunctionID functionId, const std::wstring& functionName);
	const std::wstring& GetFunctionName();
	LONG GetCallCount();
	void IncCallCount();
	FunctionID GetFunctionId();

private:
	FunctionID _functionId;
	std::wstring _functionName;
	LONG _callCount;
	
};
