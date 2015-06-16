// AiDll.cpp : 定义 DLL 应用程序的导出函数。
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