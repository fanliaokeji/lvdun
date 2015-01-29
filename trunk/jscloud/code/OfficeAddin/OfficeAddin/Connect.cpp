// Connect.cpp : Implementation of CConnect

#include "stdafx.h"
#include "Connect.h"

extern HINSTANCE g_hModule;

// CConnect

HRESULT CConnect::OnConnection(LPDISPATCH Application, ext_ConnectMode ConnectMode, LPDISPATCH AddInInst, SAFEARRAY **custom)
{
	TSAUTO();
	Application->QueryInterface(__uuidof(IDispatch), reinterpret_cast<LPVOID*>(&this->m_spApplication));
	AddInInst->QueryInterface(__uuidof(IDispatch), reinterpret_cast<LPVOID*>(&this->m_spAddInInstance));
	wchar_t path[MAX_PATH];
	::GetModuleFileName(g_hModule, path, MAX_PATH);
	std::wstring configFile = path;
	for (;!configFile.empty() && configFile[configFile.size() - 1] != L'\\'; configFile.resize(configFile.size() - 1))
		;
	configFile += L"config.ini";
	TSINFO4CXX("Config File Path: " << configFile);
	this->m_addinHelper.Initialize(configFile, false);
	return S_OK;
}

HRESULT CConnect::OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY **custom)
{
	TSAUTO();
	this->m_spApplication = NULL;
	this->m_spAddInInstance = NULL;
	return S_OK;
}

HRESULT CConnect::OnAddInsUpdate(SAFEARRAY **custom)
{
	TSAUTO();
	return S_OK;
}

HRESULT CConnect::OnStartupComplete(SAFEARRAY **custom)
{
	TSAUTO();
	this->m_addinHelper.BeginTask();
	return S_OK;
}

HRESULT CConnect::OnBeginShutdown(SAFEARRAY **custom)
{
	TSAUTO();
	this->m_addinHelper.EndTask();
	return S_OK;
}
