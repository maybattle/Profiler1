#pragma once
#include "stdafx.h"
#include "FunctionInformation.h"


CFunctionInformation::CFunctionInformation(FunctionID functionId, const std::wstring& functionName)
{
	_functionId= functionId;
	_functionName = std::wstring(functionName);
	_callCount = 0;
}

const std::wstring& CFunctionInformation::GetFunctionName(){
	return _functionName;
}

LONG CFunctionInformation::GetCallCount(){
	return _callCount;
}

void CFunctionInformation::IncCallCount(){
	_callCount++;
}


FunctionID CFunctionInformation::GetFunctionId(){
	return _functionId;
}