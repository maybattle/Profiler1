// Profiler.h : Declaration of the CProfiler

#pragma once
#include "resource.h"       // main symbols
#include "Profiler1_i.h"
#include "stdafx.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#define MAX_BUFFER_SIZE  1024
#define MAX_FUNCTIONNAME_LENGTH  1024
#define MAX_CLASSNAME_LENGTH 1024
#define MAX_PARAMETERNAME_LENGTH 1024

// CProfiler

class ATL_NO_VTABLE CProfiler :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CProfiler, &CLSID_Profiler>,
	public CorProfilerCallback
{
public:
	CProfiler();

	DECLARE_REGISTRY_RESOURCEID(IDR_PROFILER)
	DECLARE_NOT_AGGREGATABLE(CProfiler)
	BEGIN_COM_MAP(CProfiler)
		COM_INTERFACE_ENTRY(ICorProfilerCallback)
		COM_INTERFACE_ENTRY(ICorProfilerCallback2)
		COM_INTERFACE_ENTRY(ICorProfilerCallback3)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();
	STDMETHOD(Initialize)(IUnknown *pICorProfilerInfoUnk);
	STDMETHOD(Shutdown)();
	
	

	//Wird von der CLR aufgerufen, wenn der Funktion eine ID zugeordnet wird.
	//Zu den FunctionIDs werden die Funktionsnamen ermittelt und zusammen mit der FunctionID in einer Map gespeichert
	static UINT_PTR _stdcall FunctionMapper(FunctionID functionId, BOOL *pbHookFunction);

	//Callback Funktionen
	//void Enter(FunctionIDOrClientID functionIDOrClientID);
	void EnterWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo);
	//void Leave(FunctionIDOrClientID functionIDOrClientID);
	void LeaveWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo);
	//void Tailcall(FunctionIDOrClientID functionIDOrClientID);
	void TailcallWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo);
	
	~CProfiler(void);


private:
	CComQIPtr<ICorProfilerInfo> _pICorProfilerInfo;
	CComQIPtr<ICorProfilerInfo2> _pICorProfilerInfo2;
	CComQIPtr<ICorProfilerInfo3> _pICorProfilerInfo3;

	void AddFunctionToMap(FunctionID);
	STDMETHOD(SetEventMask)();
	CFunctionInformation* CProfiler::GetFunctionInformation(FunctionID functionId);
	wstring CProfiler::GetStringValueFromArgumentRange(const COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange);
	INT32 CProfiler::GetInt32ValueFromArgumentRange(const COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange);
	PCCOR_SIGNATURE CProfiler::ParseElementType( IMetaDataImport *metaDataImport, PCCOR_SIGNATURE signature, wstring &signatureText, INT32 *pElementType);
	
	//Map Speichert alle Funktionen, um sie am Ende mit dem CallCount auszugeben
	std::unordered_map<FunctionID, CFunctionInformation*> _functionInformations;

	int _callStackSize;
	CLogger* _logger; 

};

//globale Referenz auf den Profiler
extern CProfiler *_pICorProfilerCallback;    

OBJECT_ENTRY_AUTO(__uuidof(Profiler), CProfiler)
