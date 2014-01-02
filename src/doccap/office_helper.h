// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#ifndef DOCCAP_OFFICE_HELPER_H_
#define DOCCAP_OFFICE_HELPER_H_
#pragma once

#include <atlbase.h>

typedef struct _DocVersionStruct{
	int nMajorVersionNum;
	int nMinorVersionNum;
}DocVersionStruct,* pDocVersionStruct;

BOOL GetObjectBStrProperty(CComBSTR& bstr, DISPID dispId, IDispatch* pIDispatch);
void ImpGetVersionNumberFromString(LPCTSTR lpszVersion, int* majorVer,int* minorVer);
IDispatch* GetObjectProperty(DISPID dispId, IDispatch* pIDispatch);
BOOL SetObjectProperty(VARIANT var, DISPID dispId, IDispatch* pIDispatch);

BOOL GetDispIDFromName(IDispatch* pIDispatch, LPCTSTR lpName, DISPID* pDispId);

#endif

