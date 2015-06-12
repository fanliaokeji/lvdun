/* ***** BEGIN LICENSE BLOCK *****
 * (C)opyright 2008-2009 Aplix Corporation. anselm@aplixcorp.com
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * ***** END LICENSE BLOCK ***** */
#include "stdafx.h"

#include <string.h>

#include <npapi.h>
#include <prtypes.h>
#include <npfunctions.h>
#include <npruntime.h>
#include "ScriptableNPObject.h"


NPObject* nppobj;
NPP savenpp;

// Called by the browser to create a new instance of the plugin
NPError NPP_New(NPMIMEType pluginType,
				NPP npp,
				uint16 mode,
				int16 argc,
				char* argn[],
				char* argv[],
				NPSavedData* saved)
{
	TSAUTO();
	if (npp == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;
	
	//return NPERR_FILE_NOT_FOUND;
	NPError rv = NPERR_NO_ERROR;
	savenpp = npp;
	return rv;
} 


// Called by browser whenever the window is changed, including to set up or destroy
NPError NPP_SetWindow (NPP npp, NPWindow* pNPWindow)
{
	TSAUTO();
	if (npp == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	return NPERR_NO_ERROR;
} 

// Called by browser to destroy an instance of the plugin
NPError NPP_Destroy (NPP npp, NPSavedData** save)
{
	TSAUTO();
	if (npp == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;
	NPObject *pluginInstance=nppobj;
	if(!pluginInstance) {
		ScriptableNPObject::npnfuncs->releaseobject(pluginInstance);
		pluginInstance = NULL;
	}

	return NPERR_NO_ERROR;
} 

#define STRINGS_PRODUCTNAME "FlyRabbitnpCtrlPlugin"
#define STRINGS_FILEDESCRIPTION "<a href=\"http://www.feitwo.com/\">FlyRabbitnpCtrlPlugin</a> plugin."

NPError NPP_GetValue(NPP npp, NPPVariable variable, void *value) 
{
	TSAUTO();
	if (npp == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;
	NPError rv = NPERR_NO_ERROR;
	NPObject *pluginInstance = nppobj;
	switch(variable)
	{
	case NPPVpluginNameString:
		value = *((char **)value) = STRINGS_PRODUCTNAME;
		break;
	case NPPVpluginDescriptionString:    // Plugin description
		*((char **)value) = STRINGS_FILEDESCRIPTION;
		break;
	case NPPVpluginScriptableNPObject:
		// If we didn't create any plugin instance, we create it.
		if (pluginInstance)
			*(NPObject **)value = pluginInstance;
		else
		{
			pluginInstance = (NPObject *)ScriptableNPObject::NewObject(savenpp);
			nppobj = pluginInstance;
		}
		if (pluginInstance)
		{
			(NPObject *)ScriptableNPObject::RetainObject(pluginInstance);
		}
		*(NPObject **)value = pluginInstance;
		break;
	default:
		rv = NPERR_GENERIC_ERROR;
	}
	return rv;
}

/* expected by Safari on Darwin */
NPError NPP_HandleEvent(NPP instance, void* event) 
{
	TSAUTO();
	return NPERR_NO_ERROR;
}

/* EXPORT */
#ifdef __cplusplus
extern "C" {
#endif

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs *nppfuncs) 
{
	TSAUTO();

	nppfuncs->version       = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
	nppfuncs->newp          = NPP_New;
	nppfuncs->destroy       = NPP_Destroy;
	nppfuncs->getvalue      = NPP_GetValue;
	nppfuncs->event         = NPP_HandleEvent;
	nppfuncs->setwindow     = NPP_SetWindow;

	return NPERR_NO_ERROR;
}

#ifndef HIBYTE
#define HIBYTE(x) ((((uint32)(x)) & 0xff00) >> 8)
#endif

NPError OSCALL NP_Initialize(NPNetscapeFuncs *npnf)
{
	TSAUTO();
	if(npnf == NULL)
		return NPERR_INVALID_FUNCTABLE_ERROR;

	if(HIBYTE(npnf->version) > NP_VERSION_MAJOR)
		return NPERR_INCOMPATIBLE_VERSION_ERROR;

	ScriptableNPObject::npnfuncs = npnf;
	return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown() 
{
	TSAUTO();
	return NPERR_NO_ERROR;
}

// Unix only
/*
char* NP_GetMIMEDescription(void) {
	return "application/x-vnd-aplix-foo:.foo:anselm@aplix.co.jp";
}
*/

/* needs to be present for WebKit based browsers */
/*
NPError OSCALL NP_GetValue(void *npp, NPPVariable variable, void *value) {
	TSAUTO();
	return NPP_GetValue((NPP)npp, variable, value);
}
*/

#ifdef __cplusplus
}
#endif