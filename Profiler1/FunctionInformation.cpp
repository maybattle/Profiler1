#pragma once
#include "stdafx.h"



CFunctionInformation::CFunctionInformation(FunctionID functionId, const std::wstring functionName)
{
	_functionId= functionId;
	_functionName = std::wstring(functionName);
	_callCount = 0;
}

CFunctionInformation::~CFunctionInformation(){
	std::unordered_map<std::wstring, CParameterInformation*>::iterator i;
	for(i=ParameterInformations.begin();i != ParameterInformations.end();i++){
		delete i->second;
	}
	ParameterInformations.clear();
}

const std::wstring CFunctionInformation::GetFunctionName(){
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