#include "StdAfx.h"
#include ".\baseoperation.h"

CBaseOperation::CBaseOperation(void)
{
	m_OperationType = KKImg_OperationType_Invalid;
	m_wstrOperationDes = L"ÎÞÐ§²Ù×÷";

	m_nOperationId = GetOperationId();
}

CBaseOperation::~CBaseOperation(void)
{
}

bool CBaseOperation::SetParam(lua_State* luaState)
{
	return true;
}
int CBaseOperation::GetParam(lua_State* luaState)
{
	return 0;
}

int CBaseOperation::AttachListener(const wstring& wstrEventName, lua_State* luaState, int nIndex, DWORD& dwCookie)
{
	if (luaState)
	{
		m_EventContainer.AttachEvent(wstrEventName, luaState, nIndex, dwCookie);
		return S_OK;
	}
	return S_FALSE;
}
int CBaseOperation::DetachListener(const wstring& wstrEventName, DWORD dwCookie)
{
	return m_EventContainer.DetachEvent(wstrEventName, dwCookie);
}
bool CBaseOperation::OnOperationStepComplete(int m_nStatus, int m_nErrorCode)
{
	return true;
}