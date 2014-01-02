// Copyright (C) 2009, www.wyu2008.com. All rights reserved
//
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// Author: wubenqi<wubenqi@gmail.com>, 2010-08-12
//

#include "doccap/document_import_excel.h"

#include "doccap/doccap_driver.h"
#include "doccap/office_helper.h"

namespace {

BOOL GetAvailablePrinterPort(LPTSTR lpszPort, DWORD cLength) {
	DWORD dwNeeded = 0;
	DWORD dwReturned = 0;

	int nReturn=EnumPorts(NULL, 1, NULL, 0, &dwNeeded, &dwReturned);

	if(dwNeeded > 0)
	{
		LPBYTE lpBuf = new BYTE[dwNeeded];
		if(!lpBuf)
			return FALSE;

		dwReturned = 0;
		EnumPorts(NULL, 1, lpBuf, dwNeeded, &dwNeeded, &dwReturned);

		BOOL bReturn = FALSE;
		if(dwReturned > 0)
		{
			PORT_INFO_1* pPortInfo = (PORT_INFO_1*)lpBuf;
			for(DWORD i = 0; i < dwReturned; i++)
			{
				if(lstrcmpi(pPortInfo->pName, _T("FILE:")) == 0)
				{
					pPortInfo++;
					continue;
				}
				else if(pPortInfo->pName[0] == 'L' || 
					pPortInfo->pName[0] == 'l')
				{
					bReturn = TRUE;
					lstrcpyn(lpszPort, pPortInfo->pName, cLength);
					break;
				}
				else if(!bReturn)
				{
					bReturn = TRUE;
					lstrcpyn(lpszPort, pPortInfo->pName, cLength);
					pPortInfo++;
				}
			}
		}
		delete []lpBuf;

		return bReturn;
	}
	else
	{
		lstrcpy(lpszPort, "EDFPort:");
	}
	return FALSE;
}

}

DocumentImportExcel::DocumentImportExcel(const char* file_path)
	: DocumentImportBase(file_path) {
	m_bstrFilename.Append(file_path);
	m_bstrPrinterName.Append(DocCapDriver::kDocCapPrinter);
	
	pIDispatchApp_ = NULL;
	pIDispatchWorkbooks_ = NULL;
	pIDispatchWorkbook_ = NULL;
	pIDispatchWorksheet_ = NULL;

}

DocumentImportExcel::~DocumentImportExcel(void)
{
	Destroy();
}

BOOL DocumentImportExcel::ImportDocument(void)
{
	return ExcelPrintFile(m_bstrFilename, m_bstrPrinterName);
}

IDispatch* DocumentImportExcel::ExcelWorkbooksOpenFile(DocVersionStruct& struDocVersion,
								  BSTR bstrFileName, IDispatch* pIDispatchWorkbooks)
{
	VARIANT var;
	VARIANT vt[13];
	DISPPARAMS dispArgs = {vt, NULL, 13, 0};

	for(int i = 0; i < 13; i++)
	{
		vt[i].vt = VT_ERROR;
		vt[i].scode = DISP_E_PARAMNOTFOUND;
	}

	switch( struDocVersion.nMajorVersionNum )
	{
	case	7:	// Office 95
		vt[11].vt = VT_BSTR;
		vt[11].bstrVal = bstrFileName;
		dispArgs.cArgs = 12;
		break;
	case	8:	// Office 97
	default:
		vt[12].vt = VT_BSTR;
		vt[12].bstrVal = bstrFileName;
		dispArgs.cArgs = 13;
		break;
	}

	DISPID	DispIDOpen=0;
	GetDispIDFromName(pIDispatchWorkbooks, _T("Open"),
		&DispIDOpen);

	HRESULT hr = pIDispatchWorkbooks->Invoke(
		DispIDOpen,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		&var, 
		NULL, 
		NULL);

	if(hr == S_OK)
	{   
		_ASSERTE(var.vt == VT_DISPATCH);
		if( var.vt == VT_DISPATCH )
			return var.pdispVal;
	}

	return	NULL;
}

BOOL DocumentImportExcel::ExcelWorkbookPrintOut(DocVersionStruct& struDocVersion,
						   BSTR bstrPrinter, IDispatch* pIDispatchWorkbook)
{

	DISPID DispIDWorksheet = 0;
	GetDispIDFromName(pIDispatchWorkbook, "ActiveSheet",
		&DispIDWorksheet);
	//GetDispIDFromName(pIDispatchApp_, "ActiveWorkbook",
	//	&DispIDWorkbooks);

	pIDispatchWorksheet_ = GetObjectProperty(DispIDWorksheet,
		pIDispatchWorkbook);
	if(!pIDispatchWorksheet_)
		return FALSE;

	// Excel 95 need additional port info
	CComBSTR bstrPrinterInfo(bstrPrinter);
	if( struDocVersion.nMajorVersionNum<8 )
	{
		bstrPrinterInfo.Append(" on ");

		TCHAR achPort[_MAX_PATH];
		GetAvailablePrinterPort(achPort, _MAX_PATH);
		bstrPrinterInfo.Append(achPort);
	}

	VARIANT vt[7];
	for(int i = 0; i < 7; i++)
	{
		vt[i].vt = VT_ERROR;
		vt[i].scode = DISP_E_PARAMNOTFOUND;
	}

	vt[2].vt = VT_BSTR;
	vt[2].bstrVal = bstrPrinterInfo;

	DISPPARAMS dispArgs = {vt, NULL, 7, 0};

	DISPID	DispIDPrintOut = 0;
	GetDispIDFromName(pIDispatchWorksheet_, _T("PrintOut"),
		&DispIDPrintOut);

	HRESULT hr;
	hr = pIDispatchWorksheet_->Invoke(
		DispIDPrintOut, 
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		NULL, 
		NULL, 
		NULL);

	if( hr == S_OK )
		return	TRUE;

	return FALSE;
}

