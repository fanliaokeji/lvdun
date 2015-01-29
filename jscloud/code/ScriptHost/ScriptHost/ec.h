#pragma once

static inline __int64 epass(const char*  pkey = NULL )
{
	static char* ppass = "SHhost";
	if(pkey)
	{
		long nlen = (long)strlen(pkey) + 1;
		ppass = new char[nlen];
		strcpy(ppass, pkey);
		return 0;
	}
	unsigned long  plen = 0;
	__int64 mc= 8757735233305;

	plen = (unsigned long )strlen(ppass);

	for(int i=0; i<(int)plen; i++)
	{
		mc ^= ppass[i]|128;
	}
	return mc;
}

static inline void setkey(const char* const pkey = "SHhost")
{
	epass(pkey);
	return ;
}

static inline int ec(char* const pIn, unsigned long & dwInlen)
{
	char* data = pIn;
	unsigned long  flen = dwInlen;
	__int64 password = epass();
	int i=0;
	for(; i<(int)flen; i++)
	{
		pIn[i] = data[i] ^ (char)password;
		pIn[i] = data[i] ^ (char)flen;
	}
	char cpass[5] = "love";
	int j=0;
	for(; j<5; j++)
	{
		cpass[j] ^= password;
	}
	memcpy( &pIn[i], &cpass, 5);
	dwInlen = flen + 5;
	return 0;
}
//

static inline int dc(char* const pIn, unsigned long&  dwInlen)
{
	if(dwInlen <=5 || NULL == pIn )
	{
		return -1;
	}
	char *data = pIn;
	unsigned long  flen = dwInlen;
	char love[5];
	memcpy(love, &data[flen-5], 5);
	__int64 password = epass();
	for(int i=0; i<(char)5; i++)
	{
		love[i] ^= password;
	}
	if(strcmp(love, "love")!=0)
	{
		return -2;
	}
	for(int j=0; j<(int)flen-5; j++)
	{
		pIn[j]	= data[j] ^ (char)password;
		pIn[j] = data[j] ^ (char)(flen-5);
	}
	pIn[flen-5] = '\0';		
	dwInlen = flen-5;
	return 0;
}
static inline bool isec(char* const pIn, unsigned long  dwInlen)
{
	char* data = pIn;
	unsigned long flen = dwInlen;
	char love[5];
	memcpy(love, &data[flen-5], 5);
	__int64 password = epass();
	for(int i=0; i<(char)5; i++)
	{
		love[i] ^= password;
	}
	if(strcmp(love, "love")!=0)
	{
		return false;
	}
	return true;
}