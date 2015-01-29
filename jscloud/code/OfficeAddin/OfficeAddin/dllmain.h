// dllmain.h : Declaration of module class.

class COfficeAddinModule : public CAtlDllModuleT< COfficeAddinModule >
{
public :
	DECLARE_LIBID(LIBID_OfficeAddinLib)
	// DECLARE_REGISTRY_APPID_RESOURCEID(IDR_OFFICEADDIN, "{CAD1E4FE-4B98-4FED-9AE6-535BC67D2079}")
};

extern class COfficeAddinModule _AtlModule;
