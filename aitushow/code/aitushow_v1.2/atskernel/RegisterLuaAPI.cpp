#include "stdafx.h"
#include "RegisterLuaAPI.h"
#include "LuaAPI/LuaAPIUtil.h"
#include "LuaAPI/LuaAsynAPIUtil.h"
//#include "LuaAPI/LuaGraphicUtil.h"
#include "LuaAPI/LuaNotifyIcon.h"
#include "EvenListenHelper/LuaListenPre.h"
#include "EvenListenHelper/LuaPrefactory.h"
#include "LuaAPI/LuaImageProcessor.h"
#include "LuaAPI/LuaKKImageDoc.h"
#include "LuaAPI/LuaDragDropProcessor.h"
#include "KKTaskProcess/LuaOperationFactory.h"
#include "KKTaskProcess/LuaOperation.h"
#include "KKImageLoader/LuaThumbnailLoaderFactory.h"
#include "LuaAPI/LuaDragDropProcessor.h"

CRegisterLuaAPI::CRegisterLuaAPI()
{

}

CRegisterLuaAPI::~CRegisterLuaAPI()
{

}

BOOL CRegisterLuaAPI::Init(LPCTSTR lpCmdLine, LPVOID lpHookObj)
{
	TSTRACEAUTO();
	XL_LRT_ENV_HANDLE hEnv = XLLRT_GetEnv(NULL);
	if (NULL == hEnv)
	{
		TSDEBUG4CXX(L"XLLRT_GetEnv error!  hEnv == NULL");
		return FALSE;
	}
	LuaAPIUtil::RegisterObj(hEnv);
	LuaAsynUtil::RegisterSelf(hEnv);
	LuaNotifyIcon::RegisterSelf(hEnv);
	LuaListenPreFactory::RegisterObj(hEnv);
	LuaListenPre::RegisterClass(hEnv);
	CLuaKKImageDoc::RegisterLuaClass(hEnv);
	CLuaOperationFactory::RegisterSelf(hEnv);
	CLuaOperation::RegisterClass(hEnv);
	CLuaImageProcessor::RegisterSelf(hEnv);
	CLuaThumbnailLoaderFactory::RegisterObj(hEnv);
	CLuaDragDropProcessor::RegisterSelf(hEnv);

	XLLRT_ReleaseEnv(hEnv);
	return TRUE;
}
