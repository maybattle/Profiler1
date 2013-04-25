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
	_logger->WriteStringToLogFormat("Start Logging\r\n");
	return S_OK;
}

void CProfiler::FinalRelease()
{
	delete _logger;
}


//Wird von der CLR aufgerufen, um den Profiler zu initialisieren.
//RETURN von E_FAIL teilt der CLR einen Fehler oder kein Interesse am Profilen des Prozesses mit  
HRESULT CProfiler::Initialize(IUnknown *pICorProfilerInfoUnk)
{
    HRESULT hr;
    WCHAR* buffer =new WCHAR[256];
    int clrVersion = 4;
    wmemset(buffer,'0',sizeof(buffer));
    //Umgebungsvariable holen, um die CLR Version zu pruefen
    if(GetEnvironmentVariable(L"CLR_VERSION",buffer, 256)>0){
        clrVersion = _wtoi(buffer);
    }
    _logger->WriteStringToLogFormat("CLR version: %d\r\n", clrVersion);
    if(clrVersion<4) {
        return E_FAIL;
    }
    delete[] buffer;
    //ICorProfilerInfo interface holen, das wird auf jeden Fall benoetigt
    hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo, (LPVOID*) &_pICorProfilerInfo);
    if (FAILED(hr))  {
        _logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo failed\r\n");
        return E_FAIL;
    }

    //Nur CLR 4 probieren, wenn das nicht funktioniert, liegt CLR Version kleiner 4 vor. 
    hr = pICorProfilerInfoUnk->QueryInterface(IID_ICorProfilerInfo3,(LPVOID*) &_pICorProfilerInfo3);
    if (FAILED(hr)){
        _logger->WriteStringToLogFormat("QueryInterface IID_ICorProfilerInfo3 failed\r\n");
        return E_FAIL;
    }

    //Events setzen, um der CLR mitzuteilen an welchen Informationen man intessiert ist.
    hr = SetEventMask();
    if (FAILED(hr)){
        _pICorProfilerInfo->SetEventMask(COR_PRF_MONITOR_NONE);
        return E_FAIL;
    }
    
    //Setzen des FunctionID Mappers
    //Wird von CLR aufgerufen, wenn sich FunctionIDs aendern. 
    //Es wird er Name der Funktion ermittelt und ein FunctionInformation Objekt erzeugt.
    //FunctionIDs und FunctionInformations werden in einer Map gespeichert.
    hr = _pICorProfilerInfo3->SetFunctionIDMapper(FunctionMapper);
    if (FAILED(hr)){
        _logger->WriteStringToLogFormat("SetEnterLeaveFunctionHooks3 failed\r\n");
        return E_FAIL;
    }

    //Setzen der Enter, Leave, Tailcall Hooks
    hr = _pICorProfilerInfo3->SetEnterLeaveFunctionHooks3(
            (FunctionEnter3*)EnterNaked3, 
            (FunctionLeave3*)LeaveNaked3, 
            (FunctionTailcall3*)TailcallNaked3);

    if (FAILED(hr)){
        _logger->WriteStringToLogFormat("SetEnterLeaveFunctionHooks3 failed\r\n");
    }

    _logger->WriteStringToLogFormat("Profiler initialized\r\n");

    //globalen Pointer noch initialisieren
    _pICorProfilerCallback = this;
    return S_OK;
}


void CProfiler::Enter(FunctionIDOrClientID functionIDOrClientID)
{
    CFunctionInformation* functionInformation = NULL;
    std::unordered_map<FunctionID, CFunctionInformation*>::iterator 
        iter = _functionInformations.find(functionIDOrClientID.functionID);
    
    if (iter != _functionInformations.end())
    {
        functionInformation = (iter->second);
        functionInformation->IncCallCount();
        
        int padCharCount = _callStackSize * 2;
        if (padCharCount > 0)
        {
            char* padding = new char[(padCharCount) + 1];
            memset(padding, 0, padCharCount + 1);
            memset(padding, '.', padCharCount);
            _logger-> WriteStringToLogFormat("Enter function name:%s%S, id=%d\r\n", 
                padding, functionInformation->GetFunctionName().c_str(), 
                functionInformation->GetFunctionId());
            delete[] padding;
        }
        else
        {
            _logger->WriteStringToLogFormat("Enter function name:%S, id=%d\r\n", 
                functionInformation->GetFunctionName().c_str(), 
                functionInformation->GetFunctionId());
        }
    }
    else _logger->WriteStringToLogFormat("Function:%i not found.", functionIDOrClientID);
    _callStackSize++;
}


