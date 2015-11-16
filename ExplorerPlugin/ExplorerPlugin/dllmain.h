// dllmain.h : Declaration of module class.

class CExplorerPluginModule : public CAtlDllModuleT< CExplorerPluginModule >
{
public :
	DECLARE_LIBID(LIBID_ExplorerPluginLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EXPLORERPLUGIN, "{578D21B0-7B45-460B-95E8-D6DBA1FA9D1E}")
};

extern class CExplorerPluginModule _AtlModule;
