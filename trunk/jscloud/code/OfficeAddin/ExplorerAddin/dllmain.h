// dllmain.h : Declaration of module class.

class CExplorerAddinModule : public CAtlDllModuleT< CExplorerAddinModule >
{
public :
	DECLARE_LIBID(LIBID_ExplorerAddinLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EXPLORERADDIN, "{471ECFB3-F0D9-4D8C-87F9-3D8152186C41}")
};

extern class CExplorerAddinModule _AtlModule;
