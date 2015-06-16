#ifndef _XL_DLLHELPER_
#define _XL_DLLHELPER_
/************************************************************************/
/*							ʹ��˵���Ͱ�Ȩ����
/************************************************************************/
/*	����:			xian
	�汾:			1.00
	���ʱ��:		2014/1/10
	
/*										��Ȩ����: xian.small.b@gmail.com*/
/************************************************************************/
/*
ע��:	Ĭ��Ϊ	__cdeclģʽ����,
		����__stdcall �����Լ�ֱ�Ӷ��� #define _XL_STDCALL

dll������������:

		//�ӿ��ࣨ�����������ã�
		calss InterfaceClass
		{	
		public:
			virtual void hello() = 0;
		}

		//�ӿ�ʵ����
		calss MyInterfaceClass : public InterfaceClass
		{	
		public:
			virtual void hello()	{ printf("hello"); };
		}
		
		//��cpp�������浼����
		XL_InterfaceExport(MyInterfaceClass);


���ö�ʹ�÷���:(ע�����dll�ǡ�.��,���ýӿں����ǡ�->��)

		//����ӿں���
		XL_DllHelper<InterfaceClass>	MyWork;

		//���ؽӿ�dll
		MyWork.loadDll(dllpath);

		//����Ϳ���ֱ�ӵ���InterfaceClass�ӿڵĺ�����
		MyWork->hello(); 

		//�ͷ�dll
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