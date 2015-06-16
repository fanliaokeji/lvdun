#pragma once

//#ifdef _cplusplus
extern "C"{
//#endif

	void InitWget(const char* szLogFine,int nFlag);
	void ReleaseWget();
	int DownLoad (char* url, char *const localFile);
	int	setval (const char *com, const char *val);

//#ifdef _cplusplus
}
//#endif