// GsNet.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GsNet.h"


// This is an example of an exported variable
// GSNET_API int nGsNet=0;

// This is an example of an exported function.
// GSNET_API int fnGsNet(void)
// {
//	return 42;
// }

// This is the constructor of a class that has been exported.
// see GsNet.h for the class definition
// CGsNet::CGsNet()
// {
// 	return;
// }

GSNET_API LRESULT CALLBACK GsNetWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return ::CallNextHookEx(NULL, nCode, wParam, lParam);
}
