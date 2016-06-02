#include "StdAfx.h"
#include "OperationFactory.h"
#include "LoadFileOP.h"
#include "LoadBitmapOP.h"
#include "ScaleBitmapOP.h"
#include "GetExifOP.h"
#include "SaveDocFileOP.h"

COperationFactory & COperationFactory::GetInstance()
{
	static COperationFactory obj;
	return obj;
}

CBaseOperation* COperationFactory::CreateOperation(int nOPType)
{
	CBaseOperation* pRet = NULL;
	switch(nOPType)
	{
	case KKImg_OperationType_LoadImageFile:
		pRet = new CLoadFileOP();
		break;
	case KKImg_OperationType_LoadBitmap:
		pRet = new CLoadBitmapOP();
		break;
	case KKImg_OperationType_ScaleBitmap:
		pRet = new CScaleBitmapOP();
		break;
	case KKImg_OperationType_GetExif:
		pRet = new CGetExifOP();
		break;
	case KKImg_OperationType_SaveDocFile:
		pRet = new CSaveDocFileOP();
		break;
	default:
		break;
	}
	return pRet;
}
