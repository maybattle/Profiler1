#include "stdafx.h"
#include "Profiler.h"

//Pointer fuer globalen Zugriff 
CProfiler *_pICorProfilerCallback;


void __stdcall EnterGlobal(FunctionIDOrClientID functionIDOrClientID)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->Enter(functionIDOrClientID);
}


//Wird von der CLR aufgerufen
//Manuelles sichern und Wiederherstellen der Register
void __declspec(naked) EnterNaked3(FunctionIDOrClientID functionIDOrClientID)
{
    __asm
    {
        push eax
        push ecx
        push edx
        push [esp + 16]
        call EnterGlobal
        pop edx
        pop ecx
        pop eax
        ret 4
    }
}


void __stdcall LeaveGlobal(FunctionIDOrClientID functionIDOrClientID)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->Leave(functionIDOrClientID);
}

//Wird von der CLR aufgerufen
//Manuelles sichern und Wiederherstellen der Register
void __declspec( naked ) LeaveNaked3(FunctionIDOrClientID functionIDOrClientID)
{
    __asm
    {
        push eax
        push ecx
        push edx
        push [esp + 16]
        call LeaveGlobal
        pop edx
        pop ecx
        pop eax
        ret 4
    }
}

void __stdcall TailcallGlobal(FunctionIDOrClientID functionIDOrClientID)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->Tailcall(functionIDOrClientID);
}


//Wird von der CLR aufgerufen
//Manuelles sichern und Wiederherstellen der Register
void __declspec( naked ) TailcallNaked3(FunctionIDOrClientID functionIDOrClientID)
{
    __asm
    {
        push eax
        push ecx
        push edx
        push [esp + 16]
        call TailcallGlobal
        pop edx
        pop ecx
        pop eax
        ret 4
    }
}





CProfiler::CProfiler(void)
{
}


CProfiler::~CProfiler(void)
{
	_pICorProfilerCallback=NULL;
}


HRESULT CProfiler::FinalConstruct()
{
	
	_logger = new CLogger();
	_logger->WriteStringToLogFormat("FinalConstruct reached\r\n");
	return S_OK;
}

void CProfiler::FinalRelease()
{
	delete _logger;
}


//wird von der CLR aufgerufen, wenn das Profiler Objekt erzeugt wurde.
//RETURN von E_FAIL teilt der CLR einen Fehler oder kein Interesse am Profilen des Prozesses mit  
HRESULT CProfiler::Initialize(IUnknown *pICorProfilerInfoUnk)
{
	HRESULT hr;

	//globalen Pointer initialisieren
	_pICorProfilerCallback = this;

	//ICorProfilerInfo interface holen, das wird auf jeden Fall benoetigt
    hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo, (LPVOID*) &_pICorProfilerInfo);
    if (FAILED(hr))  {
		_logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo failed\r\n");
		return E_FAIL;
	}

	//Events setzen, um der CLR mitzuteilen an welchen Informationen man intessiert ist.
	hr = SetEventMask();
    if (FAILED(hr)){
		_pICorProfilerInfo->SetEventMask(COR_PRF_MONITOR_NONE);
		return E_FAIL;
	}
    

	//als CLR 4 probieren, wenn das nicht funktioniert, liegt CLR 1 oder 2 vor. 
	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo3,(LPVOID*) &_pICorProfilerInfo3);
	if (SUCCEEDED(hr)){
		//CLR 4
		_logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo3 succeeded\r\n");
		hr = _pICorProfilerInfo3->SetEnterLeaveFunctionHooks3(
				(FunctionEnter3*)EnterNaked3, 
				(FunctionLeave3*)LeaveNaked3, 
				(FunctionTailcall3*)TailcallNaked3);

		if (SUCCEEDED(hr)){
			hr = _pICorProfilerInfo3->SetFunctionIDMapper(FunctionMapper);
		}
	}
	//else {
	//	
	//	//IID_ICorProfilerInfo3 (CLR 4) konnte nicht abgefragt werden. Es liegt CLR 1 oder 2 vor.
	//	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*) &_pICorProfilerInfo2);
	//	if (SUCCEEDED(hr)){
	//		_logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo2 succeeded\r\n");
	//		hr = _pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&EnterNaked2, (FunctionLeave2*)&LeaveNaked2, (FunctionTailcall2*)&TailcallNaked2);
	//		if (SUCCEEDED(hr)){
	//			hr = _pICorProfilerInfo2->SetFunctionIDMapper(FunctionMapper);
	//		}
	//		else {
	//			return E_FAIL;
	//		}
	//	}
	//	else{
	//		_logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo2 and IID_ICorProfilerInfo2 failed\r\n");
	//		_pICorProfilerInfo2.p = NULL;
	//		return E_FAIL;
	//	}
	//}

	

	if (FAILED(hr)){
		_logger->WriteStringToLogFormat("Unable to initialize profiler.");
		return E_FAIL;
	}

	_logger->WriteStringToLogFormat("Profiler initialized.");
    return S_OK;
}


