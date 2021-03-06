#include "stdafx.h"
#include "Profiler.h"

//Pointer fuer globalen Zugriff 
CProfiler *_pICorProfilerCallback;


void __stdcall EnterGlobalWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->EnterWithInfo(functionIDOrClientID, eltInfo);
}




//Wird von der CLR aufgerufen
//Manuelles sichern und Wiederherstellen der Register
void __declspec(naked) EnterNaked3WithInfo(FunctionIDOrClientID functionIDOrClientID, 
										   COR_PRF_ELT_INFO eltInfo)
{
    __asm
    {
        push ebp
        mov ebp, esp 
        pushad
        mov edx, [ebp+0x0C]
        push edx
        mov eax, [ebp+0x08]
        push eax
        call EnterGlobalWithInfo;	
        popad
        pop ebp
        ret SIZE functionIDOrClientID + SIZE eltInfo
    }
}




void __stdcall LeaveGlobalWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->LeaveWithInfo(functionIDOrClientID, eltInfo);
}




//Wird von der CLR aufgerufen
//Manuelles sichern und Wiederherstellen der Register
void __declspec( naked ) LeaveNaked3WithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
    __asm
    {
        push ebp
		mov ebp, esp 
        pushad

		mov edx, [ebp+0x0C]
		push edx

		mov eax, [ebp+0x08]
		push eax

		call LeaveGlobalWithInfo;	
	
        popad
        pop ebp
        ret SIZE functionIDOrClientID + SIZE eltInfo
    }
}


void __stdcall TailcallGlobalWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
    if (_pICorProfilerCallback != NULL)
        _pICorProfilerCallback->TailcallWithInfo(functionIDOrClientID, eltInfo);
}



//Wird von der CLR aufgerufen
//Manuelles sichern und Wiederherstellen der Register
void __declspec( naked ) TailcallNaked3WithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
    __asm
    {
            
 push ebp
		mov ebp, esp 
        pushad

		mov edx, [ebp+0x0C]
		push edx

		mov eax, [ebp+0x08]
		push eax

		call TailcallGlobalWithInfo;	
    
	
        popad
        pop ebp
        ret SIZE functionIDOrClientID + SIZE eltInfo
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

      
    //Setzen des FunctionID Mappers
    //Wird von CLR aufgerufen, wenn sich FunctionIDs aendern. 
    //Es wird er Name der Funktion ermittelt und ein FunctionInformation Objekt erzeugt.
    //FunctionIDs und FunctionInformations werden in einer Map gespeichert.
    hr = _pICorProfilerInfo3->SetFunctionIDMapper(FunctionMapper);
    if (FAILED(hr)){
        _logger->WriteStringToLogFormat("SetFunctionIDMapper failed\r\n");
        return E_FAIL;
    }

	 //Events setzen, um der CLR mitzuteilen an welchen Informationen man intessiert ist.
    hr = SetEventMask();
    if (FAILED(hr)){
        _pICorProfilerInfo->SetEventMask(COR_PRF_MONITOR_NONE);
        return E_FAIL;
    }
	    
	//statt _pICorProfilerInfo3->SetEnterLeaveFunctionHooks3 werden jetzt die *WithInfo Funktionen als Callback verwendet.
	hr = _pICorProfilerInfo3->SetEnterLeaveFunctionHooks3WithInfo(
		(FunctionEnter3WithInfo*)EnterNaked3WithInfo,
		(FunctionLeave3WithInfo*)LeaveNaked3WithInfo,
		(FunctionTailcall3WithInfo*)TailcallNaked3WithInfo);

    if (FAILED(hr)){
        _logger->WriteStringToLogFormat("SetEnterLeaveFunctionHooks3 failed\r\n");
		return E_FAIL;
    }

    _logger->WriteStringToLogFormat("Profiler initialized\r\n");

    //globalen Pointer noch initialisieren
    _pICorProfilerCallback = this;
    return S_OK;
}

