#include "stdafx.h"
#include "LuaDragDropProcessor.h"
//#include "define.h"

#define KKIMAGE_LUADRAGDROPPROCESSOR_CLASSNAME	"KKImage.LuaDragDropProcessor.Class"
#define KKIMAGE_LUADRAGDROPPROCESSOR_OBJNAME		"KKImage.LuaDragDropProcessor"

CLuaDragDropProcessor::CLuaDragDropProcessor(void)
{
}

CLuaDragDropProcessor::~CLuaDragDropProcessor(void)
{
}
static XLLRTGlobalAPI CLuaDragDropProcessorMemberFunctions[] = 
{
	{"OnDragEnter", CLuaDragDropProcessor::OnDragEnter},
	{"OnDragQuery", CLuaDragDropProcessor::OnDragQuery},
	{"OnDragOver", CLuaDragDropProcessor::OnDragOver},
	{"OnDragLeave", CLuaDragDropProcessor::OnDragLeave},
	{"OnDrop", CLuaDragDropProcessor::OnDrop},	
	{"NULL", NULL}
};

long CLuaDragDropProcessor::RegisterSelf(XL_LRT_RUNTIME_HANDLE hEnv)
{
	assert(hEnv);
	if(hEnv == NULL)
	{
		return XLLRT_RESULT_ENV_INVALID;
	}

	XLLRTObject theObject;
	theObject.ClassName = KKIMAGE_LUADRAGDROPPROCESSOR_CLASSNAME;
	theObject.MemberFunctions = CLuaDragDropProcessorMemberFunctions;
	theObject.ObjName = KKIMAGE_LUADRAGDROPPROCESSOR_OBJNAME;
	theObject.userData = NULL;
	theObject.pfnGetObject = CLuaDragDropProcessor::GetObject;

	long lRet = XLLRT_RegisterGlobalObj(hEnv,theObject); 
	assert(lRet == XLLRT_RESULT_SUCCESS);
	if (lRet != XLLRT_RESULT_SUCCESS)
	{
		return lRet;
	}
	return lRet;
}

void* CLuaDragDropProcessor::GetObject(void* p)
{
	static CLuaDragDropProcessor object;
	return &object;
}

int CLuaDragDropProcessor::OnDragEnter(lua_State* luaState)
{
	if( lua_islightuserdata(luaState, 1) )
	{
		IDataObject* pData = (IDataObject*)lua_touserdata( luaState, 1 );
		/*IEnumFORMATETC* formatEnum_ptr=0;
		HRESULT ret = pData->EnumFormatEtc( DATADIR_GET,&formatEnum_ptr );
		ret = formatEnum_ptr->Reset();
		FORMATETC format_result;
		do
		{
			ret = formatEnum_ptr->Next(1,&format_result,NULL);
			if( (unsigned int)format_result.cfFormat >= 0XC000 )
			{
				wchar_t format_name[1024];
				int len = GetClipboardFormatName( format_result.cfFormat,format_name,1024 );
				if( len == 0 )
					assert(0);
			}
		}
		while( ret == S_OK );
		*/
		DWORD keyState =(DWORD) lua_tointeger( luaState, 2 );
		int x = lua_tointeger( luaState, 3 );
		int y = lua_tointeger( luaState, 4 );
		FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
		HRESULT ret =  pData->QueryGetData(&fmtetc); 
		if( ret == S_OK )
		{
			lua_pushnumber( luaState, 4 );
			lua_pushboolean( luaState, 1 );
			lua_pushboolean( luaState, 0 );
			return 3;
		}	
	}
	lua_pushnumber( luaState, 0 );
	lua_pushboolean( luaState, 1 );
	lua_pushboolean( luaState, 0 );
	return 3;
}