void CProfiler::Enter(FunctionIDOrClientID functionIDOrClientID)
{
	CFunctionInformation* functionInformation = NULL;
	std::unordered_map<FunctionID, CFunctionInformation*>::iterator 
		iter = _functionCharacteristics.find(functionIDOrClientID.functionID);
	
	if (iter != _functionCharacteristics.end())
	{
		functionInformation = (iter->second);
		functionInformation->IncCallCount();
		
		int padCharCount = _callStackSize * 2;
		if (padCharCount > 0)
		{
			char* padding = new char[(padCharCount) + 1];
			memset(padding, 0, padCharCount + 1);
			memset(padding, '.', padCharCount);
			_logger-> WriteStringToLogFormat("Enter function name:%s%S, id=%d, call count = %d\r\n", 
				padding, functionInformation->GetFunctionName().c_str(), 
				functionInformation->GetFunctionId(), 
				functionInformation->GetCallCount());
			delete[] padding;
		}
		else
		{
			_logger->WriteStringToLogFormat("Enter function name:%S, id=%d, call count = %d\r\n", 
				functionInformation->GetFunctionName().c_str(), 
				functionInformation->GetFunctionId(), 
				functionInformation->GetCallCount());
		}
	}
	else _logger->WriteStringToLogFormat("Function:%i not found.", functionIDOrClientID);
	_callStackSize++;
}


void CProfiler::Leave(FunctionIDOrClientID functionIDOrClientID)
{
	if (_callStackSize > 0){
		_callStackSize--;
	}
}

void CProfiler::Tailcall(FunctionIDOrClientID functionIDOrClientID)
{
	if (_callStackSize > 0){
		_callStackSize--;
	}
}


///Von der CLR aufgerufen, wenn der Profiler beendet wird.
STDMETHODIMP CProfiler::Shutdown()
{
	_logger->WriteStringToLogFormat("Write full function list."); 
	std::unordered_map<FunctionID, CFunctionInformation*>::iterator i;
	for (i = _functionCharacteristics.begin(); i != _functionCharacteristics.end(); i++){
		CFunctionInformation* functionCharacteristics = i->second;
		_logger->WriteStringToLogFormat("%S : call count = %d\r\n", functionCharacteristics->GetFunctionName().c_str(),functionCharacteristics->GetCallCount());
		delete i->second;
	}
	_functionCharacteristics.clear();
	_pICorProfilerCallback = NULL;
    _logger->WriteStringToLogFormat("Profiler shutdown!\r\n");
	return S_OK;
}


UINT_PTR CProfiler::FunctionMapper(FunctionID functionID, BOOL *pbHookFunction)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->AddFunctionToMap(functionID);
	return (UINT_PTR)functionID;
}


void CProfiler::AddFunctionToMap(FunctionID functionID)
{
	std::unordered_map<FunctionID, CFunctionInformation*>::iterator iter = _functionCharacteristics.find(functionID);
	//Wenn functionID noch nicht vorhanden, dann einfuegen.
	if (iter == _functionCharacteristics.end())
	{
		std::wstring functionName;
		
		HRESULT hr = GetFullMethodName(functionID, functionName); 
		if (FAILED(hr))
		{
			_logger->WriteStringToLogFormat("Could not find name for functionID:%d\r\n",functionID);
			return;
		}	
		CFunctionInformation* characteristics;
		characteristics = new CFunctionInformation(functionID, functionName);
		_functionCharacteristics.insert(std::pair<FunctionID, CFunctionInformation*>(functionID, characteristics));
	}
}

//Methodname aus functionId ermitteln
HRESULT CProfiler::GetFullMethodName(FunctionID functionId, std::wstring &functionName)
{
	IMetaDataImport* pIMetaDataImport = 0;
	HRESULT hr = S_OK;
	mdToken funcToken = 0;
	WCHAR szFunction[MAX_CLASSNAME_LENGTH];
	WCHAR szClass[MAX_CLASSNAME_LENGTH];

	//Hier wird das Metadata Token und die Metadata Instance geholt, 
	//um anschliessend mit der Metadata API den Functionsnamen zu ermitteln
	//http://msdn.microsoft.com/en-us/library/ms233167.aspx
	hr = _pICorProfilerInfo->GetTokenAndMetaDataFromFunction(functionId, IID_IMetaDataImport, (LPUNKNOWN *) &pIMetaDataImport, &funcToken);
	if(SUCCEEDED(hr))
	{
		mdTypeDef classTypeDef;
		ULONG cchFunction;
		ULONG cchClass;
		
		//Metadaten der Methode ermitteln
		//http://msdn.microsoft.com/en-us/library/ms233163.aspx
		hr = pIMetaDataImport->GetMethodProps(funcToken, &classTypeDef, szFunction, MAX_CLASSNAME_LENGTH, &cchFunction, 0, 0, 0, 0, 0);
		if (SUCCEEDED(hr))
		{
			//Den eigentlichen Namen der Funtion ermitteln.
			//http://msdn.microsoft.com/en-us/library/ms230143.aspx
			hr = pIMetaDataImport->GetTypeDefProps(classTypeDef, szClass, MAX_CLASSNAME_LENGTH, &cchClass, 0, 0);
			if (SUCCEEDED(hr))
			{
				//Kompletten Name aus ClassName und FunctionName erstellen
				size_t length = MAX_CLASSNAME_LENGTH;
				WCHAR fullFunctionName[MAX_CLASSNAME_LENGTH];
				_snwprintf_s(fullFunctionName,length,L"%s.%s",szClass,szFunction);
				functionName = fullFunctionName;
			}
		}
		//WICHTIG: Die Metadaten Referenz muss immer freigegeben werden.
		pIMetaDataImport->Release();
	}
	return hr;
}



