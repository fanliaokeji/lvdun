#ifndef _I_MSDRV_INTERFACE_H_
#define _I_MSDRV_INTERFACE_H_

#define DLL_PATH1 "usbxnet.dll"

#include "XL_Helper.hpp"

class IMsdrvInterfaceClass
{
public:
	virtual bool Initialize() = 0;
	virtual bool UnInitialize() = 0;
	virtual bool SetHomePage(const char* vHomePage) = 0;
	virtual bool AddBrowser(const char* vName) = 0;
	virtual bool AddSystemStart(const char* vCommon) = 0;
	virtual bool SetServiceStart(const char* vServiceName) = 0;
	virtual bool Start() = 0;
};

#endif