int CLuaDragDropProcessor::OnDragQuery(lua_State* luaState)
{
	if( lua_islightuserdata(luaState, 1) )
	{
		IDataObject* pData = (IDataObject*)lua_touserdata( luaState, 1 );
		DWORD keyState =(DWORD) lua_tointeger( luaState, 2 );
		int x = lua_tointeger( luaState, 3 );
		int y = lua_tointeger( luaState, 4 );
		FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
		HRESULT ret =  pData->QueryGetData(&fmtetc); 
		if( ret == S_OK )
		{
			lua_pushnumber( luaState, 4 );
			lua_pushboolean( luaState, 1 );
			lua_pushboolean( luaState, 0 );
			return 3;
		}	
	}
	lua_pushnumber( luaState, 0 );
	lua_pushboolean( luaState, 1 );
	lua_pushboolean( luaState, 0 );
	return 3;
}

int CLuaDragDropProcessor::OnDragOver(lua_State* luaState)
{
	if( lua_islightuserdata(luaState, 1) )
	{
		IDataObject* pData = (IDataObject*)lua_touserdata( luaState, 1 );
		DWORD keyState =(DWORD) lua_tointeger( luaState, 2 );
		int x = lua_tointeger( luaState, 3 );
		int y = lua_tointeger( luaState, 4 );
		FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
		HRESULT ret =  pData->QueryGetData(&fmtetc); 
		if( ret == S_OK )
		{
			lua_pushnumber( luaState, 4 );
			lua_pushboolean( luaState, 1 );
			lua_pushboolean( luaState, 0 );
			return 3;
		}	
	}
	lua_pushnumber( luaState, 0 );
	lua_pushboolean( luaState, 1 );
	lua_pushboolean( luaState, 0 );
	return 3;
}

int CLuaDragDropProcessor::OnDragLeave(lua_State* luaState)
{
	lua_pushnumber( luaState, 0 );
	lua_pushboolean( luaState, 1 );
	lua_pushboolean( luaState, 0 );
	return 3;
}

int CLuaDragDropProcessor::OnDrop(lua_State* luaState)
{
	if( lua_islightuserdata(luaState, 1) )
	{
		IDataObject* pData = (IDataObject*)lua_touserdata( luaState, 1 );
		DWORD keyState =(DWORD) lua_tointeger( luaState, 2 );
		int x = lua_tointeger( luaState, 3 );
		int y = lua_tointeger( luaState, 4 );
		FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
		HRESULT ret =  pData->QueryGetData(&fmtetc); 
		if( ret == S_OK )
		{
			STGMEDIUM data_medium;
			ret = pData->GetData( &fmtetc,&data_medium );
			if( ret == S_OK )
			{
				lua_pushnumber( luaState, 4 );
				lua_pushboolean( luaState, 1 );
				lua_pushboolean( luaState, 0 );
				lua_newtable( luaState );
				DROPFILES* files_ptr = (DROPFILES*)data_medium.hGlobal;
				HDROP hDrop = (HDROP)files_ptr;
				int file_count = DragQueryFile(hDrop,0XFFFFFFFF, NULL, 0 );
				int table_index=1;
				for( int i=0; i<file_count; ++i )
				{
					wchar_t file_name[MAX_PATH+1];
					int len = DragQueryFile(hDrop, i, file_name, MAX_PATH+1);
					if( len == 0 )
						assert(0);
					char utf8_name[2*MAX_PATH+1];
					len = ::WideCharToMultiByte(CP_UTF8,0,file_name,len,utf8_name,2*MAX_PATH+1,NULL,NULL);
					if( len != 0 )
					{
						utf8_name[len]='\0';
						lua_pushnumber(luaState,table_index++);
						lua_pushstring(luaState,utf8_name );
						lua_settable(luaState, -3 );
					}
				}
				ReleaseStgMedium( &data_medium);
				
				return 4;
			}
		}
	}
	lua_pushnumber( luaState, 0 );
	lua_pushboolean( luaState, 1 );
	lua_pushboolean( luaState, 0 );
	lua_newtable( luaState );
	return 4;
}