//Setzen der Events, an denen man interessiert ist. 
//Aktuell wird nur das COR_PRF_MONITOR_ENTERLEAVE verwendet, da die CLR dann alle 
//Funktionsausrufe mitteilt und diese in eine Log-Datei geschrieben werden koennen. 
HRESULT CProfiler::SetEventMask()
{
	//COR_PRF_MONITOR_NONE	= 0,
	//COR_PRF_MONITOR_FUNCTION_UNLOADS	= 0x1,
	//COR_PRF_MONITOR_CLASS_LOADS	= 0x2,
	//COR_PRF_MONITOR_MODULE_LOADS	= 0x4,
	//COR_PRF_MONITOR_ASSEMBLY_LOADS	= 0x8,
	//COR_PRF_MONITOR_APPDOMAIN_LOADS	= 0x10,
	//COR_PRF_MONITOR_JIT_COMPILATION	= 0x20,
	//COR_PRF_MONITOR_EXCEPTIONS	= 0x40,
	//COR_PRF_MONITOR_GC	= 0x80,
	//COR_PRF_MONITOR_OBJECT_ALLOCATED	= 0x100,
	//COR_PRF_MONITOR_THREADS	= 0x200,
	//COR_PRF_MONITOR_REMOTING	= 0x400,
	//COR_PRF_MONITOR_CODE_TRANSITIONS	= 0x800,
	//COR_PRF_MONITOR_ENTERLEAVE	= 0x1000,
	//COR_PRF_MONITOR_CCW	= 0x2000,
	//COR_PRF_MONITOR_REMOTING_COOKIE	= 0x4000 | COR_PRF_MONITOR_REMOTING,
	//COR_PRF_MONITOR_REMOTING_ASYNC	= 0x8000 | COR_PRF_MONITOR_REMOTING,
	//COR_PRF_MONITOR_SUSPENDS	= 0x10000,
	//COR_PRF_MONITOR_CACHE_SEARCHES	= 0x20000,
	//COR_PRF_MONITOR_CLR_EXCEPTIONS	= 0x1000000,
	//COR_PRF_MONITOR_ALL	= 0x107ffff,
	//COR_PRF_ENABLE_REJIT	= 0x40000,
	//COR_PRF_ENABLE_INPROC_DEBUGGING	= 0x80000,
	//COR_PRF_ENABLE_JIT_MAPS	= 0x100000,
	//COR_PRF_DISABLE_INLINING	= 0x200000,
	//COR_PRF_DISABLE_OPTIMIZATIONS	= 0x400000,
	//COR_PRF_ENABLE_OBJECT_ALLOCATED	= 0x800000,
	// New in VS2005
	//	COR_PRF_ENABLE_FUNCTION_ARGS	= 0x2000000,
	//	COR_PRF_ENABLE_FUNCTION_RETVAL	= 0x4000000,
	//  COR_PRF_ENABLE_FRAME_INFO	= 0x8000000,
	//  COR_PRF_ENABLE_STACK_SNAPSHOT	= 0x10000000,
	//  COR_PRF_USE_PROFILE_IMAGES	= 0x20000000,
	// End New in VS2005
	//COR_PRF_ALL	= 0x3fffffff,
	//COR_PRF_MONITOR_IMMUTABLE	= COR_PRF_MONITOR_CODE_TRANSITIONS | COR_PRF_MONITOR_REMOTING | COR_PRF_MONITOR_REMOTING_COOKIE | COR_PRF_MONITOR_REMOTING_ASYNC | COR_PRF_MONITOR_GC | COR_PRF_ENABLE_REJIT | COR_PRF_ENABLE_INPROC_DEBUGGING | COR_PRF_ENABLE_JIT_MAPS | COR_PRF_DISABLE_OPTIMIZATIONS | COR_PRF_DISABLE_INLINING | COR_PRF_ENABLE_OBJECT_ALLOCATED | COR_PRF_ENABLE_FUNCTION_ARGS | COR_PRF_ENABLE_FUNCTION_RETVAL | COR_PRF_ENABLE_FRAME_INFO | COR_PRF_ENABLE_STACK_SNAPSHOT | COR_PRF_USE_PROFILE_IMAGES

	// set the event mask 
	DWORD eventMask = (DWORD) (COR_PRF_MONITOR_ENTERLEAVE);
	_logger->WriteStringToLogFormat("Set EventMask to COR_PRF_MONITOR_ENTERLEAVE\r\n");

	return _pICorProfilerInfo->SetEventMask(eventMask);
}


