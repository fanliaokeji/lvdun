#ifndef SCRIPTABLENPOBJECT_H_
#define SCRIPTABLENPOBJECT_H_

#include <map>
#include <string>
#include <npapi.h>
#include <npfunctions.h>
#include <string>


class ScriptableNPObject : public NPObject
{
protected:
	// Class member functions that do the real work
	void Deallocate();
	void Invalidate();
	bool HasMethod(NPIdentifier name);
	bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool HasProperty(NPIdentifier name);
	bool GetProperty(NPIdentifier name, NPVariant *result);
	bool SetProperty(NPIdentifier name, const NPVariant *value);
	bool RemoveProperty(NPIdentifier name);
	bool Enumerate(NPIdentifier **identifier, uint32_t *count);
	bool Construct(const NPVariant *args, uint32_t argCount, NPVariant *result);
public:
	ScriptableNPObject(NPP npp) {//TSAUTO(); m_Instance = npp;
	}
	~ScriptableNPObject() {//TSAUTO();
	}

	// This is the method used to create the NPObject
	// This method should not be called explicitly
	// Instead, use NPN_CreateObject
	static NPObject* Allocate(NPP npp, NPClass *aClass) {
		//TSAUTO();
		return (NPObject *)new ScriptableNPObject(npp);
	}

	static ScriptableNPObject* NewObject(NPP npp) {
		//TSAUTO();
		ScriptableNPObject* newObj = (ScriptableNPObject*)npnfuncs->createobject(npp, &_npclass);
		return newObj;
	} 

	static NPObject* RetainObject(NPObject *obj) {
		//TSAUTO();
		return npnfuncs->retainobject(obj);
	}

	static void ReleaseObject(NPObject *obj) {
		//TSAUTO();
		npnfuncs->releaseobject(obj);
	}

	//
	// Given an NPString |npString|, mallocs a char array, copies it, adds
	//   null terminator
	// NOTE: This malloc's memory -- caller should free it when done
	//
	std::string CreateString(const NPString& npString) {
		std::string ret_value;
		for (unsigned int i = 0; i < npString.UTF8Length; ++i) {
			ret_value += npString.UTF8Characters[i];
		}
		return ret_value;
	}

	/////////////////////////////
	// Static NPObject methods //
	/////////////////////////////
	static void _Deallocate(NPObject *npobj);
	static void _Invalidate(NPObject *npobj);
	static bool _HasMethod(NPObject *npobj, NPIdentifier name);
	static bool _Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
	static bool _InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
	static bool _HasProperty(NPObject * npobj, NPIdentifier name);
	static bool _GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
	static bool _SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
	static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
	static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count);
	static bool _Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);

	static NPClass _npclass;
	static NPNetscapeFuncs *npnfuncs;
public:
	void Variant2NPVar(const VARIANT *var, NPVariant *npvar, NPP instance);
	void NPVar2Variant(const NPVariant *npvar, VARIANT *var);
	void BSTR2NPVar(BSTR bstr, NPVariant *npvar, NPP instance);
	BSTR Utf8StringToBstr(LPCSTR szStr, int iSize);
	std::string BstrToString(BSTR bstr);

public:
	NPP m_Instance;
	typedef std::multimap<std::string, NPObject*> CallMap;
	CallMap m_mapCall;
};



#endif // SCRIPTABLENPOBJECT_H_