void CProfiler::EnterWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
    
    CFunctionInformation* functionInformation = NULL;
    std::unordered_map<FunctionID, CFunctionInformation*>::iterator 
        iter = _functionInformations.find(functionIDOrClientID.functionID);
    
    if (iter != _functionInformations.end())
    {
        functionInformation = (iter->second);
        functionInformation->IncCallCount();
        //die gleiche Funktion kann aufgrund einer Vererbungshierachie mehrmals aufgerufen werden. 
        //Es wird nur der erste Aufruf zur Ausgabe des Parameters herangezogen werden.
        wstring commandTextFunctionName = L"System.Data.SqlClient.SqlCommand.set_CommandText";
        wstring parameterValue = L"System.Data.SqlClient.SqlParameter.set_Value";
        //wstring parameterValue = L"SampleToProfile.Beamer.Beam";

        if(functionInformation->GetFunctionName().compare(commandTextFunctionName)==0 ||
            functionInformation->GetFunctionName().compare(parameterValue)==0){
            COR_PRF_FRAME_INFO frameInfo;
            ULONG cbArgumentInfo=0;
            COR_PRF_FUNCTION_ARGUMENT_INFO *pArgumentInfo;
            //Function muss 2mal aufgerufen werden. 
            //Das erste Aufruf mit cbArgumentInfo=0, pArgumentInfo=NULL
            //als Ergebnis steht in cbArgumentInfo die Groe�e des Speicherbereiches der Funktionsargumente und die Funktion liefert FAILED(hr)
            //Der zweite Aufruf mit dem Ergebnis von cbArgumentInfo des ersten Aufrufs und dem entsprechend grossen Speicherbereich von pArgumentInfo
            HRESULT hr=_pICorProfilerInfo3->GetFunctionEnter3Info(functionIDOrClientID.functionID, eltInfo,	&frameInfo, &cbArgumentInfo,NULL);
            if(FAILED(hr) && cbArgumentInfo>0){
                pArgumentInfo = new COR_PRF_FUNCTION_ARGUMENT_INFO[cbArgumentInfo];
                hr=_pICorProfilerInfo3->GetFunctionEnter3Info(functionIDOrClientID.functionID,eltInfo,	&frameInfo, &cbArgumentInfo,pArgumentInfo);
                if(SUCCEEDED(hr)) {
                    
                    //bei nicht statischen Funktionen, wird als erstes Argument immer der this Zeiger �bergeben
                    int argumentStartIndex = functionInformation->GetIsStatic()==TRUE?0:1;
                    for(int i=argumentStartIndex;i<pArgumentInfo->numRanges; i++){
                        
                        _logger->WriteStringToLogFormat("%S:\t",functionInformation->GetFunctionName().c_str());
                        switch(functionInformation->ParameterInformations[i-argumentStartIndex]->GetNativeType()){
                            case CorElementType::ELEMENT_TYPE_STRING:{
                                wstring value = GetStringValueFromArgumentRange(&(pArgumentInfo->ranges[i]));
                                _logger->WriteStringToLogFormat("%S\r\n",value.c_str());
                                break;
                            }
						
							case CorElementType::ELEMENT_TYPE_I4:{
								INT32 value = GetInt32ValueFromArgumentRange(&(pArgumentInfo->ranges[i]));
								_logger->WriteStringToLogFormat("%d\r\n",value);
								break;
							}

							case CorElementType::ELEMENT_TYPE_VALUETYPE:{
								if(!functionInformation->ParameterInformations[i-argumentStartIndex]->GetName().compare(L"System.DateTime")==0) continue;
								INT64 value = GetDateTimeValueFromArgumentRange(&(pArgumentInfo->ranges[i]));
								_logger->WriteStringToLogFormat("%I64d\r\n",value);
								break;
							}

							
							//System.Data.SqlClient.SqlParameter.set_Value erwartet einen Parameter vom Typ Object.
							//Um den Parameterwert zu ermitteln, muss ermittelt werden welcher Datentyp sich genau hinter Object verbirgt
							case CorElementType::ELEMENT_TYPE_OBJECT:{
									
									ObjectID objectID;
									memcpy(&objectID,((const void*)(pArgumentInfo->ranges[i].startAddress)),pArgumentInfo->ranges[i].length);
									ClassID classID;
									hr = _pICorProfilerInfo3->GetClassFromObject(objectID, &classID);
									if(FAILED(hr)) continue;

									ModuleID ModuleID;
									mdTypeDef mdTypeDefToken;
									_pICorProfilerInfo3->GetClassIDInfo(classID,&ModuleID, &mdTypeDefToken);
									 IMetaDataImport* pIMetaDataImport = 0;
									hr = _pICorProfilerInfo3->GetModuleMetaData(ModuleID,CorOpenFlags::ofRead,IID_IMetaDataImport, (LPUNKNOWN *) &pIMetaDataImport);
									if(FAILED(hr)) continue;

									WCHAR szClass[MAX_CLASSNAME_LENGTH];
									ULONG cchClass;
									hr = pIMetaDataImport->GetTypeDefProps(mdTypeDefToken,szClass, MAX_CLASSNAME_LENGTH, &cchClass,0,0);
									if(pIMetaDataImport!=NULL) pIMetaDataImport->Release();
									if(FAILED(hr)) continue;
									
									ULONG32 pBufferOffset;
									hr = _pICorProfilerInfo3->GetBoxClassLayout(classID, &pBufferOffset);
									//hier nicht auf FAILED pruefen, da bei einem String HRESULT != 0 ist

									wstring typeName = szClass; 
									
									if(typeName.compare(L"System.Int32")==0){
										INT32 value;
										value = GetInt32ValueFromArgument(objectID+pBufferOffset);
										_logger->WriteStringToLogFormat("%S\t%d\r\n",typeName.c_str(),value);
									}

									if(typeName.compare(L"System.DateTime")==0){
										INT64 value;
										value = GetDateTimeValueFromArgument(objectID+pBufferOffset);
										_logger->WriteStringToLogFormat("%S\t%I64d\r\n",typeName.c_str(),value);
									}
									
									if(typeName.compare(L"System.String")==0){
										wstring value;
										value = GetStringValueFromArgument(objectID);
										_logger->WriteStringToLogFormat("%S\t%S\r\n",typeName.c_str(), value.c_str());
									}

									break;
								}
						}
					}
				}
				else{
					_logger->WriteStringToLogFormat("Error getting parameter info for function:%S",functionInformation->GetFunctionName().c_str());		
				}
			}
		}
    }
    else _logger->WriteStringToLogFormat("Function:%i not found.", functionIDOrClientID);
    _callStackSize++;
}


