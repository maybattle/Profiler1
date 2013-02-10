#pragma once
#include "stdafx.h"

class CFunctionCharacteristics
{
public:
	CFunctionCharacteristics(FunctionID functionId, const std::wstring functionName);
	std::wstring GetFunctionName();
	LONG GetCallCount();
	void IncCallCount();
	FunctionID GetFunctionId();

private:
	FunctionID _functionId;
	std::wstring _functionName;
	LONG _callCount;
	
};
