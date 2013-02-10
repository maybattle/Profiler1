#pragma once
#include "stdafx.h"
#include "FunctionCharacteristics.h"


CFunctionCharacteristics::CFunctionCharacteristics(FunctionID functionId, const std::wstring functionName)
{
	_functionId= functionId;
	_functionName = functionName;
}

std::wstring CFunctionCharacteristics::GetFunctionName(){
	return _functionName;
}

LONG CFunctionCharacteristics::GetCallCount(){
	return _callCount;
}

void CFunctionCharacteristics::IncCallCount(){
	_callCount++;
}


FunctionID CFunctionCharacteristics::GetFunctionId(){
	return _functionId;
}