// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GSNET_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GSNET_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GSNET_EXPORTS
#define GSNET_API __declspec(dllexport)
#else
#define GSNET_API __declspec(dllimport)
#endif

// This class is exported from the GsNet.dll
// class GSNET_API CGsNet {
// public:
//	CGsNet(void);
	// TODO: add your methods here.
//};

// extern GSNET_API int nGsNet;

// GSNET_API int fnGsNet(void);

GSNET_API LRESULT CALLBACK GsNetWndProc(int nCode, WPARAM wParam, LPARAM lParam);