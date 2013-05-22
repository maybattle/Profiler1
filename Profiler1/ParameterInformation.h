#pragma once
#include "stdafx.h"


typedef enum ParameterDirection {
	DIRECTION_IN = 0x00,
	DIRECTION_OUT = 0x01
} ParameterDirection; 

class CParameterInformation
{
public:
	CParameterInformation(void);
	CParameterInformation(const std::wstring name, CorElementType nativeType, INT32 position, ParameterDirection direction=DIRECTION_IN, BOOL isReturnValue=FALSE);
	const std::wstring GetName();
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

