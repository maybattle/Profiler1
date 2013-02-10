#include "stdafx.h"
#include "Profiler.h"

//Pointer fuer globalen Zugriff 
CProfiler *_pICorProfilerCallback;


void __stdcall EnterGlobal(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argInfo)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->Enter(functionID, clientData, frameInfo, argInfo);
}

//Wird von der CLR aufgerufen
//Es werden keine Register gesichert, das muss manuell mit Assembler Code geschehen
//http://www.codeproject.com/Articles/15410/Creating-a-Custom-NET-Profiler
void _declspec(naked) EnterNaked2(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{
	__asm
    {
        push    ebp                 // Create a standard frame
        mov     ebp,esp
        pushad                      // Preserve all registers

        mov     eax,[ebp+0x14]      // argumentInfo
        push    eax
        mov     ecx,[ebp+0x10]      // func
        push    ecx
        mov     edx,[ebp+0x0C]      // clientData
        push    edx
        mov     eax,[ebp+0x08]      // functionID
        push    eax
        call    EnterGlobal

        popad                       // Restore all registers
        pop     ebp                 // Restore EBP
        ret     16
    }
}


void __stdcall LeaveGlobal(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_RANGE *retvalRange)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->Leave(functionID,clientData,frameInfo,retvalRange);
}

//Wird von der CLR aufgerufen
//Es werden keine Register gesichert, das muss manuell im Assembler Code geschehen
//http://www.codeproject.com/Articles/15410/Creating-a-Custom-NET-Profiler
void _declspec(naked) LeaveNaked2(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO func, COR_PRF_FUNCTION_ARGUMENT_RANGE *retvalRange)
{
    __asm
    {
        push    ebp                 // Create a standard frame
        mov     ebp,esp
        pushad                      // Preserve all registers

        mov     eax,[ebp+0x14]      // argumentInfo
        push    eax
        mov     ecx,[ebp+0x10]      // func
        push    ecx
        mov     edx,[ebp+0x0C]      // clientData
        push    edx
        mov     eax,[ebp+0x08]      // functionID
        push    eax
        call    LeaveGlobal

        popad                       // Restore all registers
        pop     ebp                 // Restore EBP
        ret     16
    }
} 


void __stdcall FunctionTailcallGlobal(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->Tailcall(functionID,clientData,frameInfo);
}


//Wird von der CLR aufgerufen
//Es werden keine Register gesichert, das muss manuell im Assembler Code geschehen
//http://www.codeproject.com/Articles/15410/Creating-a-Custom-NET-Profiler
void _declspec(naked) TailcallNaked2(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO func)
{
    __asm
    {
        push    ebp                 // Create a standard frame
        mov     ebp,esp
        pushad                      // Preserve all registers

        mov     ecx,[ebp+0x10]      // func
        push    ecx
        mov     edx,[ebp+0x0C]      // clientData
        push    edx
        mov     eax,[ebp+0x08]      // functionID
        push    eax
        call    FunctionTailcallGlobal
 
        popad                       // Restore all registers
        pop     ebp                 // Restore EBP
        ret     12
    }
}





CProfiler::CProfiler(void)
{
}


