#pragma once
#include "stdafx.h"

class CParameterInformation
{
public:
	CParameterInformation(void);
	CParameterInformation(const std::wstring name, CorElementType nativeType, INT32 position, INT32 direction, BOOL isReturnValue);
	std::wstring GetName();
	CorElementType GetNativeType();
	INT32 GetPosition();
	INT32 GetDirection();
	BOOL GetIsReturnValue();
		

private :
	std::wstring _name;
	CorElementType _nativeType;
	INT32 _position;
	INT32 _direction;
	BOOL _isReturnValue;
};

