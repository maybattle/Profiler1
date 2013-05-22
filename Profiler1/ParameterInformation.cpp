#pragma once
#include "stdafx.h"

CParameterInformation::CParameterInformation(const wstring name, CorElementType nativeType, INT32 position, ParameterDirection direction, BOOL isReturnValue)
{
	_name = name;
	_nativeType = nativeType;
	_position = position;
	_direction = direction;
	_isReturnValue = isReturnValue;
}

const wstring CParameterInformation::GetName() { return _name;};

CorElementType CParameterInformation::GetNativeType(){return _nativeType;}

INT32 CParameterInformation::GetPosition(){return _position;}

INT32 CParameterInformation::GetDirection(){return _direction;}

BOOL CParameterInformation::GetIsReturnValue(){return _isReturnValue;}

