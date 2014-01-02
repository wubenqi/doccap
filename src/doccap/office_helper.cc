// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/office_helper.h"

BOOL GetObjectBStrProperty(CComBSTR& bstr, DISPID dispId, IDispatch* pIDispatch) {
	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};
	VARIANT var;

	HRESULT hr = pIDispatch->Invoke(
		dispId,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_PROPERTYGET,        
		&dispArgs,
		&var,
		NULL,
		NULL);

	if(hr == S_OK) {
		_ASSERTE(var.vt==VT_BSTR);
		if( var.vt==VT_BSTR ) {
			bstr.Empty();
			bstr.AppendBSTR(var.bstrVal);
			SysFreeString(var.bstrVal);
			return	TRUE;
		}
	}

	return	FALSE;
}

void ImpGetVersionNumberFromString(LPCTSTR lpszVersion, int* majorVer, int* minorVer) {
	int		idx = 0;
	int		idxMajor = 0;
	int		idxMinor = 0;
	TCHAR	tchMajor[128] = "";
	TCHAR	tchMinor[128] = "";

	if( !lpszVersion )
	{
		*majorVer = 0;
		*minorVer = 0;
		return	;
	}

	while( lpszVersion[idx] && _istdigit(lpszVersion[idx]) )
		tchMajor[idxMajor++] = lpszVersion[idx++];
	tchMajor[idxMajor] = 0;

	while( lpszVersion[idx] && lpszVersion[idx]!=_TCHAR('.') )
		idx++;

	if( lpszVersion[idx] && lpszVersion[idx]==_TCHAR('.') )
	{
		idx++;
		while( lpszVersion[idx] && _istdigit(lpszVersion[idx]) )
			tchMinor[idxMinor++] = lpszVersion[idx++];
		tchMinor[idxMinor] = 0;
	}

	*majorVer = atoi(tchMajor);
	*minorVer = atoi(tchMinor);
}

IDispatch* GetObjectProperty(DISPID dispId, IDispatch* pIDispatch) {
	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};
	VARIANT var;

	HRESULT hr = pIDispatch->Invoke(
		dispId,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_PROPERTYGET,
		&dispArgs,
		&var,
		NULL,
		NULL);

	if(hr == S_OK)
	{   
		_ASSERTE(var.vt == VT_DISPATCH);
		if( var.vt==VT_DISPATCH )
			return var.pdispVal;
	}

	return	NULL;
}

BOOL SetObjectProperty(VARIANT var, DISPID dispId, IDispatch* pIDispatch) {
	DISPPARAMS dispArgs = {NULL, NULL, 1, 1};
	dispArgs.rgvarg = &var;
	DISPID dispidPut = DISPID_PROPERTYPUT;
	dispArgs.rgdispidNamedArgs = &dispidPut;

	HRESULT hr = pIDispatch->Invoke(
		dispId,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_PROPERTYPUT,
		&dispArgs,
		NULL,
		NULL,
		NULL);

	if(hr == S_OK)
		return	TRUE;

	return	FALSE;
}

/****************************************************************/
BOOL GetDispIDFromName(IDispatch* pIDispatch, LPCTSTR lpName, DISPID* pDispId) {
	HRESULT		hr;
	CComBSTR	bstrName(lpName);

	hr = pIDispatch->GetIDsOfNames(IID_NULL, &bstrName, 1,
		LOCALE_USER_DEFAULT, pDispId);

	return	hr == S_OK;
}
/****************************************************************/