void CProfiler::Leave(FunctionIDOrClientID functionIDOrClientID)
{
    if (_callStackSize > 0)
    {
        _callStackSize--;
    }
}

void CProfiler::Tailcall(FunctionIDOrClientID functionIDOrClientID)
{
    if (_callStackSize > 0)
    {
        _callStackSize--;
    }
}


///Von der CLR aufgerufen, wenn der Profiler beendet wird.
STDMETHODIMP CProfiler::Shutdown()
{
	_logger->WriteStringToLogFormat("Write full function list\r\n"); 
	std::unordered_map<FunctionID, CFunctionInformation*>::iterator i;
	for (i = _functionInformations.begin(); i != _functionInformations.end(); i++){
		CFunctionInformation* functionCharacteristics = i->second;
		_logger->WriteStringToLogFormat("%S : call count = %d\r\n", 
			functionCharacteristics->GetFunctionName().c_str(),
			functionCharacteristics->GetCallCount());
		delete i->second;
	}
	_functionInformations.clear();
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

//Es wird er Name der Funktion ermittelt und ein FunctionInformation Objekt erzeugt.
//FunctionIDs und FunctionInformations werden in einer Map gespeichert.
void CProfiler::AddFunctionToMap(FunctionID functionID)
{
	std::unordered_map<FunctionID, CFunctionInformation*>::iterator iter = _functionInformations.find(functionID);
	//Wenn functionID noch nicht vorhanden, dann einfuegen.
	if (iter == _functionInformations.end())
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
		_functionInformations.insert(std::pair<FunctionID, CFunctionInformation*>(functionID, characteristics));
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

    //Hier werden das Metadata Token und die Metadata Instanz geholt, 
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
                size_t length = MAX_CLASSNAME_LENGTH*2;
                WCHAR fullFunctionName[MAX_CLASSNAME_LENGTH];
                _snwprintf_s(fullFunctionName,length,L"%s.%s",szClass,szFunction);
                functionName = fullFunctionName;
            }
        }
        //WICHTIG: abschliessend immer Release aufrufen.
        pIMetaDataImport->Release();
    }
    return hr;
}



//Setzen der Events, an denen man interessiert ist. 
//Aktuell wird nur das COR_PRF_MONITOR_ENTERLEAVE verwendet
//um die Funktionsaufrufe zu tracen
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
    //COR_PRF_ENABLE_FUNCTION_ARGS	= 0x2000000,
    //COR_PRF_ENABLE_FUNCTION_RETVAL	= 0x4000000,
    //COR_PRF_ENABLE_FRAME_INFO	= 0x8000000,
    //COR_PRF_ENABLE_STACK_SNAPSHOT	= 0x10000000,
    //COR_PRF_USE_PROFILE_IMAGES	= 0x20000000,
    //COR_PRF_ALL	= 0x3fffffff,
    //COR_PRF_MONITOR_IMMUTABLE	= COR_PRF_MONITOR_CODE_TRANSITIONS | COR_PRF_MONITOR_REMOTING | COR_PRF_MONITOR_REMOTING_COOKIE | COR_PRF_MONITOR_REMOTING_ASYNC | COR_PRF_MONITOR_GC | COR_PRF_ENABLE_REJIT | COR_PRF_ENABLE_INPROC_DEBUGGING | COR_PRF_ENABLE_JIT_MAPS | COR_PRF_DISABLE_OPTIMIZATIONS | COR_PRF_DISABLE_INLINING | COR_PRF_ENABLE_OBJECT_ALLOCATED | COR_PRF_ENABLE_FUNCTION_ARGS | COR_PRF_ENABLE_FUNCTION_RETVAL | COR_PRF_ENABLE_FRAME_INFO | COR_PRF_ENABLE_STACK_SNAPSHOT | COR_PRF_USE_PROFILE_IMAGES
    
    DWORD eventMask = (DWORD) (COR_PRF_MONITOR_ENTERLEAVE);
    _logger->WriteStringToLogFormat("Set EventMask to COR_PRF_MONITOR_ENTERLEAVE\r\n");

    return _pICorProfilerInfo->SetEventMask(eventMask);
}


