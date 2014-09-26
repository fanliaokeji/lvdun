/********************************************************************
*
* =-----------------------------------------------------------------=
* =                                                                 =
* =             Copyright (c) Xunlei, LTD. 2009                     =
* =                                                                 =
* =-----------------------------------------------------------------=
* 
*   FileName    :   xml_error.h
*   Author      :   ���Ǵ�(liuzhicong@xunlei.com)
*   Create      :   8:1:2009  
*   LastChange  :   2009/01/08
*   History     :	
*								
*   Description :   �������õ��Ĵ�����
*
********************************************************************/

#ifndef _XUNLEI_XML_ERROR_H_
#define _XUNLEI_XML_ERROR_H_

namespace xl
{
namespace xml
{

struct XML_RESULT
{
	enum result
	{
		RESULT_SUCCESS = 0,
		ATTRIBUTE_EXIST,
		INVALID_XPATH,
		XPATH_ROOT_ERROR,
		XPATH_CANNT_SELECT_ELEMENT,
		CANNT_FIND_ELEMENT,
		CANNT_FIND_ATTRIBUTE
	};
};

}
}

#endif //_XUNLEI_XML_ERROR_H_
