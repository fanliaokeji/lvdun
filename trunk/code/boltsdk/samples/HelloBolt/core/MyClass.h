#pragma once

/*
�������һ�����͵�C++�࣬�з������¼�������Add�����ᴥ��Result Event

*/
#include <vector>

typedef void (*funcResultCallBack) (DWORD dwUserData1,DWORD dwUserData2,int nResult);

struct CallbackNode
{
    funcResultCallBack pfnCallBack;
    DWORD dwUserData1;
    DWORD dwUserData2;
};

class MyClass
{
public:
    MyClass(void);
    ~MyClass(void);

public:
    int Add(int lhs,int rhs);
    int AttachResultListener(DWORD dwUserData1,DWORD dwUserData2,funcResultCallBack pfnCallBack);

protected:
    void FireResultEvent(int nResult);
    std::vector<CallbackNode> m_allCallBack;
};


