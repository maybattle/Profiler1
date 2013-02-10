// dllmain.h : Declaration of module class.

class CProfiler1Module : public ATL::CAtlDllModuleT< CProfiler1Module >
{
public :
	DECLARE_LIBID(LIBID_Profiler1Lib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PROFILER1, "{5FBE2D20-D585-4AA3-BC43-44F6393B4C02}")
};

extern class CProfiler1Module _AtlModule;