void CProfiler::LeaveWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
	//Rueckgabewerte von Funktionen koennen stehen nur in der Leave Funktion zur Verfuegung.
	//Um die Rueckgabewerte zu erhalten muss das Event: COR_PRF_ENABLE_FUNCTION_RETVAL aktiviert sein.
	//Die Funktion GetBeamerState befindet sich in SampleToProfile_CLR4_x86
	//Der nachfolgende Quelltext kann einkommentiert und mit dem Beispiel SampleToProfile_CLR4_x86 verwendet werden.

	/*
	wstring functionName = L"SampleToProfile.Beamer.GetBeamerState";
	CFunctionInformation* functionInformation = NULL;
    std::unordered_map<FunctionID, CFunctionInformation*>::iterator 
    iter = _functionInformations.find(functionIDOrClientID.functionID);
	if (iter != _functionInformations.end())
    {
        functionInformation = (iter->second);

		if(functionInformation->GetFunctionName().compare(functionName)==0){
			COR_PRF_FRAME_INFO frameInfo;
			COR_PRF_FUNCTION_ARGUMENT_RANGE retvalRange;

			HRESULT hr=_pICorProfilerInfo3->GetFunctionLeave3Info(functionIDOrClientID.functionID, eltInfo,	&frameInfo, &retvalRange);
			if(SUCCEEDED(hr)){

				if((functionInformation->GetReturnTypeInformation())->GetNativeType()==CorElementType::ELEMENT_TYPE_STRING){
					wstring value = GetStringValueFromArgumentRange(&retvalRange);
					_logger->WriteStringToLogFormat("Return value for:%S is:%S\r\n",functionInformation->GetFunctionName().c_str(), value.c_str());		
				}
			}
			else{
				_logger->WriteStringToLogFormat("Error getting return value for function:%S\r\n",functionInformation->GetFunctionName().c_str());		
			}
		}
	}*/
	
	if (_callStackSize > 0)
    {
        _callStackSize--;
    }
}


void CProfiler::TailcallWithInfo(FunctionIDOrClientID functionIDOrClientID, COR_PRF_ELT_INFO eltInfo)
{
    if (_callStackSize > 0)
    {
        _callStackSize--;
    }
}


wstring CProfiler::GetStringValueFromArgumentRange(const COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange){
	if(argumentRange== NULL) return NULL;
	ObjectID stringOID;
	memcpy(&stringOID,((const void*)(argumentRange->startAddress)),argumentRange->length);
	return GetStringValueFromArgument(stringOID);
}

