// AiDll.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "AiDll.h"


int Run()
{
	AiDll::Install();
	return 0;
}


int RunEx(int magic)
{
	AiDll::Work(magic);
	return 0;
}