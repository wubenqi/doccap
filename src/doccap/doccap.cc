// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "resource.h"

#include "doccap/spool_thread.h"
#include "doccap/doccap_driver.h"
#include "doccap/doccap_obj.h"

static const GUID LIBID_EDFDOCCapCtrlLib = 
{ 0xf44480d2, 0x1a03, 0x4ffc, { 0x84, 0x88, 0xbf, 0xa5, 0x16, 0xf7, 0x23, 0x7d } };

class CEDFDOCCapCtrlModule : public CAtlDllModuleT< CEDFDOCCapCtrlModule > {
public :
	DECLARE_LIBID(LIBID_EDFDOCCapCtrlLib)
	//DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EDFDOCCAPCTRL, "{981E1240-B99F-41AE-8514-933F609FA00C}")
};

CEDFDOCCapCtrlModule _AtlModule;


// DLL Èë¿Úµã
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

extern "C"  int WINAPI InstallDriver(const char* spl_dir) {
	return DocCapDriver::InstallDriver(spl_dir);
}

extern "C"  int WINAPI RemoveDriver() {
	return DocCapDriver::RemoveDriver();
}

extern "C"  IDOCCapBaseObj*	WINAPI CreateDOCCapObj() {
	CDOCCapObj * pDOCCapObj = new CDOCCapObj();
	if(pDOCCapObj)
		pDOCCapObj->Create();
	return pDOCCapObj;
}

extern "C"  long WINAPI DestroyDOCCapObj(IDOCCapBaseObj* pDOCCapBaseObj) {
	CDOCCapObj * pDOCCapObj = (CDOCCapObj*)pDOCCapBaseObj;
	if(pDOCCapObj) {
		pDOCCapObj->Destroy();
		delete pDOCCapObj;
	}
	return S_OK;
}