BOOL DocumentImportExcel::ExcelWorkbookClose(IDispatch* pIDispatchWorkbook)
{
	VARIANT vt[3];
	for(int i = 0; i < 3; i++)
	{
		vt[i].vt = VT_ERROR;
		vt[i].scode = DISP_E_PARAMNOTFOUND;
	}

	// HAWK022299-V331
	// Force Workbook to be closed without prompt user to save change.
	vt[2].vt = VT_BOOL;
	vt[2].boolVal = 0;

	DISPPARAMS dispArgs = {vt, NULL, 3, 0};

	DISPID	DispIDClose = 0;
	GetDispIDFromName(pIDispatchWorkbook, _T("Close"),
		&DispIDClose);

	HRESULT hr;
	hr = pIDispatchWorkbook->Invoke(
		DispIDClose,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		NULL, 
		NULL, 
		NULL);

	if(hr != S_OK)
		return FALSE;

	return TRUE;
}

BOOL DocumentImportExcel::ExcelQuitApplication(IDispatch* pIDispatchApp)
{
	DISPID	DispIDQuit = 0;
	GetDispIDFromName(pIDispatchApp, _T("Quit"),
		&DispIDQuit);

	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};

	HRESULT hr;
	hr = pIDispatchApp->Invoke(
		DispIDQuit,
		IID_NULL,
		LOCALE_USER_DEFAULT, 
		DISPATCH_METHOD,
		&dispArgs, 
		NULL, 
		NULL, 
		NULL);

	if( hr != S_OK )
		return	FALSE;

	return	TRUE;
}


BOOL DocumentImportExcel::ExcelPrintFile(BSTR bstrFileName, BSTR bstrPrinterName)
{
	//IDispatch* pIDispatchApp = NULL;

	CLSID clsId;
	if(CLSIDFromProgID(L"Excel.Application", &clsId) != S_OK)
		return FALSE;

	HRESULT hResult;
	hResult = CoCreateInstance(clsId, NULL, CLSCTX_SERVER,
		IID_IDispatch, (void**)&pIDispatchApp_);

	if (hResult!=S_OK)
		return FALSE;

	USES_CONVERSION;
	BOOL	bRet = TRUE;

	if(pIDispatchApp_)
	{
		// as your choice, you can make it visible
		DISPID	DispIDVisible = 0;
		GetDispIDFromName(pIDispatchApp_, "Visible",
			&DispIDVisible);

		// If you want to show Excel application,pls use following code
		/*
		VARIANT	varVisible;
		varVisible.vt = VT_BOOL;
		varVisible.boolVal = TRUE;
		SetObjectProperty(varVisible, DispIDVisible,
		pIDispatchApp);
		*/
		// Get Version String
		DISPID	DispIDVersion = 0;
		GetDispIDFromName(pIDispatchApp_, "Version",
			&DispIDVersion);

		CComBSTR bstrVersion;
		GetObjectBStrProperty(bstrVersion, DispIDVersion,
			pIDispatchApp_);

		int	majorVer;
		int	minorVer;
		ImpGetVersionNumberFromString(OLE2A(bstrVersion),
			&majorVer, &minorVer);

		DocVersionStruct struDocVersion;
		struDocVersion.nMajorVersionNum=majorVer;
		struDocVersion.nMinorVersionNum=minorVer;

		DISPID DispIDWorkbooks = 0;
		GetDispIDFromName(pIDispatchApp_, "Workbooks",
			&DispIDWorkbooks);
		//GetDispIDFromName(pIDispatchApp_, "ActiveWorkbook",
		//	&DispIDWorkbooks);

		//IDispatch* pIDispatchWorkbooks;
		pIDispatchWorkbooks_ = GetObjectProperty(DispIDWorkbooks,
			pIDispatchApp_);

		if(pIDispatchWorkbooks_)
		{
			//IDispatch* pIDispatchWorkbook_;
			pIDispatchWorkbook_ = ExcelWorkbooksOpenFile(struDocVersion,
				bstrFileName, pIDispatchWorkbooks_);

			if(pIDispatchWorkbook_)
			{
				bRet = ExcelWorkbookPrintOut(struDocVersion,
					bstrPrinterName, pIDispatchWorkbook_);
				//::WaitForSingleObject(g_hDOCCapEvent, INFINITE);
				//::ResetEvent(g_hDOCCapEvent);
			}
			else
				bRet = FALSE;

			//pIDispatchWorkbooks_->Release();
		}

		//ExcelQuitApplication(pIDispatchApp_);
		//pIDispatchApp->Release();
	}
	else
	{
		bRet = FALSE;
	}

	return bRet;

}

BOOL DocumentImportExcel::Destroy(void)
{
	if(pIDispatchWorksheet_)
		pIDispatchWorksheet_->Release();
	pIDispatchWorksheet_ = NULL;
	if(pIDispatchWorkbook_)
		ExcelWorkbookClose(pIDispatchWorkbook_);
	if(pIDispatchWorkbook_)
		pIDispatchWorkbook_->Release();
	pIDispatchWorkbook_ = NULL;
	if(pIDispatchWorkbooks_)
		pIDispatchWorkbooks_->Release();
	pIDispatchWorkbooks_ = NULL;
	if(pIDispatchApp_)
	{
		ExcelQuitApplication(pIDispatchApp_);
		pIDispatchApp_->Release();
	}
	pIDispatchApp_ = NULL;
	Sleep(2000);
	return TRUE;
}
