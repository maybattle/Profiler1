#pragma once
#include "stdafx.h"



CFunctionInformation::CFunctionInformation(FunctionID functionId, const std::wstring functionName)
{
	_functionId= functionId;
	_functionName = std::wstring(functionName);
	_callCount = 0;
}

CFunctionInformation::~CFunctionInformation(){
	while(!ParameterInformations.empty()) delete ParameterInformations.back(), ParameterInformations.pop_back();
	ParameterInformations.clear();
	if(_returnTypeInformation!=NULL) delete(_returnTypeInformation);
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

BOOL CFunctionInformation::GetIsStatic(){
	return _isStatic;
}

void CFunctionInformation::SetIsStatic(BOOL isStatic){
	_isStatic = isStatic;
}

void CFunctionInformation::SetReturnTypeInformation(CParameterInformation* pReturnTypeInformation){
	_returnTypeInformation = pReturnTypeInformation;
}
 CParameterInformation* CFunctionInformation::GetReturnTypeInformation() {
	return _returnTypeInformation;
}

FunctionID CFunctionInformation::GetFunctionId(){
	return _functionId;
}