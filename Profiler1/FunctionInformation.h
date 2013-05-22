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
	void SetIsStatic(BOOL isStatic);
	BOOL GetIsStatic();
	void SetReturnTypeInformation(CParameterInformation* pReturnTypeInformation);
	CParameterInformation* GetReturnTypeInformation();
	FunctionID GetFunctionId();
	std::vector<CParameterInformation*> ParameterInformations;

private:
	FunctionID _functionId;
	std::wstring _functionName;
	LONG _callCount;
	CParameterInformation* _returnTypeInformation;
	BOOL _isStatic;
};


