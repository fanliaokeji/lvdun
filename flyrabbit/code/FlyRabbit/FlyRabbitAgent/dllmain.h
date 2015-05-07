// dllmain.h : Declaration of module class.

class CFlyRabbitAgentModule : public CAtlDllModuleT< CFlyRabbitAgentModule >
{
public :
	DECLARE_LIBID(LIBID_FlyRabbitAgentLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_FLYRABBITAGENT, "{B67E02EE-0E39-42B6-8EF2-754B11AE3160}")
};

extern class CFlyRabbitAgentModule _AtlModule;
