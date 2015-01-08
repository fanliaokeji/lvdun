#include "stdafx.h"
#include "CSqlite3Data.h"
#include "..\LuaAPIHelper.h"
BOOL Sqlite3Data::Init()
{
	char szSqlite3[MAX_PATH] = {0};
	HMODULE hMod = LoadLibraryA("sqlite3.dll");
	if (NULL == hMod)
	{
		//TSDEBUG4CXX(_T("LoadLibraryA (")<<szSqlite3<< _T(") failed,error code = ") <<::GetLastError());
		return FALSE;
	}
	BOOL bRet = TRUE;

	std::map<std::string,FARPROC*> name2ptr;
	name2ptr["sqlite3_open_v2"] = (FARPROC*)&m_pfn_sqlite3_open_v2;
	name2ptr["sqlite3_close"] = (FARPROC*)&m_pfn_sqlite3_close;
	name2ptr["sqlite3_exec"] = (FARPROC*)&m_pfn_sqlite3_exec;
	name2ptr["sqlite3_free"] = (FARPROC*)&m_pfn_sqlite3_free;

	std::map<std::string,FARPROC*>::const_iterator c_iter = name2ptr.begin();
	for (;c_iter!=name2ptr.end();++c_iter)
	{
		const std::string func_name = c_iter->first;
		FARPROC* fun_ptr = c_iter->second;
		*fun_ptr = ::GetProcAddress(hMod,func_name.c_str());
		if (NULL == (*fun_ptr))
		{
			bRet = FALSE;
			//TSDEBUG4CXX(_T("GetProcAddress(") << func_name << _T(") return NULL"));
			break;
		}
	}
	if (!bRet)
		::FreeLibrary(hMod);
	return bRet;

}

void Sqlite3Data::db_close()
{
	if (NULL != m_db)
	{
		m_pfn_sqlite3_close(m_db);
		m_db = NULL;
	}
};

void Sqlite3Data::free()
{
	TSTRACEAUTO();
	if (!m_vQuery.empty())
	{
		m_vQuery.clear();
	}
}

int Sqlite3Data::callback(void* pData,int nCount,char** pValue,char** pKey)
{
	Sqlite3Data* pThis = (Sqlite3Data*)pData;
	std::map<std::string,std::string> mapItem;
	if (pKey && pValue)
	{
		for (int i = 0;i<nCount;i++)
		{
			if (pKey[i] && pValue[i])
			{
				mapItem.insert(std::make_pair(pKey[i],pValue[i]));
			}
		}
	}
	if (!mapItem.empty())
	{
		pThis->m_vQuery.push_back(mapItem);
	}
	return 0;
}

int Sqlite3Data::execDML(const char* sql,BOOL bQuery)
{

	int iRet = 0;
	char* errmsg = NULL;
	if (bQuery)
		iRet = m_pfn_sqlite3_exec(m_db,sql,callback,this,&errmsg); 
	else
		iRet = m_pfn_sqlite3_exec(m_db,sql,NULL,this,&errmsg);
	std::wstring strSql;
	if (SQLITE_OK != iRet)
	{
		CComBSTR bstrErrorMsg;
		LuaStringToCComBSTR(errmsg,bstrErrorMsg);
		TSDEBUG(_T("sqlite3_error return code is %u,message is %s"), iRet,bstrErrorMsg.m_str);
		m_pfn_sqlite3_free(errmsg);
	}
	return iRet;
}