wstring CProfiler::GetStringValueFromArgument(ObjectID stringOID){
	string returnValue;
	ULONG pBufferLengthOffset;
	ULONG pStringLengthOffset;
	ULONG pBufferOffset;
	_pICorProfilerInfo3->GetStringLayout(&pBufferLengthOffset, &pStringLengthOffset, &pBufferOffset);
	
	DWORD stringLength;
	memcpy(&stringLength,((const void*)(stringOID+pStringLengthOffset)),sizeof(DWORD));

	WCHAR *parameterValue = new WCHAR[(stringLength+1)*sizeof(DWORD)];
	memcpy(parameterValue,((const void*)(stringOID+pBufferOffset)),stringLength * sizeof(DWORD));
	parameterValue[stringLength*sizeof(DWORD)]= '\0';
	wstring result = parameterValue;
	delete[] parameterValue;
	return result;
}

INT32 CProfiler::GetInt32ValueFromArgumentRange(const COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange){
	if(argumentRange== NULL) return MININT32;
	return GetInt32ValueFromArgument(argumentRange->startAddress);
}

INT32 CProfiler::GetInt32ValueFromArgument(UINT_PTR startAddress){
	INT32 value;
	value = *(INT32*)startAddress;
	return value;
}

INT64 CProfiler::GetDateTimeValueFromArgumentRange(const COR_PRF_FUNCTION_ARGUMENT_RANGE *argumentRange){
	if(argumentRange== NULL) return MININT64;
	return GetDateTimeValueFromArgument(argumentRange->startAddress);
}

INT64 CProfiler::GetDateTimeValueFromArgument(UINT_PTR startAddress){
	INT64 value;
	value=	*(INT64*)(startAddress);
	//siehe DateTime Implementierung Microsoft  
	value = value & 0x3FFFFFFFFFFFFFFF;
	//Value ist Anzahl der Ticks (100 Nanosekunden Intervalle) seit dem 01.1.0001
	//http://msdn.microsoft.com/de-de/library/system.datetime.ticks.aspx
	return value;
}


///Von der CLR aufgerufen, wenn der Profiler beendet wird.
STDMETHODIMP CProfiler::Shutdown()
{
	std::unordered_map<FunctionID, CFunctionInformation*>::iterator i;
	for (i = _functionInformations.begin(); i != _functionInformations.end(); i++){
		CFunctionInformation* functionCharacteristics = i->second;
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
		CFunctionInformation* characteristics;
		characteristics = GetFunctionInformation(functionID); 
		if (characteristics==NULL)
		{
			_logger->WriteStringToLogFormat("Unable to assign function information for functionID:%d\r\n",functionID);
			return;
		}	
		_functionInformations.insert(std::pair<FunctionID, CFunctionInformation*>(functionID, characteristics));
	}
}


//Methodeninformationen (Name, Parameter, Rueckgabewert) aus functionId ermitteln
CFunctionInformation* CProfiler::GetFunctionInformation(FunctionID functionId)
 {
    IMetaDataImport* pIMetaDataImport = 0;
    HRESULT hr = S_OK;
    mdToken funcToken = 0;
    WCHAR szFunction[MAX_CLASSNAME_LENGTH];
    WCHAR szClass[MAX_CLASSNAME_LENGTH];
	CFunctionInformation* characteristics = NULL;

    //Hier werden das Metadata Token und die Metadata Instanz geholt, 
    //um anschliessend mit der Metadata API den Functionsnamen zu ermitteln
    //http://msdn.microsoft.com/en-us/library/ms233167.aspx
    hr = _pICorProfilerInfo->GetTokenAndMetaDataFromFunction(functionId, IID_IMetaDataImport, (LPUNKNOWN *) &pIMetaDataImport, &funcToken);
    if(SUCCEEDED(hr))
    {
        mdTypeDef classTypeDef;
        ULONG cchFunction;
        ULONG cchClass;
        DWORD dwAttr;
		PCCOR_SIGNATURE pMethodSigBlob;
		ULONG cbMethodSigBlob;
		ULONG ulCodeRVA = 0;
		DWORD dwImplFlags = 0;

        //Metadaten der Methode ermitteln
        //http://msdn.microsoft.com/en-us/library/ms233163.aspx
        hr = pIMetaDataImport->GetMethodProps(
				funcToken, 
				&classTypeDef, 
				szFunction, 
				MAX_CLASSNAME_LENGTH, 
				&cchFunction, 
				&dwAttr, 
				&pMethodSigBlob, 
				&cbMethodSigBlob, 
				&ulCodeRVA,
				&dwImplFlags);

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
                characteristics = new CFunctionInformation(functionId,fullFunctionName);
				characteristics->SetIsStatic((dwAttr & mdStatic)==mdStatic);
				ULONG callConvention;
				ULONG argumentCount = 0;
				INT32 typeAsEnum;

				pMethodSigBlob += CorSigUncompressData( pMethodSigBlob, &callConvention);
				if ( callConvention != IMAGE_CEE_CS_CALLCONV_FIELD) {
					pMethodSigBlob += CorSigUncompressData(pMethodSigBlob,&argumentCount);
					std::wstring returnValueTypeName;
					pMethodSigBlob = ParseElementType(pIMetaDataImport, pMethodSigBlob, returnValueTypeName, &typeAsEnum);	
					CParameterInformation* pReturnTypeInformation = new CParameterInformation(returnValueTypeName,(CorElementType)typeAsEnum,0,DIRECTION_OUT,TRUE);
					characteristics->SetReturnTypeInformation(pReturnTypeInformation);
					
					for (ULONG i=0; i<argumentCount; i++){
						std::wstring parameterTypeName;
						pMethodSigBlob = ParseElementType( pIMetaDataImport, pMethodSigBlob, parameterTypeName, &typeAsEnum);	
						CParameterInformation* pParameterInformation = new CParameterInformation(parameterTypeName,(CorElementType)typeAsEnum,i);
						characteristics->ParameterInformations.push_back(pParameterInformation);
					}
				}
            }
        }
        pIMetaDataImport->Release();
    }
    return characteristics;
}

