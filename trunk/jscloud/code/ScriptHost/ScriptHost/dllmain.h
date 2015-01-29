// dllmain.h : Declaration of module class.

class CScriptHostModule : public CAtlDllModuleT< CScriptHostModule >
{
public :
	DECLARE_LIBID(LIBID_ScriptHostLib)
	//DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SCRIPTHOST, "{2E4550F7-B5CE-49C2-B467-1DFCE6DE2576}")
};

extern class CScriptHostModule _AtlModule;
