#include "stdafx.h"
#include "Logger.h"


CLogger::CLogger(void)
{
	_logFileName= L"Profiler1.log.txt";
	CreateLogFile();
}


CLogger::~CLogger(void)
{
	CloseLogFile();
}

void CLogger::CreateLogFile(){
	
	::DeleteFile(_logFileName.c_str());
	_logFileHandle = CreateFile(_logFileName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void CLogger::CloseLogFile(){
	if (_logFileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_logFileHandle);
		_logFileHandle = INVALID_HANDLE_VALUE;
	}
}


void CLogger::WriteStringToLogFormat(const CHAR* formatString, ...)
{
	CHAR szBuffer[4096]; 
	DWORD dwWritten = 0;

	if(_logFileHandle != INVALID_HANDLE_VALUE)
	{
		
		va_list args;
		va_start( args, formatString );
		vsprintf_s(szBuffer,formatString, args );
		va_end( args );
		WriteFile(_logFileHandle, szBuffer, (DWORD)strlen(szBuffer), &dwWritten, NULL);
	}
}