//http://www.microsoft.com/en-us/download/details.aspx?id=16273
//http://clrprofiler.codeplex.com/wikipage?title=Writing%20My%20Own%20Profiler&referringTitle=Home
PCCOR_SIGNATURE CProfiler::ParseElementType(IMetaDataImport *metaDataImport, PCCOR_SIGNATURE signature, wstring &signatureText, INT32 *pElementType)
{	
	COR_SIGNATURE corSignature = *signature++;
	
	*pElementType = corSignature;
	switch (corSignature) 
	{	
	case ELEMENT_TYPE_VOID:
		signatureText.append(L"void");
		break;							
	case ELEMENT_TYPE_BOOLEAN:	
		signatureText.append(L"bool");	
		break;			
	case ELEMENT_TYPE_CHAR:
		signatureText.append(L"wchar");	
		break;							
	case ELEMENT_TYPE_I1:
		signatureText.append(L"int8");	
		break;		 		
	case ELEMENT_TYPE_U1:
		signatureText.append(L"unsigned int8");	
		break;	 		
	case ELEMENT_TYPE_I2:
		signatureText.append(L"int16");	
		break;		
	case ELEMENT_TYPE_U2:
		signatureText.append(L"unsigned int16");	
		break;					
	case ELEMENT_TYPE_I4:
		signatureText.append(L"int32");	
		break;        
	case ELEMENT_TYPE_U4:
		signatureText.append(L"unsigned int32");	
		break;				
	case ELEMENT_TYPE_I8:
		signatureText.append(L"int64");	
		break;				
	case ELEMENT_TYPE_U8:
		signatureText.append(L"unsigned int64");	
		break;		
	case ELEMENT_TYPE_R4:
		signatureText.append(L"float32");	
		break;					
	case ELEMENT_TYPE_R8:
		signatureText.append(L"float64");	
		break;			 		
	case ELEMENT_TYPE_STRING:
		signatureText.append(L"String");	
		break;	
	case ELEMENT_TYPE_VAR:
		signatureText.append(L"class variable(unsigned int8)");	
		break;		
	case ELEMENT_TYPE_MVAR:
		signatureText.append(L"method variable(unsigned int8)");	
		break;					         
	case ELEMENT_TYPE_TYPEDBYREF:
		signatureText.append(L"refany");	
		break;		 		
	case ELEMENT_TYPE_I:
		signatureText.append(L"int");	
		break;			
	case ELEMENT_TYPE_U:
		signatureText.append(L"unsigned int");	
		break;			  		
	case ELEMENT_TYPE_OBJECT:
		signatureText.append(L"Object");	
		break;		       
	case ELEMENT_TYPE_SZARRAY:	 
		signature = ParseElementType(metaDataImport, signature, signatureText, pElementType); 
		signatureText.append(L"[]");
		break;				
	case ELEMENT_TYPE_PINNED:
		signature = ParseElementType(metaDataImport, signature, signatureText, pElementType); 
		signatureText.append(L"pinned");	
		break;	        
	case ELEMENT_TYPE_PTR:   
		signature = ParseElementType(metaDataImport, signature, signatureText, pElementType); 
		signatureText.append(L"*");	
		break;           
	case ELEMENT_TYPE_BYREF:   
		signature = ParseElementType(metaDataImport, signature, signatureText, pElementType); 
		signatureText.append(L"&");	
		break;
	case ELEMENT_TYPE_VALUETYPE:   
	case ELEMENT_TYPE_CLASS:	
	case ELEMENT_TYPE_CMOD_REQD:
	case ELEMENT_TYPE_CMOD_OPT:
		{
			mdToken	token;	
			signature += CorSigUncompressToken( signature, &token ); 

			WCHAR className[ MAX_CLASSNAME_LENGTH ];
			if ( TypeFromToken( token ) == mdtTypeRef )
			{
				metaDataImport->GetTypeRefProps(token, NULL, className, MAX_CLASSNAME_LENGTH, NULL);
			}
			else
			{
				metaDataImport->GetTypeDefProps(token, className, MAX_CLASSNAME_LENGTH, NULL, NULL, NULL );
			}

			signatureText.append(className );
		}
		break;		
	case ELEMENT_TYPE_GENERICINST:
		{
			signature = ParseElementType(metaDataImport, signature, signatureText, pElementType); 

			signatureText.append(L"<");	
			ULONG arguments = CorSigUncompressData(signature);
			for (ULONG i = 0; i < arguments; ++i)
			{
				if(i != 0)
				{
					signatureText.append(L", ");	
				}

				signature = ParseElementType(metaDataImport, signature, signatureText, pElementType);
			}
			signatureText.append(L">");	
		}
		break;		        
	case ELEMENT_TYPE_ARRAY:	
		{
			signature = ParseElementType(metaDataImport, signature, signatureText, pElementType);              
			ULONG rank = CorSigUncompressData(signature);													
			if ( rank == 0 ) 
			{
				signatureText.append(L"[?]");
			}
			else 
			{		
				ULONG arraysize = (sizeof(ULONG) * 2 * rank);
				ULONG *lower = (ULONG *)_alloca(arraysize);
				memset(lower, 0, arraysize); 
				ULONG *sizes = &lower[rank];

				ULONG numsizes = CorSigUncompressData(signature);	
				for (ULONG i = 0; i < numsizes && i < rank; i++)
				{
					sizes[i] = CorSigUncompressData(signature);	
				}

				ULONG numlower = CorSigUncompressData(signature);	
				for (ULONG i = 0; i < numlower && i < rank; i++)	
				{
					lower[i] = CorSigUncompressData( signature ); 
				}

				signatureText.append(L"[");	
				for (ULONG i = 0; i < rank; ++i)	
				{					
					if (i > 0) 
					{
						signatureText.append(L",");
					}

					if (lower[i] == 0)
					{
						if(sizes[i] != 0)
						{
							WCHAR *size = new WCHAR[MAX_BUFFER_SIZE];
							size[0] = '\0';
							wsprintf(size, L"%d", sizes[i]);											
							signatureText.append(size);
						}
					}	
					else	
					{
						WCHAR *low = new WCHAR[MAX_BUFFER_SIZE];
						low[0] = '\0';
						wsprintf(low, L"%d", lower[i]);
						signatureText.append(low);
						signatureText.append(L"...");	

						if (sizes[i] != 0)	
						{
							WCHAR *size = new WCHAR[MAX_BUFFER_SIZE];
							size[0] = '\0';
							wsprintf(size, L"%d", (lower[i] + sizes[i] + 1));
							signatureText.append(size);
						}
					}
				}
				signatureText.append(L"]");
			}
		} 
		break;   		    
	default:	
	case ELEMENT_TYPE_END:	
	case ELEMENT_TYPE_SENTINEL:	
		WCHAR *elementType = new WCHAR[MAX_BUFFER_SIZE];
		elementType[0] = '\0';
		wsprintf(elementType, L"<UNKNOWN:0x%X>", corSignature);
		signatureText.append(elementType);
		break;				                      				            
	}

	return signature;
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
    
    DWORD eventMask = (DWORD) (COR_PRF_MONITOR_ENTERLEAVE 
								| COR_PRF_ENABLE_FUNCTION_ARGS 
								| COR_PRF_ENABLE_FUNCTION_RETVAL
							  );
	
	_logger->WriteStringToLogFormat("Set EventMask to COR_PRF_MONITOR_ENTERLEAVE | COR_PRF_ENABLE_FUNCTION_ARGS | COR_PRF_ENABLE_FUNCTION_RETVAL\r\n");
    return _pICorProfilerInfo3->SetEventMask(eventMask);
}



