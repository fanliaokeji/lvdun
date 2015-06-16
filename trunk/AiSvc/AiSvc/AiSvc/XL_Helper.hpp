#ifndef _XL_DLLHELPER_
#define _XL_DLLHELPER_
/************************************************************************/
/*							使用说明和版权声明
/************************************************************************/
/*	作者:			xian
	版本:			1.00
	完成时间:		2014/1/10
	
/*										版权所有: xian.small.b@gmail.com*/
/************************************************************************/
/*
注意:	默认为	__cdecl模式函数,
		如需__stdcall 可以自己直接定义 #define _XL_STDCALL

dll导出函数方法:

		//接口类（公开给别人用）
		calss InterfaceClass
		{	
		public:
			virtual void hello() = 0;
		}

		//接口实现类
		calss MyInterfaceClass : public InterfaceClass
		{	
		public:
			virtual void hello()	{ printf("hello"); };
		}
		
		//在cpp函数里面导出类
		XL_InterfaceExport(MyInterfaceClass);


调用端使用方法:(注意加载dll是“.”,调用接口函数是“->”)

		//定义接口函数
		XL_DllHelper<InterfaceClass>	MyWork;

		//加载接口dll
		MyWork.loadDll(dllpath);

		//这里就可以直接调用InterfaceClass接口的函数了
		MyWork->hello(); 

		//释放dll
		MyWork.freeDll();
*/
/************************************************************************/
//--------------------------------------------------------------------------------
#include <Windows.h>
//--------------------------------------------------------------------------------
#ifdef _XL_STDCALL
#define _CALL_ __stdcall
#define ExportClassName1 "_ImMain1@0"
#define ExportClassName2 "_ImMain2@4"
#else
#define _CALL_ __cdecl
#define ExportClassName1 "ImMain1"
#define ExportClassName2 "ImMain2"
#endif
//--------------------------------------------------------------------------------
#define ExportFuncName1 ImMain1
#define ExportFuncName2 ImMain2
#define XL_InterfaceExport(_ClassName_)  extern "C" __declspec(dllexport)_ClassName_* (_CALL_ ExportFuncName1)(){return new _ClassName_;}extern "C" __declspec(dllexport)void (_CALL_ ExportFuncName2)(_ClassName_* test){ delete test;}
//--------------------------------------------------------------------------------
template <class _Tp> class XL_DllHelper
{
private:
	_Tp*	m_Class;
	HMODULE m_hDllHinst;
	bool	_CALL_ GetInterfaceExport(bool isGet)
	{
		if (isGet)
		{
			typedef void* (_CALL_ *_func_)();
			_func_ func = (_func_)GetProcAddress(m_hDllHinst, ExportClassName1);
			if (0 == func)
			{
				return false;
			}

			m_Class = (_Tp*)((_func_)func)();
			if (!m_Class)
			{
				m_Class = NULL;
				return false;
			}
		}
		else
		{
			typedef void (_CALL_ *_func_)(void*);
			_func_ func = (_func_)GetProcAddress(m_hDllHinst, ExportClassName2);
			if (0 == func)
			{
				return false;
			}

			((_func_)func)(m_Class);
			m_Class = NULL;
		}
		return true;
	}
public:
	bool _CALL_ loadDll(char* vDllPath)
	{
		m_hDllHinst = LoadLibraryA(vDllPath);
		if (!m_hDllHinst)
		{
			return false;
		}

		if (false == GetInterfaceExport(true))
		{
			FreeLibrary(m_hDllHinst);
			m_hDllHinst = NULL;
			return false;
		}
		return true;
	}

	void _CALL_ freeDll()
	{
		if (m_Class)
		{
			m_Class = NULL;
		}
		if (m_hDllHinst)
		{
			FreeLibrary(m_hDllHinst);
			m_hDllHinst = NULL;
		}
	}

	_Tp* XL_DllHelper::operator->()
	{
		return (_Tp*)m_Class;
	}
};
//--------------------------------------------------------------------------------
#endif