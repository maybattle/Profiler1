#pragma once
#include "stdafx.h"

class CLogger
{
public:
	CLogger(void);
	~CLogger(void);
	void WriteStringToLogFormat(const CHAR* format,...);
	

private:
	std::wstring _logFileName;
	HANDLE _logFileHandle;
	void CreateLogFile();
	void CloseLogFile();
};

