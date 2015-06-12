// dllmain.h : Declaration of module class.

class CnpFlyRabbitCtrlModule : public CAtlDllModuleT< CnpFlyRabbitCtrlModule >
{
public :
	DECLARE_LIBID(LIBID_npFlyRabbitCtrlLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_NPFLYRABBITCTRL, "{AB03E49E-F82C-43A3-80CC-A41AB8755EA8}")
};

extern class CnpFlyRabbitCtrlModule _AtlModule;
