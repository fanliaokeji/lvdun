// dllmain.h : Declaration of module class.

class CBhoAddinModule : public CAtlDllModuleT< CBhoAddinModule >
{
public :
	DECLARE_LIBID(LIBID_BhoAddinLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BHOADDIN, "{9DB694C8-D1D0-4280-A2DC-DDDFF7BDE757}")
};

extern class CBhoAddinModule _AtlModule;