CProfiler::~CProfiler(void)
{
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
	_logger->WriteStringToLogFormat("Initialize reached\r\n");
	HRESULT hr;
	//globalen Pointer initialisieren
	_pICorProfilerCallback = this;

	//ICorProfilerInfo interface holen
    hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo, (LPVOID*) &_pICorProfilerInfo);
    if (FAILED(hr))  {
		_logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo failed\r\n");
		return E_FAIL;
	}

	//schauen ob auch ICorProfilerInfo2 implementiert wird (bis CLRV4)
	//wenn nicht, dann haben wir eine .Net-Version aelter als 2 vor uns 
    hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo2, (LPVOID*) &_pICorProfilerInfo2);
    if (FAILED(hr)){
		_logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo2 failed\r\n");
		_pICorProfilerInfo2.p = NULL;
	}

	hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo3,(LPVOID*) &_pICorProfilerInfo3);
	if (FAILED(hr)){
		//CLR Version 2 - 4
		_logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo3 failed\r\n");
		_pICorProfilerInfo3.p = NULL;
	}

		//Events setzen, um der CLR mitzuteilen an welchen Informationen man intessiert ist.
	hr = SetEventMask();
    if (FAILED(hr)){
		_pICorProfilerInfo->SetEventMask(COR_PRF_MONITOR_NONE);
		return E_FAIL;
	}
    

	//Die Enter, Leave und TailCall hooks werden gesetzt
	if (_pICorProfilerInfo.p!=NULL && _pICorProfilerInfo2.p== NULL)
	{
		hr = _pICorProfilerInfo->SetEnterLeaveFunctionHooks((FunctionEnter*)EnterNaked2, (FunctionLeave*)LeaveNaked2, (FunctionTailcall*)TailcallNaked2);
		_logger->WriteStringToLogFormat("set _pICorProfilerInfo->SetEnterLeaveFunctionHooks\r\n");    	
		if (SUCCEEDED(hr)){
			hr = _pICorProfilerInfo->SetFunctionIDMapper(FunctionMapper);
		}
	}
	else if(_pICorProfilerInfo2.p!=NULL && _pICorProfilerInfo3.p==NULL)
	{
		hr = _pICorProfilerInfo2->SetEnterLeaveFunctionHooks2((FunctionEnter2*)&EnterNaked2, (FunctionLeave2*)&LeaveNaked2, (FunctionTailcall2*)&TailcallNaked2);
		_logger->WriteStringToLogFormat("set _pICorProfilerInfo2->SetEnterLeaveFunctionHooks\r\n");    	
		if (SUCCEEDED(hr)){
			hr = _pICorProfilerInfo2->SetFunctionIDMapper(FunctionMapper);
		}
		
	}
	else if (_pICorProfilerInfo3.p!=NULL)
	{
		hr = _pICorProfilerInfo3->SetEnterLeaveFunctionHooks2((FunctionEnter2*)EnterNaked2, (FunctionLeave2*)LeaveNaked2, (FunctionTailcall2*)TailcallNaked2);
		_logger->WriteStringToLogFormat("set _pICorProfilerInfo3->SetEnterLeaveFunctionHooks2\r\n");  
		if (SUCCEEDED(hr))
			hr = _pICorProfilerInfo3->SetFunctionIDMapper(FunctionMapper);
	}

    
	if (FAILED(hr)){
		_logger->WriteStringToLogFormat("Unable to initialize profiler.");
	}
	_logger->WriteStringToLogFormat("Profiler initialized.");
    return S_OK;
}


void CProfiler::Enter(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo)
{
	CFunctionCharacteristics* characteristics = NULL;
	std::map<FunctionID, CFunctionCharacteristics*>::iterator iter = _functionCharacteristics.find(functionID);
	
	if (iter != _functionCharacteristics.end())
	{
		characteristics = (iter->second);
		characteristics->IncCallCount();
		
		int padCharCount = _callStackSize * 2;
		if (padCharCount > 0)
		{
			char* padding = new char[(padCharCount) + 1];
			memset(padding, 0, padCharCount + 1);
			memset(padding, '.', padCharCount);
			_logger-> WriteStringToLogFormat("Enter function name:%s%S, id=%d, call count = %d\r\n", padding, characteristics->GetFunctionName().c_str(), characteristics->GetFunctionId(), characteristics->GetCallCount());
			delete[] padding;
		}
		else
		{
			_logger->WriteStringToLogFormat("Enter function name:%S, id=%d, call count = %d\r\n", characteristics->GetFunctionName().c_str(), characteristics->GetFunctionId(), characteristics->GetCallCount());
		}
	}
	else _logger->WriteStringToLogFormat("Function:%i not found.", functionID);
	_callStackSize++;
}


void CProfiler::Leave(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo, COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange)
{
	if (_callStackSize > 0){
		_callStackSize--;
	}
}

void CProfiler::Tailcall(FunctionID functionID, UINT_PTR clientData, COR_PRF_FRAME_INFO frameInfo)
{
	if (_callStackSize > 0){
		_callStackSize--;
	}
}


///Von der CLR aufgerufen, wenn der Profiler beendet wird.
STDMETHODIMP CProfiler::Shutdown()
{
	_logger->WriteStringToLogFormat("Write full function list."); 
	std::map<FunctionID, CFunctionCharacteristics*>::iterator i;
	for (i = _functionCharacteristics.begin(); i != _functionCharacteristics.end(); i++){
		CFunctionCharacteristics* functionCharacteristics = i->second;
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
	std::map<FunctionID, CFunctionCharacteristics*>::iterator iter = _functionCharacteristics.find(functionID);
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
		CFunctionCharacteristics* characteristics;
		characteristics = new CFunctionCharacteristics(functionID, functionName);
		_functionCharacteristics.insert(std::pair<FunctionID, CFunctionCharacteristics*>(functionID, characteristics));
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


