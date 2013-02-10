#pragma once
#include "stdafx.h"
#include "CorProfilerCallback.h"


CorProfilerCallback::CorProfilerCallback(void)
{
}

STDMETHODIMP CorProfilerCallback::Initialize(IUnknown *pICorProfilerInfoUnk)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::Shutdown()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::AppDomainCreationStarted(AppDomainID appDomainID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::AppDomainCreationFinished(AppDomainID appDomainID, HRESULT hrStatus)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::AppDomainShutdownStarted(AppDomainID appDomainID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::AppDomainShutdownFinished(AppDomainID appDomainID, HRESULT hrStatus)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::AssemblyLoadStarted(AssemblyID assemblyID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::AssemblyLoadFinished(AssemblyID assemblyID, HRESULT hrStatus)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::AssemblyUnloadStarted(AssemblyID assemblyID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::AssemblyUnloadFinished(AssemblyID assemblyID, HRESULT hrStatus)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ModuleLoadStarted(ModuleID moduleID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ModuleLoadFinished(ModuleID moduleID, HRESULT hrStatus)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ModuleUnloadStarted(ModuleID moduleID)
{
    return S_OK;
}
	  
STDMETHODIMP CorProfilerCallback::ModuleUnloadFinished(ModuleID moduleID, HRESULT hrStatus)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallback::ModuleAttachedToAssembly(ModuleID moduleID, AssemblyID assemblyID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ClassLoadStarted(ClassID classID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ClassLoadFinished(ClassID classID, HRESULT hrStatus)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ClassUnloadStarted(ClassID classID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ClassUnloadFinished(ClassID classID, HRESULT hrStatus)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::FunctionUnloadStarted(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::JITCompilationStarted(FunctionID functionID, BOOL fIsSafeToBlock)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::JITCompilationFinished(FunctionID functionID, HRESULT hrStatus, BOOL fIsSafeToBlock)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::JITCachedFunctionSearchStarted(FunctionID functionID, BOOL *pbUseCachedFunction)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::JITCachedFunctionSearchFinished(FunctionID functionID, COR_PRF_JIT_CACHE result)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::JITFunctionPitched(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::JITInlining(FunctionID callerID, FunctionID calleeID, BOOL *pfShouldInline)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::UnmanagedToManagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ManagedToUnmanagedTransition(FunctionID functionID, COR_PRF_TRANSITION_REASON reason)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ThreadCreated(ThreadID threadID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ThreadDestroyed(ThreadID threadID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ThreadAssignedToOSThread(ThreadID managedThreadID, DWORD osThreadID) 
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RemotingClientInvocationStarted()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RemotingClientSendingMessage(GUID *pCookie, BOOL fIsAsync)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RemotingClientReceivingReply(GUID *pCookie, BOOL fIsAsync)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RemotingClientInvocationFinished()
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallback::RemotingServerReceivingMessage(GUID *pCookie, BOOL fIsAsync)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RemotingServerInvocationStarted()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RemotingServerInvocationReturned()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RemotingServerSendingReply(GUID *pCookie, BOOL fIsAsync)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RuntimeSuspendStarted(COR_PRF_SUSPEND_REASON suspendReason)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RuntimeSuspendFinished()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RuntimeSuspendAborted()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RuntimeResumeStarted()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RuntimeResumeFinished()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RuntimeThreadSuspended(ThreadID threadID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RuntimeThreadResumed(ThreadID threadID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::MovedReferences(ULONG cmovedObjectIDRanges, ObjectID oldObjectIDRangeStart[], ObjectID newObjectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ObjectAllocated(ObjectID objectID, ClassID classID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ObjectsAllocatedByClass(ULONG classCount, ClassID classIDs[], ULONG objects[])
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ObjectReferences(ObjectID objectID, ClassID classID, ULONG objectRefs, ObjectID objectRefIDs[])
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RootReferences(ULONG rootRefs, ObjectID rootRefIDs[])
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionThrown(ObjectID thrownObjectID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionUnwindFunctionEnter(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionUnwindFunctionLeave()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionSearchFunctionEnter(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionSearchFunctionLeave()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionSearchFilterEnter(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionSearchFilterLeave()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionSearchCatcherFound(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionCLRCatcherFound()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionCLRCatcherExecute()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionOSHandlerEnter(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionOSHandlerLeave(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionUnwindFinallyEnter(FunctionID functionID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionUnwindFinallyLeave()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionCatcherEnter(FunctionID functionID,
    											 ObjectID objectID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ExceptionCatcherLeave()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::COMClassicVTableCreated(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable, ULONG cSlots)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::COMClassicVTableDestroyed(ClassID wrappedClassID, REFGUID implementedIID, void *pVTable)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::ThreadNameChanged(ThreadID threadID, ULONG cchName, WCHAR name[])
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallback::GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason)
{
	return S_OK;
}

STDMETHODIMP CorProfilerCallback::SurvivingReferences(ULONG cSurvivingObjectIDRanges, ObjectID objectIDRangeStart[], ULONG cObjectIDRangeLength[])
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::GarbageCollectionFinished()
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::FinalizeableObjectQueued(DWORD finalizerFlags, ObjectID objectID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::RootReferences2(ULONG cRootRefs, ObjectID rootRefIDs[], COR_PRF_GC_ROOT_KIND rootKinds[], COR_PRF_GC_ROOT_FLAGS rootFlags[], UINT_PTR rootIDs[])
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::HandleCreated(GCHandleID handleID, ObjectID initialObjectID)
{
    return S_OK;
}

STDMETHODIMP CorProfilerCallback::HandleDestroyed(GCHandleID handleID)
{
    return S_OK;
}


STDMETHODIMP CorProfilerCallback::InitializeForAttach(IUnknown * pCorProfilerInfoUnk, void * pvClientData, UINT cbClientData){
	return S_OK;
}

STDMETHODIMP CorProfilerCallback::ProfilerAttachComplete(){
	return S_OK;
}
	
STDMETHODIMP CorProfilerCallback::ProfilerDetachSucceeded(){
	return S_OK;
}
