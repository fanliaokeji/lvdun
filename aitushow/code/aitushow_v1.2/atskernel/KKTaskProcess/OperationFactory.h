#pragma once
#include "BaseOperation.h"

class COperationFactory
{
public:
	~COperationFactory() {}

	static COperationFactory & GetInstance();
	CBaseOperation* CreateOperation(int nOPType);

private:
	COperationFactory() {}
};
