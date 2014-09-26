#pragma once
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include <string>


static void GetPeerId_ (std::wstring &);

inline bool GetMacByGetAdaptersAddresses(std::wstring& macOUT)
{
	bool ret = false;

	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	if (pAddresses == NULL) 
		return false;
	// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		if (pAddresses == NULL) 
			return false;
	}

	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
	{
		// If successful, output some information from the data we received
		for(PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{
			// ȷ��MAC��ַ�ĳ���Ϊ 00-00-00-00-00-00
			if(pCurrAddresses->PhysicalAddressLength != 6)
				continue;
			wchar_t acMAC[32];
			wsprintf(acMAC, L"%02X%02X%02X%02X%02X%02X",
				int (pCurrAddresses->PhysicalAddress[0]),
				int (pCurrAddresses->PhysicalAddress[1]),
				int (pCurrAddresses->PhysicalAddress[2]),
				int (pCurrAddresses->PhysicalAddress[3]),
				int (pCurrAddresses->PhysicalAddress[4]),
				int (pCurrAddresses->PhysicalAddress[5]));
			macOUT = acMAC;
			ret = true;
			break;
		}
	} 

	free(pAddresses);
	return ret;
}


void GetPeerId_(std::wstring & wstrPeerId)
{
	std::wstring wstrMAC;
	if (!GetMacByGetAdaptersAddresses(wstrMAC))
	{
		wstrMAC = L"000000000000";
	}
	wstrPeerId = wstrMAC;
	srand( (unsigned)time( NULL ) );
	for(int i=0;i<4;i++)
	{
		wchar_t szRam[2] = {0};
		wsprintf(szRam,L"%X", rand()%16);
		szRam[1]=L'\0';
		wstrPeerId += szRam;
	}
	return ;
}