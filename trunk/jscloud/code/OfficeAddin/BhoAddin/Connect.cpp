// Connect.cpp : Implementation of CConnect

#include "stdafx.h"
#include "Connect.h"


// CConnect

extern HINSTANCE g_hThisModule;

//
STDMETHODIMP CConnect::SetSite(IUnknown*pUnkSite)  
{  
	TSTRACEAUTO();
	if(pUnkSite!=NULL)  
	{  
		//����ָ��IWebBrowser2��ָ�롣  
		//pUnkSite->QueryInterface(IID_IWebBrowser2,(void**)&m_spWebBrowser);  
		StartOnlyOnce(L"bho");
	}  
	else  
	{  
		//�ڴ��ͷŻ����ָ���������Դ��  
		//m_spWebBrowser.Release();  
	}  
	//���ػ���ʵ��  
	return IObjectWithSiteImpl::SetSite(pUnkSite);  
}




//////////////////////////////////////////////////////////////////////////
void CConnect::StartOnlyOnce(const std::wstring &launchSrc,BOOL bForce)
{
	TSAUTO();

	static bool bAlreadyStart = false;
	if (bAlreadyStart)
	{
		return;	
	}
	bAlreadyStart = true;

	wchar_t path[MAX_PATH];
	::GetModuleFileName(g_hThisModule, path, MAX_PATH);
	std::wstring configFile = path;
	for (;!configFile.empty() && configFile[configFile.size() - 1] != L'\\'; configFile.resize(configFile.size() - 1))
		;
	configFile += L"config.ini";
	TSINFO4CXX("Config File Path: " << configFile);
	this->m_addinHelper.Initialize(configFile, false,launchSrc);
	this->m_addinHelper.BeginTask();
	return